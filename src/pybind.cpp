#include "agent.hpp"
#include "environment.hpp"

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
      .def(py::init<>())
      .def("init", &SoccerEnv::init)
      .def("update", &SoccerEnv::update)
      .def("reset", &SoccerEnv::reset)
      .def("state", &SoccerEnv::state)
      .def("mirror_state", &SoccerEnv::mirror_state)
      .def("step", &SoccerEnv::step)
      .def("action", &SoccerEnv::action);

  py::class_<DriveAgent>(m, "DriveAgent").def(py::init<py::float_, py::float_>()).def("action", &DriveAgent::action);
}
