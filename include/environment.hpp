#pragma once

#include "box2d/box2d.h"
#include "misc.hpp"
#include "visualize.hpp"

extern "C" {
#include "btcomm.h"
#include "imageCapture.h"
#include "roboAI.h"
}

#include <functional>
#include <memory>
#include <thread>

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

  static constexpr float base_radius = ball_radius_f;

  Ball(b2World& world, int sx = width / 2, int sy = height / 2) : spawn_x(sx), spawn_y(sy) {
    shape           = make_unique<b2CircleShape>();
    shape->m_radius = base_radius;

    fixture              = make_unique<b2FixtureDef>();
    fixture->shape       = shape.get();
    fixture->density     = ball_density;
    fixture->friction    = 1.f;
    fixture->restitution = ball_elasticity;

    fixture->restitutionThreshold = 0;

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float) spawn_x / length, (float) spawn_y / length);
    body_def->type           = b2_dynamicBody;
    body_def->linearDamping  = ball_damping_linear;
    body_def->angularDamping = ball_damping_angular;

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
  float spawn_rot;

  static constexpr float base_width  = bot_width_f / 2.f;
  static constexpr float base_height = bot_height_f / 2.f;

  Bot(b2World& world, int sx, int sy, float sr = 0.f) : spawn_x(sx), spawn_y(sy), spawn_rot(sr) {
    shape = make_unique<b2PolygonShape>();
    shape->SetAsBox(base_width, base_height);

    fixture           = make_unique<b2FixtureDef>();
    fixture->shape    = shape.get();
    fixture->density  = bot_density;
    fixture->friction = 1.f;

    body_def = make_unique<b2BodyDef>();
    body_def->position.Set((float) spawn_x / length, (float) spawn_y / length);
    body_def->type           = b2_dynamicBody;
    body_def->linearDamping  = bot_damping_linear;
    body_def->angularDamping = bot_damping_angular;

    body = world.CreateBody(body_def.get());
    body->CreateFixture(fixture.get());
  }

  void setState(int x, int y, float rot = 0);

  void reset(bool flip = false);

  void teleport();

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
  shared_ptr<sf::RenderWindow> window;
  shared_ptr<b2World> world;
  HorizontalWall top_wall;
  HorizontalWall bot_wall;
  VerticalWall left_wall;
  VerticalWall right_wall;

  BlankEnv()
      : debugDraw(nullptr), window(nullptr),
        world(make_shared<b2World>(b2Vec2_zero)), top_wall{*world, width / 2, wall_thickness, width},
        bot_wall{*world, width / 2, height - wall_thickness, width}, left_wall{*world, wall_thickness, height / 2,
                                                                               height},
        right_wall{*world, width - wall_thickness, height / 2, height} {}

  virtual void reset() = 0;

  virtual std::array<float, NState> state() const = 0;

  virtual void step() = 0;

  virtual float action(std::array<float, NInput> input) = 0;

  virtual void debug_draw() = 0;

  void init(bool render = false) {
    if (render) {
      window = make_shared<sf::RenderWindow>(sf::VideoMode(width, height), "RoboAI Window", sf::Style::Titlebar);
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
      debug_draw();
      window->display();
    }
  }
};

struct SoccerEnv : BlankEnv<11, 4> {
  Bot player1;
  Bot player2;
  Ball ball;

  int side = 0;

  std::vector<std::array<float, 10>> history;

  SoccerEnv()
      : BlankEnv(), player1{*world, 100, randomize_spawn_y ? randInRange(100, height - 100) : height / 2, 0},
        player2{*world, width - 100, randomize_spawn_y ? randInRange(100, height - 100) : height / 2, -pi},
        ball{*world} {}

  void debug_draw();

  void reset();

  std::array<float, 11> state() const;
  std::array<float, 10> savestate() const;
  std::array<float, 101> state10();

  void step();

  float action(std::array<float, 4> input);

  float dist_players() const;
  float dist_player1_ball() const;
  float dist_player2_ball() const;
  float dist_ball_net1() const;
  float dist_ball_net2() const;
};

struct LiveSoccerEnv {
  std::vector<struct AI_data> history;

  unique_ptr<std::thread> runner;

  LiveSoccerEnv();

  struct AI_data* raw_state() const;

  static std::array<float, 11> state_from_raw(struct AI_data data);

  std::array<float, 11> state();
  std::array<float, 10> savestate();
  std::array<float, 101> state10();

  void action(std::array<float, 2> input);

  void reset();

  ~LiveSoccerEnv();
};
