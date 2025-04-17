// controller_manager.cpp
#include "controller_manager.hpp"
#include <iostream>

// Define the enum inside the class
enum class State {
    OFF,
    INIT,
    STARTUP,
    ON
};

// Static state variable for managing controller state
static State currentState = State::OFF;

void ControllerManager::onPinChange(const std::string& pin, bool value) {
    if (pin == "MASTER" && value == true && currentState == State::OFF) {
        std::cout << "[PC1] Signal received: MASTER pin HIGH" << std::endl;
        currentState = State::INIT;
        std::cout << "[PC1] Controller state: OFF → INIT" << std::endl;
    }
}

void ControllerManager::tick() {
    if (currentState == State::INIT) {
        currentState = State::STARTUP;
        std::cout << "[PC1] Controller state: INIT → STARTUP" << std::endl;
    } else if (currentState == State::STARTUP) {
        currentState = State::ON;
        std::cout << "[PC1] Controller state: STARTUP → ON" << std::endl;
    }
}
