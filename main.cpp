#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>

constexpr bool render = true;

int main() {
  SoccerEnv env;
  env.init(render);

  float reward = 0;

  for (int i = 0; i < 1024; i++) {
    std::array<float, 4> action = {0, 0, 1, 0};

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

    float hit = env.action(action);

    env.update(render);
  }

  return 0;
}
