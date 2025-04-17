// peripheral_controller.hpp
#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "configurable_component.hpp"

class PeripheralController : public ConfigurableComponent {
public:
    PeripheralController();
    void onPinWrite(const std::string& pin, bool rawState);
    void tick(int frame);
    void loadFromConfig(const nlohmann::json& obj) override;

private:
    struct DebounceState {
        bool currentStable = false;
        bool lastRaw = false;
        int stableFrames = 0;
    };

    std::unordered_map<std::string, DebounceState> pinStates;
    int debounceThreshold = 3;

    void emitToMain(const std::string& pin, bool state);
};
