// init_manager.cpp
#include "init_manager.hpp"
#include <iostream>

void InitManager::begin() {
    std::cout << "[InitManager] begin() called.\n";
    // Add your initialization logic here
}

InitManager::InitManager(ControllerState& state) : state(state) {}