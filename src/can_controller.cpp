#include "can_controller.hpp"
#include "zephyr/device.h"
#include "zephyr/devicetree.h"
#include "zephyr/drivers/can.h"

const device *can_dev = DEVICE_DT_GET(DT_NODELABEL(can1));

const can_filter my_filter = {.id = 0, .mask = 0, .flags = 0};

void InitCan() {}

void CAN_Send_Message(uint16_t address, uint8_t *message) {}
