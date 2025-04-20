#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "pin_sim.hpp"
#include "pipe_bus_client.hpp" // Updated to use PipeBusClient instead of BusClient

class ConfigHelper {
public:
    // Load a specific controller's configuration from the JSON file
    static nlohmann::json loadControllerConfig(const std::string& controllerName, const std::string& configFilePath);

    // Validate wiring configuration
    static void validateWiring(const nlohmann::json& wiringConfig, const nlohmann::json& componentsConfig);

    static void initializePipes(const nlohmann::json& config);
    static void setupPinSimWiring(const nlohmann::json& wiringConfig, std::unordered_map<std::string, PinSim>& pinSims);
    static void setupControllerBus(const nlohmann::json& config, PipeBusClient& busClient);
};