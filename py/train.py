import robopy

import numpy as np

import gym

from torch import nn

import argparse

from stable_baselines3.common.noise import (
    NormalActionNoise,
    OrnsteinUhlenbeckActionNoise,
)
from stable_baselines3.common.env_checker import check_env
from stable_baselines3.common.vec_env import DummyVecEnv, SubprocVecEnv
from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3 import PPO, TD3

from env import register_envs, RoboDrive, RoboSoccer

def replay_buffer_name(model_name):
    return model_name + "_replay_buffer"

def log_path(model_name):
    return "train_logs/" + model_name

def load_model(model_name, env, train=True):
    model = TD3.load(model_name, env)

    if train:
        model.load_replay_buffer(replay_buffer_name(model_name))
    
    return model

def new_model(env):
    n_actions = env.action_space.shape[-1]
    action_noise = NormalActionNoise(
        mean=np.zeros(n_actions), sigma=0.1 * np.ones(n_actions)
    )

    model = TD3("MlpPolicy", env, action_noise=action_noise, verbose=1)
    return model

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s Env ModelName [Iterations] [SaveFrequency]",
        description="""
        Train a model on the RoboDrive or RoboSoccer environment.

        If ModelName has been used previously, training will be resumed.
        Otherwise, a new model will be trained and saved for future use.
        """
    )

    parser.add_argument('Env')
    parser.add_argument('ModelName')

    parser.add_argument('Iterations', nargs='?', default=50_000, type=int)
    parser.add_argument('SaveFrequency', nargs='?', default=25_000, type=int)
    args = parser.parse_args()

    return [args.Env, args.ModelName, args.Iterations, args.SaveFrequency]

if __name__ == "__main__":
    register_envs()

    env_name, model_name, iterations, save_freq = get_args()

    env = gym.make(env_name + "-v0")

    try:
        model = load_model(model_name, env)
    except FileNotFoundError:
        model = new_model(env)

    model.learn(
        total_timesteps=iterations,
        eval_env=env,
        eval_freq=save_freq,
        eval_log_path=log_path(model_name),
    )

    model.save(model_name)
    model.save_replay_buffer(replay_buffer_name(model_name))

    env.close()
