#include <iostream>
#include <thread>
#include <chrono>
#include "tick_engine.hpp"

int main() {
    std::cout << "Starting Fidget Reactor simulation...\n";
    int tickCount = 0;

    ControllerState state;
    MessageBus bus;
    tickEngine::TickEngine engine(state, bus);

    // Simulate power button press at tick 0 and 5
    auto simulateButtonPress = [&](int tick) {
        if (tick == 0 || tick == 5) {
            bus.pushInbound({ MessageType::BUTTON_PRESS, 0 });
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
