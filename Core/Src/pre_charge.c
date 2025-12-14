#include "pre_charge.h"
#include "inverter_broadcast.h"
#include "stm32f767xx.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_tim.h"
#include "tim.h"
#include <stdio.h>

enum VSM_STATE ecu_vsm_state = VSM_UNKNOWN_STATE;

void Walk_Through_Precharge() {
  struct Internal_States internal_states_snapshot = internal_states;

  switch (internal_states_snapshot.vsm_state) {
  case VSM_START_STATE:
    if (ecu_vsm_state == VSM_START_STATE) {
      break;
    }

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

    break;
  case VSM_PRE_CHARGE_INIT_STATE:
    // turn on the pre-charge relay
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    break;
  case VSM_PRE_CHARGE_ACTIVE_STATE:
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

    break;
  case VSM_PRE_CHARGE_COMPLETE_STATE:
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);

    break;
  case VSM_WAIT_STATE:
    // choose either FORWARD or REVERSE mode

    break;
  case VSM_READY_STATE:
    break;
  case VSM_MOTOR_RUNNING_STATE:
    break;
  case VSM_BLINK_FAULT_CODE_STATE:
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

    break;
  case VSM_SHUTDOWN_IN_PROCESS:
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

    break;
  case VSM_RECYCLE_POWER_STATE:
    break;
  default:
    printf("Unknown vsm state!\n\r");
  }

  ecu_vsm_state = internal_states_snapshot.vsm_state;
}
