#include "MainMenuScene.h"
#include "../core/Renderer.h"
#include "../systems/ParanoiaSystem.h"
#include "../systems/ConspiracyWeb.h"
#include "../systems/FolkloreEvents.h"
#include "../core/Game.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace scenes {

MainMenuScene::MainMenuScene() {
    // Buttons
    buttons_.push_back({480, 330, 320, 48, "INCEPE JOCUL"});
    buttons_.push_back({480, 400, 320, 48, "CONTINUA"});
    buttons_.push_back({480, 470, 320, 48, "IESI"});
    
    // Stars
    std::srand(1337);
    for (int i = 0; i < 80; ++i) {
        Star s;
        s.x = std::rand() % 1280;
        s.y = std::rand() % 360; // top half
        s.baseAlpha = 150.0f;
        s.phase = static_cast<float>(i * 0.7f);
        stars_.push_back(s);
    }
}

void MainMenuScene::init() {
    selectedIndex_ = 0;
    timeAccumulator_ = 0.0f;
    strigoiFlashTimer_ = 8.0f;
    globalFlickerTimer_ = 12.0f;
    saveErrorFlashTimer_ = 0.0f;
}

void MainMenuScene::onEnter(const std::string&) {
    init();
}

void MainMenuScene::onExit() {}

void MainMenuScene::update(float dt) {
    timeAccumulator_ += dt;
    
    strigoiFlashTimer_ -= dt;
    if (strigoiFlashTimer_ <= 0.0f) {
        strigoiFlashTimer_ = 8.0f + static_cast<float>(std::rand() % 4);
        strigoiX_ = static_cast<float>(200 + std::rand() % 880);
    }
    
    globalFlickerTimer_ -= dt;
    if (globalFlickerTimer_ <= 0.0f) {
        globalFlickerTimer_ = 12.0f + static_cast<float>(std::rand() % 5);
    }
    
    if (saveErrorFlashTimer_ > 0.0f) {
        saveErrorFlashTimer_ -= dt;
    }
    
    // Hover logic
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    for (size_t i = 0; i < buttons_.size(); ++i) {
        if (mx >= buttons_[i].x && mx <= buttons_[i].x + buttons_[i].w &&
            my >= buttons_[i].y && my <= buttons_[i].y + buttons_[i].h) {
            selectedIndex_ = static_cast<int>(i);
            break;
        }
    }
}

void MainMenuScene::render() {
    auto* r = reinterpret_cast<core::Renderer*>(getRendererRaw());
    if (!r) return;
    
    r->setDrawBlendMode(SDL_BLENDMODE_BLEND);

    // 1. Background
    r->setDrawColor({5, 5, 10, 255});
    r->clear();
    
    // 3. Stars
    for (const auto& s : stars_) {
        uint8_t alpha = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, 
            s.baseAlpha + std::sin(timeAccumulator_ + s.phase) * 50.0f)));
        r->setDrawColor({200, 200, 255, alpha});
        r->drawRect(s.x, s.y, 2, 2, true);
    }
    
    // 4. Moon
    r->setDrawColor({220, 215, 190, 10});
    r->drawCircle(980, 120, 100, true);
    r->setDrawColor({220, 215, 190, 20});
    r->drawCircle(980, 120, 80, true);
    r->setDrawColor({220, 215, 190, 40});
    r->drawCircle(980, 120, 65, true);
    r->setDrawColor({220, 215, 190, 255});
    r->drawCircle(980, 120, 55, true);
    
    // 2. Mountains
    core::Color mColors[3] = {{20, 25, 30, 255}, {15, 20, 25, 255}, {25, 30, 35, 255}};
    struct Peak { int x1, y1, x2, y2, x3, y3; int colorIdx; };
    Peak peaks[] = {
        {0, 720, 160, 480, 320, 720, 0},
        {160, 720, 320, 540, 480, 720, 1},
        {320, 720, 500, 420, 680, 720, 2},
        {500, 720, 680, 500, 860, 720, 0},
        {680, 720, 850, 400, 1020, 720, 1},
        {850, 720, 1050, 520, 1250, 720, 2},
        {1050, 720, 1180, 460, 1310, 720, 0}
    };
    for (auto& p : peaks) {
        r->setDrawColor(mColors[p.colorIdx]);
        r->drawTriangle(p.x1, p.y1, p.x2, p.y2, p.x3, p.y3, true);
    }
    
    // Strigoi Flash
    if (strigoiFlashTimer_ > 7.7f) {
        int sx = static_cast<int>(strigoiX_);
        r->setDrawColor({5, 5, 5, 255});
        r->drawRect(sx, 480, 12, 12, true); // Head
        r->drawRect(sx - 2, 492, 16, 40, true); // Torso
        r->drawRect(sx - 8, 495, 6, 25, true); // Left arm
        r->drawRect(sx + 14, 495, 6, 25, true); // Right arm
        r->drawRect(sx, 532, 5, 40, true); // Left leg
        r->drawRect(sx + 7, 532, 5, 40, true); // Right leg
    }

    // 5. Fog
    r->setDrawColor({180, 190, 200, 35});
    r->drawRect(0, 580, 1280, 80, true);
    r->setDrawColor({180, 190, 200, 25});
    r->drawRect(0, 620, 1280, 80, true);
    r->setDrawColor({180, 190, 200, 15});
    r->drawRect(0, 660, 1280, 80, true);
    
    // 6. Mist wisps
    for (int i = 0; i < 4; ++i) {
        float mx = std::fmod(timeAccumulator_ * 25.0f + i * 300.0f, 1380.0f) - 100.0f;
        r->setDrawColor({200, 210, 220, 20});
        r->drawRect(static_cast<int>(mx), 600 + i * 20, 150, 20, true);
    }
    
    // Title
    std::string title = "UMBRA DACIEI";
    int tw = static_cast<int>(title.length()) * 24; // 6 * 4px per char
    r->drawText(title, 640 - tw/2, 160, {200, 180, 120, 255}, 4);
    
    // Decorative Dacian Symbol
    r->drawDacianSymbol(640, 480, 40.0f, timeAccumulator_ * 20.0f, {80, 70, 50, 100});
    
    std::string subtitle = "Un joc de investigatie si groaza";
    int sw = static_cast<int>(subtitle.length()) * 12;
    r->drawText(subtitle, 640 - sw/2, 220, {140, 120, 90, 180}, 2);
    
    r->setDrawColor({100, 80, 50, 200});
    r->drawRect(1280 * 0.2f, 260, 1280 * 0.6f, 1, true); // Thin line
    
    // Buttons
    for (int i = 0; i < static_cast<int>(buttons_.size()); ++i) {
        const auto& b = buttons_[i];
        
        bool isSelected = (i == selectedIndex_);
        
        core::Color fillCol = isSelected ? core::Color{60, 50, 30, 220} : core::Color{40, 35, 25, 200};
        core::Color borderCol = isSelected ? core::Color{200, 180, 120, 255} : core::Color{100, 80, 50, 255};
        core::Color textCol = isSelected ? core::Color{200, 180, 120, 255} : core::Color{120, 100, 70, 255};
        
        if (i == 1 && saveErrorFlashTimer_ > 0.0f) {
            borderCol = {255, 50, 50, 255};
            textCol = {255, 100, 100, 255};
        }

        r->setDrawColor(fillCol);
        r->drawRect(b.x, b.y, b.w, b.h, true);
        r->setDrawColor(borderCol);
        r->drawRect(b.x, b.y, b.w, b.h, false);
        
        if (isSelected) {
            r->drawText(">", b.x - 20, b.y + 15, borderCol, 2);
        }
        
        int textW = static_cast<int>(b.text.length()) * 12;
        r->drawText(b.text, b.x + (b.w - textW) / 2, b.y + 15, textCol, 2);
    }
    
    // Text corners
    r->drawText("v0.1 // BUCURESTI 2031", 10, 700, {60, 55, 45, 180}, 1);
    std::string rights = "(C) UMBRA DACIEI // TOATE DREPTURILE REZERVATE";
    r->drawText(rights, 1270 - static_cast<int>(rights.length())*6, 700, {60, 55, 45, 180}, 1);
    
    // Global Flicker
    if (globalFlickerTimer_ > 11.9f) {
        r->setDrawColor({0, 0, 0, 180});
        r->drawRect(0, 0, 1280, 720, true);
    }
}

