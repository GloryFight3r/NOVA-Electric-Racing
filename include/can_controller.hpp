#ifndef CAN_CONTROLLER_H
#define CAN_CONTROLLER_H

#include "inverter_broadcast_parse.hpp"

#include <cstdint>

// ---------------------------------------------------------

int32_t CAN_Initialize();

// ---------------------------------------------------------

enum class CANMessageTypes : uint32_t {
  INTERNAL_STATES = 0x0AA,
  FAULT_CODES = 0x0AB,
  MOTOR_POSITION_INFORMATION = 0x0A5,
  VOLTAGE_INFORMATION = 0x0A7,
  PARAMETER_MESSAGE = 0x0C2
};

void CAN_Parse_Thread(void *p1, void *p2, void *p3);

// ---------------------------------------------------------

void CAN_Send_Message(uint16_t address, uint8_t message[]);

// ---------------------------------------------------------

extern POSSIBLE_FAULTS possible_faults_buff[64];
extern size_t faults_buffer_size;

#endif // CAN_CONTROLLER_H
