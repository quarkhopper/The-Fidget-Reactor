// configurable_component.hpp
#pragma once
#include <nlohmann/json.hpp>

class ConfigurableComponent {
public:
    virtual void loadFromConfig(const nlohmann::json& obj) = 0;
    virtual ~ConfigurableComponent() = default;
};
