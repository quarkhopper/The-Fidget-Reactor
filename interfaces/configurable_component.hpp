// configurable_component.hpp
#pragma once
#include <nlohmann/json.hpp>

class ConfigurableComponent {
public:
    virtual void loadFromConfig(const nlohmann::json& obj) = 0;
    virtual bool hasPin(const std::string& pin) const = 0;  // This method is declared in the interface
    virtual ~ConfigurableComponent() = default;
};
