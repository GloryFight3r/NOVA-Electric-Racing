#ifndef __INV_BROADCAST__
#define __INV_BROADCAST__

#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

enum VSM_STATE {
  VSM_START_STATE,
  VSM_PRE_CHARGE_INIT_STATE,
  VSM_PRE_CHARGE_ACTIVE_STATE,
  VSM_PRE_CHARGE_COMPLETE_STATE,
  VSM_WAIT_STATE,
  VSM_READY_STATE,
  VSM_MOTOR_RUNNING_STATE,
  VSM_BLINK_FAULT_CODE_STATE,
  VSM_SHUTDOWN_IN_PROCESS,
  VSM_RECYCLE_POWER_STATE,
  VSM_UNKNOWN_STATE,
};

enum INV_STATE {
  INV_POWER_ON_STATE,
  INV_STOP_STATE,
  INV_OPEN_LOOP_STATE,
  INV_CLOSED_LOOP_STATE,
  INV_WAIT_STATE,
  INV_INTERNAL_STATES,
  INV_IDLE_RUN_STATE,
  INV_IDLE_STOP_STATE,
  INV_UNKNOWN_STATE,
};

enum INV_ACTIVE_DISCHARGE_STATE {
  DISCHARGE_DISABLED,
  DISCHARGE_ENABLED,
  PERFORMING_SPEED_CHECK,
  DISCHARGE_ACTIVELY_OCCURING,
  DISCHARGE_COMPLETED,
};

enum INVERTER_RUN_MODE { TORQUE_MODE, SPEED_MODE };

/*
 * 0x0AA
 */
struct Internal_States {
  enum VSM_STATE vsm_state;

  /*
   * FOR GEN5/CM THE PWM FREQUENCY THAT IS CURRENTLY BEING USED IS REPORTED.
   * For PMxxx inverters this value show as 0.
   */
  uint8_t pwm_frequency;

  enum INV_STATE inverter_state;

  uint8_t relay_state;

  /*
   * 0 = Torque Mode
   * 1 = Speed Mode
   */
  bool run_mode;

  bool self_sensing_assist_enable;

  enum INV_ACTIVE_DISCHARGE_STATE discharge_state;

  /*
   * 0 = CAN
   * 1 = VSM
   */
  bool command_mode;

  /*
   * ONLY AVAILABLE IN GEN5/CM
   * The value of the currently expected Rolling Counter value.
   */
  int8_t rolling_counter_value;

  /*
   * 0 - Disabled
   * 1 - Enabled
   */
  bool inverter_enable_state;

  /*
   * For GEN5 / CM
   * 0 = Stall
   * 1 = High Speed
   */
  bool burst_model_mode;

  /*
   * 0 = start signal has not been activated.
   * 1 = start signal has been activated.
   */
  bool start_mode_active;

  /*
   * 0 = inverter can be enabled.
   * 1 = inverter cannot be enabled.
   */
  bool inverter_enable_lockout;

  /*
   * 1 = Forward
   * 0 = Reverse
   */
  bool direction_command;

  /*
   * 0 = BMS message is not being received.
   * 1 = BMS message is being received.
   */
  bool bms_active;

  /*
   * 0 = Torque is not being limited by the BMS.
   * 1 = Torque is being limited by the BMS.
   */
  bool bms_limiting_torque;

  /*
   * ONLY AVAILABLE IN GEN5
   * 0 = No torque limiting is occurring.
   * 1 = Torque limiting is occuring due to the motor speed exceeding the
   * maximum motor speed.
   */
  bool limit_max_speed;

  /*
   * ONLY AVAILABLE IN GEN5
   */
  bool limit_hot_spot;

  /*
   * ONLY AVAILABLE IN GEN5
   * 0 = Low speed current limiting is not occuring.
   * 1 = Low speed current limiting is occuring.
   */
  bool low_speed_limiting;

  /*
   * ONLY AVAILABLE IN GEN5
   * The bit indicates that the maximum motor current is being limited due to
   * coolant temperature.
   */
  bool coolant_temperature_limiting;

  /*
   * ONLY AVAILABLE IN GEN5
   * 0 = Not limiting.
   * 1 = Limiting.
   * Indicates if Stall Burst Model is limiting the current. For select
   * inverters only.
   */
  bool limit_stall_burst_model;
};

