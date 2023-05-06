#include "RL_Controller.hpp"
#include "Utilities/utilities.h"
#include <thread>

RL_Controller::RL_Controller() : _lcm(getLcmUrl(255)) {
  _lcm.subscribe("pd_plustau_targets", &RL_Controller::handleLcm, this);
  _lcmThread = std::thread([&]() {
    for (;;)
      _lcm.handle();
  });
}

void RL_Controller::handleLcm(const lcm::ReceiveBuffer *rbuf,
                              const std::string &chan,
                              const pd_tau_targets_lcmt *msg) {
  (void)rbuf;
  (void)chan;
  _mutex.lock();
  _command = *msg;
  _mutex.unlock();
}

void RL_Controller::runController() {
  if (!_lcm.good())
    exit(-1);

  _mutex.lock();

  for (int leg = 0; leg < 4; ++leg) {
    for (int idx = 0; idx < 3; ++idx) {
      int joint = 3 * leg + idx;
      _legController->commands[leg].qDes[idx] = _command.q_des[joint];
      _legController->commands[leg].qdDes[idx] = _command.qd_des[joint];
      _legController->commands[leg].tauFeedForward[idx] =
          _command.tau_ff[joint];
      _legController->commands[leg].kdJoint(idx, idx) = _command.kd[joint];
      _legController->commands[leg].kpJoint(idx, idx) = _command.kp[joint];
    }
  }

  _mutex.unlock();
}