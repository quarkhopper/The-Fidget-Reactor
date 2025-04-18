// subsystem.hpp
#pragma once

#include "../message_bus.hpp" // <-- this brings in the Message struct

class Subsystem {
public:
    virtual void initialize() = 0;
    virtual void on_tick() = 0;
    virtual void handle_message(const Message&) = 0;
    virtual ~Subsystem() = default;
};