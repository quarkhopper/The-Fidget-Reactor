// simulator_config.hpp

#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
#include "../interfaces/configurable_component.hpp"

class SimulatorConfig {
public:
    // Register a component with the given name
    static void registerComponent(const std::string& name, ConfigurableComponent* component);
    
    // Load all configuration files from the given directory
    static void loadAllConfigs(const std::string& configDir);
    
    // Validate the wiring file against registered components
    static void validateWiring(const std::string& wiringFile);

private:
    // Static map to store all registered components by name
    static std::unordered_map<std::string, ConfigurableComponent*> components;
};
