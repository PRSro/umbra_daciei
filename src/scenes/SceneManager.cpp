#include "SceneManager.h"
#include "MainMenuScene.h"
#include "ApartmentScene.h"
#include "ForumScene.h"
#include "FieldScene.h"
#include "ShadowRealmScene.h"
#include "EndingScene.h"
#include "../core/Renderer.h"
#include "../core/Game.h"
#include "../Config.h"
#include <SDL2/SDL.h>

namespace scenes {

SceneManager::SceneManager() {}

SceneManager::SceneManager(void* game) : game_(game) {}

SceneManager::~SceneManager() {
    scenes_.clear();
}

void SceneManager::init() {
    conspiracyWeb_.loadFromFile(Config::Paths::DOCUMENTS_JSON);
    folkloreEvents_.loadEvents();
    hud_.init();

    conspiracyWeb_.discoverNode("tartaria_tablets");

    registerScene("mainmenu", []{ return new MainMenuScene(); });
    registerScene("apartment", []{ return new ApartmentScene(); });
    registerScene("forum", []{ return new ForumScene(); });
    registerScene("field", []{ return new FieldScene(); });
    registerScene("shadow_realm", []{ return new ShadowRealmScene(); });
    registerScene("ending", []{ return new EndingScene(); });
    
    switchTo("mainmenu");
}

void SceneManager::update(float dt) {
    if (transitionState_ == TransitionState::FADE_OUT) {
        transitionAlpha_ += transitionSpeed_ * dt;
        if (transitionAlpha_ >= 255.0f) {
            transitionAlpha_ = 255.0f;
            transitionState_ = TransitionState::FADE_IN;
            
            // Do the actual switch
            if (!currentSceneId_.empty()) {
                Scene* oldScene = createScene(currentSceneId_);
                if (oldScene) oldScene->onExit();
            }
            prevSceneId_ = currentSceneId_;
            currentSceneId_ = pendingScene_;
            
            Scene* newScene = createScene(currentSceneId_);
            if (newScene) {
                newScene->onEnter(pendingFlag_);
            }
        }
        return; // Don't update game during fade out
    } else if (transitionState_ == TransitionState::FADE_IN) {
        transitionAlpha_ -= transitionSpeed_ * dt;
        if (transitionAlpha_ <= 0.0f) {
            transitionAlpha_ = 0.0f;
            transitionState_ = TransitionState::NONE;
        }
    }

    handlePauseInput();

    if (paused_) {
        // Pause menu interaction
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        bool escPressed = keys[SDL_SCANCODE_P];
        if (escPressed && !lastEscState_) {
            paused_ = false;
        }
        lastEscState_ = escPressed;
        
        static bool upHeld = false, downHeld = false, enterHeld = false;
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
            if (!upHeld) pauseSelectedIndex_ = (pauseSelectedIndex_ - 1 + 4) % 4;
            upHeld = true;
        } else upHeld = false;
        
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
            if (!downHeld) pauseSelectedIndex_ = (pauseSelectedIndex_ + 1) % 4;
            downHeld = true;
        } else downHeld = false;
        
