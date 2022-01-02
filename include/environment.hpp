#pragma once

#include "box2d/box2d.h"
#include "misc.hpp"
#include "visualize.hpp"

#include <memory>

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;

b2Vec2 left_wheel(b2Body* body);

b2Vec2 right_wheel(b2Body* body);

struct Ball {
  unique_ptr<b2CircleShape> shape;
  unique_ptr<b2FixtureDef> fixture;
  unique_ptr<b2BodyDef> body_def;
  b2Body* body;

  int spawn_x, spawn_y;

  Ball(b2World& world, int sx = width / 2, int sy = height / 2) : spawn_x(sx), spawn_y(sy) {
    shape           = make_unique<b2CircleShape>();
    shape->m_radius = ball_radius / length;

    fixture           = make_unique<b2FixtureDef>();
    fixture->shape    = shape.get();
    fixture->density  = 1.f;
    fixture->friction = 1.f;

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float) spawn_x / length, (float) spawn_y / length);
    body_def->type           = b2_dynamicBody;
    body_def->linearDamping  = 1.f;
    body_def->angularDamping = 2.f;

    body = world.CreateBody(body_def.get());
    body->CreateFixture(fixture.get());
  }

  void setPosition(int x, int y);

  void reset();

  void teleport();
};

struct Bot {
  unique_ptr<b2PolygonShape> shape;
  unique_ptr<b2FixtureDef> fixture;
  unique_ptr<b2BodyDef> body_def;
  b2Body* body;

  int spawn_x, spawn_y;

  Bot(b2World& world, int sx, int sy) : spawn_x(sx), spawn_y(sy) {
    shape = make_unique<b2PolygonShape>();
    shape->SetAsBox(bot_width_f / 2.f, bot_height_f / 2.f);

    fixture           = make_unique<b2FixtureDef>();
    fixture->shape    = shape.get();
    fixture->density  = 10.f;
    fixture->friction = 1.f;

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float) spawn_x / length, (float) spawn_y / length);
    body_def->type           = b2_dynamicBody;
    body_def->linearDamping  = 3.f;
    body_def->angularDamping = 3.f;

    body = world.CreateBody(body_def.get());
    body->CreateFixture(fixture.get());
  }

  void setPosition(int x, int y);

  void reset();

  void drive(float left, float right);
};

struct HorizontalWall {
  unique_ptr<b2PolygonShape> shape;
  unique_ptr<b2FixtureDef> fixture;
  unique_ptr<b2BodyDef> body_def;
  b2Body* body;

  HorizontalWall(b2World& world, int spawn_x, int spawn_y, int w) {
    shape = make_unique<b2PolygonShape>();
    shape->SetAsBox(w / length / 2.f, wall_thickness / length / 2.f);

    fixture        = make_unique<b2FixtureDef>();
    fixture->shape = shape.get();

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float) spawn_x / length, (float) spawn_y / length);

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
    shape->SetAsBox(wall_thickness / length / 2.f, h / length / 2.f);

    fixture        = make_unique<b2FixtureDef>();
    fixture->shape = shape.get();

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float) spawn_x / length, (float) spawn_y / length);

    body = world.CreateBody(body_def.get());
    body->CreateFixture(fixture.get());
  }
};

template <int NState, int NInput>
struct BlankEnv {
  unique_ptr<SfDebugDraw> debugDraw;
  unique_ptr<sf::RenderWindow> window;
  unique_ptr<b2World> world;
  HorizontalWall top_wall;
  HorizontalWall bot_wall;
  VerticalWall left_wall;
  VerticalWall right_wall;

  BlankEnv()
      : debugDraw(nullptr), window(nullptr),
        world(make_unique<b2World>(b2Vec2(0.f, 0.f))), top_wall{*world, width / 2, wall_thickness, width},
        bot_wall{*world, width / 2, height - wall_thickness, width}, left_wall{*world, wall_thickness, height / 2,
                                                                               height},
        right_wall{*world, width - wall_thickness, height / 2, height} {}

  virtual void reset() = 0;

  virtual std::array<float, NState> state() const = 0;

  virtual void step() = 0;

  virtual float action(std::array<float, NInput> input) = 0;

  void init(bool render = false) {
    if (render) {
      window = make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "RoboAI Window", sf::Style::Titlebar);
      window->setFramerateLimit(fps);

      debugDraw = make_unique<SfDebugDraw>(window.get());
      world->SetDebugDraw(debugDraw.get());
      debugDraw->SetFlags(0);
      debugDraw->AppendFlags(debugDraw->e_aabbBit);
      debugDraw->AppendFlags(debugDraw->e_centerOfMassBit);
      debugDraw->AppendFlags(debugDraw->e_shapeBit);
      debugDraw->AppendFlags(debugDraw->e_jointBit);
    }

    reset();
  }

  void update(bool render = false) {
    if (render) {
      window->clear(sf::Color(0, 0, 0));
      world->DebugDraw();
      window->display();
    }
  }
};

struct BallChaseEnv : BlankEnv<6, 2> {
  Bot player;
  Ball ball;

  BallChaseEnv() : BlankEnv(), player{*world, 100, height / 2}, ball{*world} {}

  void reset();

  std::array<float, 6> state() const;

  void step();

  float action(std::array<float, 2> input);

  float dist() const;
};

struct SoccerEnv : BlankEnv<10, 4> {
  Bot player1;
  Bot player2;
  Ball ball;

  SoccerEnv() : BlankEnv(), player1{*world, 100, height / 2}, player2{*world, width - 100, height / 2}, ball{*world} {}

  void reset();

  std::array<float, 10> state() const;
  std::array<float, 10> mirror_state() const;

  void step();

  float action(std::array<float, 4> input);
};