#ifndef CONSPIRACY_BOARD_H
#define CONSPIRACY_BOARD_H

#include <string>
#include <vector>
#include <unordered_map>

namespace ui {

struct ConspiracyNode {
    std::string id;
    std::string text;
    std::string position;
    bool discovered = false;
    bool connected = false;
};

struct ConspiracyConnection {
    std::string from;
    std::string to;
    std::string label;
};

class ConspiracyBoard {
public:
    ConspiracyBoard();
    ~ConspiracyBoard() = default;

    void init();
    void update(float dt);
    void render();

    void addNode(const ConspiracyNode& node);
    void connectNodes(const std::string& from, const std::string& to, const std::string& label = "");
    void discoverNode(const std::string& nodeId);
    bool isNodeDiscovered(const std::string& nodeId) const;

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

    void toggle() { visible_ = !visible_; }

private:
    std::vector<ConspiracyNode> nodes_;
    std::vector<ConspiracyConnection> connections_;
    std::unordered_map<std::string, int> nodeIndex_;
    bool visible_ = false;
};

}

#endif