#include "inverter_broadcast.h"
#include <stdbool.h>

void Parse_Internal_States(uint8_t *arr) {
  enum VSM_STATE vsm_state = arr[0];

  if (arr[0] == 14) {
    vsm_state = VSM_SHUTDOWN_IN_PROCESS;
  } else if (arr[0] == 15) {
    vsm_state = VSM_RECYCLE_POWER_STATE;
  }

  uint8_t pwm_frequency = arr[1];

  enum INV_STATE inv_state = arr[2];

  switch (arr[2]) {
  case 0:
    inv_state = INV_POWER_ON_STATE;
    break;
  case 1:
    inv_state = INV_STOP_STATE;
    break;
  case 2:
    inv_state = INV_OPEN_LOOP_STATE;
    break;
  case 3:
    inv_state = INV_CLOSED_LOOP_STATE;
    break;
  case 4:
    inv_state = INV_WAIT_STATE;
    break;
  case 5:
  case 6:
  case 7:
    inv_state = INV_INTERNAL_STATES;
    break;
  case 8:
    inv_state = INV_IDLE_RUN_STATE;
    break;
  case 9:
    inv_state = INV_IDLE_STOP_STATE;
    break;
  case 10:
  case 11:
  case 12:
    inv_state = INV_INTERNAL_STATES;
    break;
  default:
    inv_state = INV_UNKNOWN_STATE;
    break;
  }

  uint8_t inverter_run_mode = arr[3];

  bool self_sensing_assist_enable = arr[4];

  enum INV_ACTIVE_DISCHARGE_STATE active_discharge_state = arr[5];

  bool command_mode = arr[6];

  struct Internal_States internal_state = {vsm_state,
                                           pwm_frequency,
                                           inv_state,
                                           inverter_run_mode,
                                           self_sensing_assist_enable,
                                           active_discharge_state,
                                           command_mode

  };
}
