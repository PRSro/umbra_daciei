#ifndef ENDING_SCENE_H
#define ENDING_SCENE_H

#include "SceneManager.h"
#include "../core/Renderer.h"
#include "../systems/GlitchRenderer.h"
#include <string>
#include <vector>
#include <cmath>

namespace scenes {

struct Projectile {
    float x, y, vx, vy;
    float radius;
    core::Color color;
    bool homing;
    float lifetime;
};

struct ExplosionParticle {
    float x, y, vx, vy;
    float radius;
    float alpha;
    float lifetime;
    core::Color color;
};

struct FallingTriangle {
    float x, y, vx, vy;
    float rotation;
    float rotationSpeed;
    float alpha;
};

class EndingScene : public Scene {
public:
    EndingScene();
    void init() override;
    void update(float dt) override;
    void render() override;
    void onEnter(const std::string& entryFlag) override;
    void onExit() override;
    bool handleKeyDown(int key) override;

private:
    void resetCombat();
    void resetAscension();

    void updateAscension(float dt);
    void renderAscension();

    void updateCombat(float dt);
    void renderCombat();
    void renderCombatArena();
    void renderZalmoxis();
    void renderPlayerCombat();
    void renderProjectiles();
    void renderCombatHUD();
    void updateProjectiles(float dt);
    void spawnProjectileRing();
    void spawnSpiralShot();
    void spawnHomingOrbs();
    void spawnShockwave();
    void spawnRainOfProjectiles();
    void spawnCrossBeam();
    void updateZalmoxisAI(float dt);
    bool checkBeamCollision(float bx1, float by1, float bx2, float by2);

    void renderPlayerFigure(int x, int y, core::Color col, int scale = 1, float dissolve = 0.0f);
    void renderHeart(int x, int y, bool filled);

    std::string entryFlag_;
    float totalTime_ = 0.0f;

    int phase_ = 0;
    float phaseTimer_ = 0.0f;

    std::vector<std::string> typewriterLines_;
    int currentLine_ = 0;
    float lineTimer_ = 0.0f;
    int charIndex_ = 0;

    float pillarHeight_ = 0.0f;
    float playerY_ = 600.0f;
    std::vector<std::pair<float, float>> playerTrail_;
    float goldAlpha_ = 0.0f;
    std::string currentText_;

    int wordIndex_ = 0;
    float wordTimer_ = 0.0f;

    std::vector<core::Color> bgPulse_;
    int strigoiTilt_ = 0;

    bool combatMode_ = false;
    int combatPhase_ = 1;
    float zx_ = 640.0f, zy_ = 220.0f;
    float zalmoxisAngle_ = 0.0f;
    int zalmoxisHP_ = 9;
    int playerHP_ = 5;
    float punchCooldown_ = 0.0f;
    float dodgeCooldown_ = 0.0f;
    bool isDodging_ = false;
    float dodgeTimer_ = 0.0f;
    float dodgeVx_ = 0.0f, dodgeVy_ = 0.0f;
    float px_ = 640.0f, py_ = 580.0f;
    bool combatStarted_ = false;
    bool introCutsceneDone_ = false;
    float introTimer_ = 0.0f;
    float shakeMag_ = 0.0f;

    bool prevLHeld_ = false;
    bool prevPHeld_ = false;

    float spiralAngle_ = 0.0f;
    float beamTimer_ = 0.0f;
    bool beamTelegraphing_ = false;
    bool beamFiring_ = false;
    float beamStartX_ = 0.0f, beamStartY_ = 0.0f;
    float beamEndX_ = 0.0f, beamEndY_ = 0.0f;

    float phaseAnnounceTimer_ = 0.0f;
    std::string phaseAnnounceText_;
    bool phaseChanged_ = false;

    float invincibilityFlash_ = 0.0f;
    float screenFlashAlpha_ = 0.0f;
    core::Color screenFlashColor_ = {255, 255, 255, 255};

    std::string combatText_;
    float combatTextTimer_ = 0.0f;

    std::vector<Projectile> projectiles_;
    std::vector<ExplosionParticle> explosionParticles_;
    std::vector<FallingTriangle> fallingTriangles_;
    std::vector<std::pair<float, float>> afterimagePositions_;

    float crossBeamAngle_ = 0.0f;
    bool crossBeamActive_ = false;
    float crossBeamTimer_ = 0.0f;

    bool zalmoxisEnraged_ = false;
    float bgFlickerTimer_ = 0.0f;
    float heartSpawnTimer_ = 0.0f;

    bool playerWon_ = false;
    bool playerDied_ = false;
    float victoryTimer_ = 0.0f;
    float deathTimer_ = 0.0f;

    std::vector<std::pair<float, float>> stars_;
    float starRotation_ = 0.0f;
    float columnHeight_ = 0.0f;

    struct GoldenRain { float x, y, speed; };
    std::vector<GoldenRain> goldRain_;

    systems::GlitchRenderer glitchRenderer_;

    int currentDay_ = 1;
    float currentParanoia_ = 0.0f;
    int currentNodes_ = 0;
    int totalNodes_ = 15;

    bool prevSpaceHeld_ = false;
    bool prevXHeld_ = false;
};

}

#endif
