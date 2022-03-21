#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>

constexpr bool render = true;

int main() {
  /*LiveSoccerEnv env;

  int last_frame = 0;
  while (env.raw_state().frames != 0)
    ;
  while (1) {
    int frame = env.raw_state().frames;
    if (frame > last_frame) {
      last_frame = frame;
      printf("F: %6d\n", last_frame);
    }
  }*/

  SoccerEnv env;
  env.init(render);

  ManualSoccerAgent player(env);
  DefenderSoccerAgent opponent(true);

  float reward = 0;

  for (int i = 0; i < 384; i++) {
    if (render && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      env.window->close();
      break;
    }

    auto player_action   = player.action(env.state());
    auto opponent_action = opponent.action(env.state());

    float hit = env.step_to_action({player_action[0], player_action[1], opponent_action[0], opponent_action[1]});
    if (fabsf(hit) > 0.1)
      env.reset();

    env.update(render);
  }

  return 0;
}
