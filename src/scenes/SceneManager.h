#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include "../systems/ParanoiaSystem.h"
#include "../systems/ConspiracyWeb.h"
#include "../systems/FolkloreEvents.h"
#include "../ui/HUD.h"

namespace core { class Renderer; }

namespace scenes {

class Scene;

enum class TransitionState { NONE, FADE_OUT, FADE_IN };

class SceneManager {
public:
    SceneManager();
    SceneManager(void* game);
    ~SceneManager();

    void init();
    void update(float dt);
    void render();

    void switchTo(const std::string& sceneId);
    void switchTo(const std::string& sceneId, const std::string& entryFlag);

    Scene* getCurrentScene();
    const std::string& getCurrentSceneId() const { return currentSceneId_; }

    void registerScene(const std::string& id, std::function<Scene*()> factory);
    bool hasScene(const std::string& id) const;

    void pushOverlay(std::function<void()> overlay);
    void popOverlay();
    bool hasOverlay() const { return !overlays_.empty(); }

    void setCoreSystems(void* renderer, void* audio, void* input, void* save);

    systems::ParanoiaSystem& getParanoiaSystem() { return paranoiaSystem_; }
    systems::ConspiracyWeb& getConspiracyWeb() { return conspiracyWeb_; }
    systems::FolkloreEvents& getFolkloreEvents() { return folkloreEvents_; }
    ui::HUD& getHUD() { return hud_; }

    float getParanoia() const { return paranoiaSystem_.getParanoia(); }
    float getAwakening() const { return paranoiaSystem_.getAwakening(); }
    void* getAudio() const { return audio_; }

    int getCurrentDay() const { return currentDay_; }    void setCurrentDay(int day) { currentDay_ = day; }
    float getTimeOfDay() const { return timeOfDay_; }
    void setTimeOfDay(float t) { timeOfDay_ = t; }

    template<typename T>
    T* getCurrentSceneAs() {
        return dynamic_cast<T*>(getCurrentScene());
    }

    void resetGameState();
    void requestShutdown();

private:
    struct SceneEntry {
        std::string id;
        std::function<Scene*()> factory;
        std::unique_ptr<Scene> instance;
    };

    Scene* createScene(const std::string& id);
    void destroyScene(Scene* scene);
    void handlePauseInput();
    void renderPauseOverlay(core::Renderer* r);

    std::unordered_map<std::string, SceneEntry> scenes_;
    std::string currentSceneId_;
    std::string prevSceneId_;

    std::vector<std::function<void()>> overlays_;

    void* renderer_ = nullptr;
    void* audio_ = nullptr;
    void* input_ = nullptr;
    void* save_ = nullptr;
    void* game_ = nullptr;

    systems::ParanoiaSystem paranoiaSystem_;
    systems::ConspiracyWeb conspiracyWeb_;
    systems::FolkloreEvents folkloreEvents_;
    ui::HUD hud_;

    int currentDay_ = 1;
    float timeOfDay_ = 8.0f;
    float folkloreTimer_ = 0.0f;

    // Pause UI
    bool paused_ = false;
    int pauseSelectedIndex_ = 0;
    bool lastEscState_ = false;

    // Transitions
    TransitionState transitionState_ = TransitionState::NONE;
    float transitionAlpha_ = 0.0f;
    float transitionSpeed_ = 255.0f / 0.4f;
    std::string pendingScene_;
    std::string pendingFlag_;
};

class Scene {
public:
    virtual ~Scene() = default;

    virtual void init() {}
    virtual void update(float /*dt*/) {}
    virtual void render() {}

    void setRenderer(void* r) { renderer_ = r; }
    void* getRendererRaw() const { return renderer_; }
    template<typename T>
    T* getRenderer() const { return static_cast<T*>(renderer_); }

    virtual void onEnter(const std::string& /*entryFlag*/) {}
    virtual void onExit() {}

    virtual bool handleKeyDown(int /*key*/) { return false; }
    virtual bool handleMouseDown(int /*x*/, int /*y*/, int /*button*/) { return false; }

    void setManager(SceneManager* manager) { manager_ = manager; }
    SceneManager* getManager() const { return manager_; }

    void setParanoiaSystem(systems::ParanoiaSystem* ps) { paranoiaSystem_ = ps; }
    systems::ParanoiaSystem* getParanoiaSystem() const { return paranoiaSystem_; }

    void setConspiracyWeb(systems::ConspiracyWeb* cw) { conspiracyWeb_ = cw; }
    systems::ConspiracyWeb* getConspiracyWeb() const { return conspiracyWeb_; }

    void setFolkloreEvents(systems::FolkloreEvents* fe) { folkloreEvents_ = fe; }
    systems::FolkloreEvents* getFolkloreEvents() const { return folkloreEvents_; }

    void setAudio(void* a) { audio_ = a; }
    void* getAudioRaw() const { return audio_; }

protected:
    void switchTo(const std::string& sceneId) {
        if (manager_) {
            manager_->switchTo(sceneId);
        }
    }
    void switchTo(const std::string& sceneId, const std::string& entryFlag) {
        if (manager_) {
            manager_->switchTo(sceneId, entryFlag);
        }
    }

    SceneManager* manager_ = nullptr;
    void* renderer_ = nullptr;
    systems::ParanoiaSystem* paranoiaSystem_ = nullptr;
    systems::ConspiracyWeb* conspiracyWeb_ = nullptr;
    systems::FolkloreEvents* folkloreEvents_ = nullptr;
    void* audio_ = nullptr;
};

}

#endif
