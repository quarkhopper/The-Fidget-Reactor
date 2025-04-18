#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "configurable_component.hpp"
#include "controller_manager.hpp"  // To include the ControllerManager for the external reference

class PeripheralController : public ConfigurableComponent {
public:
    PeripheralController();
    ~PeripheralController() override; // Virtual destructor
    void onPinWrite(const std::string& pin, bool rawState);
    void tick(int frame);
    void loadFromConfig(const nlohmann::json& obj) override;
    
    bool hasPin(const std::string& pin) const override;

private:
    struct DebounceState {
        bool currentStable = false;
        bool lastRaw = false;
        int stableFrames = 0;
    };

    std::unordered_map<std::string, DebounceState> pinStates;
    std::unordered_map<std::string, std::string> pinLabels;  // To store pin labels

    int debounceThreshold = 3;

    void emitToMain(const std::string& pin, bool state);

};
extern ControllerManager controller;  // Add this to reference the global controller
