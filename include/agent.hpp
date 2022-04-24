#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <array>
#include <cstdlib>
#include <memory>

struct PDDriveAgent {
  float Kp, Kd;

  float prev_d;

  PDDriveAgent(const float p = 1.0, const float d = 0.0) : Kp(p), Kd(d) {}

  std::array<float, 2> action(std::array<float, 4> player_state, std::array<float, 2> target);
};

struct SoccerAgent {
  virtual std::array<float, 2> action(std::array<float, 13> input) = 0;
};

struct DefenderSoccerAgent : SoccerAgent {
  bool player2;
  float aggression;

  DefenderSoccerAgent(bool p2 = false, float a = defender_aggression) : player2(p2), aggression(a) {}

  std::array<float, 2> action(std::array<float, 13> input);
};

struct TargetedSoccerAgent : SoccerAgent {
  bool player2;
  b2Vec2 target;

  TargetedSoccerAgent(bool p2 = false, b2Vec2 t = {0, 0}) : player2(p2), target(t) {}

  std::array<float, 2> action(std::array<float, 13> input);
};

struct ChaserSoccerAgent : SoccerAgent {
  bool player2;

  ChaserSoccerAgent(bool p2 = false) : player2(p2) {}

  std::array<float, 2> action(std::array<float, 13> input);
};

struct ShooterSoccerAgent : SoccerAgent {
  bool player2;

  ShooterSoccerAgent(bool p2 = false) : player2(p2) {}

  std::array<float, 2> action(std::array<float, 13> input);
};

enum SwitchupAgentModes { DEFENDER40, DEFENDER80, DEFENDER120, RANDOM };

struct SwitchupSoccerAgent : SoccerAgent {
  static constexpr int switch_frequency = 180;

  int switch_counter;
  bool player2;
  std::unique_ptr<SoccerAgent> active_agent;

  SwitchupSoccerAgent(bool p2 = false) : switch_counter(0), player2(p2), active_agent{nullptr} {
    new_agent();
  }

  void new_agent();

  std::array<float, 2> action(std::array<float, 13> input);
};

struct ManualSoccerAgent : SoccerAgent {
  std::shared_ptr<sf::RenderWindow> window;

  ManualSoccerAgent(const SoccerEnv& env) : window(env.window) {}

  std::array<float, 2> action(std::array<float, 13> input);
};
