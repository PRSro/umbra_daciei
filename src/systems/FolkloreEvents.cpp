#include "FolkloreEvents.h"
#include "../core/Renderer.h"
#include <cstdlib>
#include <algorithm>
#include <vector>

namespace systems {

FolkloreEvents::FolkloreEvents() : globalTime_(0.0f) {}

void FolkloreEvents::loadEvents() {
    registerDefaultEvents();
}

void FolkloreEvents::update(float dt, const std::string& currentScene) {
    (void)currentScene;
    globalTime_ += dt;
}

void FolkloreEvents::fireRandom(const std::string& currentScene, float currentParanoia) {
    if (modalActive_) return;

    // Collect eligible events
    std::vector<FolkloreEvent*> eligible;
    for (auto& pair : events_) {
        auto& ev = pair.second;
        if (ev.triggered && ev.oneShot) continue;
        if (ev.paranoia_min > currentParanoia) continue;
        if (ev.triggerScene != "any" && ev.triggerScene != currentScene) continue;
        eligible.push_back(&ev);
    }

    if (eligible.empty()) return;

    int idx = std::rand() % static_cast<int>(eligible.size());
    activeEvent_ = eligible[idx];
    modalActive_ = true;
}

void FolkloreEvents::selectChoice(int index) {
    if (!modalActive_ || !activeEvent_) return;
    if (index < 0 || index >= static_cast<int>(activeEvent_->choices.size())) return;

    const auto& choice = activeEvent_->choices[index];

    if (onAddParanoia && choice.paranoiaChange != 0.0f)
        onAddParanoia(choice.paranoiaChange);
    if (onAddAwakening && choice.awakeningChange != 0.0f)
        onAddAwakening(choice.awakeningChange);
    if (onDiscoverNode && !choice.discoverNode.empty())
        onDiscoverNode(choice.discoverNode);
    if (onSwitchScene && !choice.switchScene.empty())
        onSwitchScene(choice.switchScene);

    // Mark as triggered (need to find mutable copy)
    for (auto& pair : events_) {
        if (&pair.second == activeEvent_) {
            pair.second.triggered = true;
            break;
        }
    }

    modalActive_ = false;
    activeEvent_ = nullptr;
}

void FolkloreEvents::render(core::Renderer* r) {
    if (!modalActive_ || !activeEvent_ || !r) return;

    r->setDrawBlendMode(SDL_BLENDMODE_BLEND);

    // Dark overlay
    r->setDrawColor({0, 0, 0, 180});
    r->drawRect(0, 0, 1280, 720, true);

    // Modal box
    int boxW = 600, boxH = 200;
    int boxX = (1280 - boxW) / 2;
    int boxY = (720 - boxH) / 2;

    r->setDrawColor({20, 15, 25, 240});
    r->drawRect(boxX, boxY, boxW, boxH, true);
    r->setDrawColor({100, 80, 60, 255});
    r->drawRect(boxX, boxY, boxW, boxH, false);

    // Title
    r->drawText(activeEvent_->title, boxX + 20, boxY + 15, {220, 200, 150, 255}, 2);

    // Description
    r->drawText(activeEvent_->description, boxX + 20, boxY + 45, {180, 180, 170, 255}, 1);

    // Choices
    for (int i = 0; i < static_cast<int>(activeEvent_->choices.size()); ++i) {
        int cy = boxY + 100 + i * 30;
        std::string label = "[" + std::to_string(i + 1) + "] " + activeEvent_->choices[i].text;
        r->drawText(label, boxX + 30, cy, {200, 220, 180, 255}, 2);
    }
}

void FolkloreEvents::triggerEvent(const std::string& id) {
    auto it = events_.find(id);
    if (it != events_.end()) {
        it->second.triggered = true;
    }
}

bool FolkloreEvents::hasEventTriggered(const std::string& id) const {
    auto it = events_.find(id);
    return it != events_.end() && it->second.triggered;
}

const FolkloreEvent* FolkloreEvents::getEvent(const std::string& id) const {
    auto it = events_.find(id);
    return it != events_.end() ? &it->second : nullptr;
}

std::vector<const FolkloreEvent*> FolkloreEvents::getAvailableEvents() const {
    std::vector<const FolkloreEvent*> result;
    for (auto& pair : events_) {
        if (!pair.second.triggered || !pair.second.oneShot) {
            result.push_back(&pair.second);
        }
    }
    return result;
}

std::vector<const FolkloreEvent*> FolkloreEvents::getTriggeredEvents() const {
    std::vector<const FolkloreEvent*> result;
    for (auto& pair : events_) {
        if (pair.second.triggered) {
            result.push_back(&pair.second);
        }
    }
    return result;
}

int FolkloreEvents::getTriggeredCount() const {
    int count = 0;
    for (auto& pair : events_) {
        if (pair.second.triggered) count++;
    }
    return count;
}

void FolkloreEvents::registerDefaultEvents() {
    // Event 1: Batrana cu ata rosie
    {
        FolkloreEvent ev;
        ev.id = "old_woman_thread";
        ev.title = "Batrana cu ata rosie";
        ev.description = "O batrana iti ofera ata rosie la usa. Accepti?";
        ev.triggerScene = "apartment";
        ev.paranoia_min = 0.0f;
        ev.oneShot = true;
        ev.choices.push_back({"Accepta", 0.02f, 0.0f, "", ""});
        ev.choices.push_back({"Refuza", 0.05f, 0.0f, "", ""});
        events_[ev.id] = ev;
    }

    // Event 2: Oglinda
    {
        FolkloreEvent ev;
        ev.id = "mirror_reflection";
        ev.title = "Oglinda";
        ev.description = "Oglinda arata o a doua reflexie in spatele tau.";
        ev.triggerScene = "apartment";
        ev.paranoia_min = 0.3f;
        ev.oneShot = true;
        ev.choices.push_back({"Investigheaza", 0.15f, 0.0f, "", "shadow_realm"});
        ev.choices.push_back({"Ignora", 0.1f, 0.0f, "", ""});
        events_[ev.id] = ev;
    }

    // Event 3: Iele in padure
    {
        FolkloreEvent ev;
        ev.id = "iele_singing";
        ev.title = "Iele in padure";
        ev.description = "Auzi cantec femeiesc venind din padure...";
        ev.triggerScene = "field";
        ev.paranoia_min = 0.2f;
        ev.oneShot = true;
        ev.choices.push_back({"Urmareste", 0.2f, -0.1f, "", ""});
        ev.choices.push_back({"Marcheaza pe harta", 0.0f, 0.05f, "", ""});
        events_[ev.id] = ev;
    }

    // Event 4: Notificare TikTok
    {
        FolkloreEvent ev;
        ev.id = "tiktok_zalmoxis";
        ev.title = "Notificare TikTok @ZalmoxisAdevarat";
        ev.description = "Ai o notificare. Vizionezi videoclipul?";
        ev.triggerScene = "any";
        ev.paranoia_min = 0.1f;
        ev.oneShot = false;
        ev.choices.push_back({"Vizioneza", 0.08f, 0.12f, "", ""});
        ev.choices.push_back({"Blocheaza", 0.0f, 0.0f, "", ""});
        events_[ev.id] = ev;
    }

    // Event 5: Zgomot in pereti
    {
        FolkloreEvent ev;
        ev.id = "wall_noise";
        ev.title = "Zgomot in pereti";
        ev.description = "Ceva se misca in pereti. Iti aminteste de ceva.";
        ev.triggerScene = "apartment";
        ev.paranoia_min = 0.4f;
        ev.oneShot = true;
        ev.choices.push_back({"Asculta", 0.1f, 0.0f, "dacian_realm", ""});
        ev.choices.push_back({"Ignora", 0.05f, 0.0f, "", ""});
        events_[ev.id] = ev;
    }

    // Event 6: Visul recurent
    {
        FolkloreEvent ev;
        ev.id = "recurring_dream";
        ev.title = "Visul recurent";
        ev.description = "Visezi muntii Carpati. O voce spune: 'Daca esti gata, vino.'";
        ev.triggerScene = "apartment";
        ev.paranoia_min = 0.0f;
        ev.oneShot = true;
        ev.choices.push_back({"Aminteste-ti", 0.0f, 0.05f, "", ""});
        ev.choices.push_back({"Uita", 0.0f, 0.0f, "", ""});
        events_[ev.id] = ev;
    }
}

}