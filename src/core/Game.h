#ifndef GAME_H
#define GAME_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>

#include <SDL2/SDL.h>
#include "../Config.h"

namespace scenes { class SceneManager; }

namespace core {

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    DIALOGUE,
    ENDING,
    SHUTDOWN
};

class Renderer;
class AudioManager;
class InputHandler;
class SaveSystem;

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void requestShutdown();

    void update(float dt);
    void render(Renderer& renderer);

    Renderer& getRenderer() { return *renderer_; }
    AudioManager& getAudio() { return *audio_; }
    InputHandler& getInput() { return *input_; }
    SaveSystem& getSave() { return *save_; }
    scenes::SceneManager& getScenes() { return *sceneManager_; }
    const scenes::SceneManager& getScenes() const { return *sceneManager_; }

    GameState getState() const { return state_; }
    void setState(GameState s);

    float getDeltaTime() const { return dt_; }
    double getTotalTime() const { return totalTime_; }
    uint64_t getFrameCount() const { return frameCount_; }

    void addFlag(const std::string& flag);
    bool hasFlag(const std::string& flag) const;
    void clearFlags();
    const std::unordered_set<std::string>& getFlags() const { return flags_; }

    void addNotification(const std::string& text);
    const std::vector<std::string>& getNotifications() const { return notifications_; }

    bool shouldShowEnding() const;
    std::string getEndingType() const;
    void resetEndingState() { endingChosen_ = false; }

private:
    void processInput();
    void fixedUpdate();
    void saveIfNeeded();

    SDL_Window* window_ = nullptr;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<AudioManager> audio_;
    std::unique_ptr<InputHandler> input_;
    std::unique_ptr<SaveSystem> save_;

    std::unique_ptr<scenes::SceneManager> sceneManager_;

    GameState state_ = GameState::MENU;
    GameState prevState_ = GameState::MENU;
    bool endingChosen_ = false;

    float dt_ = 0.0f;
    double totalTime_ = 0.0;
    uint64_t frameCount_ = 0;

    bool running_ = false;
    bool shutdownRequested_ = false;

    std::unordered_set<std::string> flags_;
    std::vector<std::string> notifications_;

    std::chrono::steady_clock::time_point lastFixedUpdate_;
    std::chrono::steady_clock::time_point lastSave_;

    int autoSaveCounter_ = 0;
};

}

#endif