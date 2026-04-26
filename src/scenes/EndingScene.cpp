#include "EndingScene.h"
#include "../core/Renderer.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

namespace scenes {

EndingScene::EndingScene() {
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < 80; i++) {
        float angle = (rand() % 360) * 3.14159f / 180.0f;
        float dist = 50.0f + rand() % 350;
        stars_.push_back({cos(angle) * dist, sin(angle) * dist});
    }
}

void EndingScene::init() {}

void EndingScene::resetAscension() {
    phase_ = 0;
    phaseTimer_ = 0.0f;
    typewriterLines_ = {
        "Simți podeaua dispărând sub picioare.",
        "Nu mai ești în București.",
        "Nu mai ești în timp."
    };
    currentLine_ = 0;
    lineTimer_ = 0.0f;
    charIndex_ = 0;
    pillarHeight_ = 0.0f;
    playerY_ = 600.0f;
    playerTrail_.clear();
    goldAlpha_ = 0.0f;
    wordTimer_ = 0.0f;
    strigoiTilt_ = 0;
    columnHeight_ = 0.0f;
    goldRain_.clear();
    for (int i = 0; i < 50; i++) {
        goldRain_.push_back({static_cast<float>(rand() % 1280), static_cast<float>(rand() % 720), 100.0f + rand() % 200});
    }
    bgPulse_.clear();
    for (int i = 0; i < 60; i++) {
        float t = static_cast<float>(i) / 60.0f;
        core::Color c;
        c.r = static_cast<uint8_t>(15 + t * 205);
        c.g = static_cast<uint8_t>(5 + t * 185);
        c.b = static_cast<uint8_t>(25 + t * 55);
        c.a = 255;
        bgPulse_.push_back(c);
    }
}

void EndingScene::resetCombat() {
    combatPhase_ = 1;
    zx_ = 640.0f; zy_ = 220.0f;
    zalmoxisAngle_ = 0.0f;
    zalmoxisHP_ = 9;
    playerHP_ = 5;
    punchCooldown_ = 0.0f;
    dodgeCooldown_ = 0.0f;
    isDodging_ = false;
    dodgeTimer_ = 0.0f;
    px_ = 640.0f; py_ = 580.0f;
    combatStarted_ = false;
    introCutsceneDone_ = false;
    introTimer_ = 0.0f;
    shakeMag_ = 0.0f;
    zy_ = -100.0f; // Start above screen for drop
    prevLHeld_ = false;
    prevPHeld_ = false;

    spiralAngle_ = 0.0f;
    beamTimer_ = 0.0f;
    beamTelegraphing_ = false;
    beamFiring_ = false;

    phaseAnnounceTimer_ = 0.0f;
    phaseAnnounceText_ = "";
    phaseChanged_ = false;

    invincibilityFlash_ = 0.0f;
    screenFlashAlpha_ = 0.0f;
    combatText_ = "";
    combatTextTimer_ = 0.0f;

    projectiles_.clear();
    explosionParticles_.clear();
    fallingTriangles_.clear();
    afterimagePositions_.clear();

    crossBeamAngle_ = 0.0f;
    crossBeamActive_ = false;
    crossBeamTimer_ = 0.0f;

    zalmoxisEnraged_ = false;
    bgFlickerTimer_ = 0.0f;
    heartSpawnTimer_ = 0.0f;

    playerWon_ = false;
    playerDied_ = false;
    victoryTimer_ = 0.0f;
    deathTimer_ = 0.0f;

    currentDay_ = manager_ ? manager_->getCurrentDay() : 1;
    currentParanoia_ = manager_ ? manager_->getParanoia() * 100.0f : 0.0f;
    if (conspiracyWeb_) {
        currentNodes_ = conspiracyWeb_->getDiscoveredCount();
        totalNodes_ = 15;
    } else {
        currentNodes_ = 0;
        totalNodes_ = 15;
    }
}

void EndingScene::onEnter(const std::string& entryFlag) {
    entryFlag_ = entryFlag;
    totalTime_ = 0.0f;

    if (entryFlag_ == "ascension") {
        combatMode_ = false;
        resetAscension();
    } else if (entryFlag_ == "combat") {
        combatMode_ = true;
        resetCombat();
    } else {
        combatMode_ = false;
        phase_ = 0;
        phaseTimer_ = 0.0f;
        typewriterLines_ = {"Jocul s-a terminat. Dar intrebarile raman."};
        currentLine_ = 0;
        lineTimer_ = 0.0f;
        charIndex_ = 0;
    }
}

void EndingScene::onExit() {
    projectiles_.clear();
    explosionParticles_.clear();
    fallingTriangles_.clear();
    afterimagePositions_.clear();
}

bool EndingScene::handleKeyDown(int key) {
    if (!combatMode_) {
        if (key == SDL_SCANCODE_ESCAPE || key == SDL_SCANCODE_RETURN) {
            if (phase_ >= 6) {
                switchTo("mainmenu");
                return true;
            }
        }
    } else {
        if (playerDied_ || playerWon_) {
            if (key == SDL_SCANCODE_RETURN) {
                if (playerWon_) {
                    switchTo("mainmenu");
                } else if (playerDied_) {
                    resetCombat();
                }
                return true;
            }
        }
    }
    return false;
}

void EndingScene::update(float dt) {
    totalTime_ += dt;

    if (combatMode_) {
        updateCombat(dt);
    } else {
        updateAscension(dt);
    }
}