        if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_SPACE]) {
            if (!enterHeld) {
                if (pauseSelectedIndex_ == 0) paused_ = false; // CONTINUA JOCUL
                else if (pauseSelectedIndex_ == 1) { resetGameState(); switchTo("mainmenu"); paused_ = false; }
                else if (pauseSelectedIndex_ == 2) { resetGameState(); switchTo("apartment"); paused_ = false; }
                else if (pauseSelectedIndex_ == 3) requestShutdown();
            }
            enterHeld = true;
        } else enterHeld = false;
        
        int mx, my;
        Uint32 mState = SDL_GetMouseState(&mx, &my);
        int menuX = 440, menuY = 320;
        for (int i = 0; i < 4; ++i) {
            int by = menuY + i * 55 - 15;
            if (mx >= menuX && mx <= menuX + 400 && my >= by && my <= by + 40) {
                pauseSelectedIndex_ = i;
                if (mState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                    if (i == 0) paused_ = false;
                    else if (i == 1) { resetGameState(); switchTo("mainmenu"); paused_ = false; }
                    else if (i == 2) { resetGameState(); switchTo("apartment"); paused_ = false; }
                    else if (i == 3) requestShutdown();
                }
            }
        }
        
        return; // Don't run game logic if paused
    }

    if (currentSceneId_.empty()) return;

    if (currentSceneId_ != "mainmenu") {
        paranoiaSystem_.update(dt);
        conspiracyWeb_.updatePhysics();
        folkloreEvents_.update(dt, currentSceneId_);

        hud_.setDay(currentDay_);
        hud_.setParanoia(paranoiaSystem_.getParanoia());
        hud_.setAwakening(paranoiaSystem_.getAwakening());
        hud_.setTimeOfDay(timeOfDay_);
        hud_.update(dt);

        folkloreTimer_ += dt;
        if (folkloreTimer_ >= Config::EVENT_CHECK_INTERVAL) {
            folkloreTimer_ = 0.0f;
            folkloreEvents_.fireRandom(currentSceneId_, paranoiaSystem_.getParanoia());
        }

        /*
        if (paranoiaSystem_.getParanoia() > 0.85f && currentSceneId_ == "field") {
            switchTo("shadow_realm");
        }
        */
    }

    Scene* scene = createScene(currentSceneId_);
    if (scene) {
        scene->update(dt);
    }
}

void SceneManager::handlePauseInput() {
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    bool escPressed = keys[SDL_SCANCODE_P];
    
    if (escPressed && !lastEscState_) {
        if (currentSceneId_ != "mainmenu" && currentSceneId_ != "ending") {
            paused_ = !paused_;
            pauseSelectedIndex_ = 0;
        }
    }
    lastEscState_ = escPressed;
}

void SceneManager::renderPauseOverlay(core::Renderer* r) {
    if (!r) return;
    
    r->setDrawBlendMode(SDL_BLENDMODE_BLEND);
    
    // 1. Semi transparent black full screen
    r->setDrawColor({0, 0, 0, 160});
    r->drawRect(0, 0, 1280, 720, true);
    
    // 2. Centered panel
    int px = 440, py = 190, pw = 400, ph = 340;
    r->setDrawColor({20, 18, 15, 240});
    r->drawRect(px, py, pw, ph, true);
    r->setDrawColor({100, 80, 50, 255}); // border
    r->drawRect(px, py, pw, ph, false);
    r->drawRect(px+1, py+1, pw-2, ph-2, false); // 2px thick

    // 3. Title
    std::string title = "// PAUZA //";
    int tw = static_cast<int>(title.length()) * 18; // approx 6*3 width
    r->drawText(title, 640 - tw/2, py + 20, {200, 180, 120, 255}, 3);
    
    // 4. Separator
    r->setDrawColor({100, 80, 50, 200});
    r->drawLine(px + 40, py + 70, px + pw - 40, py + 70);
    
    // 5. Menu options
    const char* options[] = {
        "CONTINUA JOCUL",
        "MENIU PRINCIPAL",
        "REINCEPE",
        "IESI DIN JOC"
    };
    
    for (int i = 0; i < 4; ++i) {
        bool selected = (pauseSelectedIndex_ == i);
        core::Color tc = selected ? core::Color{200, 180, 120, 255} : core::Color{120, 100, 70, 255};
        
        int cy = py + 130 + i * 55;
        if (selected) {
            r->drawText(">", px + 40, cy, tc, 2);
        }
        
        int w = static_cast<int>(std::string(options[i]).length()) * 12;
        r->drawText(options[i], 640 - w/2, cy, tc, 2);
    }
    
    // Stats
    char stats1[128];
    snprintf(stats1, sizeof(stats1), "Ziua: %d | Paranoia: %d%% | Trezire: %d%%",
             currentDay_, 
             static_cast<int>(paranoiaSystem_.getParanoia() * 100),
             static_cast<int>(paranoiaSystem_.getAwakening() * 100));
    r->drawText(stats1, px + 20, py + ph - 40, {150, 150, 150, 200}, 1);
    
    char stats2[128];
    snprintf(stats2, sizeof(stats2), "Noduri descoperite: %d/%d", 
             conspiracyWeb_.getDiscoveredCount(), 
             conspiracyWeb_.getTotalNodes());
    r->drawText(stats2, px + 20, py + ph - 20, {150, 150, 150, 200}, 1);
}

