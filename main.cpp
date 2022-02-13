#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>

constexpr bool render = true;

int main() {
  // LiveSoccerEnv env;

  // int last_frame = 0;
  // while (!env.raw_state())
  //   ;
  // while (1) {
  //   int frame = env.raw_state()->frames;
  //   if (frame > last_frame) {
  //     last_frame = frame;
  //     printf("F: %6d\n", last_frame);
  //   }
  // }

  PDDriveAgent controller;

  SoccerEnv env;
  env.init(render);

  ChaserSoccerAgent opponent;
  ManualSoccerAgent player(env);

  float reward = 0;

  while (1) {
    if (render && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      env.window->close();
      break;
    }

    std::array<float, 10> state  = env.state();
    std::array<float, 4> state4  = {state[0], state[1], state[2], state[3]};
    std::array<float, 4> mstate4 = {state[4], state[5], state[6], state[7]};

    auto flip_target = [](std::array<float, 2> action) { return std::array<float, 2>({1 - action[0], action[1]}); };

    auto player_action   = controller.action(state4, player.action(env.state()));
    auto opponent_action = controller.action(mstate4, flip_target(opponent.action(env.mirror_state())));

    env.step();

    float hit = env.action({player_action[0], player_action[1], opponent_action[0], opponent_action[1]});

    env.update(render);

    reward += hit;
  }

  return 0;
}
