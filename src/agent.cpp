#include "agent.hpp"

#include "misc.hpp"
#include "visualize.hpp"

std::array<float, 2> PDDriveAgent::action(std::array<float, 4> player_state, std::array<float, 2> target) {
  auto [px, py, rx, ry] = player_state;
  auto [tx, ty]         = target;

  float self_rot = atan2(ry, rx);

  float m1 = 0.8f;
  float m2 = 0.8f;

  float dx = tx - px;
  float dy = ty - py;

  if ((dx * dx + dy * dy) < 0.001f)
    return {0, 0};

  float angle = normalize_angle(-self_rot);

  float target_angle = atan2(-dy, dx);

  float d1 = atan2(sinf(target_angle - angle), cosf(target_angle - angle));
  float d2 = atan2(sinf((pi + target_angle) - angle), cosf((pi + target_angle) - angle));
  float d;

  if (fabsf(d1) < fabsf(d2)) {
    d = d1;
  } else {
    d = d2;
    m1 *= -1;
    m2 *= -1;
  }

  float dd = Kp * d + Kd * (d - prev_d);

  prev_d = d;

  m1 -= dd;
  m2 += dd;

  return {clamp(m1, -1, 1), clamp(m2, -1, 1)};
}

static std::array<float, 11> swap_players(std::array<float, 11> input) {
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by, side] = input;
  return {p2x, p2y, p2rx, p2ry, p1x, p1y, p1rx, p1ry, bx, by, 1 - side};
}

std::array<float, 2> DefenderSoccerAgent::action(std::array<float, 11> input) {
  if (player2)
    input = swap_players(input);
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by, side] = input;

  b2Vec2 our_net(0, height / length / 2.f);

  if (side)
    our_net.x = width / length;

  b2Vec2 ball(bx, by);

  b2Vec2 target = ((1 - defender_aggression) * our_net) + (defender_aggression * ball);

  return {target.x, target.y};
}

std::array<float, 2> ChaserSoccerAgent::action(std::array<float, 11> input) {
  if (player2)
    input = swap_players(input);
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by, side] = input;

  return {bx, by};
}

std::array<float, 2> ManualSoccerAgent::action(std::array<float, 11> input) {
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by, side] = input;

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
