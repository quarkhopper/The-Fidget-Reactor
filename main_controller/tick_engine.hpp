// tick_engine.hpp
#pragma once

#include <iostream>
#include <vector>
#include "controller_core.hpp"
#include "message_bus.hpp"
#include "subsystem.hpp"

namespace tickEngine {

    class TickEngine {
    public:
        TickEngine(ControllerState& state, MessageBus& bus)
            : state(state), bus(bus) {}

        void register_subsystem(Subsystem* subsystem) {
            subsystems.push_back(subsystem);
        }

        void initialize_all() {
            for (Subsystem* s : subsystems) {
                s->initialize();
            }
        }

        void tick() {
            std::cout << "[tickEngine] Tick executed.\n";

            for (Subsystem* s : subsystems) {
                s->on_tick();
            }

            bus.clear();
        }

    private:
        ControllerState& state;
        MessageBus& bus;
        std::vector<Subsystem*> subsystems;
    };

} // namespace tickEngine