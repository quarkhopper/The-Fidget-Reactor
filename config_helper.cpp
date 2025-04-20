#include "config_helper.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "nlohmann/json.hpp"
#include "pipe_bus_client.hpp" // Updated to use PipeBusClient instead of BusClient

nlohmann::json ConfigHelper::loadControllerConfig(const std::string& controllerName, const std::string& configFilePath) {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open configuration file: " + configFilePath);
    }

    nlohmann::json configJson;
    configFile >> configJson;

    for (const auto& controller : configJson["peripheral_controllers"]) {
        if (controller["name"] == controllerName) {
            return controller;
        }
    }

    throw std::runtime_error("Controller configuration not found for: " + controllerName);
}

void ConfigHelper::validateWiring(const nlohmann::json& wiringConfig, const nlohmann::json& componentsConfig) {
    for (const auto& [controllerName, pins] : wiringConfig.items()) {
        std::cout << "Validating wiring for: " << controllerName << std::endl;

        auto it = std::find_if(componentsConfig.begin(), componentsConfig.end(), [&](const nlohmann::json& component) {
            return component["name"] == controllerName;
        });

        if (it == componentsConfig.end()) {
            std::cerr << "Error: Component not found for wiring validation: " << controllerName << std::endl;
            continue;
        }

        for (const auto& [pin, alias] : pins.items()) {
            if (std::find((*it)["chip"]["pins"].begin(), (*it)["chip"]["pins"].end(), pin) == (*it)["chip"]["pins"].end()) {
                std::cerr << "Error: Pin " << pin << " not found in " << controllerName << std::endl;
            }
        }
    }
}

void ConfigHelper::initializePipes(const nlohmann::json& config) {
    // Create pipes for controller communication
    if (config.contains("shared_pipe")) {
        std::cout << "[ConfigHelper] Initializing shared pipe: " << config["shared_pipe"].get<std::string>() << std::endl;
        // Logic to create the shared pipe (controller_bus)
    }

    // Create pipes for physical layer communication
    if (config.contains("wiring")) {
        for (const auto& [controller, pins] : config["wiring"].items()) {
            for (const auto& [pin, details] : pins.items()) {
                if (details.contains("pipe")) {
                    std::cout << "[ConfigHelper] Initializing physical pipe: " << details["pipe"].get<std::string>() << std::endl;
                    // Logic to create the physical pipe
                }
            }
        }
    }
}

void ConfigHelper::setupPinSimWiring(const nlohmann::json& wiringConfig, std::unordered_map<std::string, PinSim>& pinSims) {
    for (const auto& [controller, pins] : wiringConfig.items()) {
        for (const auto& [pin, details] : pins.items()) {
            if (details.contains("signal")) {
                std::string signal = details["signal"].get<std::string>();
                std::cout << "[ConfigHelper] Setting up PinSim for signal: " << signal << " on pin: " << pin << std::endl;
                pinSims[signal] = PinSim(pin);
            }
        }
    }
}

void ConfigHelper::setupControllerBus(const nlohmann::json& config, PipeBusClient& busClient) {
    if (config.contains("shared_pipe")) {
        std::string sharedPipe = config["shared_pipe"].get<std::string>();
        std::cout << "[ConfigHelper] Setting up controller bus on shared pipe: " << sharedPipe << std::endl;
        // Logic to initialize the BusClient with the shared pipe
    }
}