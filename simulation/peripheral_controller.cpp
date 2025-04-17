// peripheral_controller.cpp
#include "peripheral_controller.hpp"
#include "controller_manager.hpp"
#include <iostream>

extern ControllerManager controller;

PeripheralController::PeripheralController() {
    // Initialize pins if needed
    pinStates["PB0"] = DebounceState(); // Example pin initialization
    pinStates["PB1"] = DebounceState();
    pinStates["PB2"] = DebounceState();
}

void PeripheralController::onPinWrite(const std::string& pin, bool rawState) {
    auto& state = pinStates[pin];  // Get the pin state
    if (rawState != state.lastRaw) {
        state.lastRaw = rawState;
        state.stableFrames = 0;  // Reset the stable frame counter
    }
}

void PeripheralController::tick(int frame) {
    for (auto& [pin, state] : pinStates) {
        if (state.lastRaw == state.currentStable) {
            state.stableFrames = 0;  // No change detected
            continue;
        }

        state.stableFrames++;
        if (state.stableFrames >= debounceThreshold) {
            state.currentStable = state.lastRaw;
            emitToMain(pin, state.currentStable);  // Emit the stable pin state
        }
    }
}

void PeripheralController::emitToMain(const std::string& pin, bool state) {
    std::cout << "[PHC_A] Debounced " << pin << " → " << (state ? "HIGH" : "LOW") << std::endl;
    controller.onPinChange(pin, state);  // Inform the controller of the pin change
}

// --- NEW: hasPin() method ---
bool PeripheralController::hasPin(const std::string& pin) const {
    return pinStates.find(pin) != pinStates.end();  // Check if the pin exists in the map
}
