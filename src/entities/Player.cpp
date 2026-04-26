#include "Player.h"
#include <SDL2/SDL.h>
#include <cmath>

namespace entities {
Player::Player() {}
void Player::init() {
    x_ = 400.0f;
    y_ = 300.0f;
}

void Player::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

void Player::setVelocity(float vx, float vy) {
    vx_ = vx;
    vy_ = vy;
}

void Player::addEvidence(int amount) {
    evidence_ += amount;
}

void Player::setFlag(const std::string& key, bool value) {
    flags_[key] = value;
}

bool Player::getFlag(const std::string& key) const {
    auto it = flags_.find(key);
    return it != flags_.end() ? it->second : false;
}

bool Player::handleKeyDown(int /*key*/) {
    // Keep for one-shot actions only
    // Movement is handled continuously in update()
    return false;
}

void Player::update(float dt) {
    const float maxSpeed = 300.0f;
    const Uint8* state = SDL_GetKeyboardState(nullptr);

    vx_ = 0.0f;
    vy_ = 0.0f;

    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) vx_ -= 1.0f;
    if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) vx_ += 1.0f;
    if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) vy_ -= 1.0f;
    if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) vy_ += 1.0f;

    if (vx_ != 0.0f && vy_ != 0.0f) {
        float length = std::sqrt(vx_ * vx_ + vy_ * vy_);
        vx_ = (vx_ / length) * maxSpeed;
        vy_ = (vy_ / length) * maxSpeed;
    } else {
        vx_ *= maxSpeed;
        vy_ *= maxSpeed;
    }

    x_ += vx_ * dt;
    y_ += vy_ * dt;
}
}