#include "environment.hpp"

#include "agent.hpp"
#include "misc.hpp"

#include <iostream>
#include <numbers>
#include <thread>

b2Vec2 left_wheel(b2Body* body) {
  b2Vec2 pos     = body->GetPosition();
  float rot      = -body->GetAngle();
  float left_rot = rot + (pi / 2.f);
  return b2Vec2(pos.x + cosf(left_rot) * bot_height_f / 2.f, pos.y - sinf(left_rot) * bot_height_f / 2.f);
}

b2Vec2 right_wheel(b2Body* body) {
  b2Vec2 pos      = body->GetPosition();
  float rot       = -body->GetAngle();
  float right_rot = rot - (pi / 2.f);
  return b2Vec2(pos.x + cosf(right_rot) * bot_height_f / 2.f, pos.y - sinf(right_rot) * bot_height_f / 2.f);
}

void Ball::setPosition(int x, int y) {
  body->SetTransform(b2Vec2(x / length, y / length), 0.f);
}

void Ball::reset() {
  float ball_r = base_radius * (1 + randfInRange(-ball_size_variance, ball_size_variance));
  ((b2CircleShape*) body->GetFixtureList()->GetShape())->m_radius = ball_r;

  int spawn_x_actual = spawn_x;
  int spawn_y_actual = spawn_y;

  if (randomize_ball_x)
    spawn_x_actual = randomX();
  if (randomize_ball_y)
    spawn_y_actual = randomY();

  setPosition(spawn_x_actual, spawn_y_actual);
  body->SetLinearVelocity(b2Vec2_zero);
}

void Ball::teleport() {
  body->SetTransform(random_pos(), 0.f);
  body->SetLinearVelocity(b2Vec2_zero);
}

void Bot::setState(int x, int y, float rot) {
  body->SetTransform(b2Vec2(x / length, y / length), rot);
}

void Bot::reset(bool flip) {
  float bot_w = base_width * (1 + randfInRange(-bot_size_variance, bot_size_variance));
  float bot_h = base_height * (1 + randfInRange(-bot_size_variance, bot_size_variance));
  ((b2PolygonShape*) body->GetFixtureList()->GetShape())->SetAsBox(bot_w, bot_h);

  int spawn_x_actual     = flip ? (width - spawn_x) : spawn_x;
  int spawn_y_actual     = spawn_y;
  float spawn_rot_actual = flip ? (pi - spawn_rot) : spawn_rot;

  if (randomize_spawn_x)
    spawn_x_actual = randomX();
  if (randomize_spawn_y)
    spawn_y_actual = randomY();
  if (randomize_spawn_rot)
    spawn_rot_actual = randfInRange(-pi, pi);

  setState(spawn_x_actual, spawn_y_actual, spawn_rot_actual);

  body->SetLinearVelocity(b2Vec2_zero);
}

void Bot::teleport() {
  body->SetTransform(random_pos(), randfInRange(-pi, pi));
  body->SetLinearVelocity(b2Vec2_zero);
}

void Bot::drive(float left, float right) {
  left  = clamp(left, -1, 1);
  right = clamp(right, -1, 1);

  left *= power_scale;
  right *= power_scale;

  float rot = body->GetAngle();

  b2Vec2 left_force(cosf(rot) * left, sinf(rot) * left);
  b2Vec2 right_force(cosf(rot) * right, sinf(rot) * right);

  body->ApplyForce(left_force, left_wheel(body), true);
  body->ApplyForce(right_force, right_wheel(body), true);
}

