#include "agent.hpp"
#include "environment.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(robopy, m) {
  py::class_<BlankEnv<6, 2>>(m, "BlankEnv");
  py::class_<BallChaseEnv, BlankEnv<6, 2>>(m, "BallChaseEnv")
      .def(py::init<>())
      .def("init", &BallChaseEnv::init)
      .def("update", &BallChaseEnv::update)
      .def("reset", &BallChaseEnv::reset)
      .def("state", &BallChaseEnv::state)
      .def("step", &BallChaseEnv::step)
      .def("action", &BallChaseEnv::action)
      .def("dist", &BallChaseEnv::dist);

  py::class_<BallChaseAgent>(m, "BallChaseAgent")
      .def(py::init<py::float_, py::float_>())
      .def("action", &BallChaseAgent::action);
}