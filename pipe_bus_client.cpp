#include <iostream>
#include "pipe_bus_client.hpp"

PipeBusClient::PipeBusClient(const std::string& client_id)
    : client_id_(client_id) {
    std::cout << "[PipeBusClient] Created with ID: " << client_id_ << std::endl;
}

PipeBusClient::~PipeBusClient() = default;

void PipeBusClient::send(const reactor::Message& /*message*/) {
    // stubbed
}

void PipeBusClient::on_receive(std::function<void(const reactor::Message&)> handler) {
    handler_ = std::move(handler);
}

const std::string& PipeBusClient::id() const {
    return client_id_;
}
