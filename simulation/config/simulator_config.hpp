// simulator_config.hpp
#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
#include "../interfaces/configurable_component.hpp"

class SimulatorConfig {
public:
    void registerComponent(const std::string& name, ConfigurableComponent* component);
    void loadAllConfigs(const std::string& configDir);

private:
    std::unordered_map<std::string, ConfigurableComponent*> components;
};