void EndingScene::updateAscension(float dt) {
    phaseTimer_ += dt;

    if (phase_ == 0) {
        lineTimer_ += dt;
        if (lineTimer_ >= 1.0f && currentLine_ < static_cast<int>(typewriterLines_.size())) {
            lineTimer_ = 0.0f;
            currentLine_++;
            charIndex_ = 0;
        }
        if (currentLine_ < static_cast<int>(typewriterLines_.size())) {
            int targetChars = static_cast<int>(lineTimer_ * 30);
            if (targetChars > static_cast<int>(typewriterLines_[currentLine_].size())) {
                targetChars = static_cast<int>(typewriterLines_[currentLine_].size());
            }
            charIndex_ = targetChars;
        }
        if (phaseTimer_ >= 3.0f) {
            phase_ = 1;
            phaseTimer_ = 0.0f;
            currentText_ = "Pilonul se ridică. Te ridici odată cu el.";
        }
    } else if (phase_ == 1) {
        pillarHeight_ = std::min((phaseTimer_) / 3.0f * 400.0f, 400.0f);
        if (phaseTimer_ >= 3.0f) {
            phase_ = 2;
            phaseTimer_ = 0.0f;
            playerTrail_.clear();
            currentText_ = "Zalmoxis te recunoaște. Sângele tău e vechi.";
        }
    } else if (phase_ == 2) {
        playerY_ = 600.0f - (phaseTimer_ / 3.0f) * 400.0f;
        if (playerY_ < 200.0f) playerY_ = 200.0f;
        
        static float trailTimer = 0.0f;
        trailTimer += dt;
        if (trailTimer >= 0.05f) {
            playerTrail_.push_back({640.0f, playerY_});
            if (playerTrail_.size() > 15) playerTrail_.erase(playerTrail_.begin());
            trailTimer = 0.0f;
        }

        if (phaseTimer_ >= 3.0f) {
            phase_ = 3;
            phaseTimer_ = 0.0f;
            currentText_ = "Ei s-au închinat. Ai venit acasă.";
        }
    } else if (phase_ == 3) {
        // Stars spiral inward
        for (auto& star : stars_) {
            // Keep original dist in mind? No, just lerp current toward 0
            star.first *= (1.0f - dt * 0.8f);
            star.second *= (1.0f - dt * 0.8f);
        }
        if (phaseTimer_ >= 3.0f) {
            phase_ = 4;
            phaseTimer_ = 0.0f;
            currentText_ = "Zalmoxis vorbește prin tine.";
            glitchRenderer_.triggerGlitch(systems::GlitchEffect::Chromatic, 1.0f, 2.0f);
        }
    } else if (phase_ == 4) {
        if (phaseTimer_ >= 3.0f) {
            phase_ = 5;
            phaseTimer_ = 0.0f;
            columnHeight_ = 0.0f;
        }
    } else if (phase_ == 5) {
        columnHeight_ = std::min(phaseTimer_ / 3.0f * 720.0f, 720.0f);
        if (phaseTimer_ >= 4.0f) {
            phase_ = 6;
            phaseTimer_ = 0.0f;
        }
    } else if (phase_ == 6) {
        goldAlpha_ = std::min(phaseTimer_ / 2.0f * 255.0f, 255.0f);
        if (phaseTimer_ >= 7.0f) { // 2s fade + 5s hold
            switchTo("mainmenu");
        }
    }
}

