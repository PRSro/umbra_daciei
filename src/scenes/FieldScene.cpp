#include "FieldScene.h"
#include "../core/Renderer.h"
#include "../systems/ParanoiaSystem.h"
#include <SDL2/SDL.h>
#include "../Config.h"
#include "../core/AudioManager.h"
#include <cmath>
#include <cstdlib>
#include <string>
#include <algorithm>

namespace scenes {

FieldScene::FieldScene() {}

void FieldScene::init() {
    if (!mapGenerated_) {
        generateMap();
        generateTreeProgram();
        mapGenerated_ = true;
    }

    // Strigoi waypoints
    waypoints_.clear();
    waypoints_.push_back({30 * 32.0f, 3 * 32.0f});
    waypoints_.push_back({35 * 32.0f, 10 * 32.0f});
    waypoints_.push_back({25 * 32.0f, 15 * 32.0f});

    // Ritual sites
    ritualSites_.clear();
    ritualSites_.push_back({10, 10, false});
    ritualSites_.push_back({30, 5, false});
    ritualSites_.push_back({20, 18, false});
    
    // Strigoi init
    strigois_.clear();
    StrigoiData s;
    s.x = 35.0f * 32.0f;
    s.y = 5.0f * 32.0f;
    s.state = StrigoiState::PATROL;
    s.waypointIdx = 0;
    s.alertTimer = 0.0f;
    s.searchTimer = 0.0f;
    s.chaseTimeout = 0.0f;
    strigois_.push_back(s);
    
    secretEntranceDiscovered_ = false;

    if (!dialogueInitialized_) {
        dialogue_.init();
        
        ui::DialogueNode n0;
        n0.id = "trimis_good";
        n0.speaker = "Trimisul lui Zamolxez";
        n0.text = "Calea ti-a fost deschisa. Te voi ajuta sa treci de umbre.";
        n0.options.push_back({"Multumesc.", "END", ""});
        dialogue_.addDialogueNode(n0);
        
        ui::DialogueNode n1;
        n1.id = "trimis_bad";
        n1.speaker = "Trimisul lui Zamolxez";
        n1.text = "Esti slabul sclav al iluziilor... Lasa umbrele sa te devoreze.";
        n1.options.push_back({"Nu!", "END", ""});
        dialogue_.addDialogueNode(n1);

        dialogueInitialized_ = true;
    }
}

void FieldScene::generateMap() {
    std::srand(42);

    // Fill with floor
    for (int y = 0; y < MAP_H; ++y)
        for (int x = 0; x < MAP_W; ++x)
            tiles_[y][x] = TileType::FLOOR;

    // Place trees at 35% density
    for (int y = 0; y < MAP_H; ++y)
        for (int x = 0; x < MAP_W; ++x)
            if (std::rand() % 100 < 35)
                tiles_[y][x] = TileType::TREE;

    // Place ritual sites
    tiles_[10][10] = TileType::RITUAL_SITE;
    tiles_[5][30] = TileType::RITUAL_SITE;
    tiles_[18][20] = TileType::RITUAL_SITE;

    // Carve a path from left to right
    int pathY = MAP_H / 2;
    for (int x = 0; x < MAP_W; ++x) {
        tiles_[pathY][x] = TileType::FLOOR;
        if (pathY > 0) tiles_[pathY - 1][x] = TileType::FLOOR;
        if (pathY < MAP_H - 1) tiles_[pathY + 1][x] = TileType::FLOOR;
        // Wander path
        if (std::rand() % 3 == 0 && pathY > 2) pathY--;
        if (std::rand() % 3 == 0 && pathY < MAP_H - 3) pathY++;
    }

    // Clear spawn area
    for (int y = 9; y < 14; ++y)
        for (int x = 0; x < 5; ++x)
            tiles_[y][x] = TileType::FLOOR;

    // Clear around ritual sites
    for (auto& rs : ritualSites_) {
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                int ny = rs.ty + dy, nx = rs.tx + dx;
                if (ny >= 0 && ny < MAP_H && nx >= 0 && nx < MAP_W)
                    if (tiles_[ny][nx] == TileType::TREE)
                        tiles_[ny][nx] = TileType::FLOOR;
            }
    }

