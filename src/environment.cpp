#include "environment.hpp"

#include <numbers>

b2Vec2 random_pos() {
  int pad = 2 * wall_thickness + ball_radius;
  return b2Vec2((float) randInRange(pad, width - pad) / length, (float) randInRange(pad, height - pad) / length);
}

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
  setPosition(spawn_x, spawn_y);
}

void Ball::teleport() {
  body->SetTransform(random_pos(), 0.f);
}

void Bot::setPosition(int x, int y) {
  body->SetTransform(b2Vec2(x / length, y / length), 0.f);
}

void Bot::reset() {
  setPosition(spawn_x, spawn_y);
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

void DriveEnv::debug_draw() {
  debugDraw->DrawPoint(b2Vec2(target_x, target_y), 20, b2Color(0, 5, 0));
}

void DriveEnv::reset() {
  player.reset();

  scramble();

  // target_x = width / length / 2.f;
  // target_y = height / length / 2.f;
}

void DriveEnv::scramble() {
  b2Vec2 target = random_pos();

  target_x = target.x;
  target_y = target.y;
}

std::array<float, 6> DriveEnv::state() const {
  b2Vec2 player_pos = player.body->GetPosition();

  float player_rot = player.body->GetAngle();

  return {player_pos.x, player_pos.y, cosf(player_rot), sinf(player_rot), target_x, target_y};
}

void DriveEnv::step() {
  world->Step(timeStep, velocityIterations, positionIterations);
}

float DriveEnv::action(std::array<float, 2> input) {
  player.drive(input[0], input[1]);

  float hit = 0;

  if (dist() < (50.f / length)) {
    scramble();
    hit = 1;
  }

  return hit;
}

float DriveEnv::dist() const {
  return (player.body->GetPosition() - b2Vec2(target_x, target_y)).Length();
}

void SoccerEnv::reset() {
  player1.reset();
  player2.reset();
  ball.reset();
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

  float player1_rot = pi - player1.body->GetAngle();
  float player2_rot = pi - player2.body->GetAngle();

  return {1.f - player2_pos.x, player2_pos.y,     cosf(player2_rot), sinf(player2_rot), 1.f - player1_pos.x,
          player1_pos.y,       cosf(player1_rot), sinf(player1_rot), 1.f - ball_pos.x,  ball_pos.y};
}

void SoccerEnv::step() {
  world->Step(timeStep, velocityIterations, positionIterations);
}

float SoccerEnv::action(std::array<float, 4> input) {
  player1.drive(input[0], input[1]);
  player2.drive(input[2], input[3]);

  float ball_to_net1 = (b2Vec2(0, 0.5) - ball.body->GetPosition()).Length();
  float ball_to_net2 = (b2Vec2(1, 0.5) - ball.body->GetPosition()).Length();

  int hit = 0;

  if (ball_to_net1 < (100.f / length)) {
    hit = -1;
    reset();
  } else if (ball_to_net2 < (100.f / length)) {
    hit = 1;
    reset();
  }

  return hit;
}
