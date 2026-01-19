#include "can_utility.hpp"
#include "peripheral_controller.hpp"
#include "zephyr/kernel.h"

#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include "can_controller.hpp"

const int32_t STACK_SIZE = 2048;

// thread responsible for receiving can messages
K_THREAD_DEFINE(can_thread_id, STACK_SIZE, CAN_Parse_Thread, nullptr, nullptr,
                nullptr, 3, 0, 0);

// thread responsible for pulse signals
K_THREAD_DEFINE(inverter_command_id, STACK_SIZE, Pulse_Command, nullptr,
                nullptr, nullptr, 2, 0, 0);

int main() {
  int32_t ret1 = CAN_Initialize();  // initialize CAN controller
  int32_t ret2 = initPeripherals(); // initialize the peripherals

  if (ret1 == -1 || ret2 == -1) { // terminate all other threads
    k_thread_abort(can_thread_id);
    k_thread_abort(inverter_command_id);
  }

  while (true) {
    k_sleep(K_FOREVER);
  }

  return 0;
}
