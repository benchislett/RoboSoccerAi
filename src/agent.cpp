#include "agent.hpp"

std::array<float, 2> DriveAgent::action(std::array<float, 6> input) {
  b2Vec2 self_pos(input[0], input[1]);
  float rot_x    = input[2];
  float rot_y    = input[3];
  float self_rot = atan2(rot_y, rot_x);
  b2Vec2 ball_pos(input[4], input[5]);

  float m1 = 0.5f;
  float m2 = 0.5f;

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

std::array<float, 2> SoccerAgent::action(std::array<float, 10> input) {
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by] = input;

  b2Vec2 our_net(0, height / length / 2.f);
  b2Vec2 ball(bx, by);

  b2Vec2 target = (our_net + ball);
  target.x /= 2.f;
  target.y /= 2.f;

  return {target.x, target.y};
}
