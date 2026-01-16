#include "relay_controller.hpp"

#include "zephyr/devicetree.h"
#include "zephyr/drivers/pwm.h"

#include <zephyr/drivers/gpio.h>

static const pwm_dt_spec pre_charge_relay_pwm =
    PWM_DT_SPEC_GET(DT_ALIAS(precharge_relay));

static const gpio_dt_spec main_relay =
    GPIO_DT_SPEC_GET(DT_ALIAS(main_relay), gpios);

void initRelays() {
  if (!pwm_is_ready_dt(&pre_charge_relay_pwm)) {
    printk("PWM device not ready\n");
    return;
  }

  pwm_set_dt(&pre_charge_relay_pwm, PWM_MSEC(40), PWM_MSEC(30));
}

void enablePrechargeRelay() {}

void disablePrechargeRelay() {}

void enableMainRelay() {}

void disableMainRelay() {}
