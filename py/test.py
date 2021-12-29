import gym

from stable_baselines3.common.evaluation import evaluate_policy
from stable_baselines3 import TD3

from train import load_model

from env import BallChase

if __name__ == "__main__":
    gym.envs.register(
        id="BallChase-v0",
        entry_point="env:BallChase",
        max_episode_steps=512,
    )

    env = gym.make("BallChase-v0")

    model = load_model(env)

    reward, std = evaluate_policy(model, env, n_eval_episodes=64)
    print("mean_reward:", reward, "+/-", std)

    evaluate_policy(model, env, n_eval_episodes=5, render=True)
