#ifndef INVERTER_BROADCAST_PARSE_H
#define INVERTER_BROADCAST_PARSE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Includes ------------------------------------------------------------------*/

/*
 * 0x0A5
 */
struct Motor_Position_Information {
  /*
   * The electrical angle of the motor as read
   * by the encoder or resolver.
   *
   * Format: Angle.
   */
  int16_t motor_angle;

  /*
   * The measured speed of the motor.
   *
   * Format: Angular Velocity.
   */
  int16_t motor_speed;

  /*
   * The actual electrical frequency of the ivnerter.
   *
   * Format: Frequency
   */
  int16_t electrical_output_freq;

  /*
   * This is used in calibration of resolver angle adjustment.
   * The range of this parameter is +-180 degrees. Values between
   * 180degrees and 360degrees are shown as negative angle.
   * For example 270 degrees is equal to -90 degrees, and
   * 190 degrees is equal to -170 degrees.
   */
  int16_t delta_resolver_filtered;
};

/*
 * 0x0A7
 */
struct Voltage_Information {
  /*
   * The actual measured value of the DC bus voltage.
   *
   * Format: High Voltage
   */
  int16_t dc_bus_voltage;

  /*
   * The calculated value of the output voltage, in peak line-neutral volts.
   *
   * Format: High Voltage
   */
  int16_t output_voltage;

  /*
   * Measured value of the voltage between Phase A and Phase B(VAB) when the
   * inverter is disabled. Vd voltage when the inverter is enabled.
   *
   * Format: High Voltage
   */
  int16_t vab_vd_voltage;

  /*
   * Measured value of the voltage between Phase B and Phase C(VBC) when the
   * inverter is disabled. Vq voltage when the inverted is enabled.
   *
   * Format: High Voltage
   */
  int16_t vbc_vq_voltage;
};

#define VSM_STATE_LIST(x)                                                      \
  X(VSM_START_STATE, 0)                                                        \
  X(VSM_PRE_CHARGE_INIT_STATE, 1)                                              \
  X(VSM_PRE_CHARGE_ACTIVE_STATE, 2)                                            \
  X(VSM_PRE_CHARGE_COMPLETE_STATE, 3)                                          \
  X(VSM_WAIT_STATE, 4)                                                         \
  X(VSM_READY_STATE, 5)                                                        \
  X(VSM_MOTOR_RUNNING_STATE, 6)                                                \
  X(VSM_BLINK_FAULT_CODE_STATE, 7)                                             \
  X(VSM_SHUTDOWN_IN_PROCESS, 14)                                               \
  X(VSM_RECYCLE_POWER_STATE, 15)                                               \
  X(VSM_UNKNOWN_STATE, 16)

enum class VSM_STATE : uint8_t {
#define X(name, value) name = value,
  VSM_STATE_LIST(X)
#undef X
};

struct VSM_STATE_INFO {
  uint8_t val;
  const char *str;
};

constexpr VSM_STATE_INFO vsm_state_info_table[] = {
#define X(name, value) {value, #name},
    VSM_STATE_LIST(X)
#undef X
};

enum class INV_STATE : uint8_t {
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

enum class INV_ACTIVE_DISCHARGE_STATE : uint8_t {
  DISCHARGE_DISABLED,
  DISCHARGE_ENABLED,
  PERFORMING_SPEED_CHECK,
  DISCHARGE_ACTIVELY_OCCURING,
  DISCHARGE_COMPLETED,
};

enum class INVERTER_RUN_MODE : uint8_t { TORQUE_MODE, SPEED_MODE };

/*
 * 0x0AA
 */
struct Internal_States {
  VSM_STATE vsm_state;

  /*
   * FOR GEN5/CM THE PWM FREQUENCY THAT IS CURRENTLY BEING USED IS REPORTED.
   * For PMxxx inverters this value show as 0.
   *
   * Format
   */
  uint8_t pwm_frequency;

  INV_STATE inverter_state;

  uint8_t relay_state;

  /*
   * 0 = Torque Mode
   * 1 = Speed Mode
   */
  bool run_mode;

  bool self_sensing_assist_enable;

  INV_ACTIVE_DISCHARGE_STATE discharge_state;

  /*
   * 0 = CAN
   * 1 = VSM
   */
  bool command_mode;

  /*
   * ONLY AVAILABLE IN GEN5/CM
   * The value of the currently expected Rolling Counter value.
   */
  uint8_t rolling_counter_value;

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

// possible_faults.def
#define POSSIBLE_FAULTS_LIST(X)                                                \
  /* POST FAULTS */                                                            \
  X(ERR_POST_HARDWARE_GATE_DESATURATION, 0)                                    \
  X(ERR_HW_OVER_CURRENT, 1)                                                    \
  X(ERR_ACCELERATION_SHORTED, 2)                                               \
  X(ERR_ACCELERATION_OPEN, 3)                                                  \
  X(ERR_CURRENT_SENSOR_LOW, 4)                                                 \
  X(ERR_CURRENT_SENSOR_HIGH, 5)                                                \
  X(ERR_MODULE_TEMPERATURE_LOW, 6)                                             \
  X(ERR_MODULE_TEMPERATURE_HIGH, 7)                                            \
  X(ERR_CONTROL_PCB_TEMPERATURE_LOW, 8)                                        \
  X(ERR_CONTROL_PCB_TEMPERATURE_HIGH, 9)                                       \
  X(ERR_GATE_DRIVE_PCB_TEMPERATURE_LOW, 10)                                    \
  X(ERR_GATE_DRIVE_PCB_TEMPERATURE_HIGH, 11)                                   \
  X(ERR_5V_SENSE_VOLTAGE_LOW, 12)                                              \
  X(ERR_5V_SENSE_VOLTAGE_HIGH, 13)                                             \
  X(ERR_12V_SENSE_VOLTAGE_LOW, 14)                                             \
  X(ERR_12V_SENSE_VOLTAGE_HIGH, 15)                                            \
  X(ERR_2V5_SENSE_VOLTAGE_LOW, 16)                                             \
  X(ERR_2V5_SENSE_VOLTAGE_HIGH, 17)                                            \
  X(ERR_1V5_SENSE_VOLTAGE_LOW, 18)                                             \
  X(ERR_1V5_SENSE_VOLTAGE_HIGH, 19)                                            \
  X(ERR_DC_BUS_VOLTAGE_HIGH, 20)                                               \
  X(ERR_DC_BUS_VOLTAGE_LOW, 21)                                                \
  X(ERR_PRE_CHARGE_TIMEOUT, 22)                                                \
  X(ERR_PRE_CHARGE_VOLTAGE_FAILURE, 23)                                        \
  X(ERR_EEPROM_CHECKSUM_FAULED, 24)                                            \
  X(ERR_EEPROM_DATA_OUT_OF_RANGE, 25)                                          \
  X(ERR_EEPROM_UPDATE_REQUIRED, 26)                                            \
  X(ERR_HARDWARE_DC_BUS_OVER_VOLTAGE_DURING_INITIALIZATION, 27)                \
  X(ERR_BRAKE_SHORTED, 30)                                                     \
  X(ERR_BRAKE_OPEN, 31)                                                        \
                                                                               \
  /* RUN FAULTS */                                                             \
  X(ERR_MOTOR_OVER_SPEED, 32)                                                  \
  X(ERR_OVER_CURRENT, 33)                                                      \
  X(ERR_OVER_VOLTAGE, 34)                                                      \
  X(ERR_INVERTER_OVER_TEMPERATURE, 35)                                         \
  X(ERR_ACCELERATOR_INPUT_SHORTED_FAULT, 36)                                   \
  X(ERR_ACCELERATOR_INPUT_OPEN_FAULT, 37)                                      \
  X(ERR_DIRECTION_COMMAND_FAULT, 38)                                           \
  X(ERR_INVERTER_RESPONSE_TIME_OUT, 39)                                        \
  X(ERR_RUN_HARDWARE_GATE_DESATURATION, 40)                                    \
  X(ERR_HARDWARE_OVER_CURRENT, 41)                                             \
  X(ERR_UNDER_VOLTAGE, 42)                                                     \
  X(ERR_CAN_COMMAND_MESSAGE_LOST, 43)                                          \
  X(ERR_MOTOR_OVER_TEMPERATURE, 44)                                            \
  X(ERR_BRAKE_INPUT_SHORTED, 48)                                               \
  X(ERR_BRAKE_INPUT_OPEN, 49)                                                  \
  X(ERR_MODULE_A_OVER_TEMPERATURE, 50)                                         \
  X(ERR_MODULE_B_OVER_TEMPERATURE, 51)                                         \
  X(ERR_MODULE_C_OVER_TEMPERATURE, 52)                                         \
  X(ERR_PCB_OVER_TEMPERATURE, 53)                                              \
  X(ERR_GATE_DRIVE_BOARD_1_OVER_TEMPERATURE, 54)                               \
  X(ERR_GATE_DRIVE_BOARD_2_OVER_TEMPERATURE, 55)                               \
  X(ERR_GATE_DRIVE_BOARD_3_OVER_TEMPERATURE, 56)                               \
  X(ERR_CURRENT_SENSOR, 57)                                                    \
  X(ERR_RESOLVER_NOT_CONNECTED, 62)

enum class POSSIBLE_FAULTS : uint64_t {
#define X(name, bit) name = (1ULL << bit),
  POSSIBLE_FAULTS_LIST(X)
#undef X
};

struct FaultInfo {
  uint64_t mask;
  const char *name;
};

constexpr FaultInfo possible_faults_table[] = {
#define X(name, bit) {(1ULL << bit), #name},
    POSSIBLE_FAULTS_LIST(X)
#undef X
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

Internal_States Parse_Internal_States(uint8_t *);

Fault_Codes Parse_Fault_Codes(uint8_t *);

size_t Check_Fault_Codes(Fault_Codes fault_codes);

Motor_Position_Information Parse_Motor_Position_Information(uint8_t *);

Voltage_Information Parse_Voltage_Information(uint8_t *);

#endif /* INVERTER_BROADCAST_PARSE_H */
