#include "misc.hpp"

int randInRange(int lo, int hi) {
  static std::mt19937 gen(std::random_device{}());

  return std::uniform_int_distribution<>{lo, hi}(gen);
}

float randfInRange(float lo, float hi) {
  static std::mt19937 gen(std::random_device{}());

  return std::uniform_real_distribution<>{lo, hi}(gen);
}

int randomX() {
  return randfInRange(50, width - 50);
}

int randomY() {
  return randfInRange(50, height - 50);
}

b2Vec2 randomCoordf() {
  return b2Vec2(randomX() / length, randomY() / length);
}

float clamp(float x, float lo, float hi) {
  if (x < lo)
    return lo;
  if (x > hi)
    return hi;
  return x;
}

float normalize_angle(float angle) {
  while (angle > pi)
    angle -= 2 * pi;
  while (angle < -pi)
    angle += 2 * pi;
  return angle;
}

b2Vec2 random_pos() {
  int pad = 2 * wall_thickness + ball_radius;
  return b2Vec2((float) randInRange(pad, width - pad) / length, (float) randInRange(pad, height - pad) / length);
}
