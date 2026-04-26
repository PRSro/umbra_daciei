#ifndef APARTMENT_SCENE_H
#define APARTMENT_SCENE_H

#include "SceneManager.h"
#include "../ui/DialogueSystem.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace scenes {

struct Interactable {
    std::string id;
    std::string name;
    float x, y;
    float width, height;
    std::string onInteractFlag;
    std::string onInspectFlag;
    std::string requiredFlag;
    bool discovered = false;
};

class ApartmentScene : public Scene {
public:
    ApartmentScene();
    virtual ~ApartmentScene() = default;

    void init() override;
    void update(float dt) override;
    void render() override;

    void onEnter(const std::string& entryFlag) override;
    void onExit() override;

    bool handleKeyDown(int key) override;
    bool handleMouseDown(int x, int y, int button) override;

    void addInteractable(const Interactable& interactable);
    void discoverInteractable(const std::string& id);
    const Interactable* getInteractable(const std::string& id) const;

    void setComputerUnlocked(bool unlocked) { computerUnlocked_ = unlocked; }
    bool isComputerUnlocked() const { return computerUnlocked_; }

    void setDay(int day) { currentDay_ = day; }
    int getDay() const { return currentDay_; }

    void advanceDay();

private:
    void loadLayout();

    std::vector<Interactable> interactables_;
    std::unordered_map<std::string, int> interactableMap_;

    int currentDay_ = 1;
    bool computerUnlocked_ = false;
    bool journalRead_ = false;

    float timeOfDay_ = 8.0f;
    float dayProgress_ = 0.0f;

    // Player position
    float playerX_ = 640.0f;
    float playerY_ = 400.0f;

    // Rain animation
    float rainOffset_ = 0.0f;

    // Sleep message
    float sleepMsgTimer_ = 0.0f;
    std::string sleepMsg_;

    // Terminal message
    float termMsgTimer_ = 0.0f;
    std::string termMsg_;

    // Folklore timer
    float folkloreTimer_ = 0.0f;

    // NPC: Vecina
    float vecinaX_ = 1220.0f;
    float vecinaY_ = 350.0f;

    // Dialogue system
    ui::DialogueSystem dialogue_;
    bool dialogueInitialized_ = false;
};

}

#endif
