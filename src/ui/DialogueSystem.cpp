#include "DialogueSystem.h"
#include "../core/Renderer.h"

namespace ui {
DialogueSystem::DialogueSystem() {}
void DialogueSystem::init() {}

void DialogueSystem::update(float dt) {
    if (!active_ || !currentNode_) return;

    // Typewriter advance
    if (!typewriterDone_) {
        typewriterTimer_ += dt;
        int targetPos = static_cast<int>(typewriterTimer_ * 33.0f); // ~30ms per char
        if (targetPos >= static_cast<int>(currentNode_->text.size())) {
            typewriterPos_ = static_cast<int>(currentNode_->text.size());
            typewriterDone_ = true;
        } else {
            typewriterPos_ = targetPos;
        }
    }
}

void DialogueSystem::render(core::Renderer* r) {
    if (!active_ || !currentNode_ || !r) return;

    r->setDrawBlendMode(SDL_BLENDMODE_BLEND);

    // Bottom panel background
    int panelH = 200;
    int panelY = 720 - panelH;
    r->setDrawColor({10, 8, 15, 230});
    r->drawRect(0, panelY, 1280, panelH, true);

    // Border
    r->setDrawColor({100, 80, 50, 255});
    r->drawRect(0, panelY, 1280, 2, true);

    // Speaker name
    core::Color speakerColor = {220, 180, 100, 255};
    if (currentNode_->speaker == "SASHA") speakerColor = {100, 200, 150, 255};
    r->drawText(currentNode_->speaker, 30, panelY + 15, speakerColor, 2);

    // Text with typewriter
    std::string visibleText = currentNode_->text.substr(0, typewriterPos_);
    r->drawText(visibleText, 30, panelY + 50, {200, 200, 190, 255}, 2);

    // Choices (only if typewriter is done)
    if (typewriterDone_ && !currentNode_->options.empty()) {
        for (int i = 0; i < static_cast<int>(currentNode_->options.size()); ++i) {
            int cy = panelY + 110 + i * 30;
            std::string label = "[" + std::to_string(i + 1) + "] " + currentNode_->options[i].text;
            r->drawText(label, 50, cy, {180, 220, 150, 255}, 2);
        }
    }
}

void DialogueSystem::startDialogue(const std::string& nodeId) {
    for (const auto& node : nodes_) {
        if (node.id == nodeId) {
            currentNode_ = &node;
            active_ = true;
            visible_ = true;
            typewriterTimer_ = 0.0f;
            typewriterPos_ = 0;
            typewriterDone_ = false;
            return;
        }
    }
    // "END" or not found — end dialogue
    endDialogue();
}

void DialogueSystem::endDialogue() {
    active_ = false;
    currentNode_ = nullptr;
    typewriterDone_ = false;
    typewriterPos_ = 0;
    typewriterTimer_ = 0.0f;
}

DialogueEffects DialogueSystem::selectOption(int index) {
    DialogueEffects effects;

    if (!currentNode_) return effects;

    // If typewriter not done, skip to full text
    if (!typewriterDone_) {
        typewriterPos_ = static_cast<int>(currentNode_->text.size());
        typewriterDone_ = true;
        return effects;
    }

    if (index >= 0 && index < static_cast<int>(currentNode_->options.size())) {
        selectedOption_ = index;
        std::string nextNode = currentNode_->options[index].nextNode;

        // Collect effects from the NEXT node
        for (const auto& node : nodes_) {
            if (node.id == nextNode) {
                effects.paranoia = node.effectParanoia;
                effects.awakening = node.effectAwakening;
                effects.discoverNode = node.effectDiscoverNode;
                break;
            }
        }

        startDialogue(nextNode);
    }
    return effects;
}

void DialogueSystem::addDialogueNode(const DialogueNode& node) {
    nodes_.push_back(node);
}
}