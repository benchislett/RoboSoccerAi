import robopy

import argparse

from random import randint
from os import listdir

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
    parser.add_argument('RootDir', nargs='?', default="model_pool", type=str)
    args = parser.parse_args()

    return [args.Epochs, args.ResumeEpoch, args.RootDir]

if __name__ == "__main__":
    register_envs()

    epochs, resume_epoch, rootdir = get_args()

    env = make_vec_env("RoboSoccer-v0", n_envs=8)
    
    agentdirs = listdir(rootdir)

    agentcache = {}
    def getagent(i, j):
        if (i, j) in agentcache:
            return agentcache[(i, j)]
        agent = SoccerAgent(f"{rootdir}/{agentdirs[j]}/model_{i}", env)
        agentcache[(i, j)] = agent
        return agent

    for i in range(resume_epoch, resume_epoch + epochs):
        agentidx = max(0, i - 1)
        active_agent = None
        for j in range(len(agentdirs)):
            active_agent = getagent(agentidx, j)

            def _get_opponent():
                which = j
                while which == j:
                    which = randint(0, len(agentdirs) - 1)
                return getagent(randint(max(0, resume_epoch - 1), agentidx), which)
            
            env.env_method("set_reset_hook", lambda env: env.set_opponent_agent(_get_opponent()))

            print("\n-------------------------------------")
            print(f"Epoch: {i} Agent {j}")
            print("-------------------------------------\n")
        
            active_agent.model.learn(total_timesteps=EPOCH_SIZE)

            active_agent.model.save(f"{rootdir}/{agentdirs[j]}/model_{i}")

    env.close()
