#include "config_helper.hpp" // Corrected include path
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "nlohmann/json.hpp"
#include "../bus/pin_sim.hpp" // Updated include path for pin_sim.hpp
#include "../bus/pipe_bus_client.hpp" // Updated to use PipeBusClient instead of BusClient

nlohmann::json ConfigHelper::loadControllerConfig(const std::string& controllerName, const std::string& configFilePath) {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open configuration file: " + configFilePath);
    }

    nlohmann::json configJson;
    configFile >> configJson;

    if (configJson.contains("main_controller") && controllerName == "main_controller") {
        return configJson["main_controller"];
    }

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
    // Create pipes for controller communication (controller_pipes)
    if (config.contains("controller_pipes")) {
        for (const auto& pipeName : config["controller_pipes"]) {
            std::string pipe = pipeName.get<std::string>();
            std::cout << "[ConfigHelper] Initializing controller pipe: " << pipe << std::endl;
            
            // Create the named pipe
            createNamedPipe(pipe);
        }
    }

    // Create pipes for physical layer communication (phy_pipes)
    if (config.contains("phy_pipes")) {
        for (const auto& pipeName : config["phy_pipes"]) {
            std::string pipe = pipeName.get<std::string>();
            std::cout << "[ConfigHelper] Initializing physical pipe: " << pipe << std::endl;
            
            // Create the named pipe
            createNamedPipe(pipe);
        }
    }
}

void ConfigHelper::createNamedPipe(const std::string& pipeName) {
    std::string pipePath = "\\\\.\\pipe\\" + pipeName;
    
    HANDLE hPipe = CreateNamedPipeA(
        pipePath.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        1024,  // Output buffer size
        1024,  // Input buffer size
        0,     // Default timeout
        NULL   // Security attributes
    );
    
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "[ConfigHelper] Failed to create pipe " << pipePath << ". Error: " << GetLastError() << std::endl;
        return;
    }
    
    std::cout << "[ConfigHelper] Successfully created pipe: " << pipePath << std::endl;
    
    // Store the pipe handle for cleanup later
    // This would require adding a static map to store pipe handles
    // pipeHandles[pipeName] = hPipe;
    
    // For simplicity, we're not keeping track of the handles which means they won't be properly closed
    // In a real implementation, you'd want to store these and close them on shutdown
}

void ConfigHelper::setupPinSimWiring(const nlohmann::json& wiringConfig, std::unordered_map<std::string, PinSim>& pinSims) {
    for (const auto& [signal, details] : wiringConfig.items()) {
        if (details.contains("target") && details.contains("pipe")) {
            std::string target = details["target"].get<std::string>();
            std::string pipe = details["pipe"].get<std::string>();
            std::cout << "[ConfigHelper] Setting up PinSim for signal: " << signal << " targeting: " << target << " via pipe: " << pipe << std::endl;
            pinSims.emplace(signal, PinSim(target, pipe));
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