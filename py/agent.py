import robopy

from net import load_model, new_model

class Agent:
    def __init__(self, model, network):
        self.model = model
        self.network = network

    def action(self, obs):
        if self.network:
            return self.model.predict(obs)[0]
        else:
            return self.model.action(obs)
    
    def predict(self, obs, **kwargs):
        if self.network:
            action = self.model.predict(obs, **kwargs)
            return action
        else:
            actions = states = []
            for o in obs:
                actions.append(self.model.action(o))
                states.append(None)
            return actions, states

class SoccerAgent(Agent):
    def __init__(self, agent_name, env):
        if not isinstance(agent_name, str):
            network = False
            model = agent_name()
        else:
            try:
                model = load_model(agent_name, env)
            except FileNotFoundError:
                model = new_model(env)
            network = True
        
        Agent.__init__(self, model, network)
    
