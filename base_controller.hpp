#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

class BaseController {
protected:
    std::string controllerName;

public:
    virtual void loadFromJson(const nlohmann::json& config) = 0;
    virtual void tick(int frame) = 0;

    void setControllerName(const std::string& name) {
        controllerName = name;
    }

    const std::string& getControllerName() const {
        return controllerName;
    }

    virtual ~BaseController() = default;
};