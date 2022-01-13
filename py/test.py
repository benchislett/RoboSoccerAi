import gym

from stable_baselines.common.evaluation import evaluate_policy
from stable_baselines.common import make_vec_env

from train import load_model

import argparse

from env import register_envs, RoboDrive, RoboSoccer

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s Env ModelName",
        description="""
        Evaluate a model on the RoboDrive or RoboSoccer environment.

        Loads ModelName, which must have been saved after training (see train.py)
        """
    )

    parser.add_argument('Env')
    parser.add_argument('ModelName')
    args = parser.parse_args()

    return [args.Env, args.ModelName]

if __name__ == "__main__":
    register_envs()

    env_name, model_name = get_args()

    env = make_vec_env(env_name + "-v0", n_envs=1)

    model = load_model(model_name, env, train=False)

    reward, std = evaluate_policy(model, env, n_eval_episodes=16)
    print("mean_reward:", reward, "+/-", std)

    evaluate_policy(model, env, n_eval_episodes=5, render=True)
