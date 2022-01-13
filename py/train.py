import robopy

import numpy as np

import gym

import argparse

from stable_baselines.common.noise import NormalActionNoise
from stable_baselines.common import make_vec_env
from stable_baselines import TD3, PPO2

from env import register_envs, RoboDrive, RoboSoccer

def log_path(model_name):
    return "train_logs/" + model_name

def load_model(model_name, env, train=True):
    # model = TD3.load(model_name, env)
    model = PPO2.load(model_name, env)
    
    return model

def new_model(env):
    # n_actions = env.action_space.shape[-1]
    # action_noise = NormalActionNoise(
    #     mean=np.zeros(n_actions), sigma=0.1 * np.ones(n_actions)
    # )

    # model = TD3("MlpPolicy", env, action_noise=action_noise, verbose=1)
    model = PPO2("MlpPolicy", env, n_steps=1024, nminibatches=32, noptepochs=10, ent_coef=0.001, verbose=1)

    return model

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s Env ModelName [Iterations]",
        description="""
        Train a model on the RoboDrive or RoboSoccer environment.

        If ModelName has been used previously, training will be resumed.
        Otherwise, a new model will be trained and saved for future use.
        """
    )

    parser.add_argument('Env')
    parser.add_argument('ModelName')

    parser.add_argument('Iterations', nargs='?', default=50000, type=int)
    args = parser.parse_args()

    return [args.Env, args.ModelName, args.Iterations]

if __name__ == "__main__":
    register_envs()

    env_name, model_name, iterations = get_args()

    env = make_vec_env(env_name + "-v0", n_envs=16)

    try:
        model = load_model(model_name, env)
    except:
        model = new_model(env)

    model.learn(total_timesteps=iterations)

    model.save(model_name)

    env.close()
