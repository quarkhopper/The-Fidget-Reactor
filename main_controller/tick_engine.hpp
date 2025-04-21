// tick_engine.hpp
#pragma once

#include <iostream>
#include <vector>
#include "controller_core.hpp"
#include "subsystems/subsystem.hpp"

namespace tickEngine {

    class TickEngine {
    public:
        TickEngine(ControllerState& state)
            : state(state) {}

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
        }

    private:
        ControllerState& state;
        std::vector<Subsystem*> subsystems;
    };

} // namespace tickEngine