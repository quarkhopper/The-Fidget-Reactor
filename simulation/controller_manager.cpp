#include "controller_manager.hpp"
#include <iostream>

// Initialize the static state variable
ControllerManager::State ControllerManager::currentState = ControllerManager::State::OFF;

ControllerManager::ControllerManager() 
    : engine(state, bus), core(state), ctrl(state), gen(state), xfer(state),
      init(state, bus), startup(state, bus), test(state, bus), shutdown(state, bus) {

    engine.register_subsystem(&core);
    engine.register_subsystem(&ctrl);
    engine.register_subsystem(&gen);
    engine.register_subsystem(&xfer);
}

void ControllerManager::onPinChange(const std::string& pin, bool value) {
    if (pin == "MASTER" && value == true && ControllerManager::currentState == ControllerManager::State::OFF) {
        std::cout << "[PC1] Signal received: MASTER pin HIGH" << std::endl;
        ControllerManager::currentState = ControllerManager::State::INIT;  // Use the correct static variable reference
        std::cout << "[PC1] Controller state: OFF → INIT" << std::endl;
    }
}

void ControllerManager::tick(int tickCount) {
    simulate_button_press(tickCount);
    engine.tick();
}

void ControllerManager::start() {
    std::cout << "[ControllerManager] Starting system...\n";
    engine.initialize_all();

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

void ControllerManager::simulate_button_press(int tick) {
    if (tick == 0 || tick == 5) {
        bus.pushInbound({
            .type = MessageType::BUTTON_PRESS,
            .sourceId = 0,
            .intValue = std::nullopt,
            .strValue = std::nullopt
        });
    }
}
