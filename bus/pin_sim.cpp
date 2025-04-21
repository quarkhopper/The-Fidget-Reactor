// pin_sim.cpp
#include "pin_sim.hpp" // Corrected include path
#include <iostream>
#include <thread>
#include <chrono>

PinSim::PinSim(const std::string& pinName)
    : pinName(pinName), currentState(false) {}

void PinSim::simulateBouncySignal(int durationMs, int intervalMs) {
    auto startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() < durationMs) {
        toggleState();
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}

void PinSim::toggleState() {
    currentState = !currentState;
    emitState();
}

void PinSim::emitState() {
    std::cout << "[PinSim] Pin " << pinName << " is now " << (currentState ? "HIGH" : "LOW") << std::endl;
}

void emitPinState(const std::string& pinName, bool state) {
    std::cout << "[PinSim] Emitting state for pin " << pinName << ": " << (state ? "HIGH" : "LOW") << std::endl;
}