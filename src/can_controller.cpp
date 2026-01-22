#include "can_controller.hpp"
#include "can_utility.hpp"
#include "inverter_broadcast_parse.hpp"
#include "inverter_command.hpp"
#include "peripheral_controller.hpp"
#include "zephyr/device.h"
#include "zephyr/devicetree.h"
#include "zephyr/drivers/can.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"

//----------------------------------------------------------------------
// DEBUG Defines

LOG_MODULE_REGISTER(can_message_parse, LOG_LEVEL_INF);

#define ERROR_DBG
#define STATUS_DBG
#define PARAMETER_RECEIVED_DBG
#define VOLTAGE_DBG

//----------------------------------------------------------------------

const device *can_dev = DEVICE_DT_GET(DT_NODELABEL(can1));

POSSIBLE_FAULTS possible_faults_buff[64];
size_t faults_buffer_size = 0;

struct can_rx_item {
  uint32_t msg_id;
  uint8_t data[8];
};

K_MSGQ_DEFINE(can_rx_q, sizeof(can_rx_item), 256, 4);

void CAN_Parse_Thread(void *p1, void *p2, void *p3) {
  can_rx_item item;
  while (true) {

    k_msgq_get(&can_rx_q, &item, K_FOREVER);

    switch (static_cast<CANMessageTypes>(item.msg_id)) {
    case CANMessageTypes::INTERNAL_STATES: {
      Internal_States parsed_internal_state = Parse_Internal_States(item.data);
      if (k_msgq_put(&state_transition_messages, &parsed_internal_state,
                     K_MSEC(30)) < 0) {
        LOG_ERR("Could not send a state transition message in time!");
      }

#ifdef STATUS_DBG

      for (const auto &x : vsm_state_info_table) {
        if (x.val == uint8_t(parsed_internal_state.vsm_state)) {
          LOG_INF("VSM State:%s -- Inverter Enable State:%d -- Relay States:%u",
                  x.str, parsed_internal_state.inverter_enable_state,
                  int32_t(parsed_internal_state.relay_state));
        }
      }

#endif

      break;
    }
    case CANMessageTypes::FAULT_CODES: {
      Fault_Codes fault_codes = Parse_Fault_Codes(item.data);

      size_t err_size = Check_Fault_Codes(fault_codes);

      if (err_size) {
        // enableFaultsLed();
#ifdef ERROR_DBG
        for (size_t i = 0; i < err_size; i++) {
          for (const auto &x : possible_faults_table) {
            if (x.mask == uint64_t(possible_faults_buff[i])) {
              LOG_INF("Error of type: %s - occured", x.name);
            }
          }
        }
#endif
      } else {
        disableFaultsLed();
      }

      break;
    }
    case CANMessageTypes::MOTOR_POSITION_INFORMATION: {
      Motor_Position_Information motor_position_information =
          Parse_Motor_Position_Information(item.data);
      break;
    }
    case CANMessageTypes::VOLTAGE_INFORMATION: {

      Voltage_Information voltage_information =
          Parse_Voltage_Information(item.data);

#ifdef VOLTAGE_DBG
      LOG_INF("Voltage Information: DC BUS Voltage: %d, Output Voltage: %d",
              voltage_information.dc_bus_voltage,
              voltage_information.output_voltage);
#endif

      break;
    }
    case CANMessageTypes::PARAMETER_MESSAGE: {
      uint16_t parameter_address = 0;
      int16_t data = 0;
      bool success = false;
      Parse_Parameter_Message(item.data, &parameter_address, &success, &data);
#ifdef PARAMETER_RECEIVED_DBG
      LOG_INF("Response from parameter message: %u %d %d", parameter_address,
              success, data);
#endif

      break;
    }
    default:
      break;
    }
  }
}

/*
 * Receive CAN messages from the inverter and enqueue them in the parser's
 * queue.
 */
void rx_callback_function(const struct device *dev, struct can_frame *frame,
                          void *user_data) {
  can_rx_item item = can_rx_item{
      .msg_id = frame->id,
      .data = {frame->data[0], frame->data[1], frame->data[2], frame->data[3],
               frame->data[4], frame->data[5], frame->data[6], frame->data[7]}};

  if (k_msgq_put(&can_rx_q, &item, K_NO_WAIT) < 0) {
    LOG_ERR("Could not send received CAN message to the parse queue!");
  }
}

int32_t CAN_Initialize() {

  if (!device_is_ready(can_dev)) {
    printk("CAN device is not ready!\n");
    LOG_ERR("CAN device is not ready!");
    return -1;
  }

  if (can_set_mode(can_dev, CAN_MODE_NORMAL) != 0) {
    LOG_ERR("Error setting CAN mode!");
    return -1;
  }

  if (can_start(can_dev) != 0) {
    LOG_ERR("Couldn't start CAN device");
    return -1;
  }

  const can_filter my_filter = {.id = 0, .mask = 0, .flags = CAN_FILTER_IDE};

  if (can_add_rx_filter(can_dev, rx_callback_function, nullptr, &my_filter) <
      0) {
    LOG_ERR("Could not add a callback function");
  }

  return 0;
}

void CAN_Send_Message(uint32_t address, const uint8_t message[]) {
  struct can_frame frame = {
      .id = address,
      .dlc = 8,
      .flags = CAN_FRAME_IDE,
      .data = {message[0], message[1], message[2], message[3], message[4],
               message[5], message[6], message[7]},
  };
  int32_t ret = can_send(can_dev, &frame, K_MSEC(20), NULL, NULL);
  if (ret < 0) {
    LOG_ERR("Could not send CAN message!");
  }
}
