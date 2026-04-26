#ifndef NPC_H
#define NPC_H

#include <string>

namespace entities {

class NPC {
public:
    NPC();
    ~NPC() = default;

    void init();

    void setId(const std::string& id) { id_ = id; }
    const std::string& getId() const { return id_; }

    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }

    void setPosition(float x, float y);
    float getX() const { return x_; }
    float getY() const { return y_; }

    void setDialogueNode(const std::string& nodeId) { dialogueNode_ = nodeId; }
    const std::string& getDialogueNode() const { return dialogueNode_; }

    void setTrust(float trust) { trust_ = trust; }
    float getTrust() const { return trust_; }

    void setMet(bool met) { met_ = met; }
    bool isMet() const { return met_; }

    void setCurrentScene(const std::string& sceneId) { currentScene_ = sceneId; }
    const std::string& getCurrentScene() const { return currentScene_; }

    bool interact();

private:
    std::string id_;
    std::string name_;
    float x_ = 0.0f, y_ = 0.0f;
    std::string dialogueNode_;
    float trust_ = 50.0f;
    bool met_ = false;
    std::string currentScene_;
};

}

#endif