void EndingScene::updateCombat(float dt) {
    if (playerDied_) {
        deathTimer_ += dt;
        if (deathTimer_ < 0.1f) {
            glitchRenderer_.triggerGlitch(systems::GlitchEffect::Noise, 0.8f, 3.0f);
            glitchRenderer_.triggerGlitch(systems::GlitchEffect::Chromatic, 0.6f, 3.0f);
        }
        if (deathTimer_ < 3.0f) {
            updateProjectiles(dt * 0.2f);
        }
        return;
    }

    if (!introCutsceneDone_) {
        introTimer_ += dt;
        if (introTimer_ < 1.0f) {
            // Step 1: Typewriter
            int len = static_cast<int>(std::string("Zalmoxis nu te recunoaste ca egal.").length());
            int visible = static_cast<int>(introTimer_ * len);
            combatText_ = std::string("Zalmoxis nu te recunoaste ca egal.").substr(0, visible);
            combatTextTimer_ = 0.1f;
        } else if (introTimer_ < 2.0f) {
            // Step 2: Drop
            if (introTimer_ < 1.1f) {
                screenFlashAlpha_ = 255.0f;
                screenFlashColor_ = {255, 255, 255, 255};
            }
            zy_ = -100.0f + (introTimer_ - 1.0f) * 320.0f / 1.0f;
            if (zy_ > 220.0f) zy_ = 220.0f;
        } else if (introTimer_ < 3.0f) {
            // Step 3: Shake
            shakeMag_ = 8.0f * (1.0f - (introTimer_ - 2.0f));
        } else if (introTimer_ < 4.0f) {
            // Step 4: Announcement
            if (introTimer_ < 3.1f && phaseAnnounceText_ != "FAZA I") {
                phaseAnnounceTimer_ = 1.0f;
                phaseAnnounceText_ = "FAZA I";
                screenFlashAlpha_ = 200.0f;
                screenFlashColor_ = {200, 0, 0, 255};
            }
        } else {
            introCutsceneDone_ = true;
            combatStarted_ = true;
            combatText_ = "";
        }
        return;
    }

    if (playerWon_) {
        victoryTimer_ += dt;
        if (victoryTimer_ >= 2.0f && victoryTimer_ < 5.0f && fallingTriangles_.empty()) {
            for (int i = 0; i < 7; i++) {
                FallingTriangle ft;
                ft.x = zx_ + cos(i * 3.14159f * 2.0f / 7.0f) * 30.0f;
                ft.y = zy_ - 40.0f;
                ft.vx = (rand() % 100 - 50) * 2.0f;
                ft.vy = 0.0f;
                ft.rotation = 0.0f;
                ft.rotationSpeed = (rand() % 100 - 50) * 5.0f;
                ft.alpha = 255.0f;
                fallingTriangles_.push_back(ft);
            }
        }
        for (auto& ft : fallingTriangles_) {
            ft.vy += 300.0f * dt;
            ft.x += ft.vx * dt;
            ft.y += ft.vy * dt;
            ft.rotation += ft.rotationSpeed * dt;
            ft.alpha -= 50.0f * dt;
        }
        fallingTriangles_.erase(
            std::remove_if(fallingTriangles_.begin(), fallingTriangles_.end(),
                [](const FallingTriangle& ft) { return ft.alpha <= 0 || ft.y > 700; }),
            fallingTriangles_.end());
        return;
    }

    combatStarted_ = true;

    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (!keys) return;

    float moveSpeed = 160.0f;
    float moveX = 0.0f, moveY = 0.0f;
    if (keys[SDL_SCANCODE_W]) moveY -= 1.0f;
    if (keys[SDL_SCANCODE_S]) moveY += 1.0f;
    if (keys[SDL_SCANCODE_A]) moveX -= 1.0f;
    if (keys[SDL_SCANCODE_D]) moveX += 1.0f;

    if (isDodging_) {
        dodgeTimer_ -= dt;
        px_ += dodgeVx_ * dt;
        py_ += dodgeVy_ * dt;
        if (dodgeTimer_ <= 0) {
            isDodging_ = false;
            dodgeCooldown_ = 1.2f;
        }
    } else {
        if (moveX != 0 || moveY != 0) {
            float len = sqrt(moveX * moveX + moveY * moveY);
            moveX /= len;
            moveY /= len;
            px_ += moveX * moveSpeed * dt;
            py_ += moveY * moveSpeed * dt;
        }

        if (dodgeCooldown_ > 0) dodgeCooldown_ -= dt;

        bool spacePressed = keys[SDL_SCANCODE_SPACE] && !prevSpaceHeld_;
        bool lPressed = keys[SDL_SCANCODE_L] && !prevLHeld_;
        bool pPressed = keys[SDL_SCANCODE_P] && !prevPHeld_;
        
        bool tripleHit = lPressed && pPressed;
        if (!tripleHit) {
            // Check if one was already held and the other is new
            tripleHit = (lPressed && keys[SDL_SCANCODE_P]) || (pPressed && keys[SDL_SCANCODE_L]);
        }

        prevSpaceHeld_ = keys[SDL_SCANCODE_SPACE];
        prevLHeld_ = keys[SDL_SCANCODE_L];
        prevPHeld_ = keys[SDL_SCANCODE_P];

        if (spacePressed && dodgeCooldown_ <= 0) {
            isDodging_ = true;
            dodgeTimer_ = 0.4f;
            if (moveX != 0 || moveY != 0) {
                float len = sqrt(moveX * moveX + moveY * moveY);
                dodgeVx_ = moveX / len * 300.0f;
                dodgeVy_ = moveY / len * 300.0f;
            } else {
                dodgeVx_ = 0;
                dodgeVy_ = -300.0f;
            }
        }

        if (tripleHit && punchCooldown_ <= 0) {
            punchCooldown_ = 1.2f;
            combatText_ = "TRIPLA LOVITURA!";
            combatTextTimer_ = 0.8f;
            
            // Launch 3 projectiles toward Zalmoxis
            float baseAngle = atan2(zy_ - py_, zx_ - px_);
            for (int i = -1; i <= 1; i++) {
                Projectile p;
                p.x = px_; p.y = py_;
                float angle = baseAngle + i * 0.2f;
                p.vx = cos(angle) * 400.0f;
                p.vy = sin(angle) * 400.0f;
                p.radius = 12.0f;
                p.color = {100, 255, 255, 255};
                p.homing = false;
                p.lifetime = 2.0f;
                projectiles_.push_back(p);
            }
        } else if (lPressed || pPressed) {
            float dx = px_ - zx_;
            float dy = py_ - zy_;
            float dist = sqrt(dx * dx + dy * dy);
            float hitRange = (combatPhase_ == 3) ? 180.0f : 120.0f;
            if (dist < hitRange && punchCooldown_ <= 0) {
                zalmoxisHP_--;
                punchCooldown_ = 0.8f;
                combatText_ = "LOVIT!";
                combatTextTimer_ = 0.5f;

                float pushDirX = dx / dist;
                float pushDirY = dy / dist;
                zx_ += pushDirX * 60.0f;
                zy_ += pushDirY * 60.0f;

                screenFlashAlpha_ = 80.0f;
                screenFlashColor_ = {255, 255, 255, 255};

                if (zalmoxisHP_ == 6 && combatPhase_ == 1) {
                    combatPhase_ = 2;
                    phaseChanged_ = true;
                    phaseAnnounceTimer_ = 2.0f;
                    phaseAnnounceText_ = "FURIA ZEULUI";
                    screenFlashAlpha_ = 150;
                    screenFlashColor_ = {100, 50, 200, 255};
                } else if (zalmoxisHP_ == 3 && combatPhase_ == 2) {
                    combatPhase_ = 3;
                    phaseChanged_ = true;
                    phaseAnnounceTimer_ = 2.0f;
                    phaseAnnounceText_ = "FORMA FINALĂ";
                    screenFlashAlpha_ = 150;
                    screenFlashColor_ = {100, 50, 200, 255};
                } else if (zalmoxisHP_ == 1) {
                    zalmoxisEnraged_ = true;
                }
            } else if (punchCooldown_ <= 0) {
                combatText_ = "PREA DEPARTE";
                combatTextTimer_ = 0.5f;
                punchCooldown_ = 0.3f;
            }
        }

        if (punchCooldown_ > 0) punchCooldown_ -= dt;
    }

    float centerX = 640.0f, centerY = 500.0f;
    float dx = px_ - centerX;
    float dy = py_ - centerY;
    float dist = sqrt(dx * dx + dy * dy);
    if (dist > 300.0f) {
        px_ = centerX + (dx / dist) * 300.0f;
        py_ = centerY + (dy / dist) * 300.0f;
    }

    py_ = std::max(200.0f, std::min(py_, 650.0f));
    px_ = std::max(340.0f, std::min(px_, 940.0f));

    updateZalmoxisAI(dt);
    updateProjectiles(dt);

    if (phaseChanged_) {
        phaseChanged_ = false;
    }

    if (phaseAnnounceTimer_ > 0) phaseAnnounceTimer_ -= dt;
    if (combatTextTimer_ > 0) combatTextTimer_ -= dt;
    if (invincibilityFlash_ > 0) invincibilityFlash_ -= dt;
    if (screenFlashAlpha_ > 0) screenFlashAlpha_ -= dt * 300.0f;

    if (zalmoxisHP_ <= 0 && !playerWon_) {
        playerWon_ = true;
        victoryTimer_ = 0.0f;
        glitchRenderer_.triggerGlitch(systems::GlitchEffect::Invert, 0.5f, 0.5f);
        for (int i = 0; i < 50; i++) {
            FallingTriangle ft;
            ft.x = zx_;
            ft.y = zy_;
            ft.vx = (rand() % 200 - 100) * 2.0f;
            ft.vy = (rand() % 200 - 300) * 1.0f;
            ft.rotation = 0.0f;
            ft.rotationSpeed = (rand() % 100 - 50) * 5.0f;
            ft.alpha = 255.0f;
            fallingTriangles_.push_back(ft);
        }
        screenFlashAlpha_ = 200.0f;
        screenFlashColor_ = {255, 255, 255, 255};
    }

    if (playerWon_) {
        victoryTimer_ += dt;
        
        std::string winText = "Ai castigat dreptul de a cunoaste.";
        int chars = static_cast<int>(victoryTimer_ * 15);
        if (chars > static_cast<int>(winText.size())) chars = static_cast<int>(winText.size());
        combatText_ = winText.substr(0, chars);
        combatTextTimer_ = 0.1f;

        if (victoryTimer_ > 6.0f) {
            combatMode_ = false;
            resetAscension();
        }
    }

    if (playerHP_ <= 0 && !playerDied_) {
        playerDied_ = true;
        deathTimer_ = 0.0f;
        for (int i = 0; i < 12; i++) {
            ExplosionParticle p;
            p.x = px_;
            p.y = py_;
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 50.0f + rand() % 150;
            p.vx = cos(angle) * speed;
            p.vy = sin(angle) * speed;
            p.radius = 4.0f + rand() % 4;
            p.alpha = 255.0f;
            p.lifetime = 1.5f;
            p.color = {80, 80, 80, 255};
            explosionParticles_.push_back(p);
        }
    }

    for (auto& p : explosionParticles_) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.alpha -= 170.0f * dt;
    }
    explosionParticles_.erase(
        std::remove_if(explosionParticles_.begin(), explosionParticles_.end(),
            [](const ExplosionParticle& p) { return p.alpha <= 0; }),
        explosionParticles_.end());
}

