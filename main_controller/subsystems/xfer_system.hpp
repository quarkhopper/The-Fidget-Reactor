// xfer_system.hpp
#pragma once

#include <iostream>
#include "../controller_core.hpp"
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

private:
    ControllerState& state;
};