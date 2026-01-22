#include "peripheral_controller.hpp"

#include "can_utility.hpp"
#include "syscalls/device.h"
#include "zephyr/devicetree.h"
#include "zephyr/drivers/pwm.h"
#include "zephyr/dt-bindings/gpio/gpio.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"
#include "zephyr/sys/util.h"
#include <zephyr/input/input.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/input/input.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/state.h>

#include <zephyr/drivers/gpio.h>

// Logging
LOG_MODULE_REGISTER(peripheral_logs, LOG_LEVEL_INF);

// ---------------------------------------------------------

static const pwm_dt_spec pre_charge_relay_pwm =
    PWM_DT_SPEC_GET(DT_ALIAS(precharge_relay));

static const gpio_dt_spec main_relay =
    GPIO_DT_SPEC_GET(DT_ALIAS(main_relay), gpios);

static const gpio_dt_spec faults_led =
    GPIO_DT_SPEC_GET(DT_ALIAS(faults_led), gpios);

static const device *rotary_encoder = DEVICE_DT_GET(DT_ALIAS(rotary_encoder));

static const int32_t GPIO_OUTPUT_CNT = 2;
static const gpio_dt_spec *gpio_output_devices[GPIO_OUTPUT_CNT] = {&main_relay,
                                                                   &faults_led};
// ---------------------------------------------------------

static const gpio_dt_spec inverter_enable_switch =
    GPIO_DT_SPEC_GET(DT_ALIAS(inv_en_sw), gpios);

static const gpio_dt_spec speed_mode_switch =
    GPIO_DT_SPEC_GET(DT_ALIAS(speed_mode_sw), gpios);

static const gpio_dt_spec faults_clear_sw =
    GPIO_DT_SPEC_GET(DT_ALIAS(faults_clear_sw), gpios);

static const gpio_dt_spec discharge_clear_sw =
    GPIO_DT_SPEC_GET(DT_ALIAS(discharge_sw), gpios);

static const gpio_dt_spec kill_switch =
    GPIO_DT_SPEC_GET(DT_ALIAS(kill_switch), gpios);

static const int32_t GPIO_INPUT_CNT = 5;
static const gpio_dt_spec *gpio_input_devices[GPIO_INPUT_CNT] = {
    &inverter_enable_switch, &speed_mode_switch, &faults_clear_sw,
    &discharge_clear_sw, &kill_switch};

static const int32_t gpio_input_flags[GPIO_INPUT_CNT] = {
    GPIO_INT_EDGE_TO_ACTIVE, GPIO_INT_EDGE_TO_ACTIVE, GPIO_INT_EDGE_TO_ACTIVE,
    GPIO_INT_EDGE_TO_ACTIVE, GPIO_INT_EDGE_BOTH};

// ---------------------------------------------------------

gpio_callback inv_switch_cb;
gpio_callback speedmode_switch_cb;
gpio_callback faults_clear_cb;
gpio_callback discharge_cb;
gpio_callback killswitch_cb;

gpio_callback *switch_callbacks[GPIO_INPUT_CNT] = {
    &inv_switch_cb, &speedmode_switch_cb, &faults_clear_cb, &discharge_cb,
    &killswitch_cb};

// ---------------------------------------------------------

void inv_switch_handler(struct k_work *work) {
  pulse_message.torque = pulse_message.speed = 0;
  pulse_message.inverter_enable ^= 1;
}

static K_WORK_DELAYABLE_DEFINE(inv_switch_worker, inv_switch_handler);

void inv_switch_isr(const struct device *dev, struct gpio_callback *cb,
                    uint32_t pins) {
  k_work_reschedule(&inv_switch_worker, K_MSEC(300));
}

// ---------------------------------------------------------

void speedmode_switch_handler(struct k_work *work) {
  pulse_message.torque = pulse_message.speed = 0;
  pulse_message.speed_mode_enable ^= 1;
}

static K_WORK_DELAYABLE_DEFINE(speedmode_switch_worker,
                               speedmode_switch_handler);

void speedmode_switch_isr(const struct device *dev, struct gpio_callback *cb,
                          uint32_t pins) {

  k_work_reschedule(&speedmode_switch_worker, K_MSEC(300));
}

// ---------------------------------------------------------

void faults_clear_handler(struct k_work *work) { Clear_Faults(); }

static K_WORK_DELAYABLE_DEFINE(faults_clear_worker, faults_clear_handler);

void faults_clear_isr(const struct device *dev, struct gpio_callback *cb,
                      uint32_t pins) {
  k_work_reschedule(&faults_clear_worker, K_MSEC(300));
}

// ---------------------------------------------------------

void discharge_switch_handler(struct k_work *work) {
  pulse_message.inverter_discharge ^= 1;
}

static K_WORK_DELAYABLE_DEFINE(discharge_switch_worker,
                               discharge_switch_handler);

void discharge_switch_isr(const struct device *dev, struct gpio_callback *cb,
                          uint32_t pins) {

  k_work_reschedule(&discharge_switch_worker, K_MSEC(300));
}

