// controller_core.hpp
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include "message_types.hpp"

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
