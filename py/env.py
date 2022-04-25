import robopy

import numpy as np

import gym

from torch import nn

from net import load_model

class RoboSoccer(gym.Env):
    metadata = {"render.modes": ["human"]}

    def __init__(self):
        super(RoboSoccer, self).__init__()

        self.opponent = None
        self.inited = False
        self.reset_hook = None
        self.raw_env = robopy.SoccerEnv()

        self.action_space = gym.spaces.Box(-1, 1, (2,), dtype=np.float32)
        self.observation_space = gym.spaces.Box(-1, 1, (13,), dtype=np.float32)

    def init(self):
        self.inited = True
        self.raw_env.init(True)

    def set_opponent_agent(self, agent):
        self.opponent = agent
    
    def _state(self):
        return np.asarray(self.raw_env.state(), dtype=np.float32)

    def _obs(self):
        return self._state()
    
    def _shot_dist(self):
        if self.raw_env.side == 1.0:
            return self.raw_env.dist_ball_net1() ** 0.5
        else:
            return self.raw_env.dist_ball_net2() ** 0.5
    
    def step(self, action):
        reward = 0.0

        reward += -1.0 * (self.raw_env.dist_player1_ball() ** 0.8)

        st = self._state()
        p1x, p1y, p1rx, p1ry, p2x, p2y, p2ry, p2rx, bx, by, bvx, bvy, side = st
        opp_st = [p2x, p2y, p2rx, p2y, p1x, p1y, p1rx, p1y, bx, by, bvx, bvy, 1 - side]

        opp_action = self.opponent.action(opp_st)

        prev_shot_dist = self._shot_dist()
        prev_dist_players = self.raw_env.dist_players()

        hit = self.raw_env.step_to_action([action[0], action[1], opp_action[0], opp_action[1]])

        cur_shot_dist = self._shot_dist()
        new_dist_players = self.raw_env.dist_players()

        reward += 100 * (prev_shot_dist - cur_shot_dist) + 200 * np.abs(prev_shot_dist - cur_shot_dist)
        reward += 10000 * hit

        # if (new_dist_players < 0.07 and prev_dist_players > 0.07):
        #     pass# reward += -1000

        obs = self._obs()

        if hit != 0:
            self.raw_env.reset()

        return obs, reward, False, {}
    
    def set_reset_hook(self, func):
        self.reset_hook = func

    def reset(self):
        self.raw_env.reset()

        if self.reset_hook:
            self.reset_hook(self)

        return self._obs()

    def render(self, mode="human"):
        if mode != "human":
            super(RoboSoccer, self).render(mode=mode)

        if not self.inited:
            self.init()

        self.raw_env.update(True)

def register_envs():
    gym.envs.register(
        id="RoboSoccer-v0",
        entry_point=RoboSoccer,
        max_episode_steps=64,
    )
