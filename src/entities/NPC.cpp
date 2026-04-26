#include "NPC.h"

namespace entities {
NPC::NPC() {}
void NPC::init() {}

void NPC::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

bool NPC::interact() {
    met_ = true;
    return true;
}
}