enum POST_FAULTS {
  ERR_POST_HARDWARE_GATE_DESATURATION = 1ULL << 0,
  ERR_HW_OVER_CURRENT = 1ULL << 1,
  ERR_ACCELERATION_SHORTED = 1ULL << 2,
  ERR_ACCELERATION_OPEN = 1ULL << 3,
  ERR_CURRENT_SENSOR_LOW = 1ULL << 4,
  ERR_CURRENT_SENSOR_HIGH = 1ULL << 5,
  ERR_MODULE_TEMPERATURE_LOW = 1ULL << 6,
  ERR_MODULE_TEMPERATURE_HIGH = 1ULL << 7,
  ERR_CONTROL_PCB_TEMPERATURE_LOW = 1ULL << 8,
  ERR_CONTROL_PCB_TEMPERATURE_HIGH = 1ULL << 9,
  ERR_GATE_DRIVE_PCB_TEMPERATURE_LOW = 1ULL << 10,
  ERR_GATE_DRIVE_PCB_TEMPERATURE_HIGH = 1ULL << 11,
  ERR_5V_SENSE_VOLTAGE_LOW = 1ULL << 12,
  ERR_5V_SENSE_VOLTAGE_HIGH = 1ULL << 13,
  ERR_12V_SENSE_VOLTAGE_LOW = 1ULL << 14,
  ERR_12V_SENSE_VOLTAGE_HIGH = 1ULL << 15,
  ERR_2V5_SENSE_VOLTAGE_LOW = 1ULL << 16,
  ERR_2V5_SENSE_VOLTAGE_HIGH = 1ULL << 17,
  ERR_1V5_SENSE_VOLTAGE_LOW = 1ULL << 18,
  ERR_1V5_SENSE_VOLTAGE_HIGH = 1ULL << 19,
  ERR_DC_BUS_VOLTAGE_HIGH = 1ULL << 20,
  ERR_DC_BUS_VOLTAGE_LOW = 1ULL << 21,
  ERR_PRE_CHARGE_TIMEOUT = 1ULL << 22,
  ERR_PRE_CHARGE_VOLTAGE_FAILURE = 1ULL << 23,
  ERR_EEPROM_CHECKSUM_FAULED = 1ULL << 24,
  ERR_EEPROM_DATA_OUT_OF_RANGE = 1ULL << 25,
  ERR_EEPROM_UPDATE_REQUIRED = 1ULL << 26,
  ERR_HARDWARE_DC_BUS_OVER_VOLTAGE_DURING_INITIALIZATION = 1ULL << 27,
  ERR_BRAKE_SHORTED = 1ULL << 30,
  ERR_BRAKE_OPEN = 1ULL << 31,
};

enum RUN_FAULTS {
  ERR_MOTOR_OVER_SPEED = 1ULL << 32,
  ERR_OVER_CURRENT = 1ULL << 33,
  ERR_OVER_VOLTAGE = 1ULL << 34,
  ERR_INVERTER_OVER_TEMPERATURE = 1ULL << 35,
  ERR_ACCELERATOR_INPUT_SHORTED_FAULT = 1ULL << 36,
  ERR_ACCELERATOR_INPUT_OPEN_FAULT = 1ULL << 37,
  ERR_DIRECTION_COMMAND_FAULT = 1ULL << 38,
  ERR_INVERTER_RESPONSE_TIME_OUT = 1ULL << 39,
  ERR_RUN_HARDWARE_GATE_DESATURATION = 1ULL << 40,
  ERR_HARDWARE_OVER_CURRENT = 1ULL << 41,
  ERR_UNDER_VOLTAGE = 1ULL << 42,
  ERR_CAN_COMMAND_MESSAGE_LOST = 1ULL << 43,
  ERR_MOTOR_OVER_TEMPERATURE = 1ULL << 44,
  ERR_BRAKE_INPUT_SHORTED = 1ULL << 48,
  ERR_BRAKE_INPUT_OPEN = 1ULL << 49,
  ERR_MODULE_A_OVER_TEMPERATURE = 1ULL << 50,
  ERR_MODULE_B_OVER_TEMPERATURE = 1ULL << 51,
  ERR_MODULE_C_OVER_TEMPERATURE = 1ULL << 52,
  ERR_PCB_OVER_TEMPERATURE = 1ULL << 53,
  ERR_GATE_DRIVE_BOARD_1_OVER_TEMPERATURE = 1ULL << 54,
  ERR_GATE_DRIVE_BOARD_2_OVER_TEMPERATURE = 1ULL << 55,
  ERR_GATE_DRIVE_BOARD_3_OVER_TEMPERATURE = 1ULL << 56,
  ERR_CURRENT_SENSOR = 1ULL << 57,
  ERR_RESOLVER_NOT_CONNECTED = 1ULL << 62,
};

/*
 * 0x0AB
 *
 * Bits correspond to POST_FAULTS and RUN_FAULTS
 */
struct Fault_Codes {
  uint64_t mask;
};

/*
 * 0x0AC
 */
struct Torque_And_Timer_Info {
  /*
   * The commanded torque.
   *
   * Format: Torque
   */
  int8_t commanded_torque;

  /*
   * The estimated motor torque based on
   * motor parameters and feedbacks.
   *
   * Format: Torque
   */
  int8_t torque_feedback;

  /*
   * This timer is updated every 3 msec. This
   * timer will roll-over in approximately 5
   * months. The timer will reset to 0 at power
   * on. Monitoring this can be useful to show
   * when a reset of the processor has occurred.
   *
   * Format: (Counts x .003) sec
   */
  int8_t power_on_timer;
};

/*
 * 0x0B0
 */
struct High_Speed_Message {
  /*
   * The commanded torque.
   *
   * Format: Torque
   */
  int8_t commanded_torque;

  /*
   * The estimated motor torque.
   *
   * Format: Torque
   */
  int8_t torque_feedback;

  /*
   * The measured motor speed.
   *
   * Format: Angular Velocity
   */
  int8_t motor_speed;

  /*
   * The actual measured DC bus voltage;
   *
   * Format: High Voltage
   */
  int8_t dc_bus_voltage;
};

/*
 * 0x0B1
 */
struct Torque_Capability {
  /*
   * The inverter's available torque.
   *
   * Format: Torque
   */
  int8_t torque_capability;
};

#ifdef __cplusplus
}
#endif

#endif /* __INV_BROADCAST__ */
