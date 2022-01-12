import robopy

import numpy as np

import gym

from torch import nn


class RoboDrive(gym.Env):
    metadata = {"render.modes": ["human"]}

    def __init__(self):
        super(RoboDrive, self).__init__()

        self.raw_env = robopy.DriveEnv()
        self.inited = False

        self.action_space = gym.spaces.Box(-1, 1, (2,), dtype=np.float32)
        self.observation_space = gym.spaces.Box(-1, 1, (6,), dtype=np.float32)

    def step(self, action):
        hit = self.raw_env.action([action[0], action[1]])

        prev_dist = self.raw_env.dist()
        self.raw_env.step()
        new_dist = self.raw_env.dist()

        reward = 10 * ((prev_dist - new_dist) + hit)

        obs = np.asarray(self.raw_env.state(), dtype=np.float32)

        return obs, reward, False, {}

    def reset(self):
        self.raw_env.reset()

        return np.asarray(self.raw_env.state(), dtype=np.float32)

    def render(self, mode="human"):
        if mode != "human":
            super(RoboDrive, self).render(mode=mode)

        if not self.inited:
            self.raw_env.init(True)
            self.inited = True

        self.raw_env.update(True)


class RoboSoccer(gym.Env):
    metadata = {"render.modes": ["human"]}

    def __init__(self):
        super(RoboSoccer, self).__init__()

        self.raw_env = robopy.SoccerEnv()
        self.inited = False

        self.action_space = gym.spaces.Box(-1, 1, (2,), dtype=np.float32)
        self.observation_space = gym.spaces.Box(-1, 1, (10,), dtype=np.float32)

    def step(self, action):
        hit = self.raw_env.action([action[0], action[1], 0, 0])

        self.raw_env.step()

        reward = hit * 1000

        obs = np.asarray(self.raw_env.state(), dtype=np.float32)

        return obs, reward, False, {}

    def reset(self):
        self.raw_env.reset()

        return np.asarray(self.raw_env.state(), dtype=np.float32)

    def render(self, mode="human"):
        if mode != "human":
            super(RoboSoccer, self).render(mode=mode)

        if not self.inited:
            self.raw_env.init(True)
            self.inited = True

        self.raw_env.update(True)

def register_envs():
    gym.envs.register(
        id="RoboDrive-v0",
        entry_point=RoboDrive,
        max_episode_steps=1024
    )

    gym.envs.register(
        id="RoboSoccer-v0",
        entry_point=RoboSoccer,
        max_episode_steps=2048,
    )
