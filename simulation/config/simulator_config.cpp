// simulator_config.cpp
#include "simulator_config.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

void SimulatorConfig::registerComponent(const std::string& name, ConfigurableComponent* component) {
    components[name] = component;
}

void SimulatorConfig::loadAllConfigs(const std::string& configDir) {
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

            // Optional: log chip type if declared
            if (configJson.contains("chip")) {
                const auto& chip = configJson["chip"];
                std::string type = chip.value("type", "unknown");
                int mhz = chip.value("frequency_mhz", 0);
                std::cout << "[SimulatorConfig] " << name << " is a " << type << " @ " << mhz << " MHz" << std::endl;
            }

            auto it = components.find(name);
            if (it != components.end()) {
                std::cout << "[SimulatorConfig] Loading config for: " << name << std::endl;
                it->second->loadFromConfig(configJson);
            } else {
                std::cerr << "[SimulatorConfig] No registered component for: " << name << std::endl;
            }
        }
    }
}