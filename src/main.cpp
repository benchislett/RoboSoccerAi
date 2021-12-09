#include "box2d/box2d.h"
#include <SFML/Graphics.hpp>

#include <cstdio>
#include <cmath>
#include <numbers>

using std::numbers::pi;

constexpr int width = 1024;

constexpr int height = 768;

constexpr int bot_width = 70;
constexpr int bot_height = 30;

constexpr float bot_width_f = (float)bot_width / (float)width;
constexpr float bot_height_f = (float)bot_height / (float)height;

float clamp(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

float angle(b2Body* body) {
  float raw_rot = body->GetAngle();
  float deg = -raw_rot;
  while (deg < 0) deg += 360;
  while (deg > 180) deg -= 360;
  return deg * pi / 180.f;
}

b2Vec2 left_wheel(b2Body* body) {
  b2Vec2 pos = body->GetPosition();
  float rot = angle(body);
  float left_rot = rot + (pi/2.f);
  return b2Vec2(pos.x + cosf(left_rot) * bot_height_f / 2.f, pos.y - sinf(left_rot) * bot_height_f / 2.f);
}

b2Vec2 right_wheel(b2Body* body) {
  b2Vec2 pos = body->GetPosition();
  float rot = angle(body);
  float right_rot = rot - (pi/2.f);
  return b2Vec2(pos.x + cosf(right_rot) * bot_height_f / 2.f, pos.y - sinf(right_rot) * bot_height_f / 2.f);
}

void Drive(b2Body* body, float left, float right) {
  left = clamp(left, -1, 1) / 100.0;
  right = clamp(right, -1, 1) / 100.0;

  float rot = angle(body);

  b2Vec2 left_force(cosf(rot) * left, sinf(rot) * -left);
  b2Vec2 right_force(cosf(rot) * right, sinf(rot) * -right);

  // printf("Rotation %f, Left wheel (%f, %f), Right wheel (%f, %f)\n", rot, left_wheel(body).x, left_wheel(body).y, right_wheel(body).x, right_wheel(body).y);
  // printf("Position (%f, %f) applying force (%f, %f)\n", body->GetPosition().x, body->GetPosition().y, left_force.x, left_force.y);
  body->ApplyForce(left_force, left_wheel(body), true);
  body->ApplyForce(right_force, right_wheel(body), true);
}

void draw_wheels(b2Body* body, sf::RenderWindow& window) {
  const float r = 1;
  auto left = left_wheel(body);
  auto right = right_wheel(body);

  sf::CircleShape left_c(r);
  sf::CircleShape right_c(r);
  left_c.setOrigin(sf::Vector2f(r, r));
  left_c.setPosition(sf::Vector2f(left.x*width, left.y*height));
  left_c.setFillColor(sf::Color::Green);
  right_c.setOrigin(sf::Vector2f(r, r));
  right_c.setPosition(sf::Vector2f(right.x*width, right.y*height));
  right_c.setFillColor(sf::Color::Green);

  window.draw(left_c);
  window.draw(right_c);
}

int main() {

  b2Vec2 gravity(0.f, 0.f);
  b2World world(gravity);

  b2BodyDef self_body_def;
  self_body_def.type = b2_dynamicBody;
  self_body_def.position.Set(bot_width_f/2, 0.5f);

  b2BodyDef opp_body_def;
  opp_body_def.type = b2_dynamicBody;
  opp_body_def.position.Set(1.f-bot_width_f/2, 0.5f);

  b2Body* self_body = world.CreateBody(&self_body_def);
  b2Body* opp_body = world.CreateBody(&opp_body_def);

  b2PolygonShape robot_shape;
  robot_shape.SetAsBox(bot_width_f/2.f, bot_height_f/2.f);

  b2FixtureDef robot_fixture;
  robot_fixture.shape = &robot_shape;
  robot_fixture.density = 10.f;
  robot_fixture.friction = 20.f;

  self_body->CreateFixture(&robot_fixture);
  opp_body->CreateFixture(&robot_fixture);

  const float timeStep = 1.f / 60.f;

  const int velocityIterations = 6;
  const int positionIterations = 2;

  const b2Vec2 right_force(1.f, 0.f);
  const b2Vec2 left_force(-1.f, 0.f);

  sf::RenderWindow window(sf::VideoMode(width, height), "RoboAI Window", sf::Style::Titlebar);
  sf::RectangleShape self_rect(sf::Vector2f(bot_width, bot_height));
  sf::RectangleShape opp_rect(sf::Vector2f(bot_width, bot_height));
  self_rect.setFillColor(sf::Color(0, 0, 255));
  opp_rect.setFillColor(sf::Color(255, 0, 0));

  window.setFramerateLimit(60);

  int i = 0;
  while (window.isOpen()) {
    window.clear(sf::Color(0, 0, 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
      Drive(self_body, 0.8, 0.8);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
      Drive(self_body, -0.8, -0.8);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
      Drive(self_body, -0.5, 0.5);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
      Drive(self_body, 0.5, -0.5);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
      self_body->SetLinearVelocity(b2Vec2(0, 0));
      opp_body->SetLinearVelocity(b2Vec2(0, 0));
      opp_body->SetAngularVelocity(0);
      self_body->SetAngularVelocity(0);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      window.close();
    }


    world.Step(timeStep, velocityIterations, positionIterations);
    b2Vec2 self_position = self_body->GetPosition();
    b2Vec2 opp_position = opp_body->GetPosition();

    double dx=(self_position.x-opp_position.x);dx*=dx;
    double dy=(self_position.y-opp_position.y);dy*=dy;
    printf("Us: (%f, %f), Them: (%f, %f), Width: %f, Dist: %f\n", self_position.x, self_position.y, opp_position.x, opp_position.y, bot_width_f, sqrtf(dx+dy));

    self_rect.setOrigin(self_rect.getSize().x / 2, self_rect.getSize().y / 2);
    opp_rect.setOrigin(opp_rect.getSize().x / 2, opp_rect.getSize().y / 2);

    self_rect.setPosition(self_position.x*width, self_position.y*height);
    self_rect.setRotation(self_body->GetAngle());
    opp_rect.setPosition(opp_position.x*width, opp_position.y*height);
    opp_rect.setRotation(opp_body->GetAngle());

    draw_wheels(self_body, window);
    window.draw(self_rect);
    window.draw(opp_rect);
    window.display();
    i++;
  }

  return 0;
}

