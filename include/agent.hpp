#include "misc.hpp"

#include <Box2D/Box2D.h>
#include <array>

struct DriveAgent {
  float Kp, Kd;

  float prev_d;

  DriveAgent(const float p = 1.0, const float d = 0.0) : Kp(p), Kd(d) {}

  std::array<float, 2> action(std::array<float, 6> input);
};