void SoccerEnv::debug_draw() {
  float midline = height / 2.f;

  std::array<b2Vec2, 4> net1 = {b2Vec2(wall_thickness / length, (midline + (net_height / 2)) / length),
                                b2Vec2((wall_thickness + net_width) / length, (midline + (net_height / 2)) / length),
                                b2Vec2((wall_thickness + net_width) / length, (midline - (net_height / 2)) / length),
                                b2Vec2(wall_thickness / length, (midline - (net_height / 2)) / length)};

  std::array<b2Vec2, 4> net2 = {
      b2Vec2((width - wall_thickness) / length, (midline + (net_height / 2)) / length),
      b2Vec2((width - wall_thickness - net_width) / length, (midline + (net_height / 2)) / length),
      b2Vec2((width - wall_thickness - net_width) / length, (midline - (net_height / 2)) / length),
      b2Vec2((width - wall_thickness) / length, (midline - (net_height / 2)) / length)};

  debugDraw->DrawSolidPolygon(net1.begin(), 4, b2Color(0, 1, 0, 0.2f));
  debugDraw->DrawSolidPolygon(net2.begin(), 4, b2Color(0, 1, 0, 0.2f));
}

void SoccerEnv::reset() {
  if (randomize_side)
    side = randInRange(0, 1);
  else
    side = 0;
  player1.reset(side);
  player2.reset(side);
  do {
    ball.reset();
  } while (is_goal() != 0);
}

std::array<float, 13> SoccerEnv::state() const {
  b2Vec2 player1_pos = player1.body->GetPosition();
  b2Vec2 player2_pos = player2.body->GetPosition();
  b2Vec2 ball_pos    = ball.body->GetPosition();
  b2Vec2 ball_vel    = ball.body->GetLinearVelocity();

  float player1_rot = player1.body->GetAngle();
  float player2_rot = player2.body->GetAngle();

  return {player1_pos.x, player1_pos.y,       (cosf(player1_rot)), sinf(player1_rot), player2_pos.x,
          player2_pos.y, (cosf(player2_rot)), sinf(player2_rot),   ball_pos.x,        ball_pos.y,
          ball_vel.x,    ball_vel.y,          (float) side};
}

void SoccerEnv::step() {
  world->Step(timeStep, velocityIterations, positionIterations);
  // Comment out these lines for a much faster visualization
  if (debugDraw)
    update(true);
}

int SoccerEnv::is_goal() const {
  b2Vec2 ball_pos = ball.body->GetPosition();

  float midline = height / 2.f;

  int hit = 0;

  if (ball_pos.y < (midline + (net_height / 2.f)) / length && ball_pos.y > (midline - (net_height / 2)) / length) {
    if (ball_pos.x < (wall_thickness + net_width) / length) {
      hit = -1;
    } else if (ball_pos.x > (width - wall_thickness - net_width) / length) {
      hit = 1;
    }
  }
  return hit;
}

std::array<float, 2> compute_action(std::array<float, 2> input, std::array<float, 13> st, bool player2) {
  if (manual_control)
    return input;
  else {
    input[0] = clamp(input[0], 0, width / length);
    input[1] = clamp(input[1], 0, height / length);
    if (player2) {
      return PDDriveAgent{1, 0}.action({st[4], st[5], st[6], st[7]}, input);
    } else {
      return PDDriveAgent{1, 0}.action({st[0], st[1], st[2], st[3]}, input);
    }
  }
}

float SoccerEnv::action(std::array<float, 4> input) {
  auto st = state();

  if (manual_control) {
    player1.drive(input[0], input[1]);
    player2.drive(input[2], input[3]);
  } else {
    auto p1_action = compute_action({input[0], input[1]}, st, false);
    auto p2_action = compute_action({input[2], input[3]}, st, true);

    player1.drive(p1_action[0], p1_action[1]);
    player2.drive(p2_action[0], p2_action[1]);
  }

  int hit = is_goal();

  if (hit != 0)
    reset();

  if (side) {
    hit *= -1;
  }

  return hit;
}

float SoccerEnv::step_to_action(std::array<float, 4> input) {
  int hit = 0;
  for (int i = 0; i < 10; i++) {
    auto st = state();

    if (manual_control) {
      player1.drive(input[0], input[1]);
      player2.drive(input[2], input[3]);
    } else {
      auto p1_action = compute_action({input[0], input[1]}, st, false);
      auto p2_action = compute_action({input[2], input[3]}, st, true);

      if (b2Vec2(p1_action[0], p1_action[1]).Length() < 0.1 && b2Vec2(p2_action[0], p2_action[1]).Length() < 0.1) {
        break;
      }

      player1.drive(p1_action[0], p1_action[1]);
      player2.drive(p2_action[0], p2_action[1]);
    }

    hit = is_goal();

    if (side)
      hit *= -1;

    if (hit != 0)
      break;

    step();
  }

  return hit;
}

