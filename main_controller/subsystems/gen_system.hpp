// gen_system.hpp
#pragma once

#include <iostream>
#include "../controller_core.hpp"
#include "subsystem.hpp"

class GenSystem : public Subsystem {
public:
GenSystem(ControllerState& state)
        : state(state) {}

    void initialize() override {
        std::cout << "[GenSystem] Initialized.\n";
    }

    void on_tick() override {
        std::cout << "[GenSystem] Tick.\n";
    }

private:
    ControllerState& state;
};