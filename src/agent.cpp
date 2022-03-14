#include "agent.hpp"

#include "misc.hpp"
#include "visualize.hpp"

std::array<float, 2> PDDriveAgent::action(std::array<float, 4> player_state, std::array<float, 2> target) {
  auto [px, py, rx, ry] = player_state;
  auto [tx, ty]         = target;

  float self_rot = atan2(ry, rx);

  float m1 = 0.5f;
  float m2 = 0.5f;

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

  b2Vec2 net(0, height / length / 2.f);

  if (side)
    net.x = width / length;

  b2Vec2 ball(bx, by);

  b2Vec2 target = net + aggression * (ball - net);

  if (manual_control)
    return PDDriveAgent{0, 1}.action({p1x, p1y, p1rx, p1ry}, {target.x, target.y});
  return {target.x, target.y};
}

std::array<float, 2> TargetedSoccerAgent::action(std::array<float, 11> input) {
  if (player2)
    input = swap_players(input);
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by, side] = input;

  if (manual_control)
    return PDDriveAgent{0, 1}.action({p1x, p1y, p1rx, p1ry}, {target.x, target.y});
  return {target.x, target.y};
}

std::array<float, 2> ChaserSoccerAgent::action(std::array<float, 11> input) {
  if (player2)
    input = swap_players(input);
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by, side] = input;

  float dx = p1x - p2x;
  float dy = p1y - p2y;
  float d  = sqrtf(dx * dx + dy * dy);

  b2Vec2 our_net(0, height / length / 2.f);

  if (side)
    our_net.x = width / length;

  if (d <= 0.1)
    return {our_net.x, our_net.y};

  if (manual_control)
    return PDDriveAgent{0, 1}.action({p1x, p1y, p1rx, p1ry}, {bx, by});
  return {bx, by};
}

void SwitchupSoccerAgent::new_agent() {
  switch_counter = 0;
  int new_mode   = randInRange(0, 3);

  constexpr int pad = 300;
  float wlo         = pad / length;
  float whi         = (width - pad) / length;
  float hlo         = pad / length;
  float hhi         = (height - pad) / length;

  active_agent.release();
  if (new_mode == DEFENDER40) {
    active_agent = std::make_unique<DefenderSoccerAgent>(player2, 0.4f);
  } else if (new_mode == DEFENDER80) {
    active_agent = std::make_unique<DefenderSoccerAgent>(player2, 0.8f);
  } else if (new_mode == DEFENDER120) {
    active_agent = std::make_unique<DefenderSoccerAgent>(player2, 1.2f);
  } else if (new_mode == RANDOM) {
    active_agent =
        std::make_unique<TargetedSoccerAgent>(player2, b2Vec2(randfInRange(wlo, whi), randfInRange(hlo, hhi)));
  } else {
    return;
  }
}

std::array<float, 2> SwitchupSoccerAgent::action(std::array<float, 11> input) {
  switch_counter++;
  if (switch_counter > switch_frequency)
    new_agent();
  return active_agent->action(input);
}

std::array<float, 2> ManualSoccerAgent::action(std::array<float, 11> input) {
  auto [p1x, p1y, p1rx, p1ry, p2x, p2y, p2rx, p2ry, bx, by, side] = input;

  std::array<float, 2> target = {bx, by};

  if (window) {
    auto [mx, my] = sf::Mouse::getPosition(*window);
    float mxf     = clamp(mx, 0, width) / length;
    float myf     = clamp(my, 0, height) / length;
    target[0]     = mxf;
    target[1]     = myf;
  }

  if (manual_control)
    return PDDriveAgent{0, 1}.action({p1x, p1y, p1rx, p1ry}, target);
  return target;
}
