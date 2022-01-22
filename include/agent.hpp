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

  std::array<float, 2> action(std::array<float, 6> input);
};

struct DefenderSoccerAgent {
  std::array<float, 2> action(std::array<float, 10> input);
};

struct ChaserSoccerAgent {
  std::array<float, 2> action(std::array<float, 10> input);
};

struct ManualSoccerAgent {
  std::shared_ptr<sf::RenderWindow> window;

  ManualSoccerAgent(const SoccerEnv& env) : window(env.window) {}

  std::array<float, 2> action(std::array<float, 10> input);
};
