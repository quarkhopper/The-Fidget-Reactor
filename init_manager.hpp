#pragma once

#include <functional>
#include <memory>
#include <string>

#include "controller_core.hpp"
#include "message_bus.hpp"
#include "bus_client.hpp"

class InitManager {
public:
    // Legacy constructor using MessageBus
    InitManager(ControllerState& state, MessageBus& bus)
        : state(state), bus(&bus) {}

    // New constructor using BusClient
    InitManager(ControllerState& state, std::shared_ptr<BusClient> bus_client);

    void begin();
    void handle_message(const Message& msg);

    void set_on_complete(std::function<void()> cb) { on_complete = cb; }
    void set_on_fault(std::function<void()> cb) { on_fault = cb; }

private:
    ControllerState& state;
    MessageBus* bus = nullptr;
    std::shared_ptr<BusClient> bus_client;

    int pending_acks = 0;

    std::function<void()> on_complete;
    std::function<void()> on_fault;
};
