import robopy

import numpy as np

import gym

from torch import nn

from stable_baselines3.common.noise import (
    NormalActionNoise,
    OrnsteinUhlenbeckActionNoise,
)
from stable_baselines3.common.env_checker import check_env
from stable_baselines3.common.vec_env import DummyVecEnv, SubprocVecEnv
from stable_baselines3.common import make_vec_env
from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3 import PPO, TD3
import imitation
from imitation.algorithms import bc
from imitation.algorithms.adversarial import GAIL
from imitation.data import rollout
from imitation.util import logger, util

from env import BallChase, Soccer

MODEL_NAME = "chase_v0"
REPLAY_BUFFER_NAME = MODEL_NAME + "_replay_buffer"
TRAIN_LOG_PATH = "train_logs/" + MODEL_NAME
TRAIN_STEPS = 50_000
TRAIN_EVAL_FREQ = 25_000


def load_model(env):
    model = TD3.load(MODEL_NAME, env)
    model.load_replay_buffer(REPLAY_BUFFER_NAME)
    return model


def new_model(env):
    n_actions = env.action_space.shape[-1]
    action_noise = NormalActionNoise(
        mean=np.zeros(n_actions), sigma=0.1 * np.ones(n_actions)
    )

    model = TD3("MlpPolicy", env, action_noise=action_noise, verbose=1)
    return model


if __name__ == "__main__":
    gym.envs.register(
        id="Soccer-v0",
        entry_point="env:Soccer",
        max_episode_steps=1024,
    )

    env = gym.make("Soccer-v0")

    n_actions = env.action_space.shape[-1]
    action_noise = NormalActionNoise(
        mean=np.zeros(n_actions), sigma=0.1 * np.ones(n_actions)
    )

    try:
        model = load_model(env)
    except FileNotFoundError:
        model = new_model(env)

    model.learn(
        total_timesteps=TRAIN_STEPS,
        eval_env=env,
        eval_freq=TRAIN_EVAL_FREQ,
        eval_log_path=TRAIN_LOG_PATH,
    )

    model.save(MODEL_NAME)
    model.save_replay_buffer(REPLAY_BUFFER_NAME)

    env.close()
