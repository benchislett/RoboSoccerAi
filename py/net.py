import stable_baselines3
from stable_baselines3 import PPO, SAC, TD3, HerReplayBuffer
from stable_baselines3.common.noise import NormalActionNoise, OrnsteinUhlenbeckActionNoise
from torch import nn as nn
import numpy as np

def log_path(model_name):
    return "train_logs/" + model_name

def load_model(model_name, env):
    model = PPO.load(model_name, env)
    return model

def save_model(model, model_name):
    model.save(model_name)

def new_model(env):
    model = PPO("MlpPolicy", env, learning_rate=3e-4,
    use_sde=True, sde_sample_freq=64,
    policy_kwargs = dict(log_std_init=-2,
                         ortho_init=False,
                         activation_fn=nn.ReLU,
                         net_arch=[dict(pi=[128, 128], vf=[128, 128])]
                        ),
    verbose=1)
    return model
