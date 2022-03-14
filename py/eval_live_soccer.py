import robopy

env = robopy.LiveSoccerEnv()

while env.raw_state().frames == 0:
    pass

last_frame = 0
while True:
    state = env.raw_state()
    frame = state.frames
    if frame > last_frame:
        print(frame)
        last_frame = frame