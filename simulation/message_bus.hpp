#pragma once
#include "controller_core.hpp"

class MessageBus {
public:
    void pushInbound(const Message& msg) {
        inbound.push_back(msg);
    }

    void emitOutbound(const Message& msg) {
        outbound.push_back(msg);
    }

    const std::vector<Message>& getInbound() const { return inbound; }
    const std::vector<Message>& getOutbound() const { return outbound; }

    void clear() {
        inbound.clear();
        outbound.clear();
    }

private:
    std::vector<Message> inbound;
    std::vector<Message> outbound;
};
