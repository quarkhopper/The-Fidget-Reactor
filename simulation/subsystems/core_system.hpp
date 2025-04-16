// core_system.hpp
#pragma once

#include <iostream>
#include "../controller_core.hpp"
#include "../message_bus.hpp"
#include "subsystem.hpp"

class CoreSystem : public Subsystem {
public:
    CoreSystem(ControllerState& state)
        : state(state) {}

    void initialize() override {
        std::cout << "[CoreSystem] Initialized.\n";
    }

    void on_tick() override {
        std::cout << "[CoreSystem] Tick.\n";
    }

    void handle_message(const Message& msg) override {
        std::cout << "[CoreSystem] Received message: type="
                  << static_cast<int>(msg.type) << "\n";
    }

private:
    ControllerState& state;
};
