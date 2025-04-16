#pragma once
#include <iostream>
#include "controller_core.hpp"
#include "message_bus.hpp"
#include "power_state_machine.hpp"

namespace tickEngine {

    class TickEngine {
    public:
        TickEngine(ControllerState& state, MessageBus& bus)
            : state(state), bus(bus), powerSM(state, bus) {}

        void tick() {
            std::cout << "[tickEngine] Tick executed.\n";

            // Handle messages like button presses
            powerSM.handleInput();

            // Advance state machine phases automatically
            powerSM.tickAdvance();

            // Clear messages for next tick
            bus.clear();
        }

    private:
        ControllerState& state;
        MessageBus& bus;
        PowerStateMachine powerSM;
    };

} // namespace tickEngine
