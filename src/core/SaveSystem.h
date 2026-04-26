#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace core {

class Game;

class SaveSystem {
public:
    SaveSystem();
    ~SaveSystem() = default;

    void init();

    bool save(const Game& game);
    bool load();

    void setParanoia(float v) { currentSave_["paranoia"] = v; }
    void setAwakening(float v) { currentSave_["awakening"] = v; }
    void setCurrentScene(const std::string& scene) { currentSave_["current_scene"] = scene; }
    void addDiscoveredNode(const std::string& nodeId);
    void addInventoryItem(const std::string& item);
    void setFlag(const std::string& flag, bool value);
    void addPlaytime(int seconds) { currentSave_["playtime_seconds"] = seconds; }

    float getParanoia() const;
    float getAwakening() const;
    std::string getCurrentScene() const;
    const std::vector<std::string>& getDiscoveredNodes() const;
    const std::vector<std::string>& getInventory() const;
    bool getFlag(const std::string& flag) const;
    int getPlaytime() const;

    const nlohmann::json& getLastSave() const { return currentSave_; }
    std::string getEndingType() const;

private:
    std::string getSavePath() const;

    nlohmann::json currentSave_;
    std::string savePath_;
};

}

#endif