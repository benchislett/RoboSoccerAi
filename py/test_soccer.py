import gym

from stable_baselines3.common.evaluation import evaluate_policy

from agent import DriveAgent, SoccerAgent

import argparse

from env import register_envs, RoboSoccer

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s PlayerName OpponentName",
        description="""
        Evaluate a pair of models on the RoboSoccer environment.
        """
    )

    parser.add_argument('PlayerName')
    parser.add_argument('OpponentName')
    args = parser.parse_args()

    return [args.PlayerName, args.OpponentName]

if __name__ == "__main__":
    register_envs()

    player_name, opponent_name = get_args()
    if opponent_name == "ManualSoccerAgent":
        raise ValueError("Opponent may not be controlled manually!")

    env = gym.make("RoboSoccer-v0")
    env.init()

    agent = SoccerAgent(player_name, env)
    opponent = SoccerAgent(opponent_name, env)

    env.set_opponent_agent(opponent)
    env.set_driver_agent(DriveAgent("ppo_drive_1", None))

    if player_name != "ManualSoccerAgent":
        reward, std = evaluate_policy(agent, env, n_eval_episodes=16)
        print("mean_reward:", reward, "+/-", std)

    evaluate_policy(agent, env, n_eval_episodes=5, render=True)
