#include "core/Game.h"
#include <iostream>
#include <csignal>
#include <memory>

namespace {
    std::unique_ptr<core::Game> g_game;
}

static void signal_handler(int) {
    if (g_game) {
        g_game->requestShutdown();
    }
}

int main(int /*argc*/, char* /*argv*/[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        g_game = std::make_unique<core::Game>();
        if (!g_game->init()) {
            std::cerr << "[FATAL] Game initialization failed\n";
            return 1;
        }

        std::cout << "[INFO] Umbra Daciei starting...\n";
        g_game->run();
        std::cout << "[INFO] Game terminated normally\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Uncaught exception: " << e.what() << "\n";
        return 1;
    }
}