void EndingScene::updateZalmoxisAI(float dt) {
    float speedMult = zalmoxisEnraged_ ? 1.3f : 1.0f;

    if (combatPhase_ == 1) {
        zalmoxisAngle_ += dt * 0.6f * speedMult;
        zx_ = 640.0f + cos(zalmoxisAngle_) * 180.0f;
        zy_ = 280.0f + sin(zalmoxisAngle_) * 80.0f;

        beamTimer_ += dt;
        float beamInterval = 5.0f / speedMult;

        if (beamTimer_ >= beamInterval - 1.5f && !beamTelegraphing_ && !beamFiring_) {
            beamTelegraphing_ = true;
            beamStartX_ = zx_;
            beamStartY_ = zy_;
            beamEndX_ = px_;
            beamEndY_ = py_;
        }

        if (beamTimer_ >= beamInterval - 1.5f && beamTimer_ < beamInterval - 1.2f) {
        }

        if (beamTimer_ >= beamInterval - 1.2f && beamTimer_ < beamInterval - 0.9f) {
            beamTelegraphing_ = false;
            beamFiring_ = true;
        }

        if (beamFiring_) {
            if (checkBeamCollision(beamStartX_, beamStartY_, beamEndX_, beamEndY_)) {
                if (!isDodging_) {
                    playerHP_--;
                    invincibilityFlash_ = 0.5f;
                    screenFlashAlpha_ = 100.0f;
                    screenFlashColor_ = {255, 50, 50, 255};
                }
            }
        }

        if (beamTimer_ >= beamInterval - 0.9f + 0.3f) {
            beamFiring_ = false;
            beamTimer_ = 0.0f;
        }

        static float ringTimer = 0.0f;
        ringTimer += dt;
        if (ringTimer >= 3.0f / speedMult) {
            ringTimer = 0.0f;
            spawnProjectileRing();
        }
    } else if (combatPhase_ == 2) {
        static float targetTimer = 0.0f;
        static float targetX = zx_;
        static float targetY = zy_;
        targetTimer += dt;

        if (targetTimer >= 2.0f / speedMult) {
            targetTimer = 0.0f;
            targetX = 440.0f + rand() % 400;
            targetY = 150.0f + rand() % 150;
        }

        float dirX = targetX - zx_;
        float dirY = targetY - zy_;
        float dirLen = sqrt(dirX * dirX + dirY * dirY);
        if (dirLen > 1.0f) {
            zx_ += (dirX / dirLen) * 200.0f * dt * speedMult;
            zy_ += (dirY / dirLen) * 200.0f * dt * speedMult;
        }

        static float spiralTimer = 0.0f;
        spiralTimer += dt;
        if (spiralTimer >= 2.0f / speedMult) {
            spiralTimer = 0.0f;
            spawnSpiralShot();
        }

        static float orbTimer = 0.0f;
        orbTimer += dt;
        if (orbTimer >= 4.0f / speedMult) {
            orbTimer = 0.0f;
            spawnHomingOrbs();
        }

        static float shockTimer = 0.0f;
        shockTimer += dt;
        if (shockTimer >= 6.0f / speedMult) {
            shockTimer = 0.0f;
            spawnShockwave();
        }

        heartSpawnTimer_ += dt;
        if (heartSpawnTimer_ >= 7.0f) {
            heartSpawnTimer_ = 0.0f;
            Projectile p;
            p.x = static_cast<float>(200 + rand() % 880);
            p.y = static_cast<float>(200 + rand() % 400);
            p.vx = 0; p.vy = 0;
            p.radius = 15.0f;
            p.color = {255, 50, 80, 255};
            p.homing = false;
            p.lifetime = 5.0f;
            projectiles_.push_back(p);
        }
    } else if (combatPhase_ == 3) {
        static float teleportTimer = 0.0f;
        teleportTimer += dt * speedMult;

        if (teleportTimer >= 1.5f) {
            teleportTimer = 0.0f;
            afterimagePositions_.push_back({zx_, zy_});
            if (afterimagePositions_.size() > 8) {
                afterimagePositions_.erase(afterimagePositions_.begin());
            }
            zx_ = 440.0f + rand() % 400;
            zy_ = 150.0f + rand() % 300;
        }

        static float rainTimer = 0.0f;
        rainTimer += dt;
        if (rainTimer >= 0.4f / speedMult) {
            rainTimer = 0.0f;
            spawnRainOfProjectiles();
        }

        crossBeamTimer_ += dt;
        if (crossBeamTimer_ >= 3.0f && !crossBeamActive_) {
            crossBeamActive_ = true;
            crossBeamAngle_ = 0.0f;
            crossBeamTimer_ = 0.0f;
        }

        if (crossBeamActive_) {
            crossBeamAngle_ += dt * 45.0f;
            if (crossBeamTimer_ >= 2.5f) {
                crossBeamActive_ = false;
                crossBeamTimer_ = 0.0f;
            }

            for (int i = 0; i < 4; i++) {
                float angle = crossBeamAngle_ + i * 90.0f;
                float ex = zx_ + cos(angle * 3.14159f / 180.0f) * 350.0f;
                float ey = zy_ + sin(angle * 3.14159f / 180.0f) * 350.0f;
                if (checkBeamCollision(zx_, zy_, ex, ey)) {
                    if (!isDodging_) {
                        playerHP_--;
                        invincibilityFlash_ = 0.5f;
                        screenFlashAlpha_ = 100.0f;
                        screenFlashColor_ = {255, 50, 50, 255};
                    }
                }
            }
        }

        bgFlickerTimer_ += dt;
    }
}

void EndingScene::spawnProjectileRing() {
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159f * 2.0f / 8.0f;
        Projectile p;
        p.x = zx_;
        p.y = zy_;
        p.vx = cos(angle) * 120.0f;
        p.vy = sin(angle) * 120.0f;
        p.radius = 8.0f;
        p.color = {200, 100, 255, 255};
        p.homing = false;
        p.lifetime = 5.0f;
        projectiles_.push_back(p);
    }
}

void EndingScene::spawnSpiralShot() {
    for (int i = 0; i < 12; i++) {
        float angle = spiralAngle_ + i * 3.14159f * 2.0f / 12.0f;
        Projectile p;
        p.x = zx_;
        p.y = zy_;
        p.vx = cos(angle) * 150.0f;
        p.vy = sin(angle) * 150.0f;
        p.radius = 8.0f;
        p.color = {200, 100, 255, 255};
        p.homing = false;
        p.lifetime = 5.0f;
        projectiles_.push_back(p);
    }
    spiralAngle_ += 30.0f * 3.14159f / 180.0f;
}

