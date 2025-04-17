// pin_sim.cpp
#include "pin_sim.hpp"
#include "controller_manager.hpp"
#include <iostream>

void emitPinState(const std::string& pinName, bool state) {
    std::cout << "[sim] Pin " << pinName << " is now " << (state ? "HIGH" : "LOW") << std::endl;
    extern ControllerManager controller;

    controller.onPinChange(pinName, state);
}