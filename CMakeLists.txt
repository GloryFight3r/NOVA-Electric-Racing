cmake_minimum_required(VERSION 3.20.0)

set(BOARD nucleo_f767zi)

set(CONF_FILE prj.conf)
find_package(Zephyr)
project(nova_app)

target_include_directories(app PUBLIC "include/")

target_sources(
  app PRIVATE src/main.cpp src/inverter_broadcast.cpp src/can_utility.cpp
              src/inverter_command.cpp src/can_controller.cpp)
