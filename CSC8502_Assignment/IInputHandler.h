#pragma once

struct MousePos {
    float x = 0.0f;
    float y = 0.0f;
};

class IInputHandler {
public:
    virtual ~IInputHandler() = default;

    virtual bool isKeyPressed(int key) const = 0;
    virtual bool isKeyHeld(int key) const = 0;
    virtual bool isKeyReleased(int key) const = 0;

    virtual bool isMousePressed(int button) const = 0;

    virtual MousePos getMousePosition() const = 0;
};