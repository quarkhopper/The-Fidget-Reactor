// pin_sim.cpp
#include "pin_sim.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>

PinSim::PinSim(const std::string& pinName, const std::string& pipeName)
    : pinName(pinName), pipeName(pipeName), currentState(false) {}

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
    // Write to the named pipe
    writeToPipe(pipeName, pinName, currentState);
}

void writeToPipe(const std::string& pipeName, const std::string& pinName, bool state) {
    std::cout << "[PinSim] Writing to pipe " << pipeName << " for pin " << pinName << ": " << (state ? "HIGH" : "LOW") << std::endl;
    
    // Create the pipe path
    std::string pipePath = "\\\\.\\pipe\\" + pipeName;
    
    // Open the pipe for writing
    HANDLE hPipe = CreateFileA(
        pipePath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "[PinSim] Failed to open pipe " << pipePath << ". Error: " << GetLastError() << std::endl;
        return;
    }
    
    // Create the message
    std::string message = pinName + ":" + (state ? "HIGH" : "LOW");
    
    // Write to the pipe
    DWORD bytesWritten;
    BOOL success = WriteFile(
        hPipe,
        message.c_str(),
        static_cast<DWORD>(message.length()),
        &bytesWritten,
        NULL
    );
    
    if (!success) {
        std::cerr << "[PinSim] Failed to write to pipe. Error: " << GetLastError() << std::endl;
    }
    
    // Close the pipe
    CloseHandle(hPipe);
}

void emitPinState(const std::string& pinName, const std::string& pipeName, bool state) {
    std::cout << "[PinSim] Emitting state for pin " << pinName << " on pipe " << pipeName << ": " << (state ? "HIGH" : "LOW") << std::endl;
    writeToPipe(pipeName, pinName, state);
}