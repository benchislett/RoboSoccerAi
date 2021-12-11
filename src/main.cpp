#include "box2d/box2d.h"
#include <torch/torch.h>
#include <SFML/Graphics.hpp>

#include <cstdio>
#include <cmath>
#include <memory>
#include <numbers>
#include <random>

using std::make_unique;
using std::unique_ptr;
using std::make_shared;
using std::shared_ptr;
using std::numbers::pi;

// physics parameters
constexpr int fps = 60;
constexpr float timeStep = 1.f / (float)fps;

constexpr int velocityIterations = 6;
constexpr int positionIterations = 2;

// dimensions in pixels
constexpr float width = 1024;
constexpr float height = 768;

// pixels per box2d meter
constexpr float length = 1024;

constexpr float wheel_power = 20;

// dimension in pixels
constexpr float bot_width = 70;
constexpr float bot_height = 30;
constexpr float ball_radius = 10;
constexpr float wall_thickness = 5;

// dimension in box2d
constexpr float bot_width_f = bot_width / length;
constexpr float bot_height_f = bot_height / length;

// neural net hyperparameters
constexpr int hidden_layers = 12;
constexpr int hidden_nodes = 12;

int randInRange(int lo, int hi) {
  static std::mt19937 gen(std::random_device{}());

  return std::uniform_int_distribution<>{lo, hi}(gen);
}

float randfInRange(float lo, float hi) {
  static std::mt19937 gen(std::random_device{}());

  return std::uniform_real_distribution<>{lo, hi}(gen);
}

sf::Color b2ColorToSfColor(b2Color color, int alpha = 255) {
  return sf::Color((sf::Uint8)(color.r*255), (sf::Uint8)(color.g*255), (sf::Uint8)(color.b*255), (sf::Uint8) alpha);
}

class SfDebugDraw : public b2Draw {
  sf::RenderWindow* window;

public:
  SfDebugDraw(sf::RenderWindow* win) : window(win) {}
  ~SfDebugDraw() {}

  void DrawPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color, bool fill) {
    sf::ConvexShape poly;
    poly.setPointCount(vertex_count);
    for (int i = 0; i < vertex_count; i++) {
      b2Vec2 vertex = vertices[i];
      poly.setPoint(i, sf::Vector2f(vertex.x*length, vertex.y*length));
    }
    poly.setOutlineColor(b2ColorToSfColor(color, 50));
    if (fill) {
      poly.setFillColor(b2ColorToSfColor(color));
    } else {
      poly.setFillColor(sf::Color::Transparent);
    }
    window->draw(poly);
  }

  void DrawPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) {
    DrawPolygon(vertices, vertex_count, color, false);
  }

  void DrawSolidPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) {
    DrawPolygon(vertices, vertex_count, color, true);
  }

  void DrawCircle(const b2Vec2& center, float radius, const b2Color& color, bool fill) {
    const float r = radius * length;
    sf::CircleShape c(r);
    c.setOrigin(r, r);
    c.setPosition(center.x*length,center.y*length);
    c.setOutlineColor(b2ColorToSfColor(color, 50));
    if (fill) {
      c.setFillColor(b2ColorToSfColor(color));
    } else {
      c.setFillColor(sf::Color::Transparent);
    }
    window->draw(c);
  }

  void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
    DrawCircle(center, radius, color, false);
  }

  void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
    DrawCircle(center, radius, color, true);
  }

  void DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
    sf::Vertex pt(sf::Vector2f(p.x*length, p.y*length), b2ColorToSfColor(color));
    window->draw(&pt, 1, sf::Points);
  }

  void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    sf::Vertex p[2];
    p[0] = sf::Vertex(sf::Vector2f(p1.x*length,p1.y*length), b2ColorToSfColor(color));
    p[1] = sf::Vertex(sf::Vector2f(p2.x*length,p2.y*length), b2ColorToSfColor(color));
    window->draw(p, 2, sf::Lines);
  }

  void DrawTransform(const b2Transform& xf) {
    const float line_length = 0.05; // box2d units

    b2Vec2 p1 = xf.p;
    DrawSegment(p1, p1 + (line_length * xf.q.GetXAxis()), b2Color(1.f, 0.f, 0.f));
    DrawSegment(p1, p1 - (line_length * xf.q.GetYAxis()), b2Color(0.f, 1.f, 0.f));
  }
};

