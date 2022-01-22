#include "agent.hpp"

#include "misc.hpp"
#include "visualize.hpp"

std::array<float, 2> PDDriveAgent::action(std::array<float, 6> input) {
  auto [px, py, rx, ry, tx, ty] = input;

  float self_rot = atan2(ry, rx);

  float m1 = 0.5f;
  float m2 = 0.5f;

  float dx = tx - px;
  float dy = ty - py;

  float angle = normalize_angle(-self_rot);

  float target_angle = atan2(-dy, dx);

  float d = atan2(sinf(target_angle - angle), cosf(target_angle - angle));

  float dd = Kp * d + Kd * (d - prev_d);

  prev_d = d;

  m1 -= dd;
  m2 += dd;

  return {clamp(m1, -1, 1), clamp(m2, -1, 1)};
}

std::array<float, 2> DefenderSoccerAgent::action(std::array<float, 10> input) {
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by] = input;

  b2Vec2 our_net(0, height / length / 2.f);
  b2Vec2 ball(bx, by);

  b2Vec2 target = (our_net + ball);
  target.x /= 2.f;
  target.y /= 2.f;

  return {target.x, target.y};
}

std::array<float, 2> ChaserSoccerAgent::action(std::array<float, 10> input) {
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by] = input;

  return {bx, by};
}

std::array<float, 2> ManualSoccerAgent::action(std::array<float, 10> input) {
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by] = input;

  if (window) {
    auto [mx, my] = sf::Mouse::getPosition(*window);
    float mxf     = clamp(mx, 0, width) / length;
    float myf     = clamp(my, 0, height) / length;

    return {mxf, myf};
  } else {
    // window not open, ballchase instead
    return {bx, by};
  }
}
