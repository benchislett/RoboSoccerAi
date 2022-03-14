import robopy

env = robopy.LiveSoccerEnv()
agent = robopy.DefenderSoccerAgent()

while env.raw_state().frames == 0:
    pass

last_frame = 0
while True:
    raw_state = env.raw_state()
    frame = raw_state.frames
    if frame > last_frame:
        last_frame = frame
        print(frame)
        state = env.state()
        action = agent.action(state)
        print('State: ', state)
        print('Action: ', action)
        env.action(action)