#include "inverter_broadcast.h"
#include <stdbool.h>
#include <stddef.h>

struct Internal_States Parse_Internal_States(uint8_t *arr) {
  enum VSM_STATE vsm_state = arr[0];

  if (arr[0] == 14) {
    vsm_state = VSM_SHUTDOWN_IN_PROCESS;
  } else if (arr[0] == 15) {
    vsm_state = VSM_RECYCLE_POWER_STATE;
  } else if (arr[0] > 7) {
    vsm_state = VSM_UNKNOWN_STATE;
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

  uint8_t relay_state = arr[3];

  bool inverter_run_mode = arr[4] & (1 << 0);

  bool self_sensing_assist_enable = arr[4] & (1 << 1);

  enum INV_ACTIVE_DISCHARGE_STATE active_discharge_state = (arr[4] >> 5);

  bool command_mode = arr[5] & (1 << 0);

  uint8_t rolling_counter_value = (arr[5] >> 4);

  bool inverter_enable_state = (arr[6] & (1 << 0));

  bool burst_model_mode = (arr[6] & (1 << 1));

  bool start_mode_active = (arr[6] & (1 << 6));

  bool inverter_enable_lockout = (arr[6] & (1 << 7));

  bool direction_command = (arr[7] & (1 << 0));

  bool bms_active = (arr[7] & (1 << 1));

  bool bms_limiting_torque = (arr[7] & (1 << 2));

  bool limit_max_speed = (arr[7] & (1 << 3));

  bool limit_hot_spot = (arr[7] & (1 << 4));

  bool low_speed_limit = (arr[7] & (1 << 5));

  bool coolant_temp_limiting = (arr[7] & (1 << 6));

  bool limit_stall_burst_model = (arr[7] & (1 << 7));

  struct Internal_States internal_state = {
      vsm_state,
      pwm_frequency,
      inv_state,
      relay_state,
      inverter_run_mode,
      self_sensing_assist_enable,
      active_discharge_state,
      command_mode,
      rolling_counter_value,
      inverter_enable_state,
      burst_model_mode,
      start_mode_active,
      inverter_enable_lockout,
      direction_command,
      bms_active,
      bms_limiting_torque,
      limit_max_speed,
      limit_hot_spot,
      low_speed_limit,
      coolant_temp_limiting,
      limit_stall_burst_model,
  };

  return internal_state;
}

struct Fault_Codes Parse_Fault_Codes(uint8_t *arr) {
  uint64_t fault_codes_as_uint64[8] = {0};

  // need to cast it into uint64_t in order to
  // make the bit shifts with width >= 8
  for (int i = 0; i < 8; i++) {
    fault_codes_as_uint64[i] = arr[i];
  }

  uint64_t final_mask =
      (fault_codes_as_uint64[0] | (fault_codes_as_uint64[1] << 8) |
       (fault_codes_as_uint64[2] << 16) | (fault_codes_as_uint64[3] << 24) |
       (fault_codes_as_uint64[4] << 32) | (fault_codes_as_uint64[5] << 40) |
       (fault_codes_as_uint64[6] << 48) | (fault_codes_as_uint64[7] << 56));

  struct Fault_Codes fault_codes = {final_mask};

  return fault_codes;
}

size_t Check_Fault_Codes(struct Fault_Codes fault_codes,
                         enum POSSIBLE_FAULTS *buff) {
  size_t k = 0;
  for (int i = 0; i < 64; i++) {
    if (fault_codes.mask & (((uint64_t)1) << i)) {
      buff[k++] = (enum POSSIBLE_FAULTS)(((uint64_t)1) << i);
    }
  }

  return k;
}

struct Motor_Position_Information
Parse_Motor_Position_Information(uint8_t *arr) {

  int16_t motor_angle = ((int16_t)arr[0]) | (((int16_t)arr[1]) << 8);
  int16_t motor_speed = ((int16_t)arr[2]) | (((int16_t)arr[3]) << 8);
  int16_t electrical_output_freq = ((int16_t)arr[4]) | (((int16_t)arr[5]) << 8);
  int16_t delta_resolver_filtered =
      ((int16_t)arr[6]) | (((int16_t)arr[7]) << 8);

  struct Motor_Position_Information motor_position_information = {
      motor_angle,
      motor_speed,
      electrical_output_freq,
      delta_resolver_filtered,
  };

  return motor_position_information;
}

struct Voltage_Information Parse_Voltage_Information(uint8_t *arr) {
  int16_t dc_buc_voltage = ((int16_t)arr[0]) | (((int16_t)arr[1]) << 8);
  int16_t output_voltage = ((int16_t)arr[2]) | (((int16_t)arr[3]) << 8);
  int16_t vab_vd_voltage = ((int16_t)arr[4]) | (((int16_t)arr[5]) << 8);
  int16_t vbc_vq_voltage = ((int16_t)arr[6]) | (((int16_t)arr[7]) << 8);

  struct Voltage_Information voltage_information = {
      dc_buc_voltage,
      output_voltage,
      vab_vd_voltage,
      vbc_vq_voltage,
  };

  return voltage_information;
}
