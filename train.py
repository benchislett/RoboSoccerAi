import robopy

import numpy as np

import gym

from torch import nn

from stable_baselines3.common.noise import (
    NormalActionNoise,
    OrnsteinUhlenbeckActionNoise,
)
from stable_baselines3.common.env_checker import check_env
from stable_baselines3 import PPO, TD3
import imitation
from imitation.algorithms import bc
from imitation.algorithms.adversarial import GAIL
from imitation.data import rollout
from imitation.util import logger, util

gym.envs.register(
    id="BallChase-v0",
    entry_point="env:BallChase",
    max_episode_steps=512,
)

env = gym.make("BallChase-v0")
eval_env = gym.make("BallChase-v0")

# model = PPO(
#     "MlpPolicy",
#     env,
#     n_steps=512,
#     batch_size=64,
#     gamma=0.99,
#     gae_lambda=0.95,
#     n_epochs=10,
#     ent_coef=0.001,
#     learning_rate=1e-3,
#     clip_range=0.2,
#     use_sde=True,
#     policy_kwargs={
#         "log_std_init": -2,
#         "ortho_init": False,
#         "activation_fn": nn.ReLU,
#         "net_arch": [{"pi": [256, 256], "vf": [256, 256]}],
#     },
#     verbose=0,
# )

n_actions = env.action_space.shape[-1]
action_noise = NormalActionNoise(
    mean=np.zeros(n_actions), sigma=0.1 * np.ones(n_actions)
)

model = TD3("MlpPolicy", env, action_noise=action_noise, verbose=1)

model.load("chase_model", env)
model.load_replay_buffer("chase_model_replay_buffer")

eval_env.reset()

model.learn(
    total_timesteps=100_000,
    eval_env=eval_env,
    eval_freq=600,
    eval_log_path="tmp/td3_evals_extra",
)

model.save("chase_model")
model.save_replay_buffer("chase_model_replay_buffer")

obs = eval_env.reset()
while True:
    action, _states = model.predict(obs, deterministic=True)
    obs, reward, done, info = eval_env.step(action)
    eval_env.render()
    if done:
        obs = eval_env.reset()

env.close()
eval_env.close()
