#pragma once

#include "box2d/box2d.h"

#include <cmath>
#include <numbers>
#include <random>

using std::numbers::pi;

// physics parameters
constexpr int fps        = 30;
constexpr float timeStep = 1.f / (float) fps;

constexpr int velocityIterations = 6;
constexpr int positionIterations = 2;

constexpr float ball_damping_linear  = 0.3f;
constexpr float ball_damping_angular = 2.f;
constexpr float bot_damping_linear   = 4.f;
constexpr float bot_damping_angular  = 5.f;

constexpr float ball_density = 1.f;
constexpr float bot_density  = 10.f;

constexpr float ball_elasticity = 0.3f; // default
// constexpr float ball_elasticity = 1.f; // bouncy ball mode

constexpr float defender_aggression = 0.8f; // 80% towards ball

constexpr bool randomize_spawn_y   = true;
constexpr float ball_size_variance = 0.2f; // 80% - 120%
constexpr float bot_size_variance  = 0.2f; // 80% - 120%
// constexpr float ball_size_variance = 0.8f; // 20% - 180%
// constexpr float bot_size_variance  = 0.8f; // 20% - 180%

constexpr float power_scale = 1.f / 85.f;

// environment parameters
constexpr int agent_fps = 1;

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
constexpr int net_width      = 3 * 40;
constexpr int net_height     = 1.5 * 175;

// dimension in box2d
constexpr float bot_width_f   = bot_width / length;
constexpr float bot_height_f  = bot_height / length;
constexpr float ball_radius_f = ball_radius / length;

int randInRange(int lo, int hi);

float randfInRange(float lo, float hi);

float clamp(float x, float lo, float hi);

float normalize_angle(float angle);

b2Vec2 random_pos();
