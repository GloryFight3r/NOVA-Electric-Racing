#include "can_utility.hpp"
#include "zephyr/kernel.h"

#include "relay_controller.hpp"
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include "can_controller.hpp"

const int32_t STACK_SIZE = 2048;

K_THREAD_DEFINE(can_thread_id, STACK_SIZE, CAN_Parse_Thread, nullptr, nullptr,
                nullptr, 3, 0, 0);
K_THREAD_DEFINE(inverter_command_id, STACK_SIZE, Pulse_Command, nullptr,
                nullptr, nullptr, 2, 0, 0);

int main() { return 0; }
