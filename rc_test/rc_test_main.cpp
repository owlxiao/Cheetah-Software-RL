#include <cstdio>
#include <thread>
#include <unistd.h>

#include "rt/rt_rc_interface.h"
#include "rt/rt_sbus.h"

static const char *switch_names_bool[2] = {"UP", "DN"};

static const char *switch_names_tri[3] = {"UP", "--", "DN"};

static Jumper_data data;

void print_sbus(Jumper_data &data) {
  printf("\n\n\n"
         "SE    [%2s]\n"
         "SH    [%2s]\n"
         "left  [%4.1f %4.1f]\n"
         "right [%4.1f %4.1f]\n",
         switch_names_tri[data.SE], switch_names_bool[data.SH],
         data.left_stick_x, data.left_stick_y, data.right_stick_x,
         data.right_stick_y);
}

void sbus_packet_complete(void) { update_jumper(&data); }

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  int use_computer_port = 1;

  if (argc == 2 && argv[1][0] == 'r') {
    use_computer_port = 0;
  }

  auto port = init_sbus(use_computer_port);

  auto sbus_thread = std::thread([&]() {
    while (true) {
      receive_sbus(port);
      if (receive_sbus(port)) {
        sbus_packet_complete();
      }
    }
  });

  for (;;) {
    for (int i = 0; i < 30; i++) {
      usleep(100000);
      print_sbus(data);
    }
  }

  sbus_thread.join();

  return 0;
}
