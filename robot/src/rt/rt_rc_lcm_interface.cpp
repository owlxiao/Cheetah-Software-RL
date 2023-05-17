#include "rt/rt_rc_lcm_interface.h"
#include "rc_command_t.hpp"
#include "rt/rt_rc_interface.h"
#include "rt/rt_sbus.h"

#include <lcm/lcm-cpp.hpp>

#include <cstring>
#include <memory>
#include <mutex>

namespace lcm {

static std::mutex _lcm_mutex;

rc_control_settings _rc_control;

void _sbus_packet_complete_at9s(const rc_command_t *msg) {
  rc_command_t data = *msg;

  float v_scale = 1.5;
  float w_scale = 2 * v_scale;

  auto estopSwitch = data.SE;
  int selectedMode = 0;

  switch (estopSwitch) {
  case Jumper_SwitchStateTri::JUMPER_TRI_DOWN:
    selectedMode = RC_mode::OFF;
    break;

  case Jumper_SwitchStateTri::JUMPER_TRI_MIDDLE:
    selectedMode = RC_mode::RECOVERY_STAND;
    break;

  default:
    selectedMode = RC_mode::OFF;
    break;
  }

  _rc_control.mode = selectedMode;
}

class Handler {
public:
  Handler() = default;
  ~Handler() = default;

public:
  void handleCommand(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                     const rc_command_t *msg) {
    (void)rbuf;
    (void)chan;

    _lcm_mutex.lock();
    _sbus_packet_complete_at9s(msg);
    _lcm_mutex.unlock();
  }
};

int interface_lcm_init(lcm::LCM * const lcm) {

  if (!lcm->good())
    return -1;

  Handler handlerObject{};
  lcm->subscribe("rc_command", &Handler::handleCommand, &handlerObject);

  return 0;
}

void get_rc_control_settings(void *settings) {
  _lcm_mutex.lock();
  v_memcpy(settings, &_rc_control, sizeof(rc_control_settings));
  _lcm_mutex.unlock();
}

} // namespace lcm