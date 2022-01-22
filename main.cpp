#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>

constexpr bool render = true;

int main() {
  PDDriveAgent agent;

  SoccerEnv env([&](auto input) { return agent.action(input); });
  env.init(render);

  ChaserSoccerAgent opponent;
  ManualSoccerAgent player(env);

  float reward = 0;

  for (int i = 0; i < 1024; i++) {
    if (render && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      env.window->close();
      break;
    }

    auto action          = player.action(env.state());
    auto opponent_action = opponent.action(env.mirror_state());

    env.step();

    float hit = env.action({action[0], action[1], 1 - opponent_action[0], opponent_action[1]});

    env.update(render);

    reward += hit;
  }

  return 0;
}
