#include "HUD.h"
#include "../core/Renderer.h"
#include "../systems/ParanoiaSystem.h"
#include <cstdlib>
#include <cmath>
#include <string>

namespace ui {
HUD::HUD() {}
void HUD::init() {}

void HUD::update(float dt) {
    if (messageTimer_ > 0) {
        messageTimer_ -= dt;
        if (messageTimer_ <= 0) {
            messageDisplay_.clear();
        }
    }
}

void HUD::render(core::Renderer* r, systems::ParanoiaSystem* ps) {
    if (!visible_ || !r) return;

    float paranoia = ps ? ps->getParanoia() : paranoia_;
    float awakening = ps ? ps->getAwakening() : awakening_;

    r->setDrawBlendMode(SDL_BLENDMODE_BLEND);

    // TOP LEFT: Day + time
    {
        std::string dayStr = "DEN " + std::to_string(day_) + " | " + std::to_string(static_cast<int>(timeOfDay_)) + "h";
        r->drawText(dayStr, 10, 8, {200, 180, 120, 255}, 2);
    }

    // TOP RIGHT: Paranoia bar
    {
        int barX = 1040;
        int barY = 8;
        int barW = 200;
        int barH = 14;

        r->drawText("PARANOIA", barX, barY - 1, {200, 150, 150, 255}, 1);
        barY += 10;

        // Background
        r->setDrawColor({30, 30, 30, 200});
        r->drawRect(barX, barY, barW, barH, true);

        // Fill — lerp green to red
        int fillW = static_cast<int>(barW * paranoia);
        uint8_t pr = static_cast<uint8_t>(50 + 150 * paranoia);
        uint8_t pg = static_cast<uint8_t>(200 - 170 * paranoia);
        uint8_t pb = static_cast<uint8_t>(50 - 20 * paranoia);
        r->setDrawColor({pr, pg, pb, 255});
        r->drawRect(barX, barY, fillW, barH, true);

        // Border
        r->setDrawColor({100, 100, 100, 255});
        r->drawRect(barX, barY, barW, barH, false);
    }

    // Below paranoia: Awakening bar
    {
        int barX = 1040;
        int barY = 38;
        int barW = 200;
        int barH = 14;

        r->drawText("TREZIRE DACICA", barX, barY - 1, {100, 150, 200, 255}, 1);
        barY += 10;

        // Background
        r->setDrawColor({30, 30, 30, 200});
        r->drawRect(barX, barY, barW, barH, true);

        // Fill
        int fillW = static_cast<int>(barW * awakening);
        r->setDrawColor({100, 150, 200, 255});
        r->drawRect(barX, barY, fillW, barH, true);

        // Border
        r->setDrawColor({100, 100, 100, 255});
        r->drawRect(barX, barY, barW, barH, false);
    }

    // BOTTOM CENTER: notification text with fade
    if (!messageDisplay_.empty() && messageTimer_ > 0) {
        uint8_t alpha = static_cast<uint8_t>(std::min(255.0f, messageTimer_ * 85.0f));
        int textX = 640 - static_cast<int>(messageDisplay_.size()) * 3; // rough center
        r->drawText(messageDisplay_, textX, 680, {220, 220, 200, alpha}, 2);
    }

    // Paranoia > 0.6: red vignette
    if (paranoia > 0.6f) {
        uint8_t vigAlpha = static_cast<uint8_t>(std::min(120.0f, (paranoia - 0.6f) * 400.0f));
        r->setDrawColor({180, 0, 0, vigAlpha});
        // Top edge
        r->drawRect(0, 0, 1280, 40, true);
        // Bottom edge
        r->drawRect(0, 680, 1280, 40, true);
        // Left edge
        r->drawRect(0, 0, 40, 720, true);
        // Right edge
        r->drawRect(1240, 0, 40, 720, true);
    }

    // Paranoia > 0.85: white noise
    if (paranoia > 0.85f) {
        r->setDrawColor({255, 255, 255, 60});
        for (int i = 0; i < 200; ++i) {
            int nx = std::rand() % 1280;
            int ny = std::rand() % 720;
            r->drawRect(nx, ny, 1, 1, true);
        }
    }
}

void HUD::setMessage(const std::string& message) {
    message_ = message;
    messageDisplay_ = message;
    messageTimer_ = 3.0f;
}

void HUD::clearMessage() {
    message_.clear();
    messageDisplay_.clear();
    messageTimer_ = 0.0f;
}
}