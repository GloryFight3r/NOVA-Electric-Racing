#ifndef CAN_UTILITY_H
#define CAN_UTILITY_H

#include "inverter_broadcast_parse.hpp"
#include "zephyr/kernel.h"

// ---------------------------------------------------------

struct StateMessage {
  uint8_t data[8];
  Internal_States new_state;
};

extern k_msgq state_transition_messages;

void State_Transition();

// ---------------------------------------------------------

const int16_t MAX_TORQUE = 120; // it is in Nm(x10)
const int16_t MAX_SPEED = 300;  // it is in rpms

struct PulseMessage {
  int16_t torque;
  int16_t speed;
  bool direction;
  bool inverter_enable;
  bool inverter_discharge;
  bool speed_mode_enable;
  int16_t torque_limit;
};

// ---------------------------------------------------------

extern PulseMessage pulse_message;

void Pulse_Command();

// ---------------------------------------------------------

void Clear_Faults();

// ---------------------------------------------------------

#endif
