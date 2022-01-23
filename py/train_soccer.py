import argparse

from random import randint

from stable_baselines3.common.env_util import make_vec_env

from agent import DriveAgent, SoccerAgent

from env import register_envs, RoboDrive, RoboSoccer

BATCH_SIZE = 1_000_000

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s ModelPrefix [ModelCount] [Epochs]",
        description="""
        Train a batch of models on the RoboSoccer environment.
        """
    )

    parser.add_argument('ModelPrefix')
    parser.add_argument('ModelCount', nargs='?', default=1, type=int)
    parser.add_argument('Epochs', nargs='?', default=1, type=int)
    args = parser.parse_args()

    return [args.ModelPrefix, args.ModelCount, args.Epochs]

def model_name(prefix, model_idx, iter_idx):
    return f"{prefix}_{model_idx}_i{iter_idx}"

if __name__ == "__main__":
    register_envs()

    model_prefix, model_count, epochs = get_args()

    env = make_vec_env("RoboSoccer-v0", n_envs=16)

    for i in range(epochs):

        for model_idx in range(model_count):

            print("\n-------------------------------------")
            print(f"Model {model_idx} epoch {i} VS ", end="")
            prev_agent_name = model_name(model_prefix, model_idx, max(0, i - 1))
            agent = SoccerAgent(prev_agent_name, env)

            if randint(0, i) == 0:
                opponent = SoccerAgent("DefenderSoccerAgent", env)
                print("DefenderSoccerAgent")
            else:
                opp_idx = randint(0, model_count - 1)
                opp_iter = randint(0, i - 1)
                opponent = SoccerAgent(model_name(model_prefix, opp_idx, opp_iter), env)
                print(f"Model {opp_idx} epoch {opp_iter}")
            
            print("-------------------------------------\n")
            
            env.env_method("set_opponent_agent", opponent)

            agent.model.learn(total_timesteps=BATCH_SIZE)

            agent.model.save(model_name(model_prefix, model_idx, i))

    env.close()
