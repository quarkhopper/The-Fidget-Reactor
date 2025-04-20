#pragma once
#include <string>
#include <variant>

// Define the ButtonPress message
struct ButtonPress {
    std::string button_id;
    bool pressed;
};

// Define the Message structure
struct Message {
    std::string from;
    std::string to;

    // Use a variant to represent the payload
    std::variant<ButtonPress> payload;

    // Helper methods for working with the payload
    bool isButtonPress() const {
        return std::holds_alternative<ButtonPress>(payload);
    }

    const ButtonPress& getButtonPress() const {
        return std::get<ButtonPress>(payload);
    }

    ButtonPress& getButtonPress() {
        return std::get<ButtonPress>(payload);
    }
};