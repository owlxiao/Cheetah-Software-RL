#ifndef _CHEETAH_SOFTWARE_RL_CONTROLLER_H_
#define _CHEETAH_SOFTWARE_RL_CONTROLLER_H_

#include "ControlParameters/ControlParameters.h"
#include "RobotController.h"
#include "pd_tau_targets_lcmt.hpp"

#include <lcm/lcm-cpp.hpp>

#include <string>
#include <thread>
#include <mutex>

class RL_Controller : public RobotController {
public:
  RL_Controller();
  virtual ~RL_Controller() {}

public:
  virtual void initializeController() override {}
  virtual void runController() override;
  virtual void updateVisualization() override {}
  virtual ControlParameters *getUserControlParameters(void) override {
    return nullptr;
  }

public:
  void handleLcm(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                 const pd_tau_targets_lcmt *msg);

private:
  lcm::LCM _lcm;
  std::thread _lcmThread;
  std::mutex _mutex;

  pd_tau_targets_lcmt _command;
};

#endif