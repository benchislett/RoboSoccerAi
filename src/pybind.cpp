#include "agent.hpp"
#include "environment.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(robopy, m) {
  py::class_<BlankEnv<11, 4>>(m, "BlankEnv_11_4");

  py::class_<SoccerEnv, BlankEnv<11, 4>>(m, "SoccerEnv")
      .def(py::init<>())
      .def_readonly("side", &SoccerEnv::side)
      .def("init", &SoccerEnv::init)
      .def("update", &SoccerEnv::update)
      .def("reset", &SoccerEnv::reset)
      .def("state", &SoccerEnv::state)
      .def("state10", &SoccerEnv::state10)
      .def("step", &SoccerEnv::step)
      .def("action", &SoccerEnv::action)
      .def("dist_players", &SoccerEnv::dist_players)
      .def("dist_player1_ball", &SoccerEnv::dist_player1_ball)
      .def("dist_player2_ball", &SoccerEnv::dist_player2_ball)
      .def("dist_ball_net1", &SoccerEnv::dist_ball_net1)
      .def("dist_ball_net2", &SoccerEnv::dist_ball_net2);

  py::class_<LiveSoccerEnv>(m, "LiveSoccerEnv")
      .def(py::init<>())
      .def("raw_state", &LiveSoccerEnv::raw_state)
      .def("reset", &LiveSoccerEnv::reset)
      .def("state", &LiveSoccerEnv::state)
      .def("state10", &LiveSoccerEnv::state10)
      .def("action", &LiveSoccerEnv::action);

  py::class_<PDDriveAgent>(m, "PDDriveAgent")
      .def(py::init<py::float_, py::float_>())
      .def("action", &PDDriveAgent::action);

  py::class_<DefenderSoccerAgent>(m, "DefenderSoccerAgent")
      .def(py::init<>())
      .def_readwrite("player2", &DefenderSoccerAgent::player2)
      .def("action", &DefenderSoccerAgent::action);
  py::class_<ChaserSoccerAgent>(m, "ChaserSoccerAgent")
      .def(py::init<>())
      .def_readwrite("player2", &ChaserSoccerAgent::player2)
      .def("action", &ChaserSoccerAgent::action);
  py::class_<ManualSoccerAgent>(m, "ManualSoccerAgent")
      .def(py::init<const SoccerEnv&>())
      .def("action", &ManualSoccerAgent::action);
}
