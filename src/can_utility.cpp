#include "can_utility.hpp"
#include "inverter_broadcast_parse.hpp"
#include "inverter_command.hpp"
#include "peripheral_controller.hpp"
#include "zephyr/kernel.h"

K_MSGQ_DEFINE(state_transition_messages, sizeof(StateMessage), 64, 4);

void State_Transition() {

  Internal_States next_state;
  while (true) {
    // get the state of the VSM
    k_msgq_get(&state_transition_messages, &next_state, K_FOREVER);

    /*
     * SW USER MANUAL, Page 28
     *
     * NOTE: It is possible to send a command to the inverter to enable the
     * relay contactors using CAN messages. That way we can get 12V power
     * driver, instead of relying on MOSFET drivers
     */
    switch (next_state.vsm_state) {
    case VSM_STATE::VSM_START_STATE:
      disableMainRelay();
      disablePrechargeRelay();

      break;
    case VSM_STATE::VSM_PRE_CHARGE_INIT_STATE:
      /*
       * Performs VDC checks and if the voltage is under the threshold,
       * transition to VSM_PRE_CHARGE_ACTIVE_STATE
       */
      disableMainRelay();
      disablePrechargeRelay();

      break;
    case VSM_STATE::VSM_PRE_CHARGE_ACTIVE_STATE:
      disableMainRelay();
      enablePrechargeRelay();

      break;
    case VSM_STATE::VSM_PRE_CHARGE_COMPLETE_STATE:
      /*
       * also preforms stability check on the voltage. Once
       * checks are completed transition to VSM_WAIT_STATE
       */
      disablePrechargeRelay();
      enableMainRelay();

      break;
    case VSM_STATE::VSM_WAIT_STATE:
      /*
       * NOTE: Inv_Cmd_Mode_EEPROM should be set to 0 for CAN mode
       * communication. This ensures that the inverter is not relying on BRAKE,
       * FORWARD and REVERSE SWITCHES.
       *
       * NOTE: Furthermore, Key_Switch_Mode is ineffective in CAN mode, so this
       * whole state should quickly transition to VSM_READY_STATE
       */
      disablePrechargeRelay();
      enableMainRelay();

      break;
    case VSM_STATE::VSM_READY_STATE:
      /*
       * The system is waiting for Enable Inverter Command,
       */
      disablePrechargeRelay();
      enableMainRelay();

      break;
    case VSM_STATE::VSM_MOTOR_RUNNING_STATE:
      disablePrechargeRelay();
      enableMainRelay();

      break;
    case VSM_STATE::VSM_BLINK_FAULT_CODE_STATE:
      disablePrechargeRelay();
      disableMainRelay();

      break;
    case VSM_STATE::VSM_SHUTDOWN_IN_PROCESS:
      disablePrechargeRelay();
      disableMainRelay();

      break;
    case VSM_STATE::VSM_RECYCLE_POWER_STATE:
      /*
       * This state indicates that the EEPROM Programming
       * has been successfully completed. For new EEPROM
       * values to take effect, the controller must be re-powered.
       */

      disablePrechargeRelay();
      disableMainRelay();

      break;
    default:
      break;
    }
  }
}

K_SEM_DEFINE(pulse_sem, 0, 1);

void pulse_timer_expiratory_func(k_timer *timer_id) { k_sem_give(&pulse_sem); }

K_TIMER_DEFINE(pulse_timer, pulse_timer_expiratory_func, NULL);

PulseMessage pulse_message;

void Pulse_Command() {
  // initialize values
  {
    pulse_message.torque = 0;
    pulse_message.speed = 0;
    pulse_message.direction = 1; // forward
    pulse_message.inverter_enable = 0;
    pulse_message.inverter_discharge = 0;
    pulse_message.speed_mode_enable = 0;
    pulse_message.torque_limit = MAX_TORQUE;
  }

  // start the timer
  k_timer_start(&pulse_timer, K_NO_WAIT, K_MSEC(600));

  while (true) {
    k_sem_take(&pulse_sem, K_FOREVER);
    printk("%d %d %d %d\n", pulse_message.torque, pulse_message.speed,
           pulse_message.inverter_enable, pulse_message.inverter_discharge);

    Send_Command(pulse_message.torque, pulse_message.speed,
                 pulse_message.direction, pulse_message.inverter_enable,
                 pulse_message.inverter_discharge,
                 pulse_message.speed_mode_enable, pulse_message.torque_limit);
  }
}

void Clear_Faults() { Send_Parameter(0x20, 1, 0x0); }
