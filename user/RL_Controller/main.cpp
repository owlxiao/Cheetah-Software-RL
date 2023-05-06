#include "RL_Controller.hpp"

#include "main_helper.h"

int main(int argc, char **argv) {
  main_helper(argc, argv, new RL_Controller());

  return 0;
}