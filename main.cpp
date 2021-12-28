#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>

constexpr bool render = true;

int main() {
  BallChaseEnv env;
  env.init(render);

  BallChaseAgent agent(1.0, 0.5);

  int i = 0;

  auto open = [&]() {
    if (render)
      return env.window->isOpen();
    else
      return true;
  };

  while (open()) {
    if (render && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      env.window->close();
      break;
    }

    env.step();

    float reward = env.action(agent.action(env.state()));
    printf("Reward: %f\n", reward);

    env.update(render);

    i++;
  }

  return 0;
}
