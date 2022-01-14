#pragma once

#include <cmath>
#include <numbers>
#include <random>

using std::numbers::pi;

// physics parameters
constexpr int fps        = 12;
constexpr float timeStep = 1.f / (float) fps;

constexpr int velocityIterations = 6;
constexpr int positionIterations = 2;

constexpr float power_scale = 1.f / 100.f;

// dimensions in pixels
constexpr int width  = 1024;
constexpr int height = 768;

// pixels per box2d meter
constexpr float length = 1024;

// dimension in pixels
constexpr int bot_width      = 70;
constexpr int bot_height     = 30;
constexpr int ball_radius    = 10;
constexpr int wall_thickness = 5;

// dimension in box2d
constexpr float bot_width_f  = bot_width / length;
constexpr float bot_height_f = bot_height / length;

int randInRange(int lo, int hi);

float randfInRange(float lo, float hi);

float clamp(float x, float lo, float hi);
