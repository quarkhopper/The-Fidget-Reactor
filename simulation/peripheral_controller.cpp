#include "peripheral_controller.hpp"
#include "controller_manager.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

PeripheralController::PeripheralController() {
    // Load wiring configuration from JSON
    std::ifstream configFile("config/wiring.json");
    if (configFile.is_open()) {
        nlohmann::json configJson;
        configFile >> configJson;

        // Iterate through controllers and initialize pins
        for (const auto& controller : configJson.items()) {
            std::string controllerName = controller.key();  // "phc_a", "main_controller"
            const auto& pins = controller.value();

            // Initialize each pin based on the configuration
            for (const auto& pin : pins.items()) {
                std::string pinName = pin.key();  // Pin name like "PB0", "PD0"
                std::string label = pin.value();  // Label like "MASTER", "RX"

                pinStates[pinName] = DebounceState();  // Initialize debounce state for pin
                pinLabels[pinName] = label;  // Store the pin label
                std::cout << "[PeripheralController] Initialized pin: " << pinName 
                          << " with label: " << label << std::endl;
            }
        }
    } else {
        std::cerr << "[PeripheralController] Failed to load wiring configuration" << std::endl;
    }
}

PeripheralController::~PeripheralController() {
    // Cleanup code, if any, can go here.
    std::cout << "[PeripheralController] Destructor called." << std::endl;
}

void PeripheralController::onPinWrite(const std::string& pin, bool rawState) {
    auto& state = pinStates[pin];  // Get the pin state
    if (rawState != state.lastRaw) {
        state.lastRaw = rawState;
        state.stableFrames = 0;  // Reset the stable frame counter
    }
}

void PeripheralController::tick(int) {
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

bool PeripheralController::hasPin(const std::string& pin) const {
    return pinStates.find(pin) != pinStates.end();  // Check if the pin exists in the map
}

void PeripheralController::loadFromConfig(const nlohmann::json& obj) {
    // Implement the loading logic from the config JSON here
    // Example:
    for (const auto& pin : obj["pins"]) {
        std::string pinName = pin["name"];
        pinStates[pinName] = DebounceState(); // Initialize the pin
    }
}
