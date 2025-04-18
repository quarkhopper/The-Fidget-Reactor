// controller_core.hpp
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <optional>

// Message Types between peripherals and main controller
enum class MessageType {
    BUTTON_PRESS,
    BUTTON_RELEASE,
    SLIDER_CHANGE,
    TICK_EVENT,
    TEMP_UPDATE,
    STATE_TRANSITION,
    DISPLAY_UPDATE,
    SYSTEM_ALERT
};

struct Message {
    MessageType type;
    uint8_t sourceId = 0; // Peripheral or subsystem origin
    std::optional<int> intValue; // e.g., slider position, temperature
    std::optional<std::string> strValue; // e.g., subsystem name or note
};

// Central controller state for each tick
struct ControllerState {
    enum class Phase {
        OFF,
        INIT,
        TEST,
        STARTUP,
        ON,
        SHUTDOWN
    } phase = Phase::OFF;

    bool scramEngaged = false;
    bool allSubsystemsOnline = false;

    std::vector<Message> inboundMessages;
    std::vector<Message> outboundMessages;

    void resetMessages() {
        inboundMessages.clear();
        outboundMessages.clear();
    }
};
