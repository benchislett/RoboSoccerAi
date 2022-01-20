#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>

constexpr bool render = true;

int main() {
  DriveAgent agent;

  SoccerAgent opponent;

  SoccerEnv env([&](auto input) { return agent.action(input); });
  env.init(render);

  float reward = 0;

  for (int i = 0; i < 1024; i++) {
    std::array<float, 2> action;

    auto [mx, my] = sf::Mouse::getPosition(*env.window);
    float mxf     = clamp(mx, 0, width) / length;
    float myf     = clamp(my, 0, height) / length;

    action = {mxf, myf};

    if (render && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      env.window->close();
      break;
    }

    auto opponent_action = opponent.action(env.mirror_state());

    env.step();

    float hit = env.action({action[0], action[1], 1 - opponent_action[0], opponent_action[1]});

    env.update(render);

    reward += hit;
  }

  return 0;
}
