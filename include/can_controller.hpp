#ifndef CAN_CONTROLLER_H
#define CAN_CONTROLLER_H

#include <cstdint>

enum class CANMessageTypes : uint32_t {
  INTERNAL_STATES = 0x0AA,
  FAULT_CODES = 0x0AB,
  MOTOR_POSITION_INFORMATION = 0x0A5,
  VOLTAGE_INFORMATION = 0x0A7,
  PARAMETER_MESSAGE = 0x0C2
};

void CAN_Initialize();

void CAN_Parse_Thread(void *p1, void *p2, void *p3);

void CAN_Send_Message(uint16_t address, uint8_t message[]);

#endif // !CAN_CONTROLLER_H
