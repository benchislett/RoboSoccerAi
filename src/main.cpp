#include "box2d/box2d.h"
#include <SFML/Graphics.hpp>

#include <cstdio>
#include <cmath>
#include <memory>
#include <numbers>

using std::make_unique;
using std::unique_ptr;
using std::numbers::pi;

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

  void reset() {
    body->SetTransform(b2Vec2(width/length/2.f, height/length/2.f), 0.f);
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

    return b2Vec2(m1, m2);
  }
};

int main() {

  b2Vec2 gravity(0.f, 0.f);
  b2World world(gravity);

  HorizontalWall top_wall(world, width/2, wall_thickness, width);
  HorizontalWall bot_wall(world, width/2, height-wall_thickness, width);

  VerticalWall left_wall(world, wall_thickness, height/2, height);
  VerticalWall right_wall(world, width-wall_thickness, height/2, height);

  Bot player_bot(world, 100, height/2);
  Bot enemy_bot(world, width-100, height/2);

  Ball ball(world);
  
  const float timeStep = 1.f / 60.f;

  const int velocityIterations = 6;
  const int positionIterations = 2;

  sf::RenderWindow window(sf::VideoMode(width, height), "RoboAI Window", sf::Style::Titlebar);

  SfDebugDraw debugDraw(&window);
  world.SetDebugDraw(&debugDraw);
  debugDraw.SetFlags(0x00ff);

  window.setFramerateLimit(60);

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
      enemy_bot.body->SetLinearVelocity(b2Vec2(0, 0));
      enemy_bot.body->SetAngularVelocity(0);
      player_bot.body->SetAngularVelocity(0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      window.close();
    }

    world.Step(timeStep, velocityIterations, positionIterations);

    b2Vec2 ball_position = ball.body->GetPosition();
    if (ball_position.x > 0.95f && (ball_position.y > 0.2f && ball_position.y < 0.8f)) {
      ball.reset();
    }

    b2Vec2 action = BallChase{}.action(player_bot.body->GetPosition(), player_bot.body->GetAngle(), enemy_bot.body->GetPosition(), enemy_bot.body->GetAngle(), ball.body->GetPosition());
    Drive(player_bot.body, action.x, action.y);

    world.DebugDraw();
    debugDraw.DrawCircle(left_wheel(player_bot.body), 0.005, b2Color(0,0,1), true);
    debugDraw.DrawCircle(right_wheel(player_bot.body), 0.005, b2Color(0,0,1), true);
    window.display();

    i++;
  }

  return 0;
}

