// phc.cpp - Peripheral Hardware Controller simulator (for one component)

#include <iostream>
#include <string>
#include <filesystem>
#include <thread>      // for std::this_thread::sleep_for
#include <chrono>      // for std::chrono::milliseconds
#include <fstream>     // for std::ifstream
#include "nlohmann/json.hpp" // for JSON parsing
#include "../interfaces/base_controller.hpp"
#include "../config/config_helper.hpp"

using json = nlohmann::json;

class PHC : public BaseController {
public:
    void loadFromJson(const nlohmann::json& config) override {
        controllerName = config["name"];
        debounceThreshold = config.value("debounce_threshold", 3);

        for (const auto& [pin, label] : config["pin_map"].items()) {
            pinStates[pin] = {false, false, 0};
            pinLabels[pin] = label;
        }
    }

    void tick(int frame) override {
        for (auto& [pin, state] : pinStates) {
            // Simulate pin state changes and debounce logic
            bool rawState = simulatePinState(pin);
            if (rawState != state.lastRaw) {
                state.stableFrames = 0;
                state.lastRaw = rawState;
            } else {
                state.stableFrames++;
                if (state.stableFrames >= debounceThreshold && state.currentStable != rawState) {
                    state.currentStable = rawState;
                    emitToMain(pin, rawState);
                }
            }
        }
    }

private:
    struct DebounceState {
        bool currentStable;
        bool lastRaw;
        int stableFrames;
    };

    std::unordered_map<std::string, DebounceState> pinStates;
    std::unordered_map<std::string, std::string> pinLabels;
    int debounceThreshold;

    bool simulatePinState(const std::string& pin) {
        // Placeholder for actual pin state simulation logic
        return false;
    }

    void emitToMain(const std::string& pin, bool state) {
        std::cout << "[PHC] Pin " << pin << " (" << pinLabels[pin] << ") changed to " << state << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: phc.exe <controller_name>" << std::endl;
        return 1;
    }

    std::string controllerName = argv[1];

    try {
        auto controllerConfig = ConfigHelper::loadControllerConfig(controllerName, "config/simulation_config.json");

        PHC controller;
        controller.loadFromJson(controllerConfig);

        std::cout << "[PHC] Peripheral controller for '" << controllerName << "' starting." << std::endl;

        int frame = 0;
        while (true) {
            controller.tick(frame++);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    } catch (const std::exception& e) {
        std::cerr << "[PHC] Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
