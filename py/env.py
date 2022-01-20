import robopy

import numpy as np

import gym

from torch import nn

from net import load_model

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

        self.raw_env.step()
        dist = self.raw_env.dist()

        reward = -dist + 100 * hit

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

        self.raw_agent = robopy.DriveAgent(1.0, 0.0)
        self.raw_env = robopy.SoccerEnv(self.raw_agent.action)
        self.opponent = robopy.SoccerAgent()
        self.inited = False

        self.action_space = gym.spaces.Box(-1, 1, (2,), dtype=np.float32)
        self.observation_space = gym.spaces.Box(-1, 1, (10,), dtype=np.float32)

    def step(self, action):
        opp_action = self.opponent.action(np.asarray(self.raw_env.mirror_state(), dtype=np.float32))

        hit = self.raw_env.action([action[0], action[1], 1 - opp_action[0], opp_action[1]])

        self.raw_env.step()
        dist = (self.raw_env.dist_player1_ball() + self.raw_env.dist_ball_net2())

        reward = -dist + 10000 * hit

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
        max_episode_steps=1024,
    )
