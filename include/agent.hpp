#include "box2d/box2d.h"
#include "misc.hpp"

#include <array>

struct BallChaseAgent {
  BallChaseAgent() {}

  std::array<float, 2> action(std::array<float, 5> input);
};
