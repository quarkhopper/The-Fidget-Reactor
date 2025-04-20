#pragma once
#include <string>
#include <mutex>
#include <functional>
#include "message_types.hpp" // Updated to use the new Message structure

class PipeBusClient {
public:
    PipeBusClient(const std::string& client_id);
    ~PipeBusClient();

    void send(const Message& message);
    void on_receive(std::function<void(const Message&)> handler);
    const std::string& id() const;

private:
    std::string client_id_;
    std::function<void(const Message&)> handler_;
    static std::mutex busMutex; // Shared mutex for synchronizing access to the bus
};
