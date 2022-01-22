#include "agent.hpp"
#include "environment.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(robopy, m) {
  py::class_<BlankEnv<6, 2>>(m, "BlankEnv_6_2");
  py::class_<BlankEnv<10, 4>>(m, "BlankEnv_10_4");

  py::class_<DriveEnv, BlankEnv<6, 2>>(m, "DriveEnv")
      .def(py::init<>())
      .def("init", &DriveEnv::init)
      .def("update", &DriveEnv::update)
      .def("reset", &DriveEnv::reset)
      .def("state", &DriveEnv::state)
      .def("step", &DriveEnv::step)
      .def("action", &DriveEnv::action)
      .def("dist", &DriveEnv::dist);

  py::class_<SoccerEnv, BlankEnv<10, 4>>(m, "SoccerEnv")
      .def(py::init<const DriveEnvAgent&>())
      .def("init", &SoccerEnv::init)
      .def("set_controller", &SoccerEnv::set_controller)
      .def("update", &SoccerEnv::update)
      .def("reset", &SoccerEnv::reset)
      .def("state", &SoccerEnv::state)
      .def("mirror_state", &SoccerEnv::mirror_state)
      .def("step", &SoccerEnv::step)
      .def("action", &SoccerEnv::action)
      .def("dist_player1_ball", &SoccerEnv::dist_player1_ball)
      .def("dist_player2_ball", &SoccerEnv::dist_player2_ball)
      .def("dist_ball_net1", &SoccerEnv::dist_ball_net1)
      .def("dist_ball_net2", &SoccerEnv::dist_ball_net2);


  py::class_<PDDriveAgent>(m, "PDDriveAgent")
      .def(py::init<py::float_, py::float_>())
      .def("action", &PDDriveAgent::action);

  py::class_<DefenderSoccerAgent>(m, "DefenderSoccerAgent")
      .def(py::init<>())
      .def("action", &DefenderSoccerAgent::action);
  py::class_<ChaserSoccerAgent>(m, "ChaserSoccerAgent").def(py::init<>()).def("action", &ChaserSoccerAgent::action);
  py::class_<ManualSoccerAgent>(m, "ManualSoccerAgent")
      .def(py::init<const SoccerEnv&>())
      .def("action", &ManualSoccerAgent::action);
}
