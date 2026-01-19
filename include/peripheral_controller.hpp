#ifndef PERIPHERAL_CONTROLLER_H
#define PERIPHERAL_CONTROLLER_H

#include "zephyr/kernel.h"
#include <cstdint>

// ---------------------------------------------------------

// initialize
int32_t initPeripherals();

// ---------------------------------------------------------

// pre-charge relays
const k_timeout_t PWM_PERIOD = K_MSEC(100);
const k_timeout_t PWM_DUTY_PERIOD = K_MSEC(50);

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
