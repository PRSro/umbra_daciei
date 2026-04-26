#include "ParanoiaSystem.h"
#include <algorithm>

namespace systems {

ParanoiaSystem::ParanoiaSystem() : paranoia_(0.0f), awakening_(0.0f), level_(ParanoiaLevel::NORMAL) {}

void ParanoiaSystem::update(float) {}

void ParanoiaSystem::addParanoia(float amount) {
    paranoia_ = std::clamp(paranoia_ + amount, 0.0f, 1.0f);
    checkLevelChange();
}

void ParanoiaSystem::reduceParanoia(float amount) {
    paranoia_ = std::clamp(paranoia_ - amount, 0.0f, 1.0f);
    checkLevelChange();
}

void ParanoiaSystem::addAwakening(float amount) {
    awakening_ = std::clamp(awakening_ + amount, 0.0f, 1.0f);
}

ParanoiaLevel ParanoiaSystem::getLevel() const { return level_; }

bool ParanoiaSystem::shouldTriggerGlitch() const {
    return paranoia_ > 0.6f;
}

bool ParanoiaSystem::shouldRevealTruth() const {
    return paranoia_ > 0.85f;
}

void ParanoiaSystem::registerEvent(const ParanoiaEvent&) {}

void ParanoiaSystem::triggerEvent(const std::string&) {}

const char* ParanoiaSystem::levelToString(ParanoiaLevel level) {
    switch (level) {
        case ParanoiaLevel::NORMAL: return "Normal";
        case ParanoiaLevel::SUSPICIOUS: return "Suspicious";
        case ParanoiaLevel::FEARFUL: return "Fearful";
        case ParanoiaLevel::DELUSIONAL: return "Delusional";
        case ParanoiaLevel::BREAKDOWN: return "Breakdown";
    }
    return "Unknown";
}

void ParanoiaSystem::checkLevelChange() {
    prevLevel_ = level_;
    if (paranoia_ < 0.2f) level_ = ParanoiaLevel::NORMAL;
    else if (paranoia_ < 0.4f) level_ = ParanoiaLevel::SUSPICIOUS;
    else if (paranoia_ < 0.6f) level_ = ParanoiaLevel::FEARFUL;
    else if (paranoia_ < 0.85f) level_ = ParanoiaLevel::DELUSIONAL;
    else level_ = ParanoiaLevel::BREAKDOWN;
}

void ParanoiaSystem::processEvents() {}

void ParanoiaSystem::applyEnvironmentalParanoia() {}

}