#ifndef INVERTER_COMMAND_H
#define INVERTER_COMMAND_H

#include <stdbool.h>
#include <stdint.h>

// ---------------------------------------------------------

/*
 * Send a command message
 *
 * page 32 of CAN Protocol document
 */
void Send_Command(int16_t torque_command, int16_t speed_command,
                  bool direction_command, bool inverter_enable,
                  bool inverter_discharge, bool speed_mode_enable,
                  int16_t commanded_torque_limit);

// ---------------------------------------------------------

/*
 * Send a parameter message
 *
 * page 38 of CAN Protocol document
 */
void Send_Parameter(uint16_t parameter_address, bool rw, int16_t data);

// ---------------------------------------------------------

void Parse_Parameter_Message(uint8_t *arr, uint16_t *parameter_address,
                             bool *success, int16_t *data);

// ---------------------------------------------------------

#endif // INVERTER_COMMAND_H
