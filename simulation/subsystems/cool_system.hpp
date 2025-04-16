// cool_system.hpp
#pragma once

#include "../controller_core.hpp"
#include "../message_bus.hpp"
#include "subsystem.hpp"
#include <iostream>

class CoolSystem : public Subsystem {
public:
    CoolSystem(ControllerState& state)
        : state(state) {}

    void initialize() override {
        std::cout << "[CoolSystem] Initialized.\n";
    }

    void on_tick() override {
        std::cout << "[CoolSystem] Tick.\n";
    }

    void handle_message(const Message& msg) override {
        std::cout << "[CoolSystem] Received message: type="
                  << static_cast<int>(msg.type) << "\n";
    }

private:
    ControllerState& state;
};