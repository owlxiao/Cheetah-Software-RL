#ifndef _RT_RC_LCM_INTERFACE_H_
#define _RT_RC_LCM_INTERFACE_H_

#include <cstddef>
#include <lcm/lcm-cpp.hpp>

namespace lcm {

int interface_lcm_init(lcm::LCM *const lcm);
void get_rc_control_settings(void *settings);

} // namespace lcm

#endif