// ---------------------------------------------------------

void killswitch_values_refresh() {
  int32_t ret = gpio_pin_get_dt(&kill_switch);

  if (ret < 0) {
    LOG_ERR("Could not read value from the kill switch!");
  } else if (ret == 0) {
    LOG_INF("Relays off!");
    disablePrechargeRelay();
    disableMainRelay();
  } else {
    LOG_INF("Relays on!");
    enablePrechargeRelay();
    enableMainRelay();
  }
}

void killswitch_handler(struct k_work *work) { killswitch_values_refresh(); }

static K_WORK_DELAYABLE_DEFINE(killswitch_worker, killswitch_handler);

void killswitch_isr(const struct device *dev, struct gpio_callback *cb,
                    uint32_t pins) {
  k_work_reschedule(&killswitch_worker, K_MSEC(300));
}

// ---------------------------------------------------------

typedef void (*isr_t)(const struct device *, struct gpio_callback *, uint32_t);

static isr_t all_isrs[] = {inv_switch_isr, speedmode_switch_isr,
                           faults_clear_isr, discharge_switch_isr,
                           &killswitch_isr};

// ---------------------------------------------------------

int32_t initPeripherals() {
  if (!pwm_is_ready_dt(&pre_charge_relay_pwm)) {
    LOG_ERR("PWM device not ready");
    return -1;
  }
  disablePrechargeRelay();

  if (!device_is_ready(rotary_encoder)) {
    LOG_ERR("Rotary is not ready!");
    return -1;
  }

  for (size_t i{0}; i < GPIO_OUTPUT_CNT; i++) {
    if (!gpio_is_ready_dt(gpio_output_devices[i])) {
      LOG_ERR("GPIO output %d was not ready.", i);
      return -1;
    }

    if (gpio_pin_configure_dt(gpio_output_devices[i], GPIO_OUTPUT_ACTIVE) < 0) {
      LOG_ERR("Could not configure GPIO output pin %d", i);
    }
    if (gpio_pin_set_dt(gpio_output_devices[i], false) < 0) {
      LOG_ERR("Could not set value to GPIO output pin %d", i);
    }
  }

  for (size_t i{0}; i < GPIO_INPUT_CNT; i++) {
    if (!gpio_is_ready_dt(gpio_input_devices[i])) {
      LOG_ERR("GPIO input %d was not ready!", i);
      return -1;
    }
    if (gpio_pin_configure_dt(gpio_input_devices[i],
                              (GPIO_INPUT | GPIO_PULL_UP)) < 0) {
      LOG_ERR("Could not configure GPIO input pin %d", i);
    }

    if (gpio_pin_interrupt_configure_dt(gpio_input_devices[i],
                                        gpio_input_flags[i]) < 0) {
      LOG_ERR("Could not configure interrupt for GPIO input pin %d", i);
    }

    if (gpio_add_callback(gpio_input_devices[i]->port, switch_callbacks[i])) {
      LOG_ERR("Could not add a callback for GPIO input pin %d", i);
    }

    gpio_init_callback(switch_callbacks[i], all_isrs[i],
                       BIT(gpio_input_devices[i]->pin));
  }
  killswitch_values_refresh();

  return 0;
}

// ---------------------------------------------------------

void enablePrechargeRelay() {
  if (pwm_set_dt(&pre_charge_relay_pwm, PWM_PERIOD, PWM_DUTY_PERIOD) < 0) {
    LOG_ERR("Could not activate the PWM pin!");
  }
}

void disablePrechargeRelay() {
  if (pwm_set_dt(&pre_charge_relay_pwm, PWM_PERIOD, 0) < 0) {
    LOG_ERR("Could not de-activate the PWM pin!");
  }
}

// ---------------------------------------------------------

void enableMainRelay() {
  if (gpio_pin_set_dt(&main_relay, true) < 0) {
    LOG_ERR("Could not activate main relay!");
  }
}

void disableMainRelay() {
  if (gpio_pin_set_dt(&main_relay, false) < 0) {
    LOG_ERR("Could not de-activate the main relay!");
  }
}
// ---------------------------------------------------------

void enableFaultsLed() {
  if (gpio_pin_set_dt(&faults_led, true) < 0) {
    LOG_ERR("Could not enable the fault LED!");
  }
}

void disableFaultsLed() {
  if (gpio_pin_set_dt(&faults_led, false) < 0) {
    LOG_ERR("Could not de-activate the fault LED");
  }
}

// ---------------------------------------------------------

static void qdec_cb(struct input_event *evt, void *user_data) {
  if (pulse_message.speed_mode_enable) {
    pulse_message.speed =
        MAX(MIN(pulse_message.speed + evt->value, MAX_SPEED), 0);
  } else {
    pulse_message.torque = MAX(
        MIN(pulse_message.torque + (TORQUE_SCALER * evt->value), MAX_TORQUE),
        0);
  }
}

INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_ALIAS(rotary_encoder)), qdec_cb, NULL);
