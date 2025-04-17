// pin_sim.cpp
#include "pin_sim.hpp"
#include "controller_manager.hpp"
#include <iostream>
#include "controller_manager.hpp"
extern ControllerManager controller;

void emitPinState(const std::string& pinName, bool state) {
    std::cout << "[sim] Pin " << pinName << " is now " << (state ? "HIGH" : "LOW") << std::endl;
    controller.onPinChange(pinName, state);
}