struct RoboNet : torch::nn::Module {
  torch::nn::Linear layers[hidden_layers+2] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
  torch::Tensor bias[hidden_layers];

  RoboNet() {
    layers[0] = register_module("input_layer", torch::nn::Linear(8, hidden_nodes));
    for (int i = 0; i < hidden_layers; i++) {
      layers[i+1] = register_module("fc" + std::to_string(i+1), torch::nn::Linear(hidden_nodes, hidden_nodes));
      bias[i] = register_parameter("bias" + std::to_string(i+1), torch::zeros(hidden_nodes));
    }
    layers[hidden_layers+1] = register_module("output_layer", torch::nn::Linear(hidden_nodes, 2));
  }

  torch::Tensor forward(torch::Tensor x) {
    x = torch::relu(layers[0]->forward(x));
    for (int i = 0; i < hidden_layers; i++) {
      x = torch::relu(layers[i+1]->forward(x)) + bias[i];
    }
    x = torch::sigmoid(layers[hidden_layers+1]->forward(x));
    return x;
  }
};

float clamp(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

b2Vec2 left_wheel(b2Body* body) {
  b2Vec2 pos = body->GetPosition();
  float rot = -body->GetAngle();
  float left_rot = rot + (pi/2.f);
  return b2Vec2(pos.x + cosf(left_rot) * bot_height_f / 2.f, pos.y - sinf(left_rot) * bot_height_f / 2.f);
}

b2Vec2 right_wheel(b2Body* body) {
  b2Vec2 pos = body->GetPosition();
  float rot = -body->GetAngle();
  float right_rot = rot - (pi/2.f);
  return b2Vec2(pos.x + cosf(right_rot) * bot_height_f / 2.f, pos.y - sinf(right_rot) * bot_height_f / 2.f);
}

void Drive(b2Body* body, float left, float right) {
  left = clamp(left, -1, 1) / 100.0; // TODO: magic constant
  right = clamp(right, -1, 1) / 100.0;

  float rot = -body->GetAngle();

  b2Vec2 left_force(cosf(rot) * left, sinf(rot) * -left);
  b2Vec2 right_force(cosf(rot) * right, sinf(rot) * -right);

  body->ApplyForce(left_force, left_wheel(body), true);
  body->ApplyForce(right_force, right_wheel(body), true);
}

struct Ball {
  unique_ptr<b2CircleShape> shape;
  unique_ptr<b2FixtureDef> fixture;
  unique_ptr<b2BodyDef> body_def;
  b2Body* body;

  Ball(b2World& world, int spawn_x = width/2, int spawn_y = height/2) {
    shape = make_unique<b2CircleShape>();
    shape->m_radius = ball_radius / length;

    fixture = make_unique<b2FixtureDef>();
    fixture->shape = shape.get();
    fixture->density = 1.f;
    fixture->friction = 1.f;

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float)spawn_x / length, (float)spawn_y / length);
    body_def->type = b2_dynamicBody;
    body_def->linearDamping = 1.f;
    body_def->angularDamping = 2.f;
  
    body = world.CreateBody(body_def.get());
    body->CreateFixture(fixture.get());
  }

  void setPosition(int x, int y) {
    body->SetTransform(b2Vec2(x/length, y/length), 0.f);
  }

  void reset() {
    setPosition(width/2, length/2);
  }

  void teleport() {
    int pad = 2 * wall_thickness + ball_radius;
    setPosition(randInRange(pad, width-pad), randInRange(pad, height-pad));
  }
};

struct Bot {
  unique_ptr<b2PolygonShape> shape;
  unique_ptr<b2FixtureDef> fixture;
  unique_ptr<b2BodyDef> body_def;
  b2Body* body;

