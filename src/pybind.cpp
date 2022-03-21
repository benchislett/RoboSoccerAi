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
      .def("step_to_action", &SoccerEnv::step_to_action)
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
  py::class_<TargetedSoccerAgent>(m, "TargetedSoccerAgent")
      .def(py::init<>())
      .def_readwrite("player2", &TargetedSoccerAgent::player2)
      .def_readwrite("target", &TargetedSoccerAgent::target)
      .def("action", &TargetedSoccerAgent::action);
  py::class_<ChaserSoccerAgent>(m, "ChaserSoccerAgent")
      .def(py::init<>())
      .def_readwrite("player2", &ChaserSoccerAgent::player2)
      .def("action", &ChaserSoccerAgent::action);
  py::class_<SwitchupSoccerAgent>(m, "SwitchupSoccerAgent")
      .def(py::init<>())
      .def_readwrite("player2", &SwitchupSoccerAgent::player2)
      .def("action", &SwitchupSoccerAgent::action);
  py::class_<ManualSoccerAgent>(m, "ManualSoccerAgent")
      .def(py::init<const SoccerEnv&>())
      .def("action", &ManualSoccerAgent::action);

  py::class_<blob>(m, "blob")
      .def_readwrite("label", &blob::label)
      .def_readwrite("blobId", &blob::blobId)
      .def_readwrite("cx", &blob::cx)
      .def_readwrite("cy", &blob::cy)
      .def_readwrite("vx", &blob::vx)
      .def_readwrite("vy", &blob::vy)
      .def_readwrite("mx", &blob::mx)
      .def_readwrite("my", &blob::my)
      .def_readwrite("dx", &blob::dx)
      .def_readwrite("dy", &blob::dy)
      .def_readwrite("x1", &blob::x1)
      .def_readwrite("x2", &blob::x2)
      .def_readwrite("y1", &blob::y1)
      .def_readwrite("y2", &blob::y2)
      .def_readwrite("R", &blob::R)
      .def_readwrite("G", &blob::G)
      .def_readwrite("B", &blob::B)
      .def_readwrite("H", &blob::H)
      .def_readwrite("S", &blob::S)
      .def_readwrite("V", &blob::V)
      .def_readwrite("idtype", &blob::idtype)
      //   .def_readwrite("next", &blob::next)
      .def_readwrite("updated", &blob::updated);

  py::class_<AI_data>(m, "AI_data")
      .def_readwrite("side", &AI_data::side)
      .def_readwrite("botCol", &AI_data::botCol)
      .def_readwrite("state", &AI_data::state)
      .def_readwrite("selfID", &AI_data::selfID)
      //   .def_readwrite("ball", &AI_data::ball)
      .def_readwrite("old_bcx", &AI_data::old_bcx)
      .def_readwrite("old_bcy", &AI_data::old_bcy)
      .def_readwrite("bvx", &AI_data::bvx)
      .def_readwrite("bvy", &AI_data::bvy)
      .def_readwrite("bmx", &AI_data::bmx)
      .def_readwrite("bmy", &AI_data::bmy)
      .def_readwrite("bdx", &AI_data::bdx)
      .def_readwrite("bdy", &AI_data::bdy)
      //   .def_readwrite("self", &AI_data::self)
      .def_readwrite("old_scx", &AI_data::old_scx)
      .def_readwrite("old_scy", &AI_data::old_scy)
      .def_readwrite("svx", &AI_data::svx)
      .def_readwrite("svy", &AI_data::svy)
      .def_readwrite("smx", &AI_data::smx)
      .def_readwrite("smy", &AI_data::smy)
      .def_readwrite("sdx", &AI_data::sdx)
      .def_readwrite("sdy", &AI_data::sdy)
      //   .def_readwrite("opp", &AI_data::opp)
      .def_readwrite("old_ocx", &AI_data::old_ocx)
      .def_readwrite("old_ocy", &AI_data::old_ocy)
      .def_readwrite("ovx", &AI_data::ovx)
      .def_readwrite("ovy", &AI_data::ovy)
      .def_readwrite("omx", &AI_data::omx)
      .def_readwrite("omy", &AI_data::omy)
      .def_readwrite("odx", &AI_data::odx)
      .def_readwrite("ody", &AI_data::ody)
      .def_readwrite("frames", &AI_data::frames);
}
