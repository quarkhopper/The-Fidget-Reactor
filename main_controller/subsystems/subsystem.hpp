// subsystem.hpp
#pragma once

class Subsystem {
public:
    virtual void initialize() = 0;
    virtual void on_tick() = 0;
    virtual ~Subsystem() = default;
};