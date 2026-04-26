#include "InputHandler.h"
#include <algorithm>

namespace core {

InputHandler::InputHandler() = default;

InputHandler::~InputHandler() = default;

void InputHandler::init() {
    currentKeys_.clear();
    previousKeys_.clear();
    currentMouse_.clear();
}

void InputHandler::shutdown() {
    currentKeys_.clear();
    previousKeys_.clear();
    currentMouse_.clear();
    previousMouse_.clear();
}

void InputHandler::poll() {
    previousKeys_ = currentKeys_;
    previousMouse_ = currentMouse_;

    SDL_PumpEvents();

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        currentKeys_[static_cast<SDL_Scancode>(i)] = (state[i] != 0);
    }

    int x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);
    mouseX_ = x;
    mouseY_ = y;

    currentMouse_[SDL_BUTTON_LEFT] = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    currentMouse_[SDL_BUTTON_RIGHT] = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    currentMouse_[SDL_BUTTON_MIDDLE] = (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;

    int rx, ry;
    SDL_GetRelativeMouseState(&rx, &ry);
    mouseRelX_ = rx;
    mouseRelY_ = ry;

    for (auto& [code, callbacks] : keyCallbacks_) {
        if (isKeyPressed(code)) {
            for (auto& cb : callbacks) {
                cb();
            }
        }
    }
}

bool InputHandler::isKeyDown(SDL_Scancode code) const {
    auto it = currentKeys_.find(code);
    return it != currentKeys_.end() && it->second;
}

bool InputHandler::isKeyPressed(SDL_Scancode code) const {
    bool current = false;
    bool previous = false;

    auto curIt = currentKeys_.find(code);
    if (curIt != currentKeys_.end()) current = curIt->second;

    auto prevIt = previousKeys_.find(code);
    if (prevIt != previousKeys_.end()) previous = prevIt->second;

    return current && !previous;
}

bool InputHandler::isKeyReleased(SDL_Scancode code) const {
    bool current = false;
    bool previous = false;

    auto curIt = currentKeys_.find(code);
    if (curIt != currentKeys_.end()) current = curIt->second;

    auto prevIt = previousKeys_.find(code);
    if (prevIt != previousKeys_.end()) previous = prevIt->second;

    return !current && previous;
}

bool InputHandler::isMouseDown(int button) const {
    auto it = currentMouse_.find(button);
    return it != currentMouse_.end() && it->second;
}

bool InputHandler::isMousePressed(int button) const {
    bool current = false;
    bool previous = false;

    auto curIt = currentMouse_.find(button);
    if (curIt != currentMouse_.end()) current = curIt->second;

    auto prevIt = previousMouse_.find(button);
    if (prevIt != previousMouse_.end()) previous = prevIt->second;

    return current && !previous;
}

void InputHandler::addKeyCallback(SDL_Scancode code, std::function<void()> callback) {
    keyCallbacks_[code].push_back(std::move(callback));
}

}