#ifndef FORUM_SCENE_H
#define FORUM_SCENE_H
#include "SceneManager.h"
#include <string>
#include <vector>

namespace scenes {

class ForumScene : public Scene {
public:
    ForumScene();
    void init() override;
    void update(float dt) override;
    void render() override;
    void onEnter(const std::string& entryFlag) override;
    void onExit() override;
    bool handleKeyDown(int key) override;
    bool handleMouseDown(int x, int y, int button) override;

private:
    void refreshThreads();

    struct Thread {
        std::string title;
        int posts;
        std::string lastPoster;
        std::string content;
    };
    std::vector<Thread> threads_;

    int openThread_ = -1;  // -1 = listing, >=0 = reading thread
    int selectedIndex_ = 0;
    float cursorBlink_ = 0.0f;
    float scrollOffset_ = 0.0f;
};

}

#endif