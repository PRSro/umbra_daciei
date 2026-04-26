#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <unordered_map>

namespace entities {

class Player {
public:
    Player();
    ~Player() = default;

    void init();

    void setPosition(float x, float y);
    float getX() const { return x_; }
    float getY() const { return y_; }

    void setVelocity(float vx, float vy);
    float getVX() const { return vx_; }
    float getVY() const { return vy_; }

    void setParanoia(float paranoia) { paranoia_ = paranoia; }
    float getParanoia() const { return paranoia_; }

    void addEvidence(int amount);
    int getEvidence() const { return evidence_; }

    void setFlag(const std::string& key, bool value);
    bool getFlag(const std::string& key) const;

    void setCurrentScene(const std::string& sceneId) { currentScene_ = sceneId; }
    const std::string& getCurrentScene() const { return currentScene_; }

    bool handleKeyDown(int key);
    void update(float dt);

private:
    float x_ = 0.0f, y_ = 0.0f;
    float vx_ = 0.0f, vy_ = 0.0f;
    float paranoia_ = 0.0f;
    int evidence_ = 0;
    std::string currentScene_;
    std::unordered_map<std::string, bool> flags_;
};

}

#endif