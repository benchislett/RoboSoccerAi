import gym

from stable_baselines3.common.evaluation import evaluate_policy

from agent import DriveAgent

import argparse

from env import register_envs, RoboDrive

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s ModelName",
        description="""
        Evaluate a model on the RoboDrive environment.
        """
    )

    parser.add_argument('ModelName')
    args = parser.parse_args()

    return [args.ModelName]

if __name__ == "__main__":
    register_envs()

    model_name, = get_args()

    env = gym.make("RoboDrive-v0")

    agent = DriveAgent(model_name, env)

    reward, std = evaluate_policy(agent, env, n_eval_episodes=16)
    print("mean_reward:", reward, "+/-", std)

    evaluate_policy(agent, env, n_eval_episodes=5, render=True)
