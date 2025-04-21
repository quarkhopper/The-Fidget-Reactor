// ctrl_system.hpp
#pragma once

#include <iostream>
#include "controller_core.hpp"
#include "subsystem.hpp"

class CtrlSystem : public Subsystem {
public:
    CtrlSystem(ControllerState& state)
        : state(state) {}

    void initialize() override {
        std::cout << "[CtrlSystem] Initialized.\n";
    }

    void on_tick() override {
        std::cout << "[CtrlSystem] Tick.\n";
    }

private:
    ControllerState& state;
};