void EndingScene::spawnHomingOrbs() {
    for (int i = 0; i < 3; i++) {
        Projectile p;
        p.x = zx_ + (rand() % 100 - 50);
        p.y = zy_ + (rand() % 100 - 50);
        p.vx = 0;
        p.vy = 0;
        p.radius = 10.0f;
        p.color = {255, 50, 200, 255};
        p.homing = true;
        p.lifetime = 8.0f;
        projectiles_.push_back(p);
    }
}

void EndingScene::spawnShockwave() {
    zy_ = 350.0f;

    for (size_t i = 0; i < 1; i++) {
        Projectile p;
        p.x = zx_;
        p.y = zy_;
        p.vx = 0;
        p.vy = 0;
        p.radius = 0;
        p.color = {180, 100, 255, 150};
        p.homing = false;
        p.lifetime = 0.8f;
        p.homing = false;
        projectiles_.push_back(p);
    }

    float playerDx = px_ - zx_;
    float playerDy = py_ - zy_;
    float playerDist = sqrt(playerDx * playerDx + playerDy * playerDy);
    if (!isDodging_ && playerDist < 350.0f) {
        playerHP_--;
        invincibilityFlash_ = 0.5f;
        screenFlashAlpha_ = 100.0f;
        screenFlashColor_ = {180, 100, 255, 255};
    }
}

void EndingScene::spawnRainOfProjectiles() {
    for (int i = -1; i <= 1; i++) {
        float baseAngle = atan2(py_ - zy_, px_ - zx_);
        float angle = baseAngle + i * 15.0f * 3.14159f / 180.0f;
        Projectile p;
        p.x = zx_;
        p.y = zy_;
        p.vx = cos(angle) * 200.0f;
        p.vy = sin(angle) * 200.0f;
        p.radius = 6.0f;
        p.color = {200, 100, 255, 255};
        p.homing = false;
        p.lifetime = 4.0f;
        projectiles_.push_back(p);
    }
}

void EndingScene::spawnCrossBeam() {
    crossBeamActive_ = true;
    crossBeamAngle_ = 0.0f;
    crossBeamTimer_ = 0.0f;
}

bool EndingScene::checkBeamCollision(float bx1, float by1, float bx2, float by2) {
    float dx = bx2 - bx1;
    float dy = by2 - by1;
    float len = sqrt(dx * dx + dy * dy);
    if (len < 1.0f) return false;

    float t = std::max(0.0f, std::min(1.0f, ((px_ - bx1) * dx + (py_ - by1) * dy) / (len * len)));
    float closestX = bx1 + t * dx;
    float closestY = by1 + t * dy;
    float closestDist = sqrt((px_ - closestX) * (px_ - closestX) + (py_ - closestY) * (py_ - closestY));

    return closestDist < 12.0f;
}

void EndingScene::updateProjectiles(float dt) {
    for (auto& p : projectiles_) {
        if (p.homing) {
            float dx = px_ - p.x;
            float dy = py_ - p.y;
            float dist = sqrt(dx * dx + dy * dy);
            if (dist > 1.0f) {
                p.vx += (dx / dist) * 60.0f * dt;
                p.vy += (dy / dist) * 60.0f * dt;
                float speed = sqrt(p.vx * p.vx + p.vy * p.vy);
                if (speed > 100.0f) {
                    p.vx = (p.vx / speed) * 100.0f;
                    p.vy = (p.vy / speed) * 100.0f;
                }
            }
        }

        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.lifetime -= dt;

        if (p.color.r == 180 && p.color.g == 100 && p.color.b == 255) {
            float expansion = (0.8f - p.lifetime) / 0.8f * 350.0f;
            p.radius = expansion;
        }
    }

    for (auto it = projectiles_.begin(); it != projectiles_.end();) {
        float centerDist = sqrt((it->x - 640.0f) * (it->x - 640.0f) + (it->y - 500.0f) * (it->y - 500.0f));

        if (it->lifetime <= 0 || centerDist > 340.0f) {
            it = projectiles_.erase(it);
            continue;
        }

        if (it->color.r == 180 && it->color.g == 100 && it->color.b == 255) {
            ++it;
            continue;
        }

        float dx = px_ - it->x;
        float dy = py_ - it->y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < it->radius + 12.0f) {
            if (it->color.r == 255 && it->color.g == 50 && it->color.b == 80) {
                // Heart pickup
                if (playerHP_ < 5) playerHP_++;
                it = projectiles_.erase(it);
                screenFlashAlpha_ = 60.0f;
                screenFlashColor_ = {50, 255, 80, 255};
                continue;
            }

            if (!isDodging_) {
                playerHP_--;
                invincibilityFlash_ = 0.5f;
                screenFlashAlpha_ = 100.0f;
                screenFlashColor_ = {255, 50, 50, 255};
            }
            it = projectiles_.erase(it);
            continue;
        }

        ++it;
    }
}

void EndingScene::render() {
    auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    if (combatMode_) {
        renderCombat();
    } else {
        glitchRenderer_.update(manager_ ? 0.016f : 0.016f); // Approximation if no dt
        renderAscension();
    }
}

