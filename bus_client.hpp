#pragma once

#include <functional>
#include <string>
#include <mutex>
#include <iostream>

namespace reactor {
    class Message;  // forward declare to avoid including pb.h yet
}

class BusClient {
protected:
    static std::mutex busMutex; // Shared mutex for synchronizing access to the bus

public:
    virtual ~BusClient() = default;

    virtual void send(const reactor::Message& message) = 0;

    virtual void on_receive(std::function<void(const reactor::Message&)> handler) = 0;

    virtual const std::string& id() const = 0;
};
