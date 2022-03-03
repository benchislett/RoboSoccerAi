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

  SoccerEnv env;
  env.init(render);

  ManualSoccerAgent player(env);
  ChaserSoccerAgent opponent(true);

  float reward = 0;

  for (int i = 0; i < 384000; i++) {
    if (render && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      env.window->close();
      break;
    }

    auto player_action   = player.action(env.state());
    auto opponent_action = opponent.action(env.state());

    env.step();

    float hit = env.action({player_action[0], player_action[1], opponent_action[0], opponent_action[1]});

    env.update(render);

    reward += hit;
  }

  return 0;
}
