#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

namespace Config {

struct Color {
    uint8_t r = 0, g = 0, b = 0, a = 255;
    constexpr Color() = default;
    constexpr Color(uint8_t rr, uint8_t gg, uint8_t bb, uint8_t aa = 255) noexcept
        : r(rr), g(gg), b(bb), a(aa) {}
};
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr float TARGET_FPS = 60.0f;
    constexpr float FIXED_TIMESTEP = 1.0f / TARGET_FPS;

    constexpr int TILE_SIZE = 32;
    constexpr int MAP_WIDTH = 40;
    constexpr int MAP_HEIGHT = 22;

    constexpr int FONT_SIZE_MONO = 16;
    constexpr int FONT_SIZE_SERIF = 20;
    constexpr int FONT_SIZE_TITLE = 28;

    constexpr float PARANOIA_MAX = 1.0f;
    constexpr float AWAKENING_MAX = 1.0f;
    constexpr float PARANOIA_DISTURBING_DOC = 0.05f;
    constexpr float PARANOIA_STRIGOI_ENCOUNTER = 0.15f;
    constexpr float PARANOIA_ENTER_SHADOW = 0.2f;
    constexpr float PARANOIA_HOIA_BACIU_RATE = 0.01f;
    constexpr float PARANOIA_SLEEP_DECAY = 0.1f;
    constexpr float PARANOIA_PRAYER_DECAY = 0.08f;
    constexpr float PARANOIA_SKEPTIC_TALK = 0.05f;
    constexpr float PARANOIA_GLITCH_threshold = 0.6f;
    constexpr float PARANOIA_SHADOW_INTRUSION = 0.85f;

    constexpr float GLITCH_UPDATE_INTERVAL = 0.5f;
    constexpr float EVENT_CHECK_INTERVAL = 45.0f;
    constexpr int AUTO_SAVE_INTERVAL = 300;

    constexpr int STRIGOI_ALERT_RADIUS = 150;
    constexpr int STRIGOI_CHASE_DURATION = 8;
    constexpr int STRIGOI_STUN_DURATION = 3;

    constexpr int SPRITE_SCALE = 3;
    constexpr int PLAYER_SPRITE_W = 16;
    constexpr int PLAYER_SPRITE_H = 24;
    constexpr int TERMINAL_TYPE_SPEED = 30;

    constexpr int CONSPIRACY_FORCE_ITERATIONS = 50;

    namespace Colors {
        constexpr Color BACKGROUND = {10, 10, 15, 255};
        constexpr Color AGED_PAPER = {200, 168, 130, 255};
        constexpr Color RED_THREAD = {204, 34, 0, 255};
        constexpr Color DACIAN_GREEN = {74, 124, 89, 255};
        constexpr Color BRONZE = {139, 115, 85, 255};
        constexpr Color MOONLIGHT = {232, 224, 208, 255};
        constexpr Color CRT_GREEN = {0, 255, 0, 255};
        constexpr Color SHADOW_REALM = {15, 10, 20, 255};
    }

    namespace Paths {
        inline const char* SAVE_FILE = ".umbra_daciei/save.json";
        inline const char* DOCUMENTS_JSON = "assets/data/documents.json";
        inline const char* LOCATIONS_JSON = "assets/data/locations.json";
        inline const char* EVENTS_JSON = "assets/data/events.json";
        inline const char* FONT_MONO = "assets/fonts/DejaVuSansMono.ttf";
        inline const char* FONT_SERIF = "assets/fonts/DejaVuSerif.ttf";
    }
}

#endif