import robopy

import gym

from stable_baselines3.common.evaluation import evaluate_policy

from agent import SoccerAgent

import argparse

from env import register_envs, RoboSoccer

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s PlayerName",
        description="""
        Evaluate a model on the RoboSoccer environment.
        """
    )

    parser.add_argument('PlayerName')
    args = parser.parse_args()

    return [args.PlayerName]

if __name__ == "__main__":
    register_envs()

    player_name, = get_args()

    env = gym.make("RoboSoccer-v0")

    agent = SoccerAgent(player_name, env)
    opponent = SoccerAgent(robopy.ChaserSoccerAgent, env)
    opponent.model.player2 = True

    env.set_opponent_agent(opponent)

    reward, std = evaluate_policy(agent, env, deterministic=True, n_eval_episodes=8)
    print("mean_reward (deterministic):", reward, "+/-", std)
    reward, std = evaluate_policy(agent, env, deterministic=False, n_eval_episodes=8)
    print("mean_reward (non-deterministic):", reward, "+/-", std)
    evaluate_policy(agent, env, n_eval_episodes=1, deterministic=True, render=True)
    evaluate_policy(agent, env, n_eval_episodes=1, deterministic=False, render=True)
