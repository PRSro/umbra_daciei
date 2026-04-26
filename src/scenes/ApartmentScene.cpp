#include "ApartmentScene.h"
#include "../core/Renderer.h"
#include "../systems/ParanoiaSystem.h"
#include "../systems/ConspiracyWeb.h"
#include "../systems/FolkloreEvents.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <string>

namespace scenes {

ApartmentScene::ApartmentScene() : currentDay_(1), computerUnlocked_(false) {}

void ApartmentScene::init() {
    loadLayout();

    // Init dialogue system with Vecina tree
    if (!dialogueInitialized_) {
        dialogue_.init();

        ui::DialogueNode n0;
        n0.id = "vecina_0";
        n0.speaker = "VECINA";
        n0.text = "Iar nu ai dormit, Sasha? Te aud noaptea vorbind singur. Si... ce sunt hartiile alea pe perete?";
        n0.options.push_back({"Cercetez ceva important", "vecina_1", ""});
        n0.options.push_back({"Nu-i treaba ta", "vecina_3", ""});
        dialogue_.addDialogueNode(n0);

        ui::DialogueNode n1;
        n1.id = "vecina_1";
        n1.speaker = "SASHA";
        n1.text = "Investighez o retea secreta. Daca iti spun, esti in pericol.";
        n1.options.push_back({"Continua", "vecina_2", ""});
        dialogue_.addDialogueNode(n1);

        ui::DialogueNode n2;
        n2.id = "vecina_2";
        n2.speaker = "VECINA";
        n2.text = "Ieri a venit un barbat. Inalt. Te cauta. Nu arata... normal. Ochii lui erau prea calmi.";
        n2.options.push_back({"Multumesc", "END", ""});
        n2.effectDiscoverNode = "strigoi_network";
        n2.effectParanoia = 0.1f;
        dialogue_.addDialogueNode(n2);

        ui::DialogueNode n3;
        n3.id = "vecina_3";
        n3.speaker = "VECINA";
        n3.text = "Bine, bine. Dar ai grija. Orasul se schimba.";
        n3.options.push_back({"La revedere", "END", ""});
        dialogue_.addDialogueNode(n3);

        dialogueInitialized_ = true;
    }
}

void ApartmentScene::loadLayout() {
    interactables_.clear();
    interactableMap_.clear();
}

void ApartmentScene::update(float dt) {
    // Handle folklore event choice keys
    if (folkloreEvents_ && folkloreEvents_->isModalActive()) {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_1]) folkloreEvents_->selectChoice(0);
        if (keys[SDL_SCANCODE_2]) folkloreEvents_->selectChoice(1);
        return; // Block other input during modal
    }

    // Handle dialogue
    if (dialogue_.isActive()) {
        dialogue_.update(dt);
        return;
    }

    // Player movement with held keys
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    float speed = 120.0f;
    if (keys[SDL_SCANCODE_W]) playerY_ -= speed * dt;
    if (keys[SDL_SCANCODE_S] && !keys[SDL_SCANCODE_LSHIFT]) playerY_ += speed * dt;
    if (keys[SDL_SCANCODE_A]) playerX_ -= speed * dt;
    if (keys[SDL_SCANCODE_D]) playerX_ += speed * dt;

    // Clamp to screen
    if (playerX_ < 6.0f) playerX_ = 6.0f;
    if (playerX_ > 1274.0f) playerX_ = 1274.0f;
    if (playerY_ < 12.0f) playerY_ = 12.0f;
    if (playerY_ > 696.0f) playerY_ = 696.0f;

    // Near conspiracy board (left wall)
    if (playerX_ < 200.0f && paranoiaSystem_) {
        paranoiaSystem_->addParanoia(0.001f);
    }

    // Day progress
    dayProgress_ += dt;
    timeOfDay_ += dt * 0.02f; // slow time
    if (timeOfDay_ >= 24.0f) timeOfDay_ -= 24.0f;

    // Update timers
    if (sleepMsgTimer_ > 0) sleepMsgTimer_ -= dt;
    if (termMsgTimer_ > 0) termMsgTimer_ -= dt;

    // Sync day with scene manager
    if (manager_) {
        manager_->setCurrentDay(currentDay_);
        manager_->setTimeOfDay(timeOfDay_);
    }
}

