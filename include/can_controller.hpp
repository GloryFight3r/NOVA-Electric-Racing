#ifndef CAN_CONTROLLER_H
#define CAN_CONTROLLER_H

#include <cstdint>

void InitCan();

void CAN_Send_Message(uint16_t address, uint8_t *message);

#endif // !CAN_CONTROLLER_H
