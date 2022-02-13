#include "environment.hpp"

#include "misc.hpp"

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
  setPosition(spawn_x, spawn_y);
  body->SetLinearVelocity(b2Vec2_zero);
}

void Ball::teleport() {
  body->SetTransform(random_pos(), 0.f);
  body->SetLinearVelocity(b2Vec2_zero);
}

void Bot::setState(int x, int y, float rot) {
  body->SetTransform(b2Vec2(x / length, y / length), rot);
}

void Bot::reset() {
  float bot_w = base_width * (1 + randfInRange(-bot_size_variance, bot_size_variance));
  float bot_h = base_height * (1 + randfInRange(-bot_size_variance, bot_size_variance));
  ((b2PolygonShape*) body->GetFixtureList()->GetShape())->SetAsBox(bot_w, bot_h);
  setState(spawn_x, spawn_y, spawn_rot);
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
  player1.reset();
  player2.reset();
  ball.reset();
  history.clear();
}

std::array<float, 10> SoccerEnv::state() const {
  b2Vec2 player1_pos = player1.body->GetPosition();
  b2Vec2 player2_pos = player2.body->GetPosition();
  b2Vec2 ball_pos    = ball.body->GetPosition();

  float player1_rot = player1.body->GetAngle();
  float player2_rot = player2.body->GetAngle();

  return {player1_pos.x, player1_pos.y,     cosf(player1_rot), sinf(player1_rot), player2_pos.x,
          player2_pos.y, cosf(player2_rot), sinf(player2_rot), ball_pos.x,        ball_pos.y};
}

std::array<float, 10> SoccerEnv::mirror_state() const {
  b2Vec2 player1_pos = player1.body->GetPosition();
  b2Vec2 player2_pos = player2.body->GetPosition();
  b2Vec2 ball_pos    = ball.body->GetPosition();

  float player1_rot = player1.body->GetAngle();
  float player2_rot = player2.body->GetAngle();

  return {1.f - player2_pos.x, player2_pos.y,      -cosf(player2_rot), sinf(player2_rot), 1.f - player1_pos.x,
          player1_pos.y,       -cosf(player1_rot), sinf(player1_rot),  1.f - ball_pos.x,  ball_pos.y};
}

void SoccerEnv::step() {
  world->Step(timeStep, velocityIterations, positionIterations);
}

float SoccerEnv::action(std::array<float, 4> input) {
  auto current_state = state();

  for (int i = 0; i < 4; i++)
    input[i] = clamp(input[i], -1, 1);

  player1.drive(input[0], input[1]);
  player2.drive(input[2], input[3]);

  b2Vec2 ball_pos = ball.body->GetPosition();

  float midline = height / 2.f;

  int hit = 0;

  if (ball_pos.y < (midline + (net_height / 2.f)) / length && ball_pos.y > (midline - (net_height / 2)) / length) {
    if (ball_pos.x < (wall_thickness + net_width) / length) {
      hit = -1;
      reset();
    } else if (ball_pos.x > (width - wall_thickness - net_width) / length) {
      hit = 1;
      reset();
    }
  }

  return hit;
}

float SoccerEnv::dist_player1_ball() const {
  return (player1.body->GetPosition() - ball.body->GetPosition()).Length();
}

float SoccerEnv::dist_player2_ball() const {
  return (player2.body->GetPosition() - ball.body->GetPosition()).Length();
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

    char videoid[16] = "/dev/video2";

    if (imageCaptureStartup(videoid, 1280, 720, 0, 0)) {
      fprintf(stderr, "Couldn't start image capture, terminating...\n");
      exit(0);
    }
  };

  runner = make_unique<std::thread>(start);
}

struct AI_data* LiveSoccerEnv::raw_state() const {
  return poll_ai_state();
}

std::array<float, 10> LiveSoccerEnv::state_from_raw(struct AI_data data) {
  float ball_x = data.old_bcx;
  float ball_y = data.old_bcy;

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

  return {self_x, self_y, self_rx, self_ry, opp_x, opp_y, opp_rx, opp_ry, ball_x, ball_y};
}

std::array<float, 10> LiveSoccerEnv::state() {
  struct AI_data latest_record = *raw_state();

  int frame_id = latest_record.frames;

  if (history.size() > 0 && history.back().frames == frame_id)
    return state_from_raw(history.back());

  history.push_back(latest_record);

  if (history.size() > 1024) {
    std::shift_left(history.begin(), history.end(), 1);
    history.pop_back();
  }

  return state_from_raw(latest_record);
}

std::array<float, 100> LiveSoccerEnv::state10() {
  std::array<float, 100> stacked_state;

  auto st = state();
  for (int idx = 0; idx < 10; idx++) {
    int row = history.size() - 1 - idx;
    if (row >= 0) {
      st = state_from_raw(history[row]);
    }
    for (int i = 0; i < 10; i++) {
      stacked_state[idx * 10 + i] = st[i];
    }
  }

  return stacked_state;
}

void LiveSoccerEnv::action(std::array<float, 2> input) {
  BT_motor_port_speed(MOTOR_A, (char) (input[0] * 99.9));
  BT_motor_port_speed(MOTOR_D, (char) (input[1] * 99.9));
  return;
}

void LiveSoccerEnv::reset() {
  history.clear();
}

LiveSoccerEnv::~LiveSoccerEnv() {
  runner->join();
}
