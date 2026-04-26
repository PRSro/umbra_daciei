#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace core {

class InputHandler {
public:
    InputHandler();
    ~InputHandler();

    void init();
    void shutdown();
    void poll();

    bool isKeyDown(SDL_Scancode code) const;
    bool isKeyPressed(SDL_Scancode code) const;
    bool isKeyReleased(SDL_Scancode code) const;

    bool isMouseDown(int button) const;
    bool isMousePressed(int button) const;
    int getMouseX() const { return mouseX_; }
    int getMouseY() const { return mouseY_; }
    int getMouseRelX() const { return mouseRelX_; }
    int getMouseRelY() const { return mouseRelY_; }

    void addKeyCallback(SDL_Scancode code, std::function<void()> callback);

private:
    std::unordered_map<SDL_Scancode, bool> currentKeys_;
    std::unordered_map<SDL_Scancode, bool> previousKeys_;

    std::unordered_map<int, bool> currentMouse_;
    std::unordered_map<int, bool> previousMouse_;

    int mouseX_ = 0;
    int mouseY_ = 0;
    int mouseRelX_ = 0;
    int mouseRelY_ = 0;

    std::unordered_map<SDL_Scancode, std::vector<std::function<void()>>> keyCallbacks_;
};

}

#endif