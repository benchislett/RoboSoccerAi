# Reinforcement Learning for Robot Soccer

This project implements reinforcement learning for the robot soccer framework introduced in CSCC85.

Included in this repository is a working copy of the RoboSoccer source code in C,
a fast simulator of the robot soccer environment in C++ using Box2D,
and a collection of python scripts for training and testing agents on the robot soccer simulated environment and evaluating them on the live environment.

Along with the C++ implementation are pybind11 bindings for the live and simulated RoboSoccer environments.

## Building / Dependencies

The project is build using CMake and requires Python 3.10.

Be sure to initialize the submodules, either when cloning with `--recurse-submodules` (formerly `--recursive`), or after cloning with `git submodule update --init
`. You might need to build the `extern/pybind11` submodule.

### CMake

#### Dependencies

All dependencies are required dependencies. Individual targets cannot be built without satisfying all dependencies of the project.

- OpenGL + GLUT (for C-RoboSoccer)
- SDL2 + SFML 2.5 (for Sim-RoboSoccer graphical interface)
- box2d (for Sim-RoboSoccer core)
- pybind11 (for C/C++ -> Python bindings, added as a submodule)

Additionally, the variable `CMAKE_CXX_FLAGS` is hardcoded with the python include directories, as `find_package(Python ...)` 
is very finnicky and does not work on my compute environment.

#### Build

Standard CMake build process.

```
mkdir build && cd build
cmake ..
make
```

#### Running the Python Scripts

Note that the python scripts which use the python-bound library `robopy` must be copied into and executed from the build directory.

For workflow, I suggest `cd build && cp ../py/* .`, and developing in the build directory, then `cp *.py ../py/` to copy back the changes before commit.

## Source Code Layout

- `extern/` contains the pybind11 submodule.
- `imagecapture/`, `robo_api/`, and `roboai/` contain the original C code for running RoboSoccer. This code belongs to Francisco Estrada, see the included licenses and READMEs for more.
- `include/` and `src/` contain the C++ simulated environment using box2d, optionally rendered using SFML/SDL2. 
- `py/` contains the python scripts for training and running agents on the robosoccer environments (live and simulated).

## Implementation Details

### Simulator and C++ Core

#### Environment

`Environment.cpp` defines the core objects in the simulator: `Bot`, `Ball`, `VerticalWall`, and `HorizontalWall`. 
These wrap box2d bodies and fixtures to place them in the environment.
They can be repeated, so an environment can be defined with many bots, balls, or walls if needed.

The environments are low-level analogs to Python's Gym environments.
`BlankEnv` which handles the environment core, world, and outlining walls.
`SoccerEnv` specializes `BlankEnv` for the RoboSoccer setup: 2 bots, 1 ball, the nets, some heuristics for calculating rewards, and optional parameter randomization.

`LiveSoccerEnv` follows a similar interface but forwards to the C source for RoboSoccer instead of using the simulated environment from `BlankEnv`.

#### Environment Parameters

`misc.hpp` implements some helper functions and includes a long list of tunable parameters for the simulation.
Everything from the physics constants to the environment randomization settings can be set here.
These are important for training sequentially along a curriculum, and changing these from one training session to the next is important for good convergence.

#### Environment Agents

`agent.hpp` defines some example agents for the robosoccer environment. These serve as opponents during the training sessions.

- `Defender` follows an interpolated point between the ball and its own net.
- `Targeted` always moves to a fixed point.
- `Chaser` always moves towards the ball.
- `Shooter` lines up a shot.
- `Switchup` rotates between strategies randomly, and can change even within an episode.
- `Manual` follows the mouse, and acts as `Chaser` if the window is not active.

#### Main Script

The `main.cpp` root script runs the C++ agent directly, mostly for debugging purposes. 
The player and oppponent agents can be easily swapped out to test various agents.
When tuning parameters or introducing new features, it is good to run on the main script before training and agent or testing with the python scripts.

Uncommenting the live soccer environment instead of the robosoccer environment will run the C++ wrapper for the C RoboSoccer environment.
It should run seamlessly as if the C code was run directly.

### Python Bindings and Training

#### Gym Environment

`env.py` defines the Gym environment atop the RoboSoccer simulator.
It calls either `step` (single step) or `step_to_action` (step until action is satisfied) on the wrapped environment.
These behaviours vary wildly and should be interchanged with care.
When changing to `step`, be sure to update the `max_episode_steps` to accomodate the slower simulation speed.

#### Gym Agents

`agent.py` provides a common wrapper for trained models and hand-written agents alike.
If `SoccerAgent` is constructed with a string, it will load the corresponding model or create a new one if none can be found.
If instead a class is passed, the class is treated as a manual agent and is instantiated directly.

#### Models

`net.py` defines the networks for training. This can be changed to use a different model, a different vendor library, or custom implementation entirely.
As long as the model supports the same interface it will work. I have found best results with the PPO model.

#### Training/Testing Scripts

`train_soccer.py` trains a single agent against an opponent. It can initialize a new model or continue training from a saved model.

`train_soccer_pool.py` trains a pool of agents against each other.

`test_soccer.py` evaluates a trained agent against any chosen opponent. The opponent must be one of the manually defined agents.

`imitate_soccer.py` is a work-in-progress using the `imitation` library.
The library is very out-of-date, and requires a previous version of stable-baselines3 to work.
I had to hack some workarounds in the source of this library in order to get this script to work at all, and even then the logging was entirely disabled.

`eval_live_soccer.py` runs the live soccer environment on a specified agent, trained or manual.
It is crucial for testing that the learned model transfers to practical usage.