void EndingScene::renderAscension() {
    auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    // Background pulsing
    if (!bgPulse_.empty()) {
        int idx = static_cast<int>(totalTime_ * 10) % bgPulse_.size();
        r->setDrawColor(bgPulse_[idx]);
        r->clear();
    } else {
        r->setDrawColor({0, 0, 0, 255});
        r->clear();
    }

    if (phase_ == 0) {
        std::string visibleText;
        if (currentLine_ < static_cast<int>(typewriterLines_.size())) {
            visibleText = typewriterLines_[currentLine_].substr(0, charIndex_);
        }
        if (!visibleText.empty()) {
            r->drawText(visibleText, 250, 350, {220, 200, 150, 255}, 2);
        }
    } else if (phase_ >= 1 && phase_ <= 3) {
        // Gold stars
        for (auto& star : stars_) {
            r->setDrawColor({220, 190, 80, 140});
            r->drawPoint(640 + static_cast<int>(star.first), 360 + static_cast<int>(star.second));
        }

        // Particle rain in Phase 1
        if (phase_ == 1) {
            for (auto& p : goldRain_) {
                p.y += p.speed * 0.016f;
                if (p.y > 720) p.y = -10;
                r->setDrawColor({255, 215, 0, 180});
                r->drawCircle(static_cast<int>(p.x), static_cast<int>(p.y), 2, true);
            }
        }

        // The Pillar
        r->setDrawColor({210, 180, 60, 255});
        int pillW = 20;
        r->drawRect(640 - pillW/2, 720 - static_cast<int>(pillarHeight_), pillW, static_cast<int>(pillarHeight_), true);

        if (phase_ >= 2) {
            // Golden Trail
            for (size_t i = 0; i < playerTrail_.size(); i++) {
                float alphaPct = static_cast<float>(i) / playerTrail_.size();
                r->setDrawColor({255, 220, 100, static_cast<uint8_t>(255 * alphaPct)});
                renderPlayerFigure(static_cast<int>(playerTrail_[i].first), static_cast<int>(playerTrail_[i].second), {255, 220, 100, static_cast<uint8_t>(255 * alphaPct)}, 1);
            }

            // Player Silhouette
            renderPlayerFigure(640, static_cast<int>(playerY_), {5, 2, 8, 255}, 1);

            // Rotating Dacian Symbol
            r->drawDacianSymbol(640, static_cast<int>(playerY_), 60.0f, totalTime_ * 45.0f, {255, 215, 0, 200});
        }

        r->drawText(currentText_, 250, 640, {220, 200, 150, 255}, 2);
    } else if (phase_ == 4) {
        r->drawText(currentText_, 350, 350, {255, 255, 255, 255}, 3);
        glitchRenderer_.render(*r);
    } else if (phase_ == 5) {
        // Dacian Column golden
        r->setDrawColor({255, 215, 0, 255});
        int colX = 640;
        int colY = 720 - static_cast<int>(columnHeight_);
        r->drawThickLine(colX, 720, colX, colY, 30);
        // Sparks at tip
        for (int i = 0; i < 5; i++) {
            int sx = colX + (rand() % 40 - 20);
            int sy = colY + (rand() % 40 - 20);
            r->setDrawColor({255, 255, 200, 200});
            r->drawCircle(sx, sy, 2, true);
        }
    } else if (phase_ == 6) {
        r->setDrawColor({255, 255, 255, static_cast<uint8_t>(goldAlpha_)});
        r->clear();
        if (goldAlpha_ > 200) {
            r->drawText("DACIA ESTE LIBERĂ. TU EȘTI DACIA.", 300, 360, {40, 35, 20, 255}, 2);
        }
    }

    glitchRenderer_.render(*r);
}

void EndingScene::renderCombat() {
        auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    int sx = 0, sy = 0;
    if (shakeMag_ > 0.1f) {
        sx = static_cast<int>(sin(totalTime_ * 40.0f) * shakeMag_);
        sy = static_cast<int>(cos(totalTime_ * 45.0f) * shakeMag_);
        // Typically we'd offsets all draws, but we can't easily change Renderer API.
        // I'll just apply it to the rendering logic by adding sx, sy to positions.
        // Wait, better yet, set viewport with offset?
        SDL_Rect view = {sx, sy, 1280, 720};
        r->setViewport(view);
    }

    if (zalmoxisEnraged_ && !playerWon_ && !playerDied_) {
        bgFlickerTimer_ += 0.05f;
        if (static_cast<int>(bgFlickerTimer_ * 20) % 2 == 0) {
            r->setDrawColor({10, 3, 18, 255});
        } else {
            r->setDrawColor({5, 1, 8, 255});
        }
    } else {
        r->setDrawColor({10, 3, 18, 255});
    }
    r->clear();

    starRotation_ += 0.3f * 0.016f;
    for (auto& star : stars_) {
        float dist = sqrt(star.first * star.first + star.second * star.second);
        float angle = atan2(star.second, star.first) + starRotation_;
        int sx = 640 + static_cast<int>(cos(angle) * dist);
        int sy = 360 + static_cast<int>(sin(angle) * dist);
        r->setDrawColor({150, 120, 180, 100});
        r->drawPoint(sx, sy);
    }

    renderCombatArena();

    if (!playerWon_) {
        for (auto& after : afterimagePositions_) {
            r->setDrawColor({120, 60, 180, 100});
            r->drawCircle(static_cast<int>(after.first), static_cast<int>(after.second), 45);
        }
        renderZalmoxis();
    }

    renderProjectiles();

    if (!playerDied_) {
        renderPlayerCombat();
    } else {
        float dissolve = std::min(1.0f, deathTimer_ / 3.0f);
        renderPlayerFigure(static_cast<int>(px_), static_cast<int>(py_), {100, 80, 120, static_cast<uint8_t>(255 * (1.0f - dissolve))}, 2, dissolve);
    }

    for (auto& p : explosionParticles_) {
        r->setDrawColor({static_cast<uint8_t>(p.color.r), static_cast<uint8_t>(p.color.g), static_cast<uint8_t>(p.color.b), static_cast<uint8_t>(p.alpha)});
        r->drawCircle(static_cast<int>(p.x), static_cast<int>(p.y), static_cast<int>(p.radius));
    }

    for (auto& ft : fallingTriangles_) {
        r->setDrawColor({200, 170, 50, static_cast<uint8_t>(ft.alpha)});

        int x0 = static_cast<int>(ft.x);
        int y0 = static_cast<int>(ft.y);
        r->drawTriangle(x0, y0, static_cast<int>(x0 + 8), static_cast<int>(y0 - 16), static_cast<int>(x0 - 8), static_cast<int>(y0), true);
    }

    if (beamTelegraphing_) {
        r->setDrawColor({255, 200, 50, 100});
        r->drawThickLine(static_cast<int>(beamStartX_), static_cast<int>(beamStartY_), static_cast<int>(beamEndX_), static_cast<int>(beamEndY_), 2);
    }

    if (beamFiring_) {
        r->setDrawColor({255, 220, 100, 220});
        r->drawThickLine(static_cast<int>(beamStartX_), static_cast<int>(beamStartY_), static_cast<int>(beamEndX_), static_cast<int>(beamEndY_), 16);
    }

    if (crossBeamActive_) {
        for (int i = 0; i < 4; i++) {
            float angle = crossBeamAngle_ + i * 90.0f;
            float ex = zx_ + cos(angle * 3.14159f / 180.0f) * 350.0f;
            float ey = zy_ + sin(angle * 3.14159f / 180.0f) * 350.0f;
            r->setDrawColor({255, 220, 100, 180});
            r->drawThickLine(static_cast<int>(zx_), static_cast<int>(zy_), static_cast<int>(ex), static_cast<int>(ey), 8);
        }
    }

    renderCombatHUD();

    if (phaseAnnounceTimer_ > 0) {
        r->drawText(phaseAnnounceText_, 400, 200, {255, 200, 50, 255}, 4);
    }

    if (combatTextTimer_ > 0) {
        r->drawText(combatText_, static_cast<int>(zx_ - 40), static_cast<int>(zy_ - 60), {255, 200, 50, 255}, 2);
    }

    if (invincibilityFlash_ > 0) {
        r->drawText("INVINCIBIL!", static_cast<int>(px_ - 40), static_cast<int>(py_ - 50), {50, 200, 255, 255}, 1);
    }

    if (screenFlashAlpha_ > 0) {
        r->setDrawColor({screenFlashColor_.r, screenFlashColor_.g, screenFlashColor_.b, static_cast<uint8_t>(screenFlashAlpha_)});
        r->drawRect(0, 0, 1280, 720, true);
    }

    if (playerDied_) {
        r->setDrawColor({0, 0, 0, static_cast<uint8_t>(std::min(deathTimer_ * 85.0f, 180.0f))});
        r->drawRect(0, 0, 1280, 720, true);

        if (deathTimer_ > 3.0f) {
            r->drawText("UMBRA TE-A INGHIȚIT", 420, 350, {200, 20, 20, 255}, 3);
            
            float fadeOut = std::min(1.0f, (deathTimer_ - 3.0f) / 2.0f);
            r->setDrawColor({0, 0, 0, static_cast<uint8_t>(fadeOut * 255)});
            r->drawRect(0, 0, 1280, 720, true);
        }

        if (deathTimer_ > 5.0f) {
            r->drawText("[INCEARCA DIN NOU]", 450, 450, {150, 150, 150, 255}, 2);
            r->drawText("[MENIU PRINCIPAL]", 470, 500, {150, 150, 150, 255}, 2);
        }
    }

    if (playerWon_) {
        if (victoryTimer_ >= 2.0f && victoryTimer_ < 5.0f) {
            r->drawText("L-ai doborat.", 450, 200, {200, 170, 50, 255}, 3);
            r->drawText("Tronul Daciei e al tau.", 380, 260, {200, 170, 50, 255}, 2);
        }

        if (victoryTimer_ >= 5.0f) {
            r->drawText("Nu ai devenit zeu prin har.", 350, 250, {200, 170, 50, 255}, 2);
            r->drawText("Ai devenit zeu prin forta.", 380, 300, {200, 170, 50, 255}, 2);
            r->drawText("Dacia e libera.", 450, 350, {200, 170, 50, 255}, 2);
        }

        if (victoryTimer_ >= 10.0f) {
            r->drawText("Apasa ENTER -> meniu principal.", 380, 500, {150, 150, 150, 255}, 2);
        }
    }

    r->resetViewport();
}

