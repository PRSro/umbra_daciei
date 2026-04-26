#include "GlitchRenderer.h"
#include "../core/Renderer.h"
#include <algorithm>
#include <random>
#include <cmath>

static SDL_Texture* getSceneTexture(core::Renderer& renderer) {
    int w = renderer.width();
    int h = renderer.height();
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surf) return nullptr;
    SDL_RenderReadPixels(renderer.get(), NULL, SDL_PIXELFORMAT_RGBA32, surf->pixels, surf->pitch);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer.get(), surf);
    SDL_FreeSurface(surf);
    return tex;
}

namespace systems {

GlitchRenderer::GlitchRenderer() : rng_(std::random_device{}()) {}

void GlitchRenderer::update(float dt) {
    cleanupExpiredEffects();
    for (auto& effect : activeGlitches_) {
        effect.elapsed += dt;
    }
}



void GlitchRenderer::render(core::Renderer& renderer) {
    if (!enabled_ || activeGlitches_.empty()) return;

    for (const auto& effect : activeGlitches_) {
        switch (effect.type) {
            case GlitchEffect::Chromatic: applyChromaticAberration(renderer, effect.intensity); break;
            case GlitchEffect::Slice: applySlice(renderer, effect.intensity); break;
            case GlitchEffect::Noise: applyNoise(renderer, effect.intensity); break;
            case GlitchEffect::Scanlines: applyScanlines(renderer, effect.intensity); break;
            case GlitchEffect::VHS: applyVHS(renderer, effect.intensity); break;
            case GlitchEffect::tear: applyTear(renderer, effect.intensity); break;
            case GlitchEffect::Invert: applyInvert(renderer, effect.intensity); break;
            case GlitchEffect::Wave: applyWave(renderer, effect.intensity); break;
        }
    }
}

void GlitchRenderer::triggerGlitch(GlitchEffect::Type type, float intensity, float duration) {
    GlitchEffect effect;
    effect.type = type;
    effect.intensity = intensity;
    effect.duration = duration;
    effect.elapsed = 0.0f;
    activeGlitches_.push_back(effect);
}

void GlitchRenderer::triggerRandomGlitch(float intensity) {
    std::uniform_int_distribution<> dist(0, 7);
    triggerGlitch(static_cast<GlitchEffect::Type>(dist(rng_)), intensity, 0.5f);
}

void GlitchRenderer::triggerMemoryFrag(float intensity) {
    std::uniform_int_distribution<> dist(0, 2);
    triggerGlitch(static_cast<GlitchEffect::Type>(dist(rng_)), intensity, 2.0f);
}

void GlitchRenderer::cleanupExpiredEffects() {
    activeGlitches_.erase(
        std::remove_if(activeGlitches_.begin(), activeGlitches_.end(),
            [](const GlitchEffect& e) { return e.elapsed >= e.duration; }),
        activeGlitches_.end()
    );
}

void GlitchRenderer::applyChromaticAberration(core::Renderer& renderer, float intensity) {
    SDL_Texture* tex = getSceneTexture(renderer);
    if (!tex) return;
    int w = renderer.width();
    int h = renderer.height();

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_ADD);

    // Red channel
    SDL_SetTextureColorMod(tex, 255, 0, 0);
    SDL_Rect rDst = {-static_cast<int>(intensity), 0, w, h};
    SDL_RenderCopy(renderer.get(), tex, NULL, &rDst);

    // Green channel
    SDL_SetTextureColorMod(tex, 0, 255, 0);
    SDL_Rect gDst = {0, 0, w, h};
    SDL_RenderCopy(renderer.get(), tex, NULL, &gDst);

    // Blue channel
    SDL_SetTextureColorMod(tex, 0, 0, 255);
    SDL_Rect bDst = {static_cast<int>(intensity), 0, w, h};
    SDL_RenderCopy(renderer.get(), tex, NULL, &bDst);

    SDL_DestroyTexture(tex);
}

