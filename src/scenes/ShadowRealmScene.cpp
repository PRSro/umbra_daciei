#include "ShadowRealmScene.h"
#include "../core/Renderer.h"
#include "../systems/ParanoiaSystem.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <string>

namespace scenes {

ShadowRealmScene::ShadowRealmScene() {
    iele_[0] = {200.0f, 300.0f};
    iele_[1] = {800.0f, 200.0f};
    iele_[2] = {500.0f, 500.0f};
}

void ShadowRealmScene::init() {}

void ShadowRealmScene::update(float dt) {
    totalTime_ += dt;
    timeInRealm_ += dt;

    // Player movement
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    float speed = 90.0f;
    if (keys[SDL_SCANCODE_W]) playerY_ -= speed * dt;
    if (keys[SDL_SCANCODE_S]) playerY_ += speed * dt;
    if (keys[SDL_SCANCODE_A]) playerX_ -= speed * dt;
    if (keys[SDL_SCANCODE_D]) playerX_ += speed * dt;

    // Clamp
    if (playerX_ < 10.0f) playerX_ = 10.0f;
    if (playerX_ > 1270.0f) playerX_ = 1270.0f;
    if (playerY_ < 10.0f) playerY_ = 10.0f;
    if (playerY_ > 710.0f) playerY_ = 710.0f;

    // Check iele collision
    for (int i = 0; i < 3; ++i) {
        float ix = iele_[i].baseX + 100.0f * std::sin(totalTime_ * 0.3f + i * 2.0f);
        float iy = iele_[i].baseY + 80.0f * std::cos(totalTime_ * 0.4f + i * 1.5f);
        float dx = playerX_ - ix, dy = playerY_ - iy;
        if (std::sqrt(dx * dx + dy * dy) < 40.0f) {
            if (paranoiaSystem_) paranoiaSystem_->addParanoia(0.2f);
            flashTimer_ = 0.1f;
        }
    }

    // Flash timer
    if (flashTimer_ > 0) flashTimer_ -= dt;

    // Text cycling
    textTimer_ += dt;
    if (textTimer_ >= 3.0f) {
        textTimer_ = 0.0f;
        textPhase_ = (textPhase_ + 1) % 5;
    }

    // Exit after 30 seconds
    if (timeInRealm_ > 30.0f) {
        if (paranoiaSystem_) {
            while (paranoiaSystem_->getParanoia() > 0.8f) {
                paranoiaSystem_->reduceParanoia(0.01f);
            }
        }
        switchTo("field");
    }
}

void ShadowRealmScene::render() {
    auto* r = reinterpret_cast<core::Renderer*>(getRendererRaw());
    if (!r) return;

    float wavyOffset = std::sin(totalTime_ * 2.0f) * 5.0f;
    int wo = static_cast<int>(wavyOffset);

    r->setDrawBlendMode(SDL_BLENDMODE_BLEND);

    // Background: deep purple
    r->setDrawColor({15, 5, 25, 255});
    r->clear();

    // Flash white
    if (flashTimer_ > 0) {
        r->setDrawColor({255, 255, 255, 200});
        r->drawRect(0, 0, 1280, 720, true);
        return;
    }

    // L-system trees (backwards growing, upward from bottom)
    r->setDrawColor({180, 0, 80, 200});
    for (int t = 0; t < 6; ++t) {
        int treeX = 100 + t * 200 + wo;
        // Simple binary tree for Shadow Realm
        std::string prog = "F[+F][-F]"; 
        r->drawLSystemTree(treeX, 720, prog, 40, 25, [](core::Color&, int&) {});
    }

    // Scanlines
    r->setDrawColor({0, 0, 0, 70});
    for (int y = 0; y < 720; y += 2) {
        r->drawRect(wo, y, 1280, 1, true);
    }

    // Floating iele
    for (int i = 0; i < 3; ++i) {
        float ix = iele_[i].baseX + 100.0f * std::sin(totalTime_ * 0.3f + i * 2.0f);
        float iy = iele_[i].baseY + 80.0f * std::cos(totalTime_ * 0.4f + i * 1.5f);
        float pulseR = 20.0f + 8.0f * std::sin(totalTime_ * 2.0f + i);
        uint8_t iAlpha = static_cast<uint8_t>(100 + 50 * std::sin(totalTime_ + i));
        r->setDrawColor({200, 200, 255, iAlpha});
        r->drawCircle(static_cast<int>(ix) + wo, static_cast<int>(iy), static_cast<int>(pulseR), true);
    }

    // Pillar of Zalmoxis
    if (!pillarActivated_) {
        r->setDrawColor({220, 200, 50, 200});
    } else {
        r->setDrawColor({255, 240, 100, 255});
    }
    r->drawRect(static_cast<int>(pillarX_) - 8 + wo, static_cast<int>(pillarY_) - 40, 16, 80, true);
    // Glow
    r->setDrawColor({255, 240, 100, 40});
    r->drawCircle(static_cast<int>(pillarX_) + wo, static_cast<int>(pillarY_), 50, true);

    // Player
    {
        int px = static_cast<int>(playerX_) + wo;
        int py = static_cast<int>(playerY_);
        r->setDrawColor({200, 180, 255, 200});
        r->drawRect(px - 1, py - 5, 3, 3, true);
        r->drawRect(px - 1, py - 2, 3, 5, true);
    }

    // Pillar hint
    {
        float pdx = playerX_ - pillarX_;
        float pdy = playerY_ - pillarY_;
        if (std::sqrt(pdx * pdx + pdy * pdy) < 60.0f && !pillarActivated_) {
            r->drawText("Press E", static_cast<int>(pillarX_) - 20 + wo,
                        static_cast<int>(pillarY_) - 55, {255, 240, 100, 200}, 2);
        }
    }

    // Flashing text
    const char* phrases[] = {
        "ZALMOXIS ASTEAPTA",
        "NU ESTI SINGUR",
        "AMINTESTE-TI",
        "SANGELE TAU E VECHI",
        "POARTA E DESCHISA"
    };
    r->drawText(phrases[textPhase_], 480 + wo, 340, {220, 200, 255, static_cast<uint8_t>(150 + 50 * std::sin(totalTime_ * 3))}, 3);

    // Dialogue text if pillar activated
    if (pillarActivated_) {
        r->drawText("Recunosti locul. Ai mai fost aici.", 350, 600, {255, 240, 150, 200}, 2);
        r->drawText("Inainte de a fi Sasha.", 420, 630, {255, 240, 150, 180}, 2);
    }
}

void ShadowRealmScene::onEnter(const std::string&) {
    timeInRealm_ = 0.0f;
    playerX_ = 640.0f;
    playerY_ = 360.0f;
    pillarActivated_ = false;
}

void ShadowRealmScene::onExit() {}

bool ShadowRealmScene::handleKeyDown(int key) {
    if (folkloreEvents_ && folkloreEvents_->isModalActive()) {
        if (key == SDL_SCANCODE_1) { folkloreEvents_->selectChoice(0); return true; }
        if (key == SDL_SCANCODE_2) { folkloreEvents_->selectChoice(1); return true; }
        return false;
    }

    if (key == SDL_SCANCODE_ESCAPE || key == SDL_SCANCODE_Q) {
        switchTo("field");
        return true;
    }

    if (key == SDL_SCANCODE_E) {
        float pdx = playerX_ - pillarX_;
        float pdy = playerY_ - pillarY_;
        if (std::sqrt(pdx * pdx + pdy * pdy) < 60.0f && !pillarActivated_) {
            pillarActivated_ = true;
            if (paranoiaSystem_) {
                paranoiaSystem_->addAwakening(0.3f);
            }
            return true;
        }
    }

    return false;
}

bool ShadowRealmScene::handleMouseDown(int, int, int) { return false; }

}