void EndingScene::renderCombatArena() {
        auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    r->setDrawColor({25, 10, 40, 255});
    r->drawCircle(640, 500, 320, true);

    r->setDrawColor({100, 60, 180, 255});
    for (int i = 0; i < 60; i++) {
        float angle1 = i * 3.14159f * 2.0f / 60.0f;
        float angle2 = (i + 1) * 3.14159f * 2.0f / 60.0f;
        int x1 = 640 + static_cast<int>(cos(angle1) * 320.0f);
        int y1 = 500 + static_cast<int>(sin(angle1) * 320.0f);
        int x2 = 640 + static_cast<int>(cos(angle2) * 320.0f);
        int y2 = 500 + static_cast<int>(sin(angle2) * 320.0f);
        r->drawLine(x1, y1, x2, y2);
    }

    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159f * 2.0f / 8.0f;
        int rx = 640 + static_cast<int>(cos(angle) * 310.0f);
        int ry = 500 + static_cast<int>(sin(angle) * 310.0f);
        r->setDrawColor({180, 150, 50, 255});
        r->drawRect(rx - 3, ry - 3, 6, 6, true);
        r->drawRect(rx - 3, ry - 3, 6, 6, true);
    }
}

void EndingScene::renderZalmoxis() {
        auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    float auraRadius = 55.0f + sin(SDL_GetTicks() / 300.0) * 15.0f;
    if (zalmoxisEnraged_) auraRadius *= 2.0f;
    r->setDrawColor({150, 100, 255, 40});
    r->drawCircle(static_cast<int>(zx_), static_cast<int>(zy_), static_cast<int>(auraRadius));

    int bodyRadius = zalmoxisEnraged_ ? 65 : 45;
    r->setDrawColor({80, 40, 160, 255});
    int robex = static_cast<int>(zx_);
    int robey = static_cast<int>(zy_) + bodyRadius;
    r->drawTriangle(robex - 45, robey, robex, robey + 120, robex + 45, robey, true);

    r->setDrawColor({120, 80, 200, 255});
    r->drawCircle(static_cast<int>(zx_), static_cast<int>(zy_), bodyRadius);

    r->setDrawColor({200, 170, 50, 255});
    for (int i = 0; i < 7; i++) {
        float angle = -90.0f + i * 30.0f;
        if (zalmoxisEnraged_) {
            angle = -90.0f + i * 25.0f;
        }
        float rad = angle * 3.14159f / 180.0f;
        int cx = static_cast<int>(zx_ + cos(rad) * (bodyRadius - 10));
        int cy = static_cast<int>(zy_ + sin(rad) * (bodyRadius - 10));
        int crownSize = zalmoxisEnraged_ ? 20 : 16;
        r->drawTriangle(cx - 4, cy, cx, cy - crownSize, cx + 4, cy, true);
    }

    r->setDrawColor({255, 220, 50, 60});
    r->drawCircle(static_cast<int>(zx_) - 15, static_cast<int>(zy_) - 5, 9);
    r->drawCircle(static_cast<int>(zx_) + 15, static_cast<int>(zy_) - 5, 9);
    r->setDrawColor({255, 220, 50, 255});
    r->drawCircle(static_cast<int>(zx_) - 15, static_cast<int>(zy_) - 5, 6);
    r->drawCircle(static_cast<int>(zx_) + 15, static_cast<int>(zy_) - 5, 6);
}