    // Place some shadow tiles
    for (int i = 0; i < 20; ++i) {
        int sx = std::rand() % MAP_W;
        int sy = std::rand() % MAP_H;
        if (tiles_[sy][sx] == TileType::FLOOR)
            tiles_[sy][sx] = TileType::SHADOW;
    }
}

bool FieldScene::isWalkable(float x, float y) const {
    int tx = static_cast<int>(x) / TILE_SZ;
    int ty = static_cast<int>(y) / TILE_SZ;
    if (tx < 0 || tx >= MAP_W || ty < 0 || ty >= MAP_H) return false;
    return tiles_[ty][tx] != TileType::TREE;
}

bool FieldScene::isRitualTile(int tx, int ty) const {
    return tx >= 0 && tx < MAP_W && ty >= 0 && ty < MAP_H && tiles_[ty][tx] == TileType::RITUAL_SITE;
}

void FieldScene::update(float dt) {
    totalTime_ += dt;
    glitchRenderer_.update(dt);

    float paranoia = paranoiaSystem_ ? paranoiaSystem_->getParanoia() : 0.0f;

    // Antigravity mechanic
    if (paranoia > Config::PARANOIA_SHADOW_INTRUSION) {
        gravityRollTimer_ += dt;
        if (gravityRollTimer_ >= 30.0f) {
            gravityRollTimer_ = 0.0f;
            if ((std::rand() % 100) < 40) { // 40% chance every 30s
                antigravityActive_ = true;
                antigravityTimer_ = 4.0f + static_cast<float>(std::rand() % 400) / 100.0f;
                isFlipping_ = true;
                flipTimer_ = 0.0f;
                gravityStrength_ = 300.0f;

                auto* audio = static_cast<core::AudioManager*>(audio_);
                if (audio) audio->playSFX("shadow_flip");
                glitchRenderer_.triggerGlitch(systems::GlitchEffect::Wave, 0.5f, 1.5f);
            }
        }
    }

    if (antigravityActive_) {
        antigravityTimer_ -= dt;
        if (antigravityTimer_ <= 0) {
            antigravityActive_ = false;
            isFlipping_ = true;
            flipTimer_ = 0.0f;
        }
    }

    if (isFlipping_) {
        flipTimer_ += dt;
        float t = std::min(1.0f, flipTimer_ / 1.5f);
        float start = antigravityActive_ ? 1.0f : -1.0f;
        float end = antigravityActive_ ? -1.0f : 1.0f;
        gravityDir_ = start + (end - start) * t;
        if (t >= 1.0f) isFlipping_ = false;
    }

    // Handle dialogue
    if (dialogue_.isActive()) {
        dialogue_.update(dt);
        return;
    }

    // Rune minigame
    if (runeActive_) {
        if (runeShowingSequence_) {
            runeShowTimer_ += dt;
            if (runeShowTimer_ >= 1.0f) {
                runeShowTimer_ = 0.0f;
                runeStep_++;
                if (runeStep_ >= 4) {
                    runeShowingSequence_ = false;
                    runeStep_ = 0;
                }
            }
        }
        return; // Block other input during minigame
    }

    // Player movement
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    float speed = 90.0f;
    float newX = playerX_, newY = playerY_;

    if (keys[SDL_SCANCODE_W]) { newY -= speed * dt; facingAngle_ = 270.0f; }
    if (keys[SDL_SCANCODE_S]) { newY += speed * dt; facingAngle_ = 90.0f; }
    if (keys[SDL_SCANCODE_A]) { newX -= speed * dt; facingAngle_ = 180.0f; }
    if (keys[SDL_SCANCODE_D]) { newX += speed * dt; facingAngle_ = 0.0f; }

    // Apply gravity
    newY += gravityDir_ * gravityStrength_ * dt;

    if (isWalkable(newX, playerY_)) playerX_ = newX;
    if (isWalkable(playerX_, newY)) playerY_ = newY;

    // Clamp Y to map boundaries
    float maxY = static_cast<float>(MAP_H * TILE_SZ);
    if (playerY_ < 0.0f) playerY_ = 0.0f;
    if (playerY_ > maxY) playerY_ = maxY;

    // Camera
    cameraX_ = playerX_ - 640.0f;
    cameraY_ = playerY_ - 360.0f;

    // Strigoi AI
    for (auto& s : strigois_) {
        float dx = playerX_ - s.x;
        float dy = playerY_ - s.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        // Check if strigoi is near ritual site — repelled
        bool nearRitual = false;
        float repelDist = 200.0f;
        for (auto& rs : ritualSites_) {
            if (rs.activated) repelDist = 300.0f;
            float rdx = s.x - rs.tx * TILE_SZ;
            float rdy = s.y - rs.ty * TILE_SZ;
            if (std::sqrt(rdx * rdx + rdy * rdy) < repelDist) {
                nearRitual = true;
                // Push away
                float len = std::sqrt(rdx * rdx + rdy * rdy);
                if (len > 0.1f) {
                    s.x += (rdx / len) * 60.0f * dt;
                    s.y += (rdy / len) * 60.0f * dt;
                }
            }
        }

        switch (s.state) {
            case StrigoiState::PATROL: {
                if (s.waypointIdx < static_cast<int>(waypoints_.size())) {
                    float wx = waypoints_[s.waypointIdx].x;
                    float wy = waypoints_[s.waypointIdx].y;
                    float wdx = wx - s.x, wdy = wy - s.y;
                    float wdist = std::sqrt(wdx * wdx + wdy * wdy);
                    if (wdist < 10.0f) {
                        s.waypointIdx = (s.waypointIdx + 1) % static_cast<int>(waypoints_.size());
                    } else {
                        float spd = 40.0f;
                        s.x += (wdx / wdist) * spd * dt;
                        s.y += (wdy / wdist) * spd * dt;
                    }
                }
                if (dist < 200.0f && !nearRitual) {
                    s.state = StrigoiState::ALERT;
                    s.alertTimer = 1.5f;
                }
                break;
            }
            case StrigoiState::ALERT: {
                s.alertTimer -= dt;
                if (s.alertTimer <= 0) {
                    s.state = StrigoiState::CHASE;
                    s.chaseTimeout = 5.0f;
                }
                break;
            }
            case StrigoiState::CHASE: {
                if (!nearRitual) {
                    if (dist > 0.1f) {
                        float spd = 80.0f;
                        s.x += (dx / dist) * spd * dt;
                        s.y += (dy / dist) * spd * dt;
                    }
                    if (dist < 20.0f) {
                        // Catch! Add paranoia, push player back
                        if (paranoiaSystem_) paranoiaSystem_->addParanoia(0.3f);
                        if (dist > 0.1f) {
                            playerX_ += (dx / dist) * 100.0f;
                            playerY_ += (dy / dist) * 100.0f;
                        }
                        s.state = StrigoiState::PATROL;
                    }
                }
                s.chaseTimeout -= dt;
                if (dist > 300.0f || s.chaseTimeout <= 0) {
                    s.state = StrigoiState::SEARCH;
                    s.searchTimer = 5.0f;
                }
                break;
            }
            case StrigoiState::SEARCH: {
                s.searchTimer -= dt;
                // Wander randomly
                s.x += (std::rand() % 3 - 1) * 30.0f * dt;
                s.y += (std::rand() % 3 - 1) * 30.0f * dt;
                if (s.searchTimer <= 0 || dist < 150.0f) {
                    s.state = StrigoiState::PATROL;
                }
                break;
            }
        }
    }

    // Particles
    if (gravityDir_ < 0.0f) {
        if (std::rand() % 5 == 0) {
            Particle p;
            p.x = static_cast<float>(std::rand() % 1280);
            p.y = 720.0f;
            p.speed = 50.0f + static_cast<float>(std::rand() % 100);
            p.alpha = 255.0f;
            particles_.push_back(p);
        }
    }

    for (auto it = particles_.begin(); it != particles_.end();) {
        it->y -= it->speed * dt;
        it->alpha -= 40.0f * dt;
        if (it->y < -10.0f || it->alpha <= 0.0f) {
            it = particles_.erase(it);
        } else {
            ++it;
        }
    }
}

