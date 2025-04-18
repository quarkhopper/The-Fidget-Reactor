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
    // Enum to represent different states of the controller
    enum class State {
        OFF,
        INIT,
        STARTUP,
        ON
    };

    ControllerManager();  // Constructor to initialize components and subsystems

    void start();  // Start system method
    void tick(int tickCount);  // Method for handling tick logic
    void onPinChange(const std::string& pin, bool state);  // Method for handling pin change

    // Static state variable for managing controller state
    static State currentState;

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

    // Simulate button press method for testing purposes
    void simulate_button_press(int tick);
};