std::array<float, 2> SoccerEnv::net_left() const {
  b2Vec2 net(0, height / length / 2.f);
  return {net.x, net.y};
}

std::array<float, 2> SoccerEnv::net_right() const {
  b2Vec2 net(width / length, height / length / 2.f);
  return {net.x, net.y};
}

float SoccerEnv::dist_player1_ball() const {
  return (player1.body->GetPosition() - ball.body->GetPosition()).Length();
}

float SoccerEnv::dist_player2_ball() const {
  return (player2.body->GetPosition() - ball.body->GetPosition()).Length();
}

float SoccerEnv::dist_players() const {
  return (player1.body->GetPosition() - player2.body->GetPosition()).Length();
}

float SoccerEnv::dist_ball_net1() const {
  return (b2Vec2(0, height / length / 2.f) - ball.body->GetPosition()).Length();
}

float SoccerEnv::dist_ball_net2() const {
  return (b2Vec2(width / length, height / length / 2.f) - ball.body->GetPosition()).Length();
}

LiveSoccerEnv::LiveSoccerEnv() {
  auto start = [&]() {
    BT_open(HEXKEY);

    int n_args = 0;
    glutInit(&n_args, NULL);

    char videoid[16] = "/dev/video1";

    if (imageCaptureStartup(videoid, 1280, 720, 1, 0)) {
      fprintf(stderr, "Couldn't start image capture, terminating...\n");
      exit(0);
    }
  };

  runner = make_unique<std::thread>(start);
}

struct AI_data LiveSoccerEnv::raw_state() const {
  return poll_ai_state();
}

std::array<float, 13> LiveSoccerEnv::state_from_raw(struct AI_data data) {
  float ball_x  = data.old_bcx;
  float ball_y  = data.old_bcy;
  float ball_vx = data.bvx;
  float ball_vy = data.bvy;

  if (data.ball != NULL) {
    ball_x = data.ball->cx;
    ball_y = data.ball->cy;
  }

  float self_x  = data.old_scx;
  float self_y  = data.old_scy;
  float self_rx = data.sdx;
  float self_ry = data.sdy;

  if (data.self != NULL) {
    self_x = data.self->cx;
    self_y = data.self->cy;
  }

  float opp_x  = data.old_ocx;
  float opp_y  = data.old_ocy;
  float opp_rx = data.odx;
  float opp_ry = data.ody;

  if (data.opp != NULL) {
    opp_x = data.opp->cx;
    opp_y = data.opp->cy;
  }

  int side = data.side;

  float wx = 1280;
  float wy = 720;

  return {self_x / wx, self_y / wy, self_rx,     self_ry, opp_x / wx, opp_y / wy,  opp_rx,
          opp_ry,      ball_x / wx, ball_y / wy, ball_vx, ball_vy,    (float) side};
}

std::array<float, 13> LiveSoccerEnv::state() {
  struct AI_data latest_record = raw_state();

  return state_from_raw(latest_record);
}

void LiveSoccerEnv::action(std::array<float, 2> input) {
  auto action = compute_action({input[0], input[1]}, state(), false);
  // std::cout << "DRIVING: " << action[0] << ", " << action[1] << std::endl;
  if (fabs(action[0]) < 0.1 && fabs(action[1]) < 0.1) {
    BT_all_stop(0);
  } else {
    BT_motor_port_speed(MOTOR_A, (char) (action[0] * 29.9));
    BT_motor_port_speed(MOTOR_D, (char) (action[1] * 29.9));
  }
  return;
}

void LiveSoccerEnv::reset() {}

LiveSoccerEnv::~LiveSoccerEnv() {
  runner->join();
}
