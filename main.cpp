#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>

constexpr bool render = true;

int main() {
  DriveEnv env;
  env.init(render);

  int i = 0;

  auto open = [&]() {
    if (render)
      return env.window->isOpen();
    else
      return true;
  };

  while (open()) {
    std::array<float, 2> action = {0, 0};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      action[0] = +1;
      action[1] = +1;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      action[0] = -1;
      action[1] = -1;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      action[0] = +1;
      action[1] = -1;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      action[0] = -1;
      action[1] = +1;
    }

    if (render && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      env.window->close();
      break;
    }

    env.step();

    auto state = env.state();

    env.action(action);
    printf("State: %f, %f, %f, %f, %f, %f\n", state[0], state[1], state[2], state[3], state[4], state[5]);

    env.update(render);

    i++;
  }

  return 0;
}
