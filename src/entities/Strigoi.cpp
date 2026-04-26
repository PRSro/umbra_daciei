#include "Strigoi.h"
#include <cmath>

namespace entities {
Strigoi::Strigoi() {}
void Strigoi::init() {}

void Strigoi::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

bool Strigoi::canSeePlayer(float playerX, float playerY, float visionRange) {
    float dx = playerX - x_;
    float dy = playerY - y_;
    float dist = std::sqrt(dx * dx + dy * dy);
    return dist <= visionRange && visible_;
}

bool Strigoi::canHearPlayer(float playerX, float playerY, float hearingRange) {
    float dx = playerX - x_;
    float dy = playerY - y_;
    float dist = std::sqrt(dx * dx + dy * dy);
    return dist <= hearingRange;
}

void Strigoi::update(float dt) {
    (void)dt;
}
}