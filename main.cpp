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

  float reward = 0;

  for (int i = 0; i < 1024; i++) {
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

    float prev_dist = env.dist();
    env.step();
    float new_dist = env.dist();


    float hit = env.action(action);

    // auto state = env.state();
    // printf("State: %f, %f, %f, %f, %f, %f\n", state[0], state[1], state[2], state[3], state[4], state[5]);

    reward += 10 * ((prev_dist - new_dist) + hit);

    env.update(render);
  }

  printf("Reward: %f\n", reward);

  return 0;
}
