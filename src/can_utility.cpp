#include "can_utility.hpp"
#include "inverter_broadcast.hpp"
#include "inverter_command.hpp"
#include "zephyr/kernel.h"

K_SEM_DEFINE(pulse_sem, 0, 1);

void pulse_timer_expiratory_func(k_timer *timer_id) { k_sem_give(&pulse_sem); }

K_TIMER_DEFINE(pulse_timer, pulse_timer_expiratory_func, NULL);

VSM_STATE ecu_vsm_state = VSM_STATE::VSM_UNKNOWN_STATE;

void State_Transition() {
  Internal_States internal_states_snapshot = internal_states;

  /*
   * SW USER MANUAL, Page 28
   *
   * NOTE: It is possible to send a command to the inverter to enable the relay
   * contactors using CAN messages. That way we can get 12V power driver,
   * instead of relying on MOSFET drivers
   */
  switch (internal_states_snapshot.vsm_state) {
  case VSM_STATE::VSM_START_STATE:

    // HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

    break;
  case VSM_STATE::VSM_PRE_CHARGE_INIT_STATE:
    /*
     * Performs VDC checks and if the voltage is under the threshold,
     * transition to VSM_PRE_CHARGE_ACTIVE_STATE
     */
    break;
  case VSM_STATE::VSM_PRE_CHARGE_ACTIVE_STATE:
    // HAL_GPIO_WritePin(
    //     GPIOF, GPIO_PIN_13,
    //     GPIO_PIN_RESET); // turn off the main circuit relay, just in case
    // HAL_TIM_PWM_Start(&htim1,
    //                   TIM_CHANNEL_1); // activate the pre-charge circuit
    //                   relay

    break;
  case VSM_STATE::VSM_PRE_CHARGE_COMPLETE_STATE:
    // HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); // de-activate the pre-charge
    // relay HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13,
    //                  GPIO_PIN_SET); // turn on the main circuit relay
    /*
     * also preforms stability check on the voltage. Once
     * checks are completed transition to VSM_WAIT_STATE
     */
    break;
  case VSM_STATE::VSM_WAIT_STATE:
    /*
     * NOTE: Inv_Cmd_Mode_EEPROM should be set to 0 for CAN mode communication.
     * This ensures that the inverter is not relying on BRAKE, FORWARD and
     * REVERSE SWITCHES.
     *
     * NOTE: Furthermore, Key_Switch_Mode is ineffective in CAN mode, so this
     * whole state should quickly transition to VSM_READY_STATE
     */
    break;
  case VSM_STATE::VSM_READY_STATE:
    /*
     * The system is waiting for Enable Inverter Command,
     */
    break;
  case VSM_STATE::VSM_MOTOR_RUNNING_STATE:
    break;
  case VSM_STATE::VSM_BLINK_FAULT_CODE_STATE:
    // HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

    break;
  case VSM_STATE::VSM_SHUTDOWN_IN_PROCESS:
    // HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

    break;
  case VSM_STATE::VSM_RECYCLE_POWER_STATE:
    break;
  default:
    break;
    // printf("Unknown vsm state!\n\r");
  }

  ecu_vsm_state = internal_states_snapshot.vsm_state;
}

void Pulse_Command() {
  while (true) {
    k_sem_take(&pulse_sem, K_FOREVER);

    Send_Command(10, 0, 1, 1, 0, 0, 0);
  }
}