void FieldScene::render() {
    auto* r = reinterpret_cast<core::Renderer*>(getRendererRaw());
    if (!r) return;

    float paranoia = paranoiaSystem_ ? paranoiaSystem_->getParanoia() : 0.0f;

    r->setDrawColor({10, 15, 10, 255});
    r->clear();

    int cx = static_cast<int>(cameraX_);
    int cy = static_cast<int>(cameraY_);

    // Determine visible tile range
    int startTX = std::max(0, cx / TILE_SZ);
    int startTY = std::max(0, cy / TILE_SZ);
    int endTX = std::min(MAP_W, (cx + 1280) / TILE_SZ + 1);
    int endTY = std::min(MAP_H, (cy + 720) / TILE_SZ + 1);

    r->setDrawBlendMode(SDL_BLENDMODE_BLEND);

    float breatheY = antigravityActive_ ? std::sin(totalTime_ * 2.0f) * 8.0f : 0.0f;

    for (int ty = startTY; ty < endTY; ++ty) {
        for (int tx = startTX; tx < endTX; ++tx) {
            int screenX = tx * TILE_SZ - cx;
            int screenY = ty * TILE_SZ - cy + static_cast<int>(breatheY);

            switch (tiles_[ty][tx]) {
                case TileType::FLOOR:
                    r->setDrawColor({15, 35, 20, 255});
                    r->drawRect(screenX, screenY, TILE_SZ, TILE_SZ, true);
                    break;
                case TileType::TREE:
                    r->setDrawColor(Config::Colors::DACIAN_GREEN);
                    {
                        auto ruleGenerator = [&](core::Color& col, int& angle) {
                            float paranoia = paranoiaSystem_ ? paranoiaSystem_->getParanoia() : 0.0f;
                            // Randomly varies branch angle by ±15° based on paranoia level
                            angle += static_cast<int>((std::rand() % 31 - 15) * paranoia);
                            
                            // Changes color from DACIAN_GREEN to a sickly yellow as paranoia increases
                            float t = paranoia;
                            col.r = static_cast<uint8_t>(74 * (1.0f - t) + 200 * t);
                            col.g = static_cast<uint8_t>(124 * (1.0f - t) + 200 * t);
                            col.b = static_cast<uint8_t>(89 * (1.0f - t) + 50 * t);
                            
                            // At paranoia > 0.7, occasionally draws branches in SHADOW_REALM color
                            if (paranoia > 0.7f && (std::rand() % 10 < 3)) {
                                col = {15, 10, 20, 255};
                            }
                        };
                        r->drawLSystemTree(screenX + TILE_SZ/2, screenY + TILE_SZ, treeProgram_, TILE_SZ/2, 25, ruleGenerator);
                    }
                    break;
                case TileType::SHADOW: {
                    r->setDrawColor({8, 8, 12, 255});
                    uint8_t sAlpha = static_cast<uint8_t>(180 + 50 * std::sin(totalTime_ * 2.0f + tx + ty));
                    r->setDrawColor({8, 8, 12, sAlpha});
                    r->drawRect(screenX, screenY, TILE_SZ, TILE_SZ, true);
                    break;
                }
                case TileType::RITUAL_SITE: {
                    r->setDrawColor({15, 35, 20, 255});
                    r->drawRect(screenX, screenY, TILE_SZ, TILE_SZ, true);
                    // Check if activated
                    bool activated = false;
                    for (auto& rs : ritualSites_) {
                        if (rs.tx == tx && rs.ty == ty) { activated = rs.activated; break; }
                    }
                    uint8_t glow = activated ? static_cast<uint8_t>(180) : static_cast<uint8_t>(100);
                    r->setDrawColor({glow, 20, 20, 200});
                    r->drawCircle(screenX + TILE_SZ/2, screenY + TILE_SZ/2, 12, true);
                    // 4 rune marks
                    r->setDrawColor({200, 150, 50, 255});
                    r->drawRect(screenX + TILE_SZ/2 - 1, screenY + 2, 3, 3, true); // N
                    r->drawRect(screenX + TILE_SZ/2 - 1, screenY + TILE_SZ - 5, 3, 3, true); // S
                    r->drawRect(screenX + 2, screenY + TILE_SZ/2 - 1, 3, 3, true); // W
                    r->drawRect(screenX + TILE_SZ - 5, screenY + TILE_SZ/2 - 1, 3, 3, true); // E
                    break;
                }
            }
        }
    }

    // Vision cone (triangle)
    {
        float rad = facingAngle_ * 3.14159f / 180.0f;
        float coneLen = 120.0f;
        float halfAngle = 30.0f * 3.14159f / 180.0f;
        int px = static_cast<int>(playerX_ - cameraX_);
        int py = static_cast<int>(playerY_ - cameraY_);
        int tx1 = px + static_cast<int>(coneLen * std::cos(rad - halfAngle));
        int ty1 = py + static_cast<int>(coneLen * std::sin(rad - halfAngle));
        int tx2 = px + static_cast<int>(coneLen * std::cos(rad + halfAngle));
        int ty2 = py + static_cast<int>(coneLen * std::sin(rad + halfAngle));
        r->setDrawColor({255, 255, 200, 30});
        r->drawTriangle(px, py, tx1, ty1, tx2, ty2, true);
    }

    // Player figure
    {
        int px = static_cast<int>(playerX_ - cameraX_);
        int py = static_cast<int>(playerY_ - cameraY_);
        
        if (gravityDir_ >= 0.0f) {
            r->setDrawColor({200, 200, 190, 255});
            r->drawRect(px - 1, py - 5, 3, 3, true); // head
            r->drawRect(px - 1, py - 2, 3, 5, true);  // body
        } else {
            static core::Texture whiteTex;
            if (!whiteTex.get()) {
                whiteTex = r->createTexture(1, 1);
                SDL_SetRenderTarget(r->get(), whiteTex.get());
                SDL_SetRenderDrawColor(r->get(), 255, 255, 255, 255);
                SDL_RenderClear(r->get());
                SDL_SetRenderTarget(r->get(), NULL);
            }

            SDL_Rect headDst = {px - 1, py + 2, 3, 3};
            SDL_Rect bodyDst = {px - 1, py - 3, 3, 5};
            
            r->setDrawColor({200, 200, 190, 255});
            r->copyEx(whiteTex, NULL, &headDst, 0, NULL, SDL_FLIP_VERTICAL);
            r->copyEx(whiteTex, NULL, &bodyDst, 0, NULL, SDL_FLIP_VERTICAL);
        }
    }

    // Cyan Secret Entrance
    r->setDrawColor({0, 255, 255, 100});
    r->drawRect(20 * TILE_SZ - cx, 2 * TILE_SZ - cy, TILE_SZ, TILE_SZ, true);

    // Strigoi figures
    for (auto& s : strigois_) {
        int sx = static_cast<int>(s.x - cameraX_);
        int sy = static_cast<int>(s.y - cameraY_);
        r->setDrawColor({180, 30, 30, 255});
        r->drawRect(sx - 3, sy - 16, 6, 6, true);  // head
        r->drawRect(sx - 2, sy - 10, 4, 14, true);  // body
        r->drawRect(sx - 6, sy - 8, 12, 3, true);   // arms
        r->drawRect(sx - 2, sy + 4, 4, 10, true);   // legs

        // Alert indicator
        if (s.state == StrigoiState::ALERT) {
            r->drawText("!", sx - 2, sy - 24, {255, 50, 50, 255}, 2);
        }
        if (s.state == StrigoiState::CHASE) {
            r->drawText("!!", sx - 4, sy - 24, {255, 0, 0, 255}, 2);
        }
    }

    // Floating particles
    for (const auto& p : particles_) {
        r->setDrawBlendMode(SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r->get(), Config::Colors::MOONLIGHT.r, Config::Colors::MOONLIGHT.g, Config::Colors::MOONLIGHT.b, static_cast<uint8_t>(p.alpha));
        r->drawCircle(static_cast<int>(p.x), static_cast<int>(p.y), 2, true);
    }

    // Fog overlay
    {
        uint8_t fogAlpha = static_cast<uint8_t>(std::min(255.0f, 60.0f + paranoia * 80.0f));
        r->setDrawColor({10, 25, 15, fogAlpha});
        r->drawRect(0, 0, 1280, 720, true);
    }

    // Vignette
    if (antigravityActive_) {
        float pulse = (std::abs(gravityDir_) * 0.5f + 0.5f) * (std::sin(totalTime_ * 4.0f) * 0.2f + 0.8f);
        uint8_t vAlpha = static_cast<uint8_t>(100 * pulse);
        r->setDrawColor({0, 0, 0, vAlpha});
        
        // Draw 4 corners
        r->drawRect(0, 0, 300, 200, true);   // TL
        r->drawRect(980, 0, 300, 200, true); // TR
        r->drawRect(0, 520, 300, 200, true); // BL
        r->drawRect(980, 520, 300, 200, true); // BR
    }

    // Rune minigame overlay
    if (runeActive_) {
        r->setDrawColor({0, 0, 0, 200});
        r->drawRect(400, 250, 480, 200, true);
        r->setDrawColor({200, 150, 50, 255});
        r->drawRect(400, 250, 480, 200, false);

        r->drawText("RUNE MINIGAME", 520, 265, {200, 180, 100, 255}, 2);

        const char* arrows[] = {"UP", "DOWN", "LEFT", "RIGHT"};
        if (runeShowingSequence_) {
            r->drawText("Memoreaza:", 530, 300, {200, 200, 180, 255}, 2);
            if (runeStep_ < 4) {
                r->drawText(arrows[runeSequence_[runeStep_]], 580, 340, {255, 220, 100, 255}, 3);
            }
        } else {
            std::string prompt = "Pas " + std::to_string(runeStep_ + 1) + "/4: Apasa sageata";
            r->drawText(prompt, 460, 310, {200, 200, 180, 255}, 2);
            r->drawText("Arrow keys", 520, 360, {150, 200, 100, 255}, 2);
        }
    }

    // Interaction hint near ritual site or secret entrance
    if (!runeActive_ && !dialogue_.isActive()) {
        int ptx = static_cast<int>(playerX_) / TILE_SZ;
        int pty = static_cast<int>(playerY_) / TILE_SZ;
        
        bool nearRS = false;
        for (auto& rs : ritualSites_) {
            if (std::abs(ptx - rs.tx) <= 1 && std::abs(pty - rs.ty) <= 1 && !rs.activated) {
                r->drawText("Press E for ritual", 550, 650, {200, 180, 100, 200}, 2);
                nearRS = true;
                break;
            }
        }
        
        if (!nearRS && std::abs(ptx - 20) <= 1 && pty <= 3 && !secretEntranceDiscovered_) {
            r->drawText("Press E to inspect entrance", 550, 650, {0, 255, 255, 200}, 2);
        }
    }
    
    if (dialogue_.isActive()) {
        dialogue_.render(r);
    }

    glitchRenderer_.render(*r);

    r->drawText("ESC: Apartment", 10, 700, {100, 100, 80, 150}, 1);
}

