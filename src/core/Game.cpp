#include "Game.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "InputHandler.h"
#include "SaveSystem.h"
#include "../scenes/SceneManager.h"
#include "../Config.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace core {

Game::Game() = default;

Game::~Game() {
    if (window_) {
        SDL_DestroyWindow(window_);
    }
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        return false;
    }

    if (TTF_Init() < 0) {
        // TTF init failure is non-fatal — we have bitmap font fallback
    }

    window_ = SDL_CreateWindow(
        "Umbra Daciei",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Config::WINDOW_WIDTH,
        Config::WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!window_) {
        return false;
    }

    renderer_ = std::make_unique<Renderer>();
    renderer_->init(window_);

    audio_ = std::make_unique<AudioManager>();
    audio_->init();

    input_ = std::make_unique<InputHandler>();
    input_->init();

    save_ = std::make_unique<SaveSystem>();
    save_->init();

    scenes::SceneManager* sm = new scenes::SceneManager(this);
    sceneManager_.reset(sm);
    sm->setCoreSystems(renderer_.get(), audio_.get(), input_.get(), save_.get());
    if (sm) sm->init();

    lastFixedUpdate_ = std::chrono::steady_clock::now();
    lastSave_ = std::chrono::steady_clock::now();

    running_ = true;
    setState(GameState::PLAYING);
    return true;
}

void Game::run() {
    while (running_ && !shutdownRequested_) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastFixedUpdate_).count();
        dt_ = elapsed / 1000000.0f;
        // Cap dt to prevent spiral of death
        if (dt_ > 0.1f) dt_ = 0.1f;
        lastFixedUpdate_ = now;

        totalTime_ += dt_;
        frameCount_++;

        processInput();
        fixedUpdate();
        saveIfNeeded();

        render(*renderer_);
    }
}

void Game::requestShutdown() {
    shutdownRequested_ = true;
}

void Game::processInput() {
    input_->poll();

    // Let the current scene handle key input
    if (sceneManager_ && state_ == GameState::PLAYING) {
        scenes::Scene* scene = sceneManager_->getCurrentScene();
        if (scene) {
            // Check key events
            for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
                if (input_->isKeyPressed(static_cast<SDL_Scancode>(i))) {
                    scene->handleKeyDown(i);
                }
            }
        }
    }

    if (input_->isKeyPressed(SDL_SCANCODE_F5)) {
        save_->save(*this);
    }

    // Quit on window close
    SDL_Event event;
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_QUIT, SDL_QUIT) > 0) {
        running_ = false;
    }
}

void Game::fixedUpdate() {
    if (state_ == GameState::PLAYING) {
        if (sceneManager_) {
            sceneManager_->update(dt_);

            // Dramatic ending trigger - when awakening >= 0.95
            float awakening = sceneManager_->getAwakening();
            if (!endingChosen_ && awakening >= 0.95f) {
                endingChosen_ = true;
                std::string type = save_->getEndingType();
                if (type == "true_believer" || type == "zalmoxis_ascendant") {
                    sceneManager_->switchTo("ending", "ascension");
                } else {
                    sceneManager_->switchTo("ending", "combat");
                }
            }

            // Reset endingChosen when we leave the ending screen (handled in game reset)
        }
    }
}

void Game::saveIfNeeded() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastSave_).count();

    if (elapsed >= Config::AUTO_SAVE_INTERVAL || autoSaveCounter_ == 0) {
        save_->save(*this);
        lastSave_ = now;
    }
    autoSaveCounter_++;
}

void Game::update(float dt) {
    if (state_ == GameState::PLAYING) {
        if (sceneManager_) {
            sceneManager_->update(dt);
        }
    }
}

void Game::render(Renderer& renderer) {
    renderer.setDrawColor(Config::Colors::BACKGROUND);
    renderer.clear();

    if (sceneManager_) {
        sceneManager_->render();  // SceneManager calls present() after HUD
    }
    // DO NOT call renderer.present() here — SceneManager::render() does it
}

void Game::setState(GameState s) {
    if (state_ != s) {
        prevState_ = state_;
        state_ = s;
    }
}

void Game::addFlag(const std::string& flag) {
    flags_.insert(flag);
}

bool Game::hasFlag(const std::string& flag) const {
    return flags_.find(flag) != flags_.end();
}

void Game::clearFlags() {
    flags_.clear();
}

void Game::addNotification(const std::string& text) {
    notifications_.push_back(text);
    if (notifications_.size() > 5) {
        notifications_.erase(notifications_.begin());
    }
}

bool Game::shouldShowEnding() const {
    return state_ == GameState::ENDING;
}

std::string Game::getEndingType() const {
    return save_->getEndingType();
}

}