void SceneManager::render() {
    if (currentSceneId_.empty()) return;

    auto* r = static_cast<core::Renderer*>(renderer_);
    if (!r) return;

    Scene* scene = createScene(currentSceneId_);
    if (scene) {
        scene->render();
    }

    for (auto& overlay : overlays_) {
        overlay();
    }

    if (currentSceneId_ != "mainmenu" && currentSceneId_ != "ending") {
        folkloreEvents_.render(r);
        hud_.render(r, &paranoiaSystem_);
    }

    if (paused_) {
        renderPauseOverlay(r);
    }

    // Fades
    if (transitionState_ != TransitionState::NONE) {
        r->setDrawBlendMode(SDL_BLENDMODE_BLEND);
        r->setDrawColor({0, 0, 0, static_cast<uint8_t>(transitionAlpha_)});
        r->drawRect(0, 0, 1280, 720, true);
    }

    r->present();
}

void SceneManager::switchTo(const std::string& sceneId) {
    switchTo(sceneId, "");
}

void SceneManager::switchTo(const std::string& sceneId, const std::string& entryFlag) {
    if (transitionState_ == TransitionState::NONE) {
        pendingScene_ = sceneId;
        pendingFlag_ = entryFlag;
        transitionState_ = TransitionState::FADE_OUT;
    }
}

Scene* SceneManager::getCurrentScene() {
    return createScene(currentSceneId_);
}

void SceneManager::registerScene(const std::string& id, std::function<Scene*()> factory) {
    scenes_[id] = {id, factory, nullptr};
}

bool SceneManager::hasScene(const std::string& id) const {
    return scenes_.find(id) != scenes_.end();
}

void SceneManager::pushOverlay(std::function<void()> overlay) {
    overlays_.push_back(overlay);
}

void SceneManager::popOverlay() {
    if (!overlays_.empty()) overlays_.pop_back();
}

void SceneManager::setCoreSystems(void* renderer, void* audio, void* input, void* save) {
    renderer_ = renderer;
    audio_ = audio;
    input_ = input;
    save_ = save;
}

Scene* SceneManager::createScene(const std::string& id) {
    auto it = scenes_.find(id);
    if (it == scenes_.end()) return nullptr;

    if (!it->second.instance) {
        it->second.instance.reset(it->second.factory());
        it->second.instance->setManager(this);
        it->second.instance->setRenderer(renderer_);
        it->second.instance->setAudio(audio_);
        it->second.instance->setParanoiaSystem(&paranoiaSystem_);
        it->second.instance->setConspiracyWeb(&conspiracyWeb_);
        it->second.instance->setFolkloreEvents(&folkloreEvents_);
        it->second.instance->init();
    }
    return it->second.instance.get();
}

void SceneManager::destroyScene(Scene*) {}

void SceneManager::resetGameState() {
    currentDay_ = 1;
    timeOfDay_ = 8.0f;
    paranoiaSystem_.reduceParanoia(1.0f);
    // To reset awakening, let's cast back the Game? Actually Game manages flags.
    if (game_) {
        core::Game* g = static_cast<core::Game*>(game_);
        g->clearFlags();
        g->resetEndingState();
    }
    conspiracyWeb_ = systems::ConspiracyWeb(); // Reset
    conspiracyWeb_.loadFromFile(Config::Paths::DOCUMENTS_JSON);
    conspiracyWeb_.discoverNode("tartaria_tablets");
    folkloreEvents_ = systems::FolkloreEvents(); 
    folkloreEvents_.loadEvents();
    
    // Also reinit scenes that store persistent states (Apartment is persistent)
    for (auto& pair : scenes_) {
        if (pair.second.instance) {
            pair.second.instance.reset(); // Destroy existing
        }
    }
}

void SceneManager::requestShutdown() {
    if (game_) {
        static_cast<core::Game*>(game_)->requestShutdown();
    }
}

}