void FieldScene::onEnter(const std::string&) {}
void FieldScene::onExit() {}

bool FieldScene::handleKeyDown(int key) {
    if (folkloreEvents_ && folkloreEvents_->isModalActive()) {
        if (key == SDL_SCANCODE_1) { folkloreEvents_->selectChoice(0); return true; }
        if (key == SDL_SCANCODE_2) { folkloreEvents_->selectChoice(1); return true; }
        return false;
    }

    if (runeActive_ && !runeShowingSequence_) {
        int input = -1;
        if (key == SDL_SCANCODE_UP) input = 0;
        if (key == SDL_SCANCODE_DOWN) input = 1;
        if (key == SDL_SCANCODE_LEFT) input = 2;
        if (key == SDL_SCANCODE_RIGHT) input = 3;

        if (input >= 0) {
            if (input == runeSequence_[runeStep_]) {
                runeStep_++;
                if (runeStep_ >= 4) {
                    // Success!
                    runeActive_ = false;
                    if (currentRitualIdx_ >= 0 && currentRitualIdx_ < static_cast<int>(ritualSites_.size())) {
                        ritualSites_[currentRitualIdx_].activated = true;
                    }
                    if (paranoiaSystem_) {
                        paranoiaSystem_->addAwakening(0.15f);
                        paranoiaSystem_->reduceParanoia(0.1f);
                    }
                    // Check if all rituals activated
                    bool allActive = true;
                    for (auto& rs : ritualSites_) {
                        if (!rs.activated) { allActive = false; break; }
                    }
                    if (allActive && manager_) {
                        // Set flag for ending detection
                    }
                }
            } else {
                // Failed — reset
                runeActive_ = false;
            }
            return true;
        }
        return false;
    }

    if (key == SDL_SCANCODE_ESCAPE || key == SDL_SCANCODE_Q) {
        switchTo("apartment");
        return true;
    }

    if (dialogue_.isActive()) {
        if (key == SDL_SCANCODE_1) {
            dialogue_.selectOption(0);
            return true;
        }
        return false;
    }

    if (key == SDL_SCANCODE_E) {
        int ptx = static_cast<int>(playerX_) / TILE_SZ;
        int pty = static_cast<int>(playerY_) / TILE_SZ;
        
        for (int i = 0; i < static_cast<int>(ritualSites_.size()); ++i) {
            auto& rs = ritualSites_[i];
            if (std::abs(ptx - rs.tx) <= 1 && std::abs(pty - rs.ty) <= 1 && !rs.activated) {
                runeActive_ = true;
                runeStep_ = 0;
                runeShowTimer_ = 0.0f;
                runeShowingSequence_ = true;
                currentRitualIdx_ = i;
                for (int j = 0; j < 4; ++j) runeSequence_[j] = std::rand() % 4;
                return true;
            }
        }
        
        if (std::abs(ptx - 20) <= 1 && pty <= 3 && !secretEntranceDiscovered_) {
            secretEntranceDiscovered_ = true;
            if (paranoiaSystem_ && paranoiaSystem_->getAwakening() >= 0.5f) {
                dialogue_.startDialogue("trimis_good");
            } else {
                dialogue_.startDialogue("trimis_bad");
                for (int i = 0; i < 100; ++i) {
                    StrigoiData s;
                    s.x = playerX_ + (std::rand() % 800 - 400);
                    s.y = playerY_ + (std::rand() % 800 - 400);
                    s.state = StrigoiState::CHASE;
                    s.chaseTimeout = 999.0f;
                    
                    // Keep them off screen initially if possible
                    while (std::abs(s.x - playerX_) < 200) s.x += 200;
                    
                    strigois_.push_back(s);
                }
            }
            return true;
        }
    }

    return false;
}

void FieldScene::generateTreeProgram() {
    std::string axiom = "F";
    std::string rules = "FF+[+F-F-F]-[-F+F+F]";
    
    std::string current = axiom;
    for (int i = 0; i < 4; i++) {
        std::string next = "";
        for (char c : current) {
            if (c == 'F') next += rules;
            else next += c;
        }
        current = next;
    }
    treeProgram_ = current;
}

}