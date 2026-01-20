#include "can_utility.hpp"
#include "peripheral_controller.hpp"
#include "zephyr/kernel.h"

#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include "can_controller.hpp"

const int32_t STACK_SIZE = 2048;

//-------------------------------------------------------------------------------------------------------------------------

/*
 * Debug definitions
 */

// #define RELAYS_DEBUG

//-------------------------------------------------------------------------------------------------------------------------

// thread responsible for receiving can messages
K_THREAD_DEFINE(can_thread_id, STACK_SIZE, CAN_Parse_Thread, nullptr, nullptr,
                nullptr, 2, 0, 0);

// thread responsible for pulse signals
K_THREAD_DEFINE(inverter_command_id, STACK_SIZE, Pulse_Command, nullptr,
                nullptr, nullptr, 1, 0, 0);

// transitions the states
K_THREAD_DEFINE(state_transition_thread_id, STACK_SIZE, State_Transition,
                nullptr, nullptr, nullptr, 2, 0, 0);

int main() {
  int32_t ret1 = CAN_Initialize();  // initialize CAN controller
  int32_t ret2 = initPeripherals(); // initialize the peripherals
  //
  if (ret1 == -1 || ret2 == -1) { // terminate all other threads
    printk("Aborted threads!\n");
    k_thread_abort(can_thread_id);
    k_thread_abort(inverter_command_id);
  }

  while (true) {
#ifdef RELAYS_DEBUG
    enablePrechargeRelay();
    k_sleep(K_MSEC(1500));

    disablePrechargeRelay();

    k_sleep(K_MSEC(1500));

    enableMainRelay();
    k_sleep(K_MSEC(1500));

    disableMainRelay();
    k_sleep(K_MSEC(1500));
#endif
    k_sleep(K_MSEC(100));
  }

  return 0;
}
