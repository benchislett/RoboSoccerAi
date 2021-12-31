#include "environment.hpp"

#include <numbers>

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
  int pad = 2 * wall_thickness + ball_radius;
  setPosition(randInRange(pad, width - pad), randInRange(pad, height - pad));
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

void BallChaseEnv::reset() {
  player.reset();
  ball.teleport();
}

std::array<float, 6> BallChaseEnv::state() const {
  b2Vec2 player_pos = player.body->GetPosition();
  b2Vec2 ball_pos   = ball.body->GetPosition();

  float player_rot = player.body->GetAngle();

  return {player_pos.x, player_pos.y, cosf(player_rot), sinf(player_rot), ball_pos.x, ball_pos.y};
}

void BallChaseEnv::step() {
  world->Step(timeStep, velocityIterations, positionIterations);
}

float BallChaseEnv::action(std::array<float, 2> input) {
  player.drive(input[0], input[1]);

  float reward = -dist();
  float hit    = 0;

  if (dist() < (60.f / length)) {
    ball.teleport();
    hit = 1;
  }

  return (reward + (hit * 1000.f));
}

float BallChaseEnv::dist() const {
  return (player.body->GetPosition() - ball.body->GetPosition()).Length();
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

  float player1_to_ball = (player1.body->GetPosition() - ball.body->GetPosition()).Length();
  float ball_to_net1    = (b2Vec2(0, 0.5) - ball.body->GetPosition()).Length();
  float ball_to_net2    = (b2Vec2(1, 0.5) - ball.body->GetPosition()).Length();

  float reward = -player1_to_ball - ball_to_net2 + ball_to_net1;

  if (ball_to_net1 < (100.f / length)) {
    reward -= 1000;
    reset();
  } else if (ball_to_net2 < (100.f / length)) {
    reward += 1000;
    reset();
  }

  return reward;
}
