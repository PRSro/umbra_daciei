#ifndef SHADOW_REALM_SCENE_H
#define SHADOW_REALM_SCENE_H

#include "SceneManager.h"

namespace scenes {

class ShadowRealmScene : public Scene {
public:
    ShadowRealmScene();
    void init() override;
    void update(float dt) override;
    void render() override;
    void onEnter(const std::string& entryFlag) override;
    void onExit() override;
    bool handleKeyDown(int key) override;
    bool handleMouseDown(int x, int y, int button) override;

private:
    float playerX_ = 640.0f;
    float playerY_ = 360.0f;
    float totalTime_ = 0.0f;
    float timeInRealm_ = 0.0f;

    // Iele positions
    struct Iele { float baseX, baseY; };
    Iele iele_[3];

    // Pillar
    float pillarX_ = 640.0f;
    float pillarY_ = 360.0f;
    bool pillarActivated_ = false;

    // Flash effect
    float flashTimer_ = 0.0f;

    // Text cycling
    int textPhase_ = 0;
    float textTimer_ = 0.0f;
};

}

#endif