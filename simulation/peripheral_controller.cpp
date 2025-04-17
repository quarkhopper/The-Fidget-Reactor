// peripheral_controller.cpp
#include "peripheral_controller.hpp"
#include "controller_manager.hpp"
#include <iostream>

extern ControllerManager controller;

PeripheralController::PeripheralController() {}

void PeripheralController::onPinWrite(const std::string& pin, bool rawState) {
    auto& state = pinStates[pin];
    if (rawState != state.lastRaw) {
        state.lastRaw = rawState;
        state.stableFrames = 0;
    }
}

void PeripheralController::tick(int frame) {
    for (auto& [pin, state] : pinStates) {
        if (state.lastRaw == state.currentStable) {
            state.stableFrames = 0; // still same as stable
            continue;
        }

        state.stableFrames++;
        if (state.stableFrames >= debounceThreshold) {
            state.currentStable = state.lastRaw;
            emitToMain(pin, state.currentStable);
        }
    }
}

void PeripheralController::emitToMain(const std::string& pin, bool state) {
    std::cout << "[PHC_A] Debounced " << pin << " → " << (state ? "HIGH" : "LOW") << std::endl;
    controller.onPinChange(pin, state);
}
