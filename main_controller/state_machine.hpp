#pragma once
#include <iostream>
#include "controller_core.hpp"


class PowerStateMachine {
public:
    PowerStateMachine(ControllerState& state, MessageBus& bus)
        : state(state), bus(bus) {}

    void handleInput() {
        for (const auto& msg : bus.getInbound()) {
             if (msg.type == MessageType::BUTTON_PRESS && msg.sourceId == POWER_BUTTON_ID) {
                handlePowerButton();
            }
        }
    }

    void tickAdvance() {
        switch (state.phase) {
            case ControllerState::Phase::INIT:
                transitionTo(ControllerState::Phase::STARTUP);
                break;
            case ControllerState::Phase::STARTUP:
                transitionTo(ControllerState::Phase::ON);
                break;
            case ControllerState::Phase::SHUTDOWN:
                transitionTo(ControllerState::Phase::OFF);
                break;
            default:
                break;
        }
    }

private:
    static constexpr uint8_t POWER_BUTTON_ID = 0;

    ControllerState& state;
    MessageBus& bus;

    void handlePowerButton() {
        switch (state.phase) {
            case ControllerState::Phase::OFF:
                transitionTo(ControllerState::Phase::INIT);
                break;
            case ControllerState::Phase::ON:
                transitionTo(ControllerState::Phase::SHUTDOWN);
                break;
            default:
                break;
        }
    }

    void transitionTo(ControllerState::Phase next) {
        std::cout << "[PowerStateMachine] Transitioning to phase: ";
        switch (next) {
            case ControllerState::Phase::OFF: std::cout << "OFF"; break;
            case ControllerState::Phase::INIT: std::cout << "INIT"; break;
            case ControllerState::Phase::TEST: std::cout << "TEST"; break;
            case ControllerState::Phase::STARTUP: std::cout << "STARTUP"; break;
            case ControllerState::Phase::ON: std::cout << "ON"; break;
            case ControllerState::Phase::SHUTDOWN: std::cout << "SHUTDOWN"; break;
        }
        std::cout << std::endl;
    
        state.phase = next;
        bus.emitOutbound({
            .type = MessageType::STATE_TRANSITION,
            .sourceId = POWER_BUTTON_ID,
            .intValue = static_cast<int>(next),
            .strValue = std::nullopt
        });
    }
   
    
};
