// simulator_config.cpp

#include "simulator_config.hpp"
#include "peripheral_controller.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

// Static map to store all components
std::unordered_map<std::string, ConfigurableComponent*> SimulatorConfig::components;

void SimulatorConfig::registerComponent(const std::string& name, ConfigurableComponent* component) {
    components[name] = component;
}

void SimulatorConfig::loadAllConfigs(const std::string& configDir) {
    // Iterate through the config directory
    for (const auto& entry : fs::directory_iterator(configDir)) {
        if (entry.path().extension() == ".json") {
            std::ifstream inFile(entry.path());
            if (!inFile) {
                std::cerr << "[SimulatorConfig] Failed to open " << entry.path() << std::endl;
                continue;
            }

            nlohmann::json configJson;
            inFile >> configJson;

            std::string name = entry.path().stem().string();

            // Log chip type if it's available in the config
            if (configJson.contains("chip")) {
                const auto& chip = configJson["chip"];
                std::string type = chip.value("type", "unknown");
                int mhz = chip.value("frequency_mhz", 0);
                std::cout << "[SimulatorConfig] " << name << " is a " << type << " @ " << mhz << " MHz" << std::endl;
            }

            // Dynamically instantiate components based on the config
            if (name == "phc_a") {
                static PeripheralController phcA;
                registerComponent(name, &phcA);
                phcA.loadFromConfig(configJson);
            }
            // Add additional components here...

            // Validate wiring for each component
            validateWiring("config/wiring.json");
        }
    }
}

void SimulatorConfig::validateWiring(const std::string& wiringFile) {
    std::ifstream inFile(wiringFile);
    if (!inFile) {
        std::cerr << "[SimulatorConfig] Failed to open wiring file" << std::endl;
        return;
    }

    nlohmann::json wiringJson;
    inFile >> wiringJson;

    // Iterate through the wiring configuration and validate pin mappings
    for (auto& [controllerName, pins] : wiringJson.items()) {
        std::cout << "[SimulatorConfig] Validating wiring for: " << controllerName << std::endl;

        // Check if the pins exist in the component configuration
        for (auto& [pin, alias] : pins.items()) {
            if (components.find(controllerName) != components.end()) {
                auto component = components[controllerName];
                if (!component->hasPin(pin)) {
                    std::cerr << "[SimulatorConfig] Error: Pin " << pin << " not found in " << controllerName << std::endl;
                }
            }
        }
    }
}
