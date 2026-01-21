#include "can_utility.hpp"
#include "peripheral_controller.hpp"
#include "zephyr/kernel.h"

#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include "can_controller.hpp"
#include "zephyr/logging/log.h"

const int32_t STACK_SIZE = 2048;

//-------------------------------------------------------------------------------------------------------------------------

LOG_MODULE_REGISTER(main_logger, LOG_LEVEL_INF);

/*
 * Debug definitions
 */

// #define RELAYS_DEBUG

//-------------------------------------------------------------------------------------------------------------------------

// thread responsible for receiving can messages
K_THREAD_DEFINE(can_msg_receive_thread_id, STACK_SIZE, CAN_Parse_Thread,
                nullptr, nullptr, nullptr, 2, 0, 0);

// thread responsible for pulse signals
K_THREAD_DEFINE(inverter_pulse_command_thread_id, STACK_SIZE, Pulse_Command,
                nullptr, nullptr, nullptr, 1, 0, 0);

// transitions the states
K_THREAD_DEFINE(state_transition_thread_id, STACK_SIZE, State_Transition,
                nullptr, nullptr, nullptr, 2, 0, 0);

int main() {
  int32_t ret1 = CAN_Initialize();  // initialize CAN controller
  int32_t ret2 = initPeripherals(); // initialize the peripherals

  if (ret1 == -1 || ret2 == -1) { // terminate all other threads
    LOG_ERR("Aborted all threads!");
    k_thread_abort(can_msg_receive_thread_id);
    k_thread_abort(inverter_pulse_command_thread_id);
    k_thread_abort(state_transition_thread_id);
  }

  k_thread_suspend(k_current_get());

#ifdef RELAYS_DEBUG
  while (true) {
    enablePrechargeRelay();
    k_sleep(K_MSEC(1500));

    disablePrechargeRelay();

    k_sleep(K_MSEC(1500));

    enableMainRelay();
    k_sleep(K_MSEC(1500));

    disableMainRelay();
    k_sleep(K_MSEC(1500));
  }
#endif

  return 0;
}
