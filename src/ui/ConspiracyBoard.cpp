#include "ConspiracyBoard.h"

namespace ui {
ConspiracyBoard::ConspiracyBoard() {}
void ConspiracyBoard::init() {}
void ConspiracyBoard::update(float) {}
void ConspiracyBoard::render() {}

void ConspiracyBoard::addNode(const ConspiracyNode& node) {
    nodes_.push_back(node);
    nodeIndex_[node.id] = nodes_.size() - 1;
}

void ConspiracyBoard::connectNodes(const std::string& from, const std::string& to, const std::string& label) {
    connections_.push_back({from, to, label});
}

void ConspiracyBoard::discoverNode(const std::string& nodeId) {
    auto it = nodeIndex_.find(nodeId);
    if (it != nodeIndex_.end()) {
        nodes_[it->second].discovered = true;
    }
}

bool ConspiracyBoard::isNodeDiscovered(const std::string& nodeId) const {
    auto it = nodeIndex_.find(nodeId);
    if (it != nodeIndex_.end()) {
        return nodes_[it->second].discovered;
    }
    return false;
}
}