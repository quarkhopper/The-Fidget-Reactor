// controller_manager.hpp
#pragma once

#include "controller_core.hpp"
#include "message_bus.hpp"
#include "tick_engine.hpp"
#include "subsystems/core_system.hpp"
#include "subsystems/ctrl_system.hpp"
#include "subsystems/gen_system.hpp"
#include "subsystems/xfer_system.hpp"
#include "process_managers/init_manager.hpp"
#include "process_managers/startup_manager.hpp"
#include "process_managers/test_manager.hpp"
#include "process_managers/shutdown_manager.hpp"

class ControllerManager {
public:
    ControllerManager()
        : engine(state, bus),
          core(state),
          ctrl(state),
          gen(state),
          xfer(state),
          init(state, bus),
          startup(state, bus),
          test(state, bus),
          shutdown(state, bus) {

        engine.register_subsystem(&core);
        engine.register_subsystem(&ctrl);
        engine.register_subsystem(&gen);
        engine.register_subsystem(&xfer);
    }

    void start() {
        std::cout << "[ControllerManager] Starting system...\n";
        engine.initialize_all();

        // Example: wire up a complete/fault callback for init
        init.set_on_complete([this]() {
            std::cout << "[ControllerManager] Init complete.\n";
            // TODO: transition to startup phase
        });

        init.set_on_fault([this]() {
            std::cout << "[ControllerManager] Init failed.\n";
            // TODO: trigger fault behavior
        });

        init.begin();
    }

    void tick(int tickCount) {
        simulate_button_press(tickCount);
        engine.tick();
    }

private:
    ControllerState state;
    MessageBus bus;
    tickEngine::TickEngine engine;

    CoreSystem core;
    CtrlSystem ctrl;
    GenSystem gen;
    XferSystem xfer;

    InitManager init;
    StartupManager startup;
    TestManager test;
    ShutdownManager shutdown;

    void simulate_button_press(int tick) {
        if (tick == 0 || tick == 5) {
            bus.pushInbound({
                .type = MessageType::BUTTON_PRESS,
                .sourceId = 0,
                .intValue = std::nullopt,
                .strValue = std::nullopt
            });
        }
    }
};
