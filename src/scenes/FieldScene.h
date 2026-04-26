#ifndef FIELD_SCENE_H
#define FIELD_SCENE_H
#include "SceneManager.h"
#include <array>
#include <vector>
#include "../ui/DialogueSystem.h"
#include "../systems/GlitchRenderer.h"

namespace scenes {

enum class TileType : int { FLOOR=0, TREE=1, SHADOW=2, RITUAL_SITE=3 };

enum class StrigoiState { PATROL, ALERT, CHASE, SEARCH };

class FieldScene : public Scene {
public:
    FieldScene();
    void init() override;
    void update(float dt) override;
    void render() override;
    void onEnter(const std::string& entryFlag) override;
    void onExit() override;
    bool handleKeyDown(int key) override;

private:
    static constexpr int MAP_W = 40;
    static constexpr int MAP_H = 23;
    static constexpr int TILE_SZ = 32;

    TileType tiles_[MAP_H][MAP_W];
    bool mapGenerated_ = false;

    // Player
    float playerX_ = 64.0f;
    float playerY_ = 368.0f;
    float facingAngle_ = 0.0f;

    // Camera
    float cameraX_ = 0.0f;
    float cameraY_ = 0.0f;

    // Strigois
    struct StrigoiData {
        float x, y;
        StrigoiState state;
        float alertTimer;
        float searchTimer;
        float chaseTimeout;
        int waypointIdx;
    };
    std::vector<StrigoiData> strigois_;

    // Secret Entrance & Dialogue
    ui::DialogueSystem dialogue_;
    bool dialogueInitialized_ = false;
    bool secretEntranceDiscovered_ = false;

    struct Waypoint { float x, y; };
    std::vector<Waypoint> waypoints_;

    // Ritual sites
    struct RitualSite { int tx, ty; bool activated; };
    std::vector<RitualSite> ritualSites_;

    // Rune minigame
    bool runeActive_ = false;
    int runeStep_ = 0;
    int runeSequence_[4];
    float runeShowTimer_ = 0.0f;
    bool runeShowingSequence_ = true;
    int runeInput_ = -1;
    int currentRitualIdx_ = -1;

    // Time
    float totalTime_ = 0.0f;
    float gravityRollTimer_ = 0.0f;
    float gravityDir_ = 1.0f;  // 1.0 = down, -1.0 = up
    float gravityStrength_ = 200.0f; // Adjusted to be noticeable
    float antigravityTimer_ = 0.0f;
    bool antigravityActive_ = false;
    float flipTimer_ = 0.0f;
    bool isFlipping_ = false;

    systems::GlitchRenderer glitchRenderer_;

    struct Particle { float x, y, speed, alpha; };
    std::vector<Particle> particles_;

    void generateMap();
    void generateTreeProgram();
    bool isWalkable(float x, float y) const;
    bool isRitualTile(int tx, int ty) const;

    std::string treeProgram_;
};

}
#endif