void EndingScene::renderPlayerCombat() {
        auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    if (isDodging_) {
        std::vector<std::pair<float, float>> ghosts;
        for (int i = 0; i < 4; i++) {
            float t = static_cast<float>(i + 1) / 4.0f;
            ghosts.push_back({px_ - dodgeVx_ * t * 0.05f, py_ - dodgeVy_ * t * 0.05f});
        }
        int alphas[] = {120, 80, 40, 15};
        for (int i = 0; i < 4; i++) {
            r->setDrawColor({100, 80, 120, static_cast<uint8_t>(alphas[i])});
            renderPlayerFigure(static_cast<int>(ghosts[i].first), static_cast<int>(ghosts[i].second), {100, 80, 120, static_cast<uint8_t>(alphas[i])}, 2);
        }
    }

    core::Color playerCol = isDodging_ ? core::Color{150, 200, 255, 255} : core::Color{100, 80, 120, 255};
    renderPlayerFigure(static_cast<int>(px_), static_cast<int>(py_), playerCol, 2);

    if (punchCooldown_ > 0) {
        float progress = 1.0f - (punchCooldown_ / 0.8f);
        r->setDrawColor({200, 150, 50, 150});
        int arcSegments = 8;
        for (int i = 0; i < arcSegments; i++) {
            float angle1 = -1.57f + (static_cast<float>(i) / arcSegments) * 3.14f * progress;
            float angle2 = -1.57f + (static_cast<float>(i + 1) / arcSegments) * 3.14f * progress;
            int x1 = static_cast<int>(px_ + cos(angle1) * 25.0f);
            int y1 = static_cast<int>(py_ + sin(angle1) * 25.0f);
            int x2 = static_cast<int>(px_ + cos(angle2) * 25.0f);
            int y2 = static_cast<int>(py_ + sin(angle2) * 25.0f);
            r->drawLine(x1, y1, x2, y2);
        }
    }

    if (dodgeCooldown_ > 0 && !isDodging_) {
        float progress = 1.0f - (dodgeCooldown_ / 1.2f);
        r->setDrawColor({50, 150, 200, 150});
        int arcSegments = 8;
        for (int i = 0; i < arcSegments; i++) {
            float angle1 = 1.57f + (static_cast<float>(i) / arcSegments) * 3.14f * progress;
            float angle2 = 1.57f + (static_cast<float>(i + 1) / arcSegments) * 3.14f * progress;
            int x1 = static_cast<int>(px_ + cos(angle1) * 20.0f);
            int y1 = static_cast<int>(py_ + sin(angle1) * 20.0f);
            int x2 = static_cast<int>(px_ + cos(angle2) * 20.0f);
            int y2 = static_cast<int>(py_ + sin(angle2) * 20.0f);
            r->drawLine(x1, y1, x2, y2);
        }
    }
}

void EndingScene::renderProjectiles() {
        auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    for (auto& p : projectiles_) {
        if (p.color.r == 180 && p.color.g == 100 && p.color.b == 255) {
            float expansion = (0.8f - p.lifetime) / 0.8f * 350.0f;
            if (expansion > 0) {
                r->setDrawColor({180, 100, 255, 150});
                r->drawCircle(static_cast<int>(zx_), static_cast<int>(zy_), static_cast<int>(expansion));
            }
            continue;
        }

        if (p.color.r == 255 && p.color.g == 50 && p.color.b == 80) {
            renderHeart(static_cast<int>(p.x - 10), static_cast<int>(p.y - 10), true);
            continue;
        }

        r->setDrawColor(p.color);
        r->drawCircle(static_cast<int>(p.x), static_cast<int>(p.y), static_cast<int>(p.radius));
    }
}

void EndingScene::renderCombatHUD() {
        auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    r->drawText("ZALMOXIS", 440, 30, {200, 170, 50, 255}, 2);

    r->setDrawColor({30, 10, 50, 255});
    r->drawRect(440, 60, 400, 18, true);

    float hpPercent = static_cast<float>(zalmoxisHP_) / 9.0f;
    core::Color hpCol;
    hpCol.r = static_cast<uint8_t>(100 + (200 - 100) * (1.0f - hpPercent));
    hpCol.g = static_cast<uint8_t>(50 + (50 - 50) * (1.0f - hpPercent));
    hpCol.b = static_cast<uint8_t>(200 + (50 - 200) * (1.0f - hpPercent));
    hpCol.a = 255;
    r->setDrawColor(hpCol);
    r->drawRect(440, 60, static_cast<int>(400 * hpPercent), 18, true);

    r->setDrawColor({150, 100, 255, 255});
    r->drawRect(440, 60, 400, 18, false);

    r->setDrawColor({255, 255, 255, 100});
    r->drawLine(573, 60, 573, 78);
    r->drawLine(706, 60, 706, 78);

    for (int i = 0; i < 5; i++) {
        renderHeart(40 + i * 28, 40, i < playerHP_);
    }
    
    if (introCutsceneDone_) {
        r->drawText("WASD: miscare  |  L: loveste  |  P: hold attack  |  SPACE: esquiva", 300, 680, {100, 90, 70, 255}, 1);
    } else {
        // Draw the intro typewriter text centered
        if (!combatText_.empty()) {
            r->drawText(combatText_, 320, 360, {220, 200, 150, 255}, 2);
        }
        
        // Even during intro, render Zalmoxis if he's on screen
        if (introTimer_ > 1.0f) {
            renderZalmoxis();
        }
    }
}

void EndingScene::renderHeart(int x, int y, bool filled) {
        auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    core::Color col = filled ? core::Color{220, 50, 80, 255} : core::Color{60, 20, 30, 255};
    r->setDrawColor(col);

    r->drawCircle(x + 4, y + 4, 4, true);
    r->drawCircle(x + 14, y + 4, 4, true);
    r->drawRect(x + 2, y + 6, 16, 8, true);
    r->drawTriangle(x + 4, y + 10, x + 16, y + 10, x + 10, y + 18, true);
}

void EndingScene::renderPlayerFigure(int x, int y, core::Color col, int scale, float dissolve) {
        auto* r = getRenderer<core::Renderer>();
    if (!r) return;

    r->setDrawColor(col);

    int headSize = 6 * scale;
    int bodyW = 10 * scale;
    int bodyH = 20 * scale;
    int armW = 4 * scale;
    int armH = 14 * scale;
    int legW = 4 * scale;
    int legH = 16 * scale;

    r->drawRect(x - headSize/2, y - 30 * scale, headSize, headSize, true);
    r->drawRect(x - bodyW/2, y - 30 * scale + headSize, bodyW, bodyH, true);
    r->drawRect(x - bodyW/2 - armW, y - 28 * scale, armW, armH, true);
    r->drawRect(x + bodyW/2, y - 28 * scale, armW, armH, true);
    
    // Dissolve effect: scatter some pixels
    if (dissolve > 0.01f) {
        r->setDrawColor({col.r, col.g, col.b, static_cast<uint8_t>(255 * dissolve)});
        for (int i = 0; i < static_cast<int>(50 * dissolve); i++) {
            int ox = rand() % (40 * scale) - (20 * scale);
            int oy = rand() % (40 * scale) - (20 * scale);
            r->drawPoint(x + ox, y + oy);
        }
    }

    r->drawRect(x - legW - 2, y - 10 * scale + bodyH, legW, legH, true);
    r->drawRect(x + 2, y - 10 * scale + bodyH, legW, legH, true);
}

}
