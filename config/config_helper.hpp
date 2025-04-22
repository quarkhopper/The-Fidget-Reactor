#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "../bus/pin_sim.hpp"
#include "../bus/pipe_bus_client.hpp"
#include <windows.h>

class ConfigHelper {
public:
    // Load a specific controller's configuration from the JSON file
    static nlohmann::json loadControllerConfig(const std::string& controllerName, const std::string& configFilePath);

    // Validate wiring configuration
    static void validateWiring(const nlohmann::json& wiringConfig, const nlohmann::json& componentsConfig);

    // Initialize named pipes from configuration
    static void initializePipes(const nlohmann::json& config);
    
    // Create a named pipe with the given name
    static void createNamedPipe(const std::string& pipeName);

    // Set up PinSim wiring based on configuration
    static void setupPinSimWiring(const nlohmann::json& wiringConfig, std::unordered_map<std::string, PinSim>& pinSims);
    
    // Set up controller bus client
    static void setupControllerBus(const nlohmann::json& config, PipeBusClient& busClient);
};