#include "agent.hpp"
#include "environment.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(robopy, m) {
  py::class_<BlankEnv<6, 2>>(m, "BlankEnv_6_2");
  py::class_<BlankEnv<10, 4>>(m, "BlankEnv_10_4");

  py::class_<BallChaseEnv, BlankEnv<6, 2>>(m, "BallChaseEnv")
      .def(py::init<>())
      .def("init", &BallChaseEnv::init)
      .def("update", &BallChaseEnv::update)
      .def("reset", &BallChaseEnv::reset)
      .def("state", &BallChaseEnv::state)
      .def("step", &BallChaseEnv::step)
      .def("action", &BallChaseEnv::action)
      .def("dist", &BallChaseEnv::dist);

  py::class_<SoccerEnv, BlankEnv<10, 4>>(m, "SoccerEnv")
      .def(py::init<>())
      .def("init", &SoccerEnv::init)
      .def("update", &SoccerEnv::update)
      .def("reset", &SoccerEnv::reset)
      .def("state", &SoccerEnv::state)
      .def("state", &SoccerEnv::mirror_state)
      .def("step", &SoccerEnv::step)
      .def("action", &SoccerEnv::action);

  py::class_<BallChaseAgent>(m, "BallChaseAgent")
      .def(py::init<py::float_, py::float_>())
      .def("action", &BallChaseAgent::action);
}