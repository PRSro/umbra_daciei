#ifndef MAIN_MENU_SCENE_H
#define MAIN_MENU_SCENE_H

#include "SceneManager.h"
#include <vector>

namespace scenes {

class MainMenuScene : public Scene {
public:
    MainMenuScene();
    void init() override;
    void update(float dt) override;
    void render() override;
    void onEnter(const std::string& entryFlag) override;
    void onExit() override;
    bool handleKeyDown(int key) override;
    bool handleMouseDown(int x, int y, int button) override;

private:
    struct Star {
        int x, y;
        float baseAlpha;
        float phase;
    };
    std::vector<Star> stars_;

    struct Button {
        int x, y, w, h;
        std::string text;
    };
    std::vector<Button> buttons_;

    int selectedIndex_ = 0;
    float timeAccumulator_ = 0.0f;
    float strigoiFlashTimer_ = 0.0f;
    float globalFlickerTimer_ = 0.0f;
    float saveErrorFlashTimer_ = 0.0f;
    
    float strigoiX_ = 0.0f;

    void executeSelected();
    bool saveExists() const;
    void resetGameState();
};

}

#endif
