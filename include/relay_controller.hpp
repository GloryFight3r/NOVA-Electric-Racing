#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

// initialize
void initRelays();

// pre-charge relays
void enablePrechargeRelay();
void disablePrechargeRelay();

// main relays
void enableMainRelay();
void disableMainRelay();

#endif // !RELAY_CONTROLLER_H
