// pin_sim.hpp
#pragma once
#include <string>

class PinSim {
public:
    PinSim(const std::string& pinName);

    // Simulate a bouncy signal for the pin
    void simulateBouncySignal(int durationMs, int intervalMs);

private:
    std::string pinName;
    bool currentState;

    // Toggle the pin state and emit the new state
    void toggleState();

    // Emit the current state (e.g., log or notify listeners)
    void emitState();
};

void emitPinState(const std::string& pinName, bool state);
