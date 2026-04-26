#ifndef FOLKLORE_EVENTS_H
#define FOLKLORE_EVENTS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace core { class Renderer; }

namespace systems {

struct FolkloreChoice {
    std::string text;
    float paranoiaChange = 0.0f;
    float awakeningChange = 0.0f;
    std::string discoverNode;
    std::string switchScene;
};

struct FolkloreEvent {
    std::string id;
    std::string title;
    std::string description;
    std::string triggerScene;  // "apartment", "field", "any"
    float paranoia_min = 0.0f;
    std::vector<FolkloreChoice> choices;
    bool triggered = false;
    bool oneShot = true;
};

class FolkloreEvents {
public:
    FolkloreEvents();

    void loadEvents();
    void update(float dt, const std::string& currentScene);

    void fireRandom(const std::string& currentScene, float currentParanoia);
    void render(core::Renderer* renderer);

    void selectChoice(int index);
    bool isModalActive() const { return modalActive_; }

    void triggerEvent(const std::string& id);
    bool hasEventTriggered(const std::string& id) const;

    const FolkloreEvent* getEvent(const std::string& id) const;
    std::vector<const FolkloreEvent*> getAvailableEvents() const;
    std::vector<const FolkloreEvent*> getTriggeredEvents() const;

    void setGlobalTime(float time) { globalTime_ = time; }
    float getGlobalTime() const { return globalTime_; }

    int getEventCount() const { return static_cast<int>(events_.size()); }
    int getTriggeredCount() const;

    // Callbacks set by SceneManager
    std::function<void(float)> onAddParanoia;
    std::function<void(float)> onAddAwakening;
    std::function<void(const std::string&)> onDiscoverNode;
    std::function<void(const std::string&)> onSwitchScene;

private:
    void registerDefaultEvents();

    std::unordered_map<std::string, FolkloreEvent> events_;
    float globalTime_ = 0.0f;

    // Modal state
    bool modalActive_ = false;
    const FolkloreEvent* activeEvent_ = nullptr;
};

}

#endif
