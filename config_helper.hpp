#pragma once
#include <string>
#include <nlohmann/json.hpp>

class ConfigHelper {
public:
    // Load a specific controller's configuration from the JSON file
    static nlohmann::json loadControllerConfig(const std::string& controllerName, const std::string& configFilePath);

    // Validate wiring configuration
    static void validateWiring(const nlohmann::json& wiringConfig, const nlohmann::json& componentsConfig);
};