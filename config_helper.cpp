#include "config_helper.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

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