bool MainMenuScene::saveExists() const {
    std::string path;
    if (const char* home = std::getenv("HOME")) {
        path = std::string(home) + "/.umbra_daciei/save.json";
        std::ifstream f(path);
        return f.good();
    }
    return false;
}

void MainMenuScene::resetGameState() {
    if (manager_) {
        manager_->setCurrentDay(1);
        manager_->setTimeOfDay(8.0f);
        if (paranoiaSystem_) {
            paranoiaSystem_->reduceParanoia(1.0f); // Reset to 0
            // Awakening reset depends on implementation, we'll assume it's additive and we can't easily set to 0.
            // But we can recreate the system or just call a reset if available. 
            // Wait, there's no reset in ParanoiaSystem. Let's just deduct 1.0f.
            paranoiaSystem_->reduceParanoia(1.0f); 
            // no reduceAwakening in ParanoiaSystem? Actually we'll need to check later or bypass.
        }
    }
}

void MainMenuScene::executeSelected() {
    if (selectedIndex_ == 0) {
        // INCEPE JOCUL
        if (manager_) {
             manager_->resetGameState(); // We will implement this in SceneManager
             switchTo("apartment");
        }
    } else if (selectedIndex_ == 1) {
        // CONTINUA
        if (saveExists()) {
            if (manager_) {
                // Game should load here, but we just switch to apartment for now
                switchTo("apartment");
            }
        } else {
            saveErrorFlashTimer_ = 0.5f;
        }
    } else if (selectedIndex_ == 2) {
        // IESI
        if (manager_) {
            manager_->requestShutdown();
        }
    }
}

bool MainMenuScene::handleKeyDown(int key) {
    if (key == SDL_SCANCODE_W || key == SDL_SCANCODE_UP) {
        selectedIndex_--;
        if (selectedIndex_ < 0) selectedIndex_ = 2;
        return true;
    }
    if (key == SDL_SCANCODE_S || key == SDL_SCANCODE_DOWN) {
        selectedIndex_++;
        if (selectedIndex_ > 2) selectedIndex_ = 0;
        return true;
    }
    if (key == SDL_SCANCODE_RETURN || key == SDL_SCANCODE_SPACE) {
        executeSelected();
        return true;
    }
    return false;
}

bool MainMenuScene::handleMouseDown(int x, int y, int button) {
    if (button == SDL_BUTTON_LEFT) {
        for (size_t i = 0; i < buttons_.size(); ++i) {
            if (x >= buttons_[i].x && x <= buttons_[i].x + buttons_[i].w &&
                y >= buttons_[i].y && y <= buttons_[i].y + buttons_[i].h) {
                selectedIndex_ = static_cast<int>(i);
                executeSelected();
                return true;
            }
        }
    }
    return false;
}

}
