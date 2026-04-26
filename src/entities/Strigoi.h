#ifndef STRIGOI_H
#define STRIGOI_H

#include <string>

namespace entities {

class Strigoi {
public:
    Strigoi();
    ~Strigoi() = default;

    void init();

    void setId(const std::string& id) { id_ = id; }
    const std::string& getId() const { return id_; }

    void setPosition(float x, float y);
    float getX() const { return x_; }
    float getY() const { return y_; }

    void setActive(bool active) { active_ = active; }
    bool isActive() const { return active_; }

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

    void setHuntPhase(int phase) { huntPhase_ = phase; }
    int getHuntPhase() const { return huntPhase_; }

    void setTarget(const std::string& targetId) { target_ = targetId; }
    const std::string& getTarget() const { return target_; }

    void setCurrentScene(const std::string& sceneId) { currentScene_ = sceneId; }
    const std::string& getCurrentScene() const { return currentScene_; }

    bool canSeePlayer(float playerX, float playerY, float visionRange);
    bool canHearPlayer(float playerX, float playerY, float hearingRange);

    void update(float dt);

private:
    std::string id_;
    float x_ = 0.0f, y_ = 0.0f;
    bool active_ = false;
    bool visible_ = false;
    int huntPhase_ = 0;
    std::string target_;
    std::string currentScene_;
};

}

#endif