#include "box2d/box2d.h"
#include <SFML/Graphics.hpp>

#include <cstdio>
#include <cmath>
#include <numbers>

using std::numbers::pi;

constexpr int width = 1024;
constexpr int height = 768;

constexpr float wheel_power = 0.01f;

constexpr int bot_width = 70;
constexpr int bot_height = 30;

constexpr float bot_width_f =  (float)bot_width / (float)width;
constexpr float bot_height_f = (float)bot_height / (float)height;

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
      poly.setPoint(i, sf::Vector2f(vertex.x*width, vertex.y*height));
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
    const float r = radius * fmin(width, height);
    sf::CircleShape c(r);
    c.setOrigin(r, r);
    c.setPosition(center.x*width,center.y*height);
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
    sf::Vertex pt(sf::Vector2f(p.x*width, p.y*height), b2ColorToSfColor(color));
    window->draw(&pt, 1, sf::Points);
  }

  void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    sf::Vertex p[2];
    p[0] = sf::Vertex(sf::Vector2f(p1.x*width,p1.y*height), b2ColorToSfColor(color));
    p[1] = sf::Vertex(sf::Vector2f(p2.x*width,p2.y*height), b2ColorToSfColor(color));
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
  left = clamp(left, -1, 1) / 100.0;
  right = clamp(right, -1, 1) / 100.0;

  float rot = -body->GetAngle();

  b2Vec2 left_force(cosf(rot) * left, sinf(rot) * -left);
  b2Vec2 right_force(cosf(rot) * right, sinf(rot) * -right);

  body->ApplyForce(left_force, left_wheel(body), true);
  body->ApplyForce(right_force, right_wheel(body), true);
}

int main() {

  b2Vec2 gravity(0.f, 0.f);
  b2World world(gravity);

  // WALLS

  b2PolygonShape horizontal_wall_shape;
  horizontal_wall_shape.SetAsBox(1.f, 0.01f);

  b2PolygonShape vertical_wall_shape;
  vertical_wall_shape.SetAsBox(0.01f, 1.f);

  b2FixtureDef horizontal_wall_fixture;
  horizontal_wall_fixture.shape = &horizontal_wall_shape;

  b2FixtureDef vertical_wall_fixture;
  vertical_wall_fixture.shape = &vertical_wall_shape;

  b2BodyDef top_wall_body_def;
  top_wall_body_def.position.Set(0.f, 0.01f);
  b2Body* top_wall_body = world.CreateBody(&top_wall_body_def);
  top_wall_body->CreateFixture(&horizontal_wall_fixture);

  b2BodyDef bottom_wall_body_def;
  bottom_wall_body_def.position.Set(0.f, 0.99f);
  b2Body* bottom_wall_body = world.CreateBody(&bottom_wall_body_def);
  bottom_wall_body->CreateFixture(&horizontal_wall_fixture);

  b2BodyDef left_wall_body_def;
  left_wall_body_def.position.Set(0.01f, 0.f);
  b2Body* left_wall_body = world.CreateBody(&left_wall_body_def);
  left_wall_body->CreateFixture(&vertical_wall_fixture);

  b2BodyDef right_wall_body_def;
  right_wall_body_def.position.Set(0.99f, 0.f);
  b2Body* right_wall_body = world.CreateBody(&right_wall_body_def);
  right_wall_body->CreateFixture(&vertical_wall_fixture);

  // BOTS

  b2BodyDef self_body_def;
  self_body_def.type = b2_dynamicBody;
  self_body_def.position.Set(0.1f, 0.5f);
  self_body_def.linearDamping = 3.f;
  self_body_def.angularDamping = 3.f;

  b2BodyDef opp_body_def;
  opp_body_def.type = b2_dynamicBody;
  opp_body_def.position.Set(0.9f, 0.5f);
  opp_body_def.linearDamping = 3.f;
  opp_body_def.angularDamping = 3.f;

  b2Body* self_body = world.CreateBody(&self_body_def);
  b2Body* opp_body = world.CreateBody(&opp_body_def);

  b2PolygonShape robot_shape;
  robot_shape.SetAsBox(bot_width_f/2.f, bot_height_f/2.f);

  b2FixtureDef robot_fixture;
  robot_fixture.shape = &robot_shape;
  robot_fixture.density = 10.f;
  robot_fixture.friction = 1.f;

  self_body->CreateFixture(&robot_fixture);
  opp_body->CreateFixture(&robot_fixture);

  // BALL
  
  b2CircleShape ball_shape;
  ball_shape.m_radius = 0.02f;

  b2FixtureDef ball_fixture;
  ball_fixture.shape = &ball_shape;
  ball_fixture.density = 1.f;
  ball_fixture.friction = 5.f;
  
  b2BodyDef ball_body_def;
  ball_body_def.type = b2_dynamicBody;
  ball_body_def.position.Set(0.5f, 0.5f);
  ball_body_def.linearDamping = 0.5f;
  ball_body_def.angularDamping = 1.f;

  b2Body* ball_body = world.CreateBody(&ball_body_def);
  ball_body->CreateFixture(&ball_fixture);

  // PHYSICS

  const float timeStep = 1.f / 60.f;

  const int velocityIterations = 6;
  const int positionIterations = 2;

  // GRAPHICS

  sf::RenderWindow window(sf::VideoMode(width, height), "RoboAI Window", sf::Style::Titlebar);

  SfDebugDraw debugDraw(&window);
  world.SetDebugDraw(&debugDraw);
  debugDraw.SetFlags(0x00ff);

  window.setFramerateLimit(60);

  int i = 0;
  while (window.isOpen()) {
    window.clear(sf::Color(0, 0, 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
      Drive(self_body, 0.8, 0.8);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
      Drive(self_body, -0.8, -0.8);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
      Drive(self_body, -0.5, 0.5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
      Drive(self_body, 0.5, -0.5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
      self_body->SetLinearVelocity(b2Vec2(0, 0));
      opp_body->SetLinearVelocity(b2Vec2(0, 0));
      opp_body->SetAngularVelocity(0);
      self_body->SetAngularVelocity(0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      window.close();
    }

    world.Step(timeStep, velocityIterations, positionIterations);

    b2Vec2 ball_position = ball_body->GetPosition();
    if (ball_position.x > 0.95f && (ball_position.y > 0.35f && ball_position.y < 0.7f)) {
      ball_body->SetTransform(b2Vec2(0.5f, 0.5f), 0.f);
    }

    world.DebugDraw();
    debugDraw.DrawCircle(left_wheel(self_body), 0.005, b2Color(0,0,1), true);
    debugDraw.DrawCircle(right_wheel(self_body), 0.005, b2Color(0,0,1), true);
    window.display();

    i++;
  }

  return 0;
}

