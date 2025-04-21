#include <iostream>
#include "pipe_bus_client.hpp" // Corrected include path
#include <stdexcept>
#include <windows.h> // For named pipe operations
#include "message_types.hpp" // Corrected include path

std::mutex PipeBusClient::busMutex;

PipeBusClient::PipeBusClient(const std::string& client_id) : client_id_(client_id) {
    // Initialization logic for the named pipe client
    std::cout << "[PipeBusClient] Initialized client with ID: " << client_id_ << std::endl;
}

PipeBusClient::~PipeBusClient() {
    // Cleanup logic for the named pipe client
    std::cout << "[PipeBusClient] Destroyed client with ID: " << client_id_ << std::endl;
}

void PipeBusClient::send(const Message& message) {
    std::lock_guard<std::mutex> lock(busMutex);

    // Serialize the message
    std::string serializedMessage = message.from + "," + message.to + "," + (message.isButtonPress() ? "ButtonPress" : "Unknown");

    // Simulate writing to the named pipe
    std::cout << "[PipeBusClient] Writing serialized message: " << serializedMessage << std::endl;
}

void PipeBusClient::on_receive(std::function<void(const Message&)> handler) {
    handler_ = handler;
    std::cout << "[PipeBusClient] Receive handler set for client " << client_id_ << std::endl;
    // Simulate listening for messages
}

const std::string& PipeBusClient::id() const {
    return client_id_;
}
