#include "Models.h"
#include "ProximalPolicyOptimization.h"
#include "agent.hpp"
#include "box2d/box2d.h"
#include "environment.hpp"
#include "misc.hpp"
#include "visualize.hpp"

#include <cstdio>
#include <vector>

template <long unsigned int N>
torch::Tensor array_to_tensor(std::array<float, N> T) {
  torch::Tensor out = torch::zeros({N});

  int i  = 0;
  auto a = out.accessor<float, 1>();
  for (auto c : T) {
    a[i++] = c;
  }

  return out;
}

torch::Tensor float_to_tensor(float x) {
  torch::Tensor out = torch::zeros({1});

  out.accessor<float, 1>()[0] = x;
  return out;
}

using VT = std::vector<torch::Tensor>;

int main() {
  BallChaseEnv env;

  // Model parameters
  uint n_in  = 5;
  uint n_out = 2;
  double std = 1e-2;

  ActorCritic model(n_in, n_out, std);
  model->to(torch::kF32);
  model->normal(0., std);
  torch::optim::Adam opt(model->parameters(), 3e-3);

  // Training time!
  uint n_iter          = 10250;
  uint n_steps         = 2048;
  uint n_epochs        = 10000;
  uint mini_batch_size = 512;
  uint ppo_epochs      = 4;
  double beta          = 1e-3;

  VT states;
  VT actions;
  VT rewards;
  VT dones;

  VT log_probs;
  VT returns;
  VT values;

  float best_avg_reward = -FLT_MAX;
  float avg_reward      = 0;

  uint c = 0;

  torch::load(model, "latest_model.pt");

  for (uint e = 1; e <= n_epochs; e++) {
    for (uint i = 0; i < n_iter; i++) {
      auto state_tensor = array_to_tensor(env.state());

      states.push_back(state_tensor);

      auto av     = model->forward(state_tensor);
      auto action = std::get<0>(av);
      auto value  = std::get<1>(av);

      actions.push_back(action);
      values.push_back(value);
      log_probs.push_back(model->log_prob(action));

      std::array<float, 2> input;
      input[0] = action[0].item<float>();
      input[1] = action[1].item<float>();

      int hits = env.act(input);

      float prev_dist = env.dist();

      env.step();

      float err    = prev_dist - env.dist();
      float reward = err + (hits * 10);

      avg_reward += reward;

      rewards.push_back(float_to_tensor(reward));
      dones.push_back(float_to_tensor((float) hits));

      c++;

      if (c % n_steps == 0) {
        values.push_back(std::get<1>(model->forward(states[states.size() - 2])));

        returns = PPO::returns(rewards, dones, values, .99, .95);

        torch::Tensor t_log_probs  = torch::stack(log_probs).detach();
        torch::Tensor t_returns    = torch::stack(returns).detach();
        torch::Tensor t_values     = torch::stack(values).detach();
        torch::Tensor t_states     = torch::stack(states);
        torch::Tensor t_actions    = torch::stack(actions);
        torch::Tensor t_advantages = t_returns - t_values.slice(0, 0, n_steps);

        PPO::update(model, t_states, t_actions, t_log_probs, t_returns, t_advantages, opt, n_steps, ppo_epochs,
                    mini_batch_size, beta);

        c = 0;

        states.clear();
        actions.clear();
        rewards.clear();
        dones.clear();

        log_probs.clear();
        returns.clear();
        values.clear();
      }
    }

    printf("Epoch [%3d], Reward: %f\n", e, avg_reward);
    if (avg_reward > best_avg_reward) {
      best_avg_reward = avg_reward;
      torch::save(model, "best_model.pt");
    }

    if (e % 100 == 0) {
      torch::save(model, "model_" + std::to_string(e) + ".pt");
      torch::save(model, "latest_model.pt");
    }

    avg_reward = 0;

    env.reset();
  }

  env.reset();

  sf::RenderWindow window(sf::VideoMode(width, height), "RoboAI Window", sf::Style::Titlebar);

  SfDebugDraw debugDraw(&window);
  env.world->SetDebugDraw(&debugDraw);
  debugDraw.SetFlags(0x00ff);

  window.setFramerateLimit(fps);

  while (window.isOpen()) {
    window.clear(sf::Color(0, 0, 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
      env.player.drive(0.8, 0.8);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
      env.player.drive(-0.8, -0.8);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
      env.player.drive(-0.5, 0.5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
      env.player.drive(0.5, -0.5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
      env.player.body->SetLinearVelocity(b2Vec2(0, 0));
      env.player.body->SetAngularVelocity(0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      window.close();
      break;
    }

    auto state_tensor = array_to_tensor(env.state());

    states.push_back(state_tensor);

    auto av     = model->forward(state_tensor);
    auto action = std::get<0>(av);

    std::array<float, 2> input;
    input[0] = action[0].item<float>();
    input[1] = action[1].item<float>();

    env.act(input);
    env.step();

    env.world->DebugDraw();
    window.display();
  }

  return 0;
}
