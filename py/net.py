import stable_baselines3
from stable_baselines3 import PPO, SAC, TD3
from stable_baselines3.common.noise import NormalActionNoise, OrnsteinUhlenbeckActionNoise
from torch import nn as nn
import numpy as np

def log_path(model_name):
    return "train_logs/" + model_name

def load_model(model_name, env):
    model = TD3.load(model_name, env)
    # model = PPO.load(model_name, env)
    # model = SAC.load(model_name, env)
    # model.load_replay_buffer(model_name + "_buffer")
    return model

def save_model(model, model_name):
    model.save(model_name)
    # model.save_replay_buffer(model_name + "_buffer")

def new_model(env):
    n_actions = env.action_space.shape[-1]
    action_noise = NormalActionNoise(mean=np.zeros(n_actions), sigma=0.2 * np.ones(n_actions))
    model = TD3("MlpPolicy", env, action_noise=action_noise, learning_rate=3e-4, learning_starts=1, policy_kwargs=dict(net_arch=[256, 256]), verbose=1)

    # model = SAC("MlpPolicy", env, learning_rate=3e-4,
    # policy_kwargs=dict(net_arch=[256, 256]),
    # use_sde=True, use_sde_at_warmup=True, sde_sample_freq=4,
    # verbose=1)

    # model = PPO("MlpPolicy", env, learning_rate=3e-4,
    # use_sde=True, sde_sample_freq=4,
    # policy_kwargs = dict(log_std_init=-2,
    #                      ortho_init=False,
    #                      activation_fn=nn.ReLU,
    #                      net_arch=[dict(pi=[128, 128], vf=[128, 128])]
    #                     ),
    # verbose=1)
    return model