void GlitchRenderer::applySlice(core::Renderer& renderer, float intensity) {
    SDL_Texture* tex = getSceneTexture(renderer);
    if (!tex) return;
    int w = renderer.width();
    int h = renderer.height();

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    int y = 0;
    while (y < h) {
        int sliceHeight = std::uniform_int_distribution<>(5, 30)(rng_);
        if (y + sliceHeight > h) sliceHeight = h - y;

        int offsetX = 0;
        if (std::uniform_real_distribution<>(0.0, 1.0)(rng_) < 0.3) {
            offsetX = std::uniform_int_distribution<>(-static_cast<int>(intensity), static_cast<int>(intensity))(rng_);
        }

        SDL_Rect src = {0, y, w, sliceHeight};
        SDL_Rect dst = {offsetX, y, w, sliceHeight};
        SDL_RenderCopy(renderer.get(), tex, &src, &dst);

        y += sliceHeight;
    }
    SDL_DestroyTexture(tex);
}

void GlitchRenderer::applyNoise(core::Renderer& renderer, float intensity) {
    int w = renderer.width();
    int h = renderer.height();
    int numPixels = static_cast<int>(intensity * w * h * 0.05f);
    std::uniform_int_distribution<> distroX(0, w - 1);
    std::uniform_int_distribution<> distroY(0, h - 1);
    std::uniform_int_distribution<> distroColor(0, 255);

    for (int i = 0; i < numPixels; ++i) {
        SDL_SetRenderDrawColor(renderer.get(), distroColor(rng_), distroColor(rng_), distroColor(rng_), 255);
        SDL_RenderDrawPoint(renderer.get(), distroX(rng_), distroY(rng_));
    }
}

void GlitchRenderer::applyScanlines(core::Renderer& renderer, float intensity) {
    int w = renderer.width();
    int h = renderer.height();
    int alpha = static_cast<int>(intensity * 100);
    if (alpha > 255) alpha = 255;
    if (alpha < 0) alpha = 0;

    SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, alpha);
    for (int y = 0; y < h; y += 2) {
        SDL_RenderDrawLine(renderer.get(), 0, y, w, y);
    }
}

void GlitchRenderer::applyVHS(core::Renderer& renderer, float intensity) {
    applyChromaticAberration(renderer, intensity * 0.5f);
    applyScanlines(renderer, intensity * 0.5f);
    applyNoise(renderer, intensity * 0.5f);
}

void GlitchRenderer::applyTear(core::Renderer& renderer, float intensity) {
    SDL_Texture* tex = getSceneTexture(renderer);
    if (!tex) return;
    int w = renderer.width();
    int h = renderer.height();

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    int numTears = std::uniform_int_distribution<>(1, 3)(rng_);
    std::vector<int> splits;
    splits.push_back(0);
    for (int i = 0; i < numTears; ++i) {
        splits.push_back(std::uniform_int_distribution<>(0, h)(rng_));
    }
    splits.push_back(h);
    std::sort(splits.begin(), splits.end());

    for (size_t i = 0; i < splits.size() - 1; ++i) {
        int y1 = splits[i];
        int y2 = splits[i + 1];
        int height = y2 - y1;
        if (height <= 0) continue;

        int offsetX = std::uniform_int_distribution<>(-static_cast<int>(intensity), static_cast<int>(intensity))(rng_);
        SDL_Rect src = {0, y1, w, height};
        SDL_Rect dst = {offsetX, y1, w, height};
        SDL_RenderCopy(renderer.get(), tex, &src, &dst);
    }
    SDL_DestroyTexture(tex);
}

void GlitchRenderer::applyInvert(core::Renderer& renderer, float) {
    SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_MOD);
    // Draw inverted color modifier. 
    // Using SDL_ComposeCustomBlendMode is the real way to invert, but instructions strictly said SDL_BLENDMODE_MOD
    // For mod, anything other than white dims/tints the screen. We'll set it to a purplish tint to look inverted.
    SDL_SetRenderDrawColor(renderer.get(), 255, 0, 255, 255);
    SDL_Rect rect = {0, 0, renderer.width(), renderer.height()};
    SDL_RenderFillRect(renderer.get(), &rect);
    SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_NONE);
}

void GlitchRenderer::applyWave(core::Renderer& renderer, float intensity) {
    SDL_Texture* tex = getSceneTexture(renderer);
    if (!tex) return;
    int w = renderer.width();
    int h = renderer.height();

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    float freq = 0.05f;
    for (int y = 0; y < h; ++y) {
        int offsetX = static_cast<int>(std::sin(y * freq + renderer.getTime()) * intensity);
        SDL_Rect src = {0, y, w, 1};
        SDL_Rect dst = {offsetX, y, w, 1};
        SDL_RenderCopy(renderer.get(), tex, &src, &dst);
    }
    SDL_DestroyTexture(tex);
}

}