  Bot(b2World& world, int spawn_x, int spawn_y) {
    shape = make_unique<b2PolygonShape>();
    shape->SetAsBox(bot_width_f/2.f, bot_height_f/2.f);

    fixture = make_unique<b2FixtureDef>();
    fixture->shape = shape.get();
    fixture->density = 10.f;
    fixture->friction = 1.f;

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float)spawn_x / length, (float)spawn_y / length);
    body_def->type = b2_dynamicBody;
    body_def->linearDamping = 3.f;
    body_def->angularDamping = 3.f;
  
    body = world.CreateBody(body_def.get());
    body->CreateFixture(fixture.get());
  }
};

struct HorizontalWall {
  unique_ptr<b2PolygonShape> shape;
  unique_ptr<b2FixtureDef> fixture;
  unique_ptr<b2BodyDef> body_def;
  b2Body* body;

  HorizontalWall(b2World& world, int spawn_x, int spawn_y, int w) {
    shape = make_unique<b2PolygonShape>();
    shape->SetAsBox(w/length/2.f, wall_thickness/length/2.f);

    fixture = make_unique<b2FixtureDef>();
    fixture->shape = shape.get();

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float)spawn_x / length, (float)spawn_y / length);
  
    body = world.CreateBody(body_def.get());
    body->CreateFixture(fixture.get());
  }
};

struct VerticalWall {
  unique_ptr<b2PolygonShape> shape;
  unique_ptr<b2FixtureDef> fixture;
  unique_ptr<b2BodyDef> body_def;
  b2Body* body;

  VerticalWall(b2World& world, int spawn_x, int spawn_y, int h) {
    shape = make_unique<b2PolygonShape>();
    shape->SetAsBox(wall_thickness/length/2.f, h/length/2.f);

    fixture = make_unique<b2FixtureDef>();
    fixture->shape = shape.get();

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float)spawn_x / length, (float)spawn_y / length);
  
    body = world.CreateBody(body_def.get());
    body->CreateFixture(fixture.get());
  }
};

struct Strategy {
  virtual b2Vec2 action(b2Vec2 self_pos, float self_rot, b2Vec2 opp_pos, float opp_rot, b2Vec2 ball_pos) = 0;
};

struct AiStrategy : Strategy {
  shared_ptr<RoboNet> net;
  shared_ptr<torch::optim::Adam> optimizer;

  AiStrategy() {
    net = make_shared<RoboNet>();
    optimizer = make_shared<torch::optim::Adam>(net->parameters());
  }

  float train(torch::Tensor inputs, torch::Tensor outputs) {
    optimizer->zero_grad();

    torch::Tensor prediction = net->forward(inputs);
    torch::Tensor loss = torch::nn::functional::mse_loss(prediction, outputs);
    loss.backward();
    optimizer->step();

    return loss.item<float>();
  }

  b2Vec2 action(b2Vec2 self_pos, float self_rot, b2Vec2 opp_pos, float opp_rot, b2Vec2 ball_pos) {
    while (self_rot < 0) self_rot += 2*pi;
    while (self_rot > 2*pi) self_rot -= 2*pi;

    torch::Tensor input = torch::zeros(8);
    auto input_a = input.accessor<float,1>();
    input_a[0] = self_pos.x;
    input_a[1] = self_pos.y;
    input_a[2] = self_rot;
    input_a[3] = opp_pos.x;
    input_a[4] = opp_pos.y;
    input_a[5] = opp_rot;
    input_a[6] = ball_pos.x;
    input_a[7] = ball_pos.y;

    torch::Tensor output = net->forward(input);
    auto output_a = output.accessor<float,1>();
    return b2Vec2(output_a[0], output_a[1]);
  }
};

struct BallChase : Strategy {
  b2Vec2 action(b2Vec2 self_pos, float self_rot, b2Vec2 opp_pos, float opp_rot, b2Vec2 ball_pos) {
    float m1 = 0.5f;
    float m2 = 0.5f;

    float dx = ball_pos.x - self_pos.x;
    float dy = self_pos.y - ball_pos.y;

    float angle = -self_rot;
    while (angle > pi) angle -= 2*pi;
    while (angle < -pi) angle += 2*pi;

    float target_angle = atan2(dy, dx);

    float d = atan2(sinf(target_angle-angle), cosf(target_angle-angle));

    m1 -= d;
    m2 += d;

    return b2Vec2(clamp(m1, -1, 1), clamp(m2, -1, 1));
  }
};

int step_ballchase(b2World& world, Bot& player, Ball& ball, Strategy& strat) {
  int hit = 0;

  world.Step(timeStep, velocityIterations, positionIterations);

  b2Vec2 ball_position = ball.body->GetPosition();
  b2Vec2 player_position = player.body->GetPosition();
  if ((player_position-ball_position).Length() < (60/length)) {
    ball.teleport();
    hit++;
  }

  b2Vec2 action = strat.action(player_position, player.body->GetAngle(), b2Vec2(0, 0), 0, ball_position);
  Drive(player.body, action.x, action.y);

  return hit;
}

int main() {

  b2Vec2 gravity(0.f, 0.f);
  b2World world(gravity);

  HorizontalWall top_wall(world, width/2, wall_thickness, width);
  HorizontalWall bot_wall(world, width/2, height-wall_thickness, width);

  VerticalWall left_wall(world, wall_thickness, height/2, height);
  VerticalWall right_wall(world, width-wall_thickness, height/2, height);

  Bot player_bot(world, 100, height/2);
  // Bot enemy_bot(world, width-100, height/2);

  Ball ball(world);
  
  sf::RenderWindow window(sf::VideoMode(width, height), "RoboAI Window", sf::Style::Titlebar);

  SfDebugDraw debugDraw(&window);
  world.SetDebugDraw(&debugDraw);
  debugDraw.SetFlags(0x00ff);

  window.setFramerateLimit(fps);
  
  auto AiStrat = AiStrategy{};
  auto SimpleStrat = BallChase{};
  /*float loss_acc = 0;
  const int batch_size = 100000;
  torch::Tensor inputs = torch::randn({batch_size, 8});
  torch::Tensor outputs = torch::randn({batch_size, 2});
  auto inputs_a = inputs.accessor<float, 2>();
  auto outputs_a = outputs.accessor<float, 2>();
  for (int j = 0;; j++) {
    for (int i = 0; i < batch_size; i++) {
      b2Vec2 ball_position = b2Vec2(randfInRange(0, width/length), randfInRange(0, height/length));
      b2Vec2 player_position = b2Vec2(randfInRange(0, width/length), randfInRange(0, height/length));
      float angle = randfInRange(0, 2*pi);
      b2Vec2 action = SimpleStrat.action(player_position, angle, b2Vec2(0, 0), 0, ball_position);

      inputs_a[i][0] = player_position.x;
      inputs_a[i][1] = player_position.y;
      inputs_a[i][2] = angle;
      inputs_a[i][3] = 0;
      inputs_a[i][4] = 0;
      inputs_a[i][5] = 0;
      inputs_a[i][6] = ball_position.x;
      inputs_a[i][7] = ball_position.y;

      outputs_a[i][0] = action.x;
      outputs_a[i][1] = action.y;
    }

    loss_acc += AiStrat.train(inputs, outputs);
    if (j % 10 == 0) {
      loss_acc /= 10.0;
      printf("%d: Loss: %f\n", j, loss_acc);
      loss_acc = 0;
      torch::save(AiStrat.net, "models/model_" + std::to_string(j) + ".pt");
    }
  }

  return 0;*/
  torch::load(AiStrat.net, "models/model_1710.pt");

  int hits = 0;
  int i = 0;
  while (window.isOpen()) {
    window.clear(sf::Color(0, 0, 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
      Drive(player_bot.body, 0.8, 0.8);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
      Drive(player_bot.body, -0.8, -0.8);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
      Drive(player_bot.body, -0.5, 0.5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
      Drive(player_bot.body, 0.5, -0.5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
      player_bot.body->SetLinearVelocity(b2Vec2(0, 0));
      player_bot.body->SetAngularVelocity(0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      window.close();
      break;
    }

    hits += step_ballchase(world, player_bot, ball, AiStrat);

    world.DebugDraw();
    debugDraw.DrawCircle(left_wheel(player_bot.body), 0.005, b2Color(0,0,1), true);
    debugDraw.DrawCircle(right_wheel(player_bot.body), 0.005, b2Color(0,0,1), true);
    window.display();

    i++;
  }

  return 0;
}

