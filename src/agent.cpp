#include "agent.hpp"

std::array<float, 2> BallChaseAgent::action(std::array<float, 5> input) {
  b2Vec2 self_pos(input[0], input[1]);
  float self_rot = input[2];
  b2Vec2 ball_pos(input[3], input[4]);

  float m1 = Kp;
  float m2 = Kp;

  float dx = ball_pos.x - self_pos.x;
  float dy = self_pos.y - ball_pos.y;

  float angle = -self_rot;
  while (angle > pi)
    angle -= 2 * pi;
  while (angle < -pi)
    angle += 2 * pi;

  float target_angle = atan2(dy, dx);

  float d = atan2(sinf(target_angle - angle), cosf(target_angle - angle));

  float dd = Kp * d + Kd * (d - prev_d);

  prev_d = d;

  m1 -= dd;
  m2 += dd;

  return {clamp(m1, -1, 1), clamp(m2, -1, 1)};
}