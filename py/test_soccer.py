import robopy

import gym

from stable_baselines3.common.evaluation import evaluate_policy

from agent import SoccerAgent

import argparse

from env import register_envs, RoboSoccer

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s PlayerName Opponent",
        description="""
        Evaluate a model on the RoboSoccer environment.
        """
    )

    parser.add_argument('PlayerName')
    parser.add_argument('Opponent', nargs='?', default="ChaserSoccerAgent", type=str)
    args = parser.parse_args()

    return [args.PlayerName, args.Opponent]

if __name__ == "__main__":
    register_envs()

    player_name, opponent = get_args()

    env = gym.make("RoboSoccer-v0")

    agent = SoccerAgent(player_name, env)

    if opponent == "ManualSoccerAgent":
        reset = lambda env: env.set_opponent_agent(robopy.ManualSoccerAgent(env.raw_env))
        env.set_reset_hook(reset)
        opponent = SoccerAgent(robopy.ChaserSoccerAgent, env)
    else:    
        opponent = SoccerAgent(getattr(robopy, opponent), env)

    env.set_opponent_agent(opponent)

    reward, std = evaluate_policy(agent, env, deterministic=True, n_eval_episodes=8)
    print("mean_reward (deterministic):", reward, "+/-", std)
    reward, std = evaluate_policy(agent, env, deterministic=False, n_eval_episodes=8)
    print("mean_reward (non-deterministic):", reward, "+/-", std)
    evaluate_policy(agent, env, n_eval_episodes=5, deterministic=True, render=True)
    evaluate_policy(agent, env, n_eval_episodes=5, deterministic=False, render=True)
