#pragma once

#include "bus_client.hpp"

class PipeBusClient : public BusClient {
public:
    PipeBusClient(const std::string& client_id);
    ~PipeBusClient() override;

    void send(const reactor::Message& message) override;
    void on_receive(std::function<void(const reactor::Message&)> handler) override;
    const std::string& id() const override;

private:
    std::string client_id_;
    std::function<void(const reactor::Message&)> handler_;
};
