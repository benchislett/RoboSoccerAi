import stable_baselines3
from stable_baselines3 import PPO, SAC, TD3
from torch import nn as nn
7	
def log_path(model_name):
    return "train_logs/" + model_name

def load_model(model_name, env):
    model = PPO.load(model_name, env)
    # model = SAC.load(model_name, env)
    # model.load_replay_buffer(model_name + "_buffer")
    return model

def save_model(model, model_name):
    model.save(model_name)
    # model.save_replay_buffer(model_name + "_buffer")

def new_model(env):
    # model = SAC("MlpPolicy", env, learning_rate=3e-4,
    # policy_kwargs=dict(net_arch=[256, 256]),
    # use_sde=True, use_sde_at_warmup=True, sde_sample_freq=4,
    # verbose=1)

    model = PPO("MlpPolicy", env, learning_rate=1e-4,
    use_sde=True, sde_sample_freq=64,
    ent_coef=0.001,
    clip_range=0.2,
    policy_kwargs = dict(log_std_init=-2,
                         ortho_init=False,
                         activation_fn=nn.ReLU,
                         net_arch=[dict(pi=[256, 256], vf=[256, 256])]
                        ),
    
    verbose=1)
    return model
