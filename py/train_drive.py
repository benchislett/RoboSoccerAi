import argparse

from stable_baselines3.common.env_util import make_vec_env

from agent import DriveAgent

from env import register_envs, RoboDrive

def get_args():
    parser = argparse.ArgumentParser(
        usage="%(prog)s ModelName [Iterations]",
        description="""
        Train a model on the RoboDrive environment.
        """
    )

    parser.add_argument('ModelName')

    parser.add_argument('Iterations', nargs='?', default=50_000, type=int)
    args = parser.parse_args()

    return [args.ModelName, args.Iterations]

if __name__ == "__main__":
    register_envs()

    model_name, iterations = get_args()

    env = make_vec_env("RoboDrive-v0", n_envs=16)

    agent = DriveAgent(model_name, env)
    assert agent.network, "Model must be trainable. Premade agents not allowed for training."

    agent.model.learn(total_timesteps=iterations)

    agent.model.save(model_name)

    env.close()
