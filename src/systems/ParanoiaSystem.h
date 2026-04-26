#ifndef PARANOIA_SYSTEM_H
#define PARANOIA_SYSTEM_H

#include <string>
#include <vector>
#include <functional>

namespace systems {

enum class ParanoiaLevel {
    NORMAL,
    SUSPICIOUS,
    FEARFUL,
    DELUSIONAL,
    BREAKDOWN
};

struct ParanoiaEvent {
    float triggerParanoia;
    std::string description;
    std::function<void()> callback;
};

class ParanoiaSystem {
public:
    ParanoiaSystem();

    void update(float dt);
    void addParanoia(float amount);
    void reduceParanoia(float amount);

    float getParanoia() const { return paranoia_; }
    ParanoiaLevel getLevel() const;
    float getAwakening() const { return awakening_; }

    void addAwakening(float amount);
    bool shouldTriggerGlitch() const;
    bool shouldRevealTruth() const;

    void registerEvent(const ParanoiaEvent& event);
    void triggerEvent(const std::string& description);

    void setPlayerName(const std::string& name) { playerName_ = name; }
    const std::string& getPlayerName() const { return playerName_; }

    static const char* levelToString(ParanoiaLevel level);

private:
    void checkLevelChange();
    void processEvents();
    void applyEnvironmentalParanoia();

    float paranoia_ = 0.0f;
    float awakening_ = 0.0f;
    ParanoiaLevel level_ = ParanoiaLevel::NORMAL;
    ParanoiaLevel prevLevel_ = ParanoiaLevel::NORMAL;

    std::vector<ParanoiaEvent> events_;
    std::vector<std::string> recentEvents_;

    std::string playerName_;

    float environmentalParanoia_ = 0.0f;
    float timeSinceLastEvent_ = 0.0f;
};

}

#endif
