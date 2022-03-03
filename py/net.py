from stable_baselines3 import PPO, SAC, TD3
from torch import nn as nn
7	
def log_path(model_name):
    return "train_logs/" + model_name

def load_model(model_name, env):
    model = PPO.load(model_name, env)
    # model = SAC.load(model_name, env)
    return model

def new_model(env):
    # model = SAC("MlpPolicy", env, learning_rate=3e-4,
    # policy_kwargs=dict(net_arch=[128, 128]),
    # use_sde=True, use_sde_at_warmup=True, sde_sample_freq=64,
    # verbose=1)

    model = PPO("MlpPolicy", env, learning_rate=3e-4,
    use_sde=True, sde_sample_freq=4,
    gae_lambda=0.9,
    clip_range=0.4,
    policy_kwargs = dict(log_std_init=-2,
                         ortho_init=False,
                         activation_fn=nn.ReLU,
                         net_arch=[dict(pi=[256, 256], vf=[256, 256])]
                        ),
    
    verbose=1)
    return model
