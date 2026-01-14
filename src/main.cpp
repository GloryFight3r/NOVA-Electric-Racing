#include "can_utility.hpp"
#include "zephyr/devicetree.h"
#include "zephyr/drivers/pwm.h"
#include "zephyr/dt-bindings/pwm/pwm.h"
#include "zephyr/kernel.h"
#include "zephyr/sys/printk.h"

#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include "can_controller.hpp"

const int32_t STACK_SIZE = 2048;

K_THREAD_DEFINE(can_thread_id, STACK_SIZE, CAN_Parse_Thread, nullptr, nullptr,
                nullptr, 3, 0, 0);
K_THREAD_DEFINE(inverter_command_id, STACK_SIZE, Pulse_Command, nullptr,
                nullptr, nullptr, 2, 0, 0);

const pwm_dt_spec pwm = PWM_DT_SPEC_GET(DT_ALIAS(mypwm));

int main() {
  const struct device *usb_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
  uint32_t dtr = 0;
  int ret = 0;

  // No need to initialize, as it happens during OS initialization, just enable
  // it.
  if (usb_enable(NULL)) {
    return 1;
  }

  // This will block until the computer opens the USB
  while (!dtr) {
    uart_line_ctrl_get(usb_dev, UART_LINE_CTRL_DTR, &dtr);
    k_sleep(K_MSEC(100));
  }

  ret = pwm_set_dt(&pwm, PWM_MSEC(20), PWM_MSEC(18));

  // CAN_Initialize();

  while (true) {
    printk("%d\n", ret);
  }

  return 0;
}
