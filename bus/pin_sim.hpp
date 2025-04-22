// pin_sim.hpp
#pragma once
#include <string>

class PinSim {
public:
    PinSim(const std::string& pinName, const std::string& pipeName);

    // Simulate a bouncy signal for the pin
    void simulateBouncySignal(int durationMs, int intervalMs);

private:
    std::string pinName;
    std::string pipeName;
    bool currentState;

    // Toggle the pin state and emit the new state
    void toggleState();

    // Emit the current state to the pipe
    void emitState();
};

// Helper function to write to named pipe
void writeToPipe(const std::string& pipeName, const std::string& pinName, bool state);

// External function to emit pin state
void emitPinState(const std::string& pinName, const std::string& pipeName, bool state);
