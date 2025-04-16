#pragma once
#include "controller_core.hpp"

class PowerStateMachine {
public:
    PowerStateMachine(ControllerState& state, MessageBus& bus)
        : state(state), bus(bus) {}

    void handleInput() {
        for (const auto& msg : state.inboundMessages) {
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
        state.phase = next;
        bus.emitOutbound({ MessageType::STATE_TRANSITION, POWER_BUTTON_ID, static_cast<int>(next) });
    }
};
