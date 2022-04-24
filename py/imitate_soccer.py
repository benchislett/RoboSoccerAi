import robopy

import argparse

from random import randint

from net import save_model, new_model

from agent import SoccerAgent

from env import register_envs, RoboSoccer

import gym
from stable_baselines3 import PPO
from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3.common.vec_env import DummyVecEnv
from stable_baselines3.ppo import MlpPolicy

import imitation
from imitation.algorithms import bc
from imitation.algorithms.adversarial import AIRL
from imitation.rewards.reward_nets import BasicRewardNet
from imitation.data import rollout
from imitation.data.rollout import make_sample_until, flatten_trajectories
from imitation.data.wrappers import RolloutInfoWrapper

register_envs()

env = gym.make("RoboSoccer-v0")
opponent = SoccerAgent(robopy.DefenderSoccerAgent, env)
env.set_opponent_agent(opponent)

expert = SoccerAgent(robopy.ChaserSoccerAgent, env)

print("Sampling expert transitions.")
rollouts = rollout.generate_trajectories(expert, DummyVecEnv([lambda: RolloutInfoWrapper(env)]), make_sample_until(n_timesteps=None, n_episodes=10000))

imitation.util.logger.configure("train_logs/")

venv = DummyVecEnv([lambda: env])
learner = new_model(venv)
trainer = AIRL(
    expert_data=flatten_trajectories(rollouts),
    expert_batch_size=1024,
    venv=venv,
    gen_algo=learner,
)
# trainer = GAIL(
#     venv,
#     flatten_trajectories(rollouts),
#     64,
#     gen_algo=learner,
# )

trainer.train(1_000_000)
save_model(learner, "imitate/model_0")
