#include "rc_command_t.hpp"
#include "rt/rt_rc_interface.h"
#include "rt/rt_sbus.h"

#include <lcm/lcm-cpp.hpp>

#include <cstdio>
#include <mutex>
#include <thread>

static rc_command_t _rc_command{};
static lcm::LCM _lcm{"udpm://239.255.76.67:7667?ttl=255"};
static std::mutex mutex;

int init_lcm(void) {
  if (!_lcm.good()) {
    fprintf(stderr, "LCM init error!\n");
    return -1;
  }

  return 0;
}

void sbus_packet_complete(void) {
  Jumper_data data;
  update_jumper(&data);

  _rc_command.SE = data.SE;
  _rc_command.SH = data.SH;

  _rc_command.left_stick_x = data.left_stick_x;
  _rc_command.left_stick_y = data.left_stick_y;
  _rc_command.right_stick_x = data.right_stick_x;
  _rc_command.right_stick_y = data.right_stick_y;
}

int main(void) {
  auto port = init_sbus(false);

  auto sbus_thread = std::thread([&]() {
    while (true) {
      receive_sbus(port);
      if (receive_sbus(port)) {
        mutex.lock();
        sbus_packet_complete();
        mutex.unlock();
      }
    }
  });

  if (init_lcm() < 0) {
    return -1;
  }

  for (;;) {
    mutex.lock();
    _lcm.publish("rc_command", &_rc_command);
    mutex.unlock();
  }

  sbus_thread.join();

  return 0;
}