import argparse

from random import randint

from stable_baselines3.common.env_util import make_vec_env

from agent import DriveAgent, SoccerAgent

from env import register_envs, RoboDrive, RoboSoccer

BATCH_SIZE = 294_912

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s ModelPrefix [ModelCount] [Epochs]",
        description="""
        Train a batch of models on the RoboSoccer environment.
        """
    )

    parser.add_argument('ModelPrefix')
    parser.add_argument('ModelCount', nargs='?', default=1, type=int)
    parser.add_argument('Epochs', nargs='?', default=9999999, type=int)
    parser.add_argument('ResumeEpoch', nargs='?', default=0, type=int)
    args = parser.parse_args()

    return [args.ModelPrefix, args.ModelCount, args.Epochs, args.ResumeEpoch]

def model_name(prefix, model_idx, iter_idx):
    return f"{prefix}_{model_idx}_i{iter_idx}"

if __name__ == "__main__":
    register_envs()

    model_prefix, model_count, epochs, resume_epoch = get_args()

    env = make_vec_env("RoboSoccer-v0", n_envs=16)

    for i in range(resume_epoch, resume_epoch + epochs):

        def new_opponent(individual_env):
            if randint(0, i) == 0:
                opponent = SoccerAgent("DefenderSoccerAgent", env)
            else:
                opp_idx = randint(0, model_count - 1)
                opp_iter = randint(0, i - 1)
                opponent = SoccerAgent(model_name(model_prefix, opp_idx, opp_iter), env)
            individual_env.set_opponent_agent(opponent)
        
        env.env_method("set_reset_hook", new_opponent)

        for model_idx in range(model_count):

            print("\n-------------------------------------")
            print(f"Training Model {model_idx} Epoch {i}")
            prev_agent_name = model_name(model_prefix, model_idx, max(0, i - 1))
            agent = SoccerAgent(prev_agent_name, env)
            print("-------------------------------------\n")
            
            agent.model.learn(total_timesteps=BATCH_SIZE)

            agent.model.save(model_name(model_prefix, model_idx, i))

    env.close()
