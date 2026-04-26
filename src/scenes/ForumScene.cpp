#include "ForumScene.h"
#include "../core/Renderer.h"
#include "../systems/ParanoiaSystem.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <string>

namespace scenes {

ForumScene::ForumScene() {}

void ForumScene::init() {
    refreshThreads();
}

void ForumScene::refreshThreads() {
    threads_.clear();
    // Default system threads
    threads_.push_back({"[URGENT] Tunelurile sub Casa Poporului - CONFIRMAT", 47, "@lumina_neagra",
        "Am fost acolo. Sub etajul -4 exista un coridor care nu apare pe nicio schema.\nAm vazut simboluri dacice pe pereti.\nSecuritatea m-a prins si m-a dat afara. Nu sunt nebun."});
    threads_.push_back({"[DOVADA] Tablitele Tartaria = cheia portalului", 123, "@dacul_treaz",
        "Am studiat inscriptiile timp de 3 ani. Ele contin coordonate.\nNu sunt scriere - sunt o harta.\nHarta catre ceva ce nu ar trebui sa existe."});
    threads_.push_back({"[BREAKING] Zalmoxis.net va fi INCHIS in 48h - salvati datele", 567, "@admin",
        "Au trimis o scrisoare de la ANCOM. Ne inchid. Dar noi stim adevarul.\nSalvati totul. Faceti backup. Adevarul nu poate fi sters."});

    // Dynamic threads from Conspiracy Web
    if (conspiracyWeb_) {
        auto& nodes = const_cast<systems::ConspiracyWeb*>(conspiracyWeb_)->getNodes();
        for (auto* node : nodes) {
            if (node->discovered) {
                Thread t;
                t.title = "[LEAK] " + node->title;
                t.posts = static_cast<int>(node->paranoia_on_read * 100);
                t.lastPoster = "@zalmoxis_fan";
                t.content = "ID: " + node->id + "\nTIP: " + node->type + "\n\n" + node->content;
                threads_.push_back(t);
            }
        }
    }
}

void ForumScene::update(float dt) {
    cursorBlink_ += dt;
    if (cursorBlink_ > 1.0f) cursorBlink_ -= 1.0f;
}

void ForumScene::render() {
    auto* r = reinterpret_cast<core::Renderer*>(getRendererRaw());
    if (!r) return;

    // Black background
    r->setDrawColor({0, 0, 0, 255});
    r->clear();

    // Header bar
    r->setDrawColor({0, 40, 20, 255});
    r->drawRect(0, 0, 1280, 35, true);
    r->drawText("=== ZALMOXIS.NET v2.3 === [CRIPTAT] === BINE AI VENIT, SASHA ===",
                80, 10, {0, 230, 100, 255}, 2);

    if (openThread_ == -1) {
        // Thread listing
        int startIdx = static_cast<int>(scrollOffset_);
        int visibleCount = 11;
        
        for (int i = 0; i < visibleCount && (startIdx + i) < static_cast<int>(threads_.size()); ++i) {
            int idx = startIdx + i;
            int rowY = 50 + i * 55;

            // Selection highlight
            if (selectedIndex_ == idx) {
                r->setDrawColor({0, 60, 30, 255});
                r->drawRect(0, rowY, 1280, 55, true);
            } else if (idx % 2 == 0) {
                r->setDrawColor({10, 20, 10, 255});
                r->drawRect(0, rowY, 1280, 55, true);
            }
            
            // Thread number
            std::string numStr = std::to_string(idx + 1) + ".";
            r->drawText(numStr, 20, rowY + 8, {0, 180, 80, 255}, 2);

            // Title
            r->drawText(threads_[idx].title, 60, rowY + 8, {0, 220, 100, 255}, 2);

            // Post count + last poster
            std::string meta = std::to_string(threads_[idx].posts) + " posts | " + threads_[idx].lastPoster;
            r->drawText(meta, 60, rowY + 32, {0, 120, 60, 200}, 1);
        }

        // Scroll indicators
        if (scrollOffset_ > 0) r->drawText(" ^^^ ", 1200, 45, {0, 255, 0, 255}, 2);
        if (scrollOffset_ + visibleCount < threads_.size()) r->drawText(" vvv ", 1200, 660, {0, 255, 0, 255}, 2);

        r->drawText("Arrows: Navegare | ENTER: Citete | ESC: Ieire", 300, 695, {0, 150, 80, 200}, 1);
    } else {
        // Reading a thread
        int idx = openThread_;
        if (idx >= 0 && idx < static_cast<int>(threads_.size())) {
            // Thread header
            r->setDrawColor({0, 30, 15, 255});
            r->drawRect(0, 40, 1280, 40, true);
            r->drawText(threads_[idx].title, 30, 50, {0, 255, 120, 255}, 2);

            // Thread content (wrapped)
            std::string contentStr = threads_[idx].content;
            int maxCharsPerLine = 1200 / (6 * 2); // roughly 100 chars
            std::string currentLine = "";
            int ty = 100;
            
            for (size_t i = 0; i < contentStr.length(); ++i) {
                char c = contentStr[i];
                currentLine += c;
                if (c == '\n' || currentLine.length() >= static_cast<size_t>(maxCharsPerLine)) {
                    r->drawText(currentLine, 30, ty, {0, 200, 100, 255}, 2);
                    ty += 25;
                    currentLine = "";
                }
            }
            if (!currentLine.empty()) {
                r->drawText(currentLine, 30, ty, {0, 200, 100, 255}, 2);
            }

            // Meta info
            std::string metaStr = "Postat de: " + threads_[idx].lastPoster + " | " +
                                  std::to_string(threads_[idx].posts) + " raspunsuri";
            r->drawText(metaStr, 30, 200, {0, 120, 60, 200}, 1);

            // Separator
            r->setDrawColor({0, 80, 40, 200});
            r->drawLine(30, 230, 1250, 230);

            // Simulated replies
            r->drawText("@anonimul: Confirm. Am vazut si eu.", 30, 250, {0, 160, 80, 200}, 2);
            r->drawText("@dacul_treaz: Nu va lasati pacaliti. Verificati sursa.", 30, 290, {0, 160, 80, 200}, 2);
            r->drawText("@lumina_neagra: ...ei ne urmaresc...", 30, 330, {0, 140, 70, 180}, 2);

            r->drawText("Press ESC to go back", 30, 695, {0, 150, 80, 200}, 1);
        }
    }
}

void ForumScene::onEnter(const std::string&) {
    openThread_ = -1;
    selectedIndex_ = 0;
    scrollOffset_ = 0;
    refreshThreads();
}
void ForumScene::onExit() {}

bool ForumScene::handleKeyDown(int key) {
    if (folkloreEvents_ && folkloreEvents_->isModalActive()) {
        if (key == SDL_SCANCODE_1) { folkloreEvents_->selectChoice(0); return true; }
        if (key == SDL_SCANCODE_2) { folkloreEvents_->selectChoice(1); return true; }
        return false;
    }

    if (key == SDL_SCANCODE_ESCAPE || key == SDL_SCANCODE_Q) {
        if (openThread_ >= 0) {
            openThread_ = -1;  // Back to thread list
            return true;
        }
        switchTo("apartment");
        return true;
    }

    if (openThread_ == -1) {
        if (key == SDL_SCANCODE_UP || key == SDL_SCANCODE_W) {
            if (selectedIndex_ > 0) {
                selectedIndex_--;
                if (selectedIndex_ < scrollOffset_) scrollOffset_--;
            }
            return true;
        }
        if (key == SDL_SCANCODE_DOWN || key == SDL_SCANCODE_S) {
            if (selectedIndex_ < static_cast<int>(threads_.size()) - 1) {
                selectedIndex_++;
                if (selectedIndex_ >= scrollOffset_ + 11) scrollOffset_++;
            }
            return true;
        }
        if (key == SDL_SCANCODE_RETURN || key == SDL_SCANCODE_SPACE) {
            if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(threads_.size())) {
                openThread_ = selectedIndex_;
                if (paranoiaSystem_) {
                    paranoiaSystem_->addParanoia(0.03f);
                    paranoiaSystem_->addAwakening(0.02f);
                }
                return true;
            }
        }
    }

    return false;
}

bool ForumScene::handleMouseDown(int, int, int) { return false; }

}