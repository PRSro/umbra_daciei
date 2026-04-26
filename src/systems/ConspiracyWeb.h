#ifndef CONSPIRACY_WEB_H
#define CONSPIRACY_WEB_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace systems {

struct ConspiracyNode {
    std::string id;
    std::string title;
    std::string content;
    std::string type;
    bool discovered = false;
    std::vector<std::string> connections;
    float paranoia_on_read = 0.0f;
    float awakening_on_read = 0.0f;
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
};

class ConspiracyWeb {
public:
    ConspiracyWeb();

    void loadFromFile(const std::string& path);
    void discoverNode(const std::string& id);
    bool isDiscovered(const std::string& id) const;
    const ConspiracyNode* getNode(const std::string& id) const;

    void updatePhysics();
    void updatePositions();

    int getTotalNodes() const { return static_cast<int>(nodes_.size()); }
    int getDiscoveredCount() const;
    float getDiscoveryRatio() const;

    const std::vector<ConspiracyNode*>& getNodes() { return nodePtrs_; }

private:
    void loadNodes(const nlohmann::json& data);
    void loadHardcodedFallback();
    void runForceSimulation();
    void addNode(const std::string& id, const std::string& title, const std::string& type,
                 const std::vector<std::string>& connections, float paranoia, float awakening, const std::string& content);

    std::unordered_map<std::string, ConspiracyNode> nodes_;
    std::vector<ConspiracyNode*> nodePtrs_;
};

}

#endif