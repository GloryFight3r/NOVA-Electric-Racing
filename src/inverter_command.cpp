#include "inverter_command.hpp"
#include "can_controller.hpp"

#include <cstdint>

void Send_Command(int16_t torque_command, int16_t speed_command,
                  bool direction_command, bool inverter_enable,
                  bool inverter_discharge, bool speed_mode_enable,
                  int16_t commanded_torque_limit) {

  uint8_t constructed_message[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  constructed_message[0] = torque_command & ((1 << 8) - 1);
  constructed_message[1] = torque_command >> 8;

  constructed_message[2] = speed_command & ((1 << 8) - 1);
  constructed_message[3] = speed_command >> 8;

  constructed_message[4] = (uint8_t)direction_command;

  constructed_message[5] |= inverter_enable;
  constructed_message[5] |= ((uint8_t)inverter_discharge) << 1;
  constructed_message[5] |= ((uint8_t)speed_mode_enable) << 2;

  constructed_message[6] = commanded_torque_limit & ((1 << 8) - 1);
  constructed_message[7] = commanded_torque_limit >> 8;

  CAN_Send_Message(0x0C0, constructed_message);
}

/*
 * Send a parameter message
 *
 * page 38 of CAN Protocol document
 */
void Send_Parameter(uint16_t parameter_address, bool rw, int16_t data) {
  uint8_t constructed_message[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  constructed_message[0] = parameter_address & ((1 << 8) - 1);
  constructed_message[1] = parameter_address >> 8;

  constructed_message[2] = (uint8_t)rw;

  constructed_message[4] = data & ((1 << 8) - 1);
  constructed_message[5] = data >> 8;

  CAN_Send_Message(0x0C1, constructed_message);
}

void Parse_Parameter_Message(uint8_t *arr, uint16_t *parameter_address,
                             int16_t *data) {
  *parameter_address = (uint16_t)arr[0] | (((uint16_t)arr[1]) << 8);

  *data = (int16_t)arr[1] | ((int16_t)arr[1] << 8);
}
