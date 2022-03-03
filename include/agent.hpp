#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <array>
#include <memory>

struct PDDriveAgent {
  float Kp, Kd;

  float prev_d;

  PDDriveAgent(const float p = 1.0, const float d = 0.0) : Kp(p), Kd(d) {}

  std::array<float, 2> action(std::array<float, 4> player_state, std::array<float, 2> target);
};

struct DefenderSoccerAgent {
  bool player2;

  DefenderSoccerAgent(bool p2 = false) : player2(p2) {}

  std::array<float, 2> action(std::array<float, 11> input);
};

struct ChaserSoccerAgent {
  bool player2;

  ChaserSoccerAgent(bool p2 = false) : player2(p2) {}

  std::array<float, 2> action(std::array<float, 11> input);
};

struct ManualSoccerAgent {
  std::shared_ptr<sf::RenderWindow> window;

  ManualSoccerAgent(const SoccerEnv& env) : window(env.window) {}

  std::array<float, 2> action(std::array<float, 11> input);
};
