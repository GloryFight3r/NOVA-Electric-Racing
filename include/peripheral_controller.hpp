#ifndef PERIPHERAL_CONTROLLER_H
#define PERIPHERAL_CONTROLLER_H

#include "zephyr/dt-bindings/pwm/pwm.h"
#include "zephyr/kernel.h"
#include <cstdint>

// ---------------------------------------------------------

// initialize
int32_t initPeripherals();

// ---------------------------------------------------------

// pre-charge relays
const int32_t PWM_PERIOD = PWM_MSEC(1000);
const int32_t PWM_DUTY_PERIOD = PWM_MSEC(1000);
const int16_t TORQUE_SCALER = 10;

void enablePrechargeRelay();
void disablePrechargeRelay();

// ---------------------------------------------------------

// main relays
void enableMainRelay();
void disableMainRelay();

// ---------------------------------------------------------

// fault led
void enableFaultsLed();
void disableFaultsLed();

// ---------------------------------------------------------

#endif // !PERIPHERAL_CONTROLLER_H
