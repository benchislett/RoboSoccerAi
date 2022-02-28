import robopy

import argparse

from random import randint

from stable_baselines3.common.env_util import make_vec_env

from agent import SoccerAgent

from env import register_envs, RoboSoccer

EPOCH_SIZE = 500_000

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s [Epochs] [ResumeEpoch]",
        description="""
        Train a model on the RoboSoccer environment.
        """
    )

    parser.add_argument('Epochs', nargs='?', default=9999999, type=int)
    parser.add_argument('ResumeEpoch', nargs='?', default=0, type=int)
    args = parser.parse_args()

    return [args.Epochs, args.ResumeEpoch]

if __name__ == "__main__":
    register_envs()

    epochs, resume_epoch = get_args()

    env = make_vec_env("RoboSoccer-v0", n_envs=8)
    
    opponent = SoccerAgent(robopy.DefenderSoccerAgent, env)
    opponent.model.player2 = True
    env.env_method("set_opponent_agent", opponent)

    agent = SoccerAgent(f"models/model_{resume_epoch}", env)

    for i in range(resume_epoch, resume_epoch + epochs):

        print("\n-------------------------------------")
        print(f"Epoch: {i}")
        print("-------------------------------------\n")
        
        agent.model.learn(total_timesteps=EPOCH_SIZE)

        agent.model.save(f"models/model_{i}")

    env.close()
