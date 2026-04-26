#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include <string>
#include <vector>

namespace core { class Renderer; }

namespace ui {

struct DialogueOption {
    std::string text;
    std::string nextNode;
    std::string requiredFlag;
};

struct DialogueNode {
    std::string id;
    std::string speaker;
    std::string text;
    std::vector<DialogueOption> options;
    // Effects on reaching this node
    std::string effectDiscoverNode;
    float effectParanoia = 0.0f;
    float effectAwakening = 0.0f;
};

struct DialogueEffects {
    float paranoia = 0.0f;
    float awakening = 0.0f;
    std::string discoverNode;
};

class DialogueSystem {
public:
    DialogueSystem();
    ~DialogueSystem() = default;

    void init();
    void update(float dt);
    void render(core::Renderer* renderer);

    void startDialogue(const std::string& nodeId);
    void endDialogue();

    bool isActive() const { return active_; }
    const DialogueNode* getCurrentNode() const { return currentNode_; }

    DialogueEffects selectOption(int index);
    void addDialogueNode(const DialogueNode& node);

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

private:
    std::vector<DialogueNode> nodes_;
    const DialogueNode* currentNode_ = nullptr;
    int selectedOption_ = 0;
    bool active_ = false;
    bool visible_ = false;

    // Typewriter effect
    float typewriterTimer_ = 0.0f;
    int typewriterPos_ = 0;
    bool typewriterDone_ = false;
};

}

#endif