void ApartmentScene::render() {
    auto* r = reinterpret_cast<core::Renderer*>(getRendererRaw());
    if (!r) return;

    float paranoia = paranoiaSystem_ ? paranoiaSystem_->getParanoia() : 0.0f;

    // Background
    r->setDrawColor({10, 10, 15, 255});
    r->clear();

    // Dacian Symbol Watermark
    if (paranoia > 0.6f) {
        uint8_t alpha = static_cast<uint8_t>((paranoia - 0.6f) * 100.0f);
        r->setDrawBlendMode(SDL_BLENDMODE_BLEND);
        r->drawDacianSymbol(640, 360, 200.0f, 0.0f, {255, 255, 255, alpha});
    }

    // Back wall
    r->setDrawColor({50, 45, 40, 255});
    r->drawRect(0, 50, 1280, 250, true);

    // Wall sections
    r->setDrawColor({60, 55, 50, 255});
    r->drawRect(200, 80, 280, 100, true);
    r->drawRect(520, 80, 280, 100, true);
    r->drawRect(840, 80, 280, 100, true);

    // Window (with rain)
    r->setDrawColor({20, 30, 50, 255});
    r->drawRect(950, 100, 200, 180, true);

    // Animated rain
    rainOffset_ += 80.0f * (1.0f/60.0f); // approximate dt
    r->setDrawColor({60, 80, 140, 180});
    for (int i = 0; i < 12; i++) {
        float xBase = 955.0f + static_cast<float>(i) * 16.0f;
        float yBase = 100.0f + std::fmod(rainOffset_ + i * 25.0f, 180.0f);
        int x1 = static_cast<int>(xBase);
        int y1 = static_cast<int>(yBase);
        r->drawLine(x1, y1, x1 + 5, y1 + 30);
    }

    // Desk
    r->setDrawColor({80, 50, 30, 255});
    r->drawRect(250, 400, 500, 30, true);

    // CRT Monitor on desk
    r->setDrawColor({30, 35, 30, 255});
    r->drawRect(350, 360, 80, 50, true);

    // CRT glow — 3 concentric circles
    bool flickerSkip = paranoia > 0.5f && (std::rand() % 10 < static_cast<int>(paranoia * 10));
    if (!flickerSkip) {
        r->setDrawBlendMode(SDL_BLENDMODE_BLEND);
        r->setDrawColor({0, 200, 80, 25});
        r->drawCircle(390, 385, 80, true);
        r->setDrawColor({0, 200, 80, 50});
        r->drawCircle(390, 385, 55, true);
        r->setDrawColor({0, 200, 80, 80});
        r->drawCircle(390, 385, 35, true);
    }

    // CRT scanlines
    r->setDrawColor({0, 200, 0, 200});
    for (int y = 360; y < 410; y += 3) {
        r->drawLine(355, y, 425, y);
    }

    // Floor
    r->setDrawColor({25, 20, 18, 255});
    r->drawRect(0, 480, 1280, 240, true);

    // Desk legs
    r->setDrawColor({70, 45, 25, 255});
    r->drawRect(260, 430, 8, 50, true);
    r->drawRect(740, 430, 8, 50, true);

    // === Conspiracy board (left wall) ===
    r->setDrawColor({139, 115, 85, 255});
    r->drawRect(50, 100, 120, 180, true);

    // Draw discovered nodes on the board
    if (conspiracyWeb_) {
        auto& nodes = const_cast<systems::ConspiracyWeb*>(conspiracyWeb_)->getNodes();
        // Draw connections first
        for (auto* node : nodes) {
            if (!node->discovered) continue;
            for (const auto& connId : node->connections) {
                const auto* other = conspiracyWeb_->getNode(connId);
                if (other && other->discovered) {
                    r->setDrawColor({180, 30, 30, 200});
                    r->drawLine(static_cast<int>(node->x), static_cast<int>(node->y),
                               static_cast<int>(other->x), static_cast<int>(other->y));
                }
            }
        }
        // Draw nodes
        for (auto* node : nodes) {
            if (!node->discovered) continue;
            core::Color nc = {200, 180, 100, 255}; // DOCUMENT default
            if (node->type == "ENTITY") nc = {200, 50, 50, 255};
            else if (node->type == "ARTIFACT") nc = {100, 150, 200, 255};
            else if (node->type == "TESTIMONY" || node->type == "SYMBOL") nc = {150, 200, 100, 255};
            r->setDrawColor(nc);
            r->drawRect(static_cast<int>(node->x) - 4, static_cast<int>(node->y) - 4, 8, 8, true);
        }
    }

    // === NPC: Vecina (right edge doorway) ===
    {
        int vx = static_cast<int>(vecinaX_);
        int vy = static_cast<int>(vecinaY_);
        // Doorway
        r->setDrawColor({40, 30, 25, 255});
        r->drawRect(vx - 20, vy - 50, 40, 100, true);
        // Vecina silhouette
        r->setDrawColor({180, 150, 130, 255});
        r->drawRect(vx - 4, vy - 30, 8, 8, true);  // head
        r->setDrawColor({120, 80, 60, 255});
        r->drawRect(vx - 5, vy - 22, 10, 16, true); // torso
        r->drawRect(vx - 3, vy - 6, 6, 14, true);   // legs
    }

    // === Player silhouette ===
    {
        int px = static_cast<int>(playerX_);
        int py = static_cast<int>(playerY_);
        r->setDrawColor({180, 180, 170, 255});
        // Head
        r->drawRect(px - 3, py - 24, 6, 6, true);
        // Torso
        r->drawRect(px - 4, py - 18, 8, 10, true);
        // Left arm
        r->drawRect(px - 8, py - 16, 4, 8, true);
        // Right arm
        r->drawRect(px + 4, py - 16, 4, 8, true);
        // Legs
        r->drawRect(px - 3, py - 8, 6, 8, true);
    }

    // === Interaction hints ===
    // Near desk
    if (playerX_ > 250 && playerX_ < 700 && playerY_ > 380 && playerY_ < 450) {
        if (!computerUnlocked_) {
            r->drawText("Press E to examine", 400, 350, {200, 200, 150, 200}, 1);
        } else {
            r->drawText("Press C for Zalmoxis.net", 400, 350, {0, 200, 100, 200}, 1);
        }
    }

    // Near Vecina
    float vecinaDist = std::sqrt((playerX_ - vecinaX_) * (playerX_ - vecinaX_) +
                                 (playerY_ - vecinaY_) * (playerY_ - vecinaY_));
    if (vecinaDist < 80.0f && !dialogue_.isActive()) {
        r->drawText("Press E to talk", static_cast<int>(vecinaX_) - 40, static_cast<int>(vecinaY_) - 60,
                    {200, 200, 150, 200}, 1);
    }

    // Press F hint
    r->drawText("F: Forest  S: Sleep", 10, 700, {100, 100, 80, 150}, 1);

    // Sleep message
    if (sleepMsgTimer_ > 0) {
        r->drawText(sleepMsg_, 500, 350, {200, 180, 220, 200}, 2);
    }

    // Terminal message
    if (termMsgTimer_ > 0) {
        r->drawText(termMsg_, 300, 340, {0, 200, 100, 200}, 2);
    }

    // Dialogue rendering
    if (dialogue_.isActive()) {
        dialogue_.render(r);
    }
}

void ApartmentScene::onEnter(const std::string&) {}

void ApartmentScene::onExit() {}

bool ApartmentScene::handleKeyDown(int key) {
    // Handle folklore modal
    if (folkloreEvents_ && folkloreEvents_->isModalActive()) {
        if (key == SDL_SCANCODE_1) { folkloreEvents_->selectChoice(0); return true; }
        if (key == SDL_SCANCODE_2) { folkloreEvents_->selectChoice(1); return true; }
        return false;
    }

    // Handle dialogue input
    if (dialogue_.isActive()) {
        if (key == SDL_SCANCODE_1) {
            auto effects = dialogue_.selectOption(0);
            if (effects.paranoia != 0.0f && paranoiaSystem_) paranoiaSystem_->addParanoia(effects.paranoia);
            if (!effects.discoverNode.empty() && conspiracyWeb_) conspiracyWeb_->discoverNode(effects.discoverNode);
            return true;
        }
        if (key == SDL_SCANCODE_2) {
            auto effects = dialogue_.selectOption(1);
            if (effects.paranoia != 0.0f && paranoiaSystem_) paranoiaSystem_->addParanoia(effects.paranoia);
            if (!effects.discoverNode.empty() && conspiracyWeb_) conspiracyWeb_->discoverNode(effects.discoverNode);
            return true;
        }
        return false;
    }

    if (key == SDL_SCANCODE_E) {
        // Near Vecina?
        float vecinaDist = std::sqrt((playerX_ - vecinaX_) * (playerX_ - vecinaX_) +
                                     (playerY_ - vecinaY_) * (playerY_ - vecinaY_));
        if (vecinaDist < 80.0f) {
            dialogue_.startDialogue("vecina_0");
            return true;
        }

        // Near desk?
        if (playerX_ > 250 && playerX_ < 700 && playerY_ > 380 && playerY_ < 450) {
            if (!computerUnlocked_) {
                computerUnlocked_ = true;
                if (paranoiaSystem_) paranoiaSystem_->addAwakening(0.05f);
                termMsg_ = "TERMINAL: Connection established...";
                termMsgTimer_ = 3.0f;
            }
            return true;
        }
        return false;
    }

    if (key == SDL_SCANCODE_C && computerUnlocked_) {
        switchTo("forum");
        return true;
    }

    if (key == SDL_SCANCODE_F) {
        switchTo("field");
        return true;
    }

    if (key == SDL_SCANCODE_S && !SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LSHIFT]) {
        // Sleep
        advanceDay();
        if (paranoiaSystem_) paranoiaSystem_->reduceParanoia(0.08f);
        sleepMsg_ = "...vise tulburate...";
        sleepMsgTimer_ = 2.0f;
        return true;
    }

    return false;
}

bool ApartmentScene::handleMouseDown(int, int, int) {
    return false;
}

void ApartmentScene::addInteractable(const Interactable& interactable) {
    interactableMap_[interactable.id] = static_cast<int>(interactables_.size());
    interactables_.push_back(interactable);
}

void ApartmentScene::discoverInteractable(const std::string& id) {
    auto it = interactableMap_.find(id);
    if (it != interactableMap_.end()) {
        interactables_[it->second].discovered = true;
    }
}

const Interactable* ApartmentScene::getInteractable(const std::string& id) const {
    auto it = interactableMap_.find(id);
    if (it != interactableMap_.end()) {
        return &interactables_[it->second];
    }
    return nullptr;
}

void ApartmentScene::advanceDay() {
    currentDay_++;
    dayProgress_ = 0.0f;
    timeOfDay_ = 8.0f;
}

}