#include "misc.hpp"

int randInRange(int lo, int hi) {
  static std::mt19937 gen(std::random_device{}());

  return std::uniform_int_distribution<>{lo, hi}(gen);
}

float randfInRange(float lo, float hi) {
  static std::mt19937 gen(std::random_device{}());

  return std::uniform_real_distribution<>{lo, hi}(gen);
}

float clamp(float x, float lo, float hi) {
  if (x < lo)
    return lo;
  if (x > hi)
    return hi;
  return x;
}
