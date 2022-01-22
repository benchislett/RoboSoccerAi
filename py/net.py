from stable_baselines3 import PPO, TD3

def log_path(model_name):
    return "train_logs/" + model_name

def load_model(model_name, env):
    model = PPO.load(model_name, env)
    return model

def new_model(env):
    model = PPO("MlpPolicy", env, verbose=1)
    return model
    