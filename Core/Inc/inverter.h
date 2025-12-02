#ifndef __INV__
#define __INV__

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
  VSM_UNKNOWN_STATE
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
  INV_UNKNOWN_STATE
};

struct Internal_States {
  enum VSM_STATE VSM_State;
  uint8_t PWM_Frequency;
  enum INV_STATE Inverter_State;
  uint8_t Relay_State;
  bool Inverter_Run_Mode;
  bool Self_Sensing_Assist_Enable;
  uint8_t Inverter_Active_Discharge_State;
};

#ifdef __cplusplus
}
#endif

#endif /* __INV__ */
