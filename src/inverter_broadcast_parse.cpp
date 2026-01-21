#include "inverter_broadcast_parse.hpp"
#include "can_controller.hpp"
#include <stdbool.h>
#include <stddef.h>

Internal_States Parse_Internal_States(uint8_t *arr) {
  VSM_STATE vsm_state = static_cast<VSM_STATE>(arr[0]);

  uint8_t pwm_frequency = arr[1];

  INV_STATE inv_state = static_cast<INV_STATE>(arr[2]);

  switch (arr[2]) {
  case 0:
    inv_state = INV_STATE::INV_POWER_ON_STATE;
    break;
  case 1:
    inv_state = INV_STATE::INV_STOP_STATE;
    break;
  case 2:
    inv_state = INV_STATE::INV_OPEN_LOOP_STATE;
    break;
  case 3:
    inv_state = INV_STATE::INV_CLOSED_LOOP_STATE;
    break;
  case 4:
    inv_state = INV_STATE::INV_WAIT_STATE;
    break;
  case 5:
  case 6:
  case 7:
    inv_state = INV_STATE::INV_INTERNAL_STATES;
    break;
  case 8:
    inv_state = INV_STATE::INV_IDLE_RUN_STATE;
    break;
  case 9:
    inv_state = INV_STATE::INV_IDLE_STOP_STATE;
    break;
  case 10:
  case 11:
  case 12:
    inv_state = INV_STATE::INV_INTERNAL_STATES;
    break;
  default:
    inv_state = INV_STATE::INV_UNKNOWN_STATE;
    break;
  }

  uint8_t relay_state = arr[3];

  bool inverter_run_mode = arr[4] & (1 << 0);

  bool self_sensing_assist_enable = arr[4] & (1 << 1);

  INV_ACTIVE_DISCHARGE_STATE active_discharge_state =
      static_cast<INV_ACTIVE_DISCHARGE_STATE>(arr[4] >> 5);

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

  return Internal_States{
      .vsm_state = vsm_state,
      .pwm_frequency = pwm_frequency,
      .inverter_state = inv_state,
      .relay_state = relay_state,
      .run_mode = inverter_run_mode,
      .self_sensing_assist_enable = self_sensing_assist_enable,
      .discharge_state = active_discharge_state,
      .command_mode = command_mode,
      .rolling_counter_value = rolling_counter_value,
      .inverter_enable_state = inverter_enable_state,
      .burst_model_mode = burst_model_mode,
      .start_mode_active = start_mode_active,
      .inverter_enable_lockout = inverter_enable_lockout,
      .direction_command = direction_command,
      .bms_active = bms_active,
      .bms_limiting_torque = bms_limiting_torque,
      .limit_max_speed = limit_max_speed,
      .limit_hot_spot = limit_hot_spot,
      .low_speed_limiting = low_speed_limit,
      .coolant_temperature_limiting = coolant_temp_limiting,
      .limit_stall_burst_model = limit_stall_burst_model,
  };
}

Fault_Codes Parse_Fault_Codes(uint8_t *arr) {
  uint64_t fault_codes_as_uint64[8] = {0, 0, 0, 0, 0, 0, 0, 0};

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

  return Fault_Codes{.mask = final_mask};
}

size_t Check_Fault_Codes(Fault_Codes fault_codes) {
  faults_buffer_size = 0;
  for (int i = 0; i < 64; i++) {
    if (fault_codes.mask & (((uint64_t)1) << i)) {
      possible_faults_buff[faults_buffer_size++] =
          static_cast<POSSIBLE_FAULTS>(uint64_t{1} << i);
    }
  }
  return faults_buffer_size;
}

Motor_Position_Information Parse_Motor_Position_Information(uint8_t *arr) {
  int16_t motor_angle = ((int16_t)arr[0]) | (((int16_t)arr[1]) << 8);
  int16_t motor_speed = ((int16_t)arr[2]) | (((int16_t)arr[3]) << 8);
  int16_t electrical_output_freq = ((int16_t)arr[4]) | (((int16_t)arr[5]) << 8);
  int16_t delta_resolver_filtered =
      ((int16_t)arr[6]) | (((int16_t)arr[7]) << 8);

  return Motor_Position_Information{
      .motor_angle = motor_angle,
      .motor_speed = motor_speed,
      .electrical_output_freq = electrical_output_freq,
      .delta_resolver_filtered = delta_resolver_filtered,
  };
}

Voltage_Information Parse_Voltage_Information(uint8_t *arr) {
  int16_t dc_buc_voltage = ((int16_t)arr[0]) | (((int16_t)arr[1]) << 8);
  int16_t output_voltage = ((int16_t)arr[2]) | (((int16_t)arr[3]) << 8);
  int16_t vab_vd_voltage = ((int16_t)arr[4]) | (((int16_t)arr[5]) << 8);
  int16_t vbc_vq_voltage = ((int16_t)arr[6]) | (((int16_t)arr[7]) << 8);

  return Voltage_Information{
      .dc_bus_voltage = dc_buc_voltage,
      .output_voltage = output_voltage,
      .vab_vd_voltage = vab_vd_voltage,
      .vbc_vq_voltage = vbc_vq_voltage,
  };
}
