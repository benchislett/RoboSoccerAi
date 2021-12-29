import robopy

import numpy as np

import gym

from torch import nn

expert_agent = robopy.BallChaseAgent(0.75, 0.0)


def expert(obs):
    return np.asarray(expert_agent.action(obs.tolist()), dtype=np.float32)


class BallChase(gym.Env):
    metadata = {"render.modes": ["human"]}

    def __init__(self):
        super(BallChase, self).__init__()

        self.raw_env = robopy.BallChaseEnv()
        self.inited = False

        self.action_space = gym.spaces.Box(-1, 1, (2,), dtype=np.float32)
        self.observation_space = gym.spaces.Box(-10, 10, (6,), dtype=np.float32)

    def step(self, action):
        reward = self.raw_env.action([action[0], action[1]])
        self.raw_env.step()

        obs = np.asarray(self.raw_env.state(), dtype=np.float32)

        # expert_action = expert(obs)

        return obs, reward, False, {}

    def reset(self):
        self.raw_env.reset()

        return np.asarray(self.raw_env.state(), dtype=np.float32)

    def render(self, mode="human"):
        if mode != "human":
            super(BallChase, self).render(mode=mode)

        if not self.inited:
            self.raw_env.init(True)
            self.inited = True

        self.raw_env.update(True)
