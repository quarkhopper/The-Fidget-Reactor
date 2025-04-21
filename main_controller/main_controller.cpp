#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "tick_engine.hpp"
#include "init_manager.hpp"
#include "subsystems/core_system.hpp"
#include "subsystems/ctrl_system.hpp"
#include "subsystems/gen_system.hpp"
#include "subsystems/xfer_system.hpp"
#include "../bus/pipe_bus_client.hpp"


int main() {
    std::cout << "Starting Fidget Reactor simulation...\n";
    int tickCount = 0;

    ControllerState state;
    auto bus_client = std::make_shared<PipeBusClient>("init"); 
    InitManager testInit(state);
    tickEngine::TickEngine engine(state);

    CoreSystem core(state);
    CtrlSystem ctrl(state);
    GenSystem gen(state);
    XferSystem xfer(state);

    engine.register_subsystem(&core);
    engine.register_subsystem(&ctrl);
    engine.register_subsystem(&gen);
    engine.register_subsystem(&xfer);

    engine.initialize_all();

    std::cout << "Simulation complete.\n";
    return 0;
}
