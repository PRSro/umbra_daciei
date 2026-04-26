#include "SaveSystem.h"
#include "Game.h"
#include "../scenes/SceneManager.h"
#include "../Config.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

using json = nlohmann::json;

namespace core {

SaveSystem::SaveSystem() : currentSave_(json::object()) {}

void SaveSystem::init() {
    savePath_ = getSavePath();

    std::string dir = savePath_;
    size_t pos = dir.find_last_of('/');
    if (pos != std::string::npos) {
        dir = dir.substr(0, pos);
        mkdir(dir.c_str(), 0755);
    }
}

std::string SaveSystem::getSavePath() const {
    const char* home = getenv("HOME");
    std::string path = home ? home : ".";
    path += "/" + std::string(Config::Paths::SAVE_FILE);
    return path;
}

bool SaveSystem::save(const Game& game) {
    if (currentSave_.is_null()) currentSave_ = json::object();
    currentSave_["paranoia"] = game.getScenes().getParanoia();
    currentSave_["awakening"] = game.getScenes().getAwakening();
    currentSave_["playtime_seconds"] = static_cast<int>(game.getTotalTime());

    std::ofstream out(savePath_);
    if (out.is_open()) {
        out << currentSave_.dump(2);
        out.close();
        return true;
    }
    return false;
}

bool SaveSystem::load() {
    std::ifstream in(savePath_);
    if (in.is_open()) {
        try {
            in >> currentSave_;
            in.close();
            return true;
        } catch (...) {
        }
    }

    currentSave_ = json::object();
    currentSave_["paranoia"] = 0.0;
    currentSave_["awakening"] = 0.0;
    currentSave_["discovered_nodes"] = json::array();
    currentSave_["inventory"] = json::array();
    currentSave_["flags"] = json::object();
    currentSave_["current_scene"] = "apartment";
    currentSave_["playtime_seconds"] = 0;
    return false;
}

void SaveSystem::addDiscoveredNode(const std::string& nodeId) {
    if (!currentSave_.contains("discovered_nodes")) {
        currentSave_["discovered_nodes"] = json::array();
    }
    auto& arr = currentSave_["discovered_nodes"];
    for (const auto& id : arr) {
        if (id == nodeId) return;
    }
    arr.push_back(nodeId);
}

void SaveSystem::addInventoryItem(const std::string& item) {
    if (!currentSave_.contains("inventory")) {
        currentSave_["inventory"] = json::array();
    }
    currentSave_["inventory"].push_back(item);
}

void SaveSystem::setFlag(const std::string& flag, bool value) {
    if (!currentSave_.contains("flags")) {
        currentSave_["flags"] = json::object();
    }
    currentSave_["flags"][flag] = value;
}

float SaveSystem::getParanoia() const {
    if (currentSave_.contains("paranoia")) {
        return currentSave_["paranoia"].get<float>();
    }
    return 0.0f;
}

float SaveSystem::getAwakening() const {
    if (currentSave_.contains("awakening")) {
        return currentSave_["awakening"].get<float>();
    }
    return 0.0f;
}

std::string SaveSystem::getCurrentScene() const {
    if (currentSave_.contains("current_scene")) {
        return currentSave_["current_scene"].get<std::string>();
    }
    return "apartment";
}

const std::vector<std::string>& SaveSystem::getDiscoveredNodes() const {
    static std::vector<std::string> empty;
    if (currentSave_.contains("discovered_nodes")) {
        const auto& arr = currentSave_["discovered_nodes"];
        static std::vector<std::string> result;
        result.clear();
        for (const auto& id : arr) {
            result.push_back(id.get<std::string>());
        }
        return result;
    }
    return empty;
}

const std::vector<std::string>& SaveSystem::getInventory() const {
    static std::vector<std::string> empty;
    if (currentSave_.contains("inventory")) {
        const auto& arr = currentSave_["inventory"];
        static std::vector<std::string> result;
        result.clear();
        for (const auto& item : arr) {
            result.push_back(item.get<std::string>());
        }
        return result;
    }
    return empty;
}

bool SaveSystem::getFlag(const std::string& flag) const {
    if (currentSave_.contains("flags") && currentSave_["flags"].contains(flag)) {
        return currentSave_["flags"][flag].get<bool>();
    }
    return false;
}

int SaveSystem::getPlaytime() const {
    if (currentSave_.contains("playtime_seconds")) {
        return currentSave_["playtime_seconds"].get<int>();
    }
    return 0;
}

std::string SaveSystem::getEndingType() const {
    if (!currentSave_.is_object()) return "skeptic";

    float paranoia = 0.0f;
    float awakening = 0.0f;
    int nodes = 0;
    bool ritual = false;
    bool sold = false;

    if (currentSave_.contains("paranoia")) paranoia = currentSave_["paranoia"].get<float>();
    if (currentSave_.contains("awakening")) awakening = currentSave_["awakening"].get<float>();
    if (currentSave_.contains("discovered_nodes")) nodes = currentSave_["discovered_nodes"].size();
    if (currentSave_.contains("flags")) {
        if (currentSave_["flags"].contains("ritual_completed")) ritual = currentSave_["flags"]["ritual_completed"];
        if (currentSave_["flags"].contains("sold_out")) sold = currentSave_["flags"]["sold_out"];
    }

    int totalNodes = 15;
    float nodeRatio = static_cast<float>(nodes) / totalNodes;

    if (sold) return "globalist";
    if (nodeRatio >= 0.8f && awakening >= 0.3f && paranoia < 0.5f) return "skeptic";
    if (ritual && awakening >= 0.8f) return "true_believer";
    if (paranoia > 0.9f && awakening > 0.6f) return "hybrid_insane";
    if (nodeRatio >= 1.0f && ritual && std::abs(paranoia - 0.66f) < 0.01f) return "zalmoxis_ascendant";

    return "skeptic";
}

}