// xfer_system.hpp
#pragma once

#include <iostream>
#include "controller_core.hpp"
#include "message_bus.hpp"
#include "subsystem.hpp"

class XferSystem : public Subsystem {
public:
    XferSystem(ControllerState& state)
        : state(state) {}

    void initialize() override {
        std::cout << "[XferSystem] Initialized.\n";
    }

    void on_tick() override {
        std::cout << "[XferSystem] Tick.\n";
    }

    void handle_message(const Message& msg) override {
        std::cout << "[XferSystem] Received message: type="
                  << static_cast<int>(msg.type) << "\n";
    }

private:
    ControllerState& state;
};