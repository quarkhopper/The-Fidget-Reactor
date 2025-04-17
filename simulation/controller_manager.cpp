// controller_manager.cpp
#include "controller_manager.hpp"
#include <iostream>

namespace ControllerManager {

    enum class State {
        OFF,
        INIT,
        STARTUP,
        ON
    };

    static State currentState = State::OFF;

    void onPinChange(const std::string& pin, bool value) {
        if (pin == "MASTER" && value == true && currentState == State::OFF) {
            std::cout << "[PC1] Signal received: MASTER pin HIGH" << std::endl;
            currentState = State::INIT;
            std::cout << "[PC1] Controller state: OFF → INIT" << std::endl;
        }
    }

    void tick() {
        if (currentState == State::INIT) {
            currentState = State::STARTUP;
            std::cout << "[PC1] Controller state: INIT → STARTUP" << std::endl;
        } else if (currentState == State::STARTUP) {
            currentState = State::ON;
            std::cout << "[PC1] Controller state: STARTUP → ON" << std::endl;
        }
    }

} // namespace ControllerManager