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

std::array<float, 5> BallChaseEnv::state() const {
  b2Vec2 player_pos = player.body->GetPosition();
  b2Vec2 ball_pos   = ball.body->GetPosition();

  float player_rot = player.body->GetAngle();
  while (player_rot > 2 * pi)
    player_rot -= 2 * pi;
  while (player_rot < 0)
    player_rot += 2 * pi;

  return {player_pos.x, player_pos.y, player_rot, ball_pos.x, ball_pos.y};
}

void BallChaseEnv::step() {
  world->Step(timeStep, velocityIterations, positionIterations);
}

int BallChaseEnv::act(std::array<float, 2> input) {
  player.drive(input[0], input[1]);

  if (dist() < (60.f / length)) {
    ball.teleport();
    player.reset();
    return 1;
  } else {
    return 0;
  }
}

float BallChaseEnv::dist() {
  return (player.body->GetPosition() - ball.body->GetPosition()).Length();
}
