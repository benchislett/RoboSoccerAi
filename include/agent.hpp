#include "box2d/box2d.h"
#include "misc.hpp"

#include <array>

struct DriveAgent {
  float Kp, Kd;

  float prev_d;

  DriveAgent(const float p = 1.0, const float d = 0.0) : Kp(p), Kd(d) {}

  std::array<float, 2> action(std::array<float, 6> input);
};

struct SoccerAgent {
  std::array<float, 2> action(std::array<float, 10> input);
};
