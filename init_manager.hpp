#pragma once

#include <functional>
#include <memory>
#include <string>

#include "controller_core.hpp"

class InitManager {
public:
    // Constructor using only ControllerState
    InitManager(ControllerState& state) : state(state) {}

    void begin();
    void handle_message(const Message& msg);

    void set_on_complete(std::function<void()> cb) { on_complete = cb; }
    void set_on_fault(std::function<void()> cb) { on_fault = cb; }

private:
    ControllerState& state;

    int pending_acks = 0;

    std::function<void()> on_complete;
    std::function<void()> on_fault;
};
