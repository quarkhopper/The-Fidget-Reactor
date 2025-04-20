#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "tick_engine.hpp"
#include "init_manager.hpp"
#include "core_system.hpp"
#include "ctrl_system.hpp"
#include "gen_system.hpp"
#include "xfer_system.hpp"
#include "pipe_bus_client.hpp"


int main() {
    std::cout << "Starting Fidget Reactor simulation...\n";
    int tickCount = 0;

    ControllerState state;
    auto bus_client = std::make_shared<PipeBusClient>("init"); 
    InitManager testInit(state, bus_client);
    MessageBus bus;
    tickEngine::TickEngine engine(state, bus);

    CoreSystem core(state);
    CtrlSystem ctrl(state);
    GenSystem gen(state);
    XferSystem xfer(state);

    engine.register_subsystem(&core);
    engine.register_subsystem(&ctrl);
    engine.register_subsystem(&gen);
    engine.register_subsystem(&xfer);

    engine.initialize_all();

    auto simulateButtonPress = [&](int tick) {
        if (tick == 0 || tick == 5) {
            bus.pushInbound({
                .type = MessageType::BUTTON_PRESS,
                .sourceId = 0,
                .intValue = std::nullopt,
                .strValue = std::nullopt
            });
        }
    };

    while (tickCount < 10) {
        std::cout << "Tick " << tickCount << std::endl;
        simulateButtonPress(tickCount);
        engine.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        tickCount++;
    }

    std::cout << "Simulation complete.\n";
    return 0;
}
