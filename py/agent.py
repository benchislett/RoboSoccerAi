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
            return self.model.predict(obs, **kwargs)
        else:
            actions = states = []
            for o in obs:
                actions.append(self.model.action(o))
                states.append(None)
            return actions, states

class DriveAgent(Agent):
    def __init__(self, agent_name, env):
        # model = network = None

        if agent_name == "PDDriveAgent":
            model = robopy.PDDriveAgent(1.0, 0.0)
            network = False
        else:
            try:
                model = load_model(agent_name, env)
            except FileNotFoundError:
                model = new_model(env)
            network = True
        
        Agent.__init__(self, model, network)

class SoccerAgent(Agent):
    def __init__(self, agent_name, env):
        # model = network = None

        if agent_name == "DefenderSoccerAgent":
            model = robopy.DefenderSoccerAgent()
            network = False
        elif agent_name == "ChaserSoccerAgent":
            model = robopy.ChaserSoccerAgent()
            network = False
        elif agent_name == "ManualSoccerAgent":
            model = robopy.ManualSoccerAgent(env.raw_env)
            network = False
        else:
            try:
                model = load_model(agent_name, env)
            except FileNotFoundError:
                model = new_model(env)
            network = True
        
        Agent.__init__(self, model, network)
    
