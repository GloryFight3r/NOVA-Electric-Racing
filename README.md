# Nova Electric Racing – ECU Firmware

This repository contains the firmware for the **Electronic Control Unit (ECU)** developed by the **Nova Electric Racing Student Team**  

URL to team website: https://novaracing.nl

## Scope

This repository focuses on low-level ECU firmware, including:
- Hardware abstraction and peripheral drivers
- Real-time task scheduling and communication
- Inverter and motor control interfaces

Higher-level vehicle logic, telemetry visualization, and mechanical design are out of scope.

## Technologies
### Hardware
- The firmware is intended to run on an **STM32F767ZI** microcontroller and is currently developed and tested using the **STM32 NUCLEO-144 MB1137** development board. 
(ST user manual: https://www.st.com/resource/en/user_manual/um1974-stm32-nucleo144-boards-mb1137-stmicroelectronics.pdf)

- Motor: EMRAX 268 Medium Voltage
- Inverter Cascadia Rinehart PM150DZR
- BMS (Battery Management System) - Orion BMS2

### Software
Zephyr RTOS is used to meet hard real-time requirements imposed by the inverter, BMS, and motor control loops. Its preemptive scheduler, device tree system, and driver model simplify deterministic task execution and peripheral configuration compared to bare-metal approaches.

### Peripherals
CAN protocol facilitates the communication between the BMS, the inverter and the ECU. 
UART is used for the serial communication between the ECU and the serial port of the PC when monitoring the logs.

## Repository Structure

```text
.
├── include/             # Public headers
├── src/                 # Firmware source code
├── CMakeLists.txt       # Build configuration
├── prj.conf             # Project configuration
├── app.overlay          # Board/application DTS
├── .gitignore
└── README.md
```

## Project Status
The codebase is **under active development**.

The next planned milestone is:
- **Calibration of the EMRAX 268 electric motor**(success)
- **Initial motor spin test**(success)
- 
!(Video of the test)[https://drive.google.com/file/d/1JdEKr2Tl9ZLnUU5_6DwbyOzA3YaV_2Uz/view?usp=drive_link]

## Getting started 
- Clone the repository
- Download and install Zephyr SDK
- Navigate to Zephyr installation directory, using the command

```
cd ~/zephyrproject
```
- Activate the virtual environment
```
source .venv/bin/activate
```
- Build the project
```
west build -p always -b nucleo_f767zi ~/<path-to-the-cloned-project>
```
- Flash the built project to the board
```
west flash -r openocd
```

## Monitoring the logs
I recommend using minicom for establishing the serial communication between the PC and the ECU. Any other 
serial communication program should also work.

## Notes
This project is part of an ongoing student-led electric motorcycle development effort.  
Interfaces, parameters, and control logic are expected to evolve as testing progresses.

