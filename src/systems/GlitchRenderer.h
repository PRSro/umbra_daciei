#ifndef GLITCH_RENDERER_H
#define GLITCH_RENDERER_H

#include <vector>
#include <string>
#include <functional>
#include <random>

namespace core { class Renderer; }

namespace systems {

struct GlitchEffect {
    enum Type {
        Chromatic,
        Slice,
        Noise,
        Scanlines,
        VHS,
        tear,
        Invert,
        Wave
    };

    Type type;
    float intensity;
    float duration;
    float elapsed;
    std::function<void()> callback;
};

class GlitchRenderer {
public:
    GlitchRenderer();

    void update(float dt);
    void render(core::Renderer& renderer);

    void triggerGlitch(GlitchEffect::Type type, float intensity, float duration);
    void triggerRandomGlitch(float intensity);
    void triggerMemoryFrag(float intensity);

    bool isGlitchActive() const { return activeGlitches_.size() > 0; }
    float getGlitchIntensity() const { return currentIntensity_; }

    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }

    void setParanoiaLevel(float paranoia) { paranoiaLevel_ = paranoia; }
    float getParanoiaLevel() const { return paranoiaLevel_; }

    void onBreakdown(std::function<void()> callback) { breakdownCallback_ = callback; }

private:
    void applyChromaticAberration(core::Renderer& renderer, float intensity);
    void applySlice(core::Renderer& renderer, float intensity);
    void applyNoise(core::Renderer& renderer, float intensity);
    void applyScanlines(core::Renderer& renderer, float intensity);
    void applyVHS(core::Renderer& renderer, float intensity);
    void applyTear(core::Renderer& renderer, float intensity);
    void applyInvert(core::Renderer& renderer, float intensity);
    void applyWave(core::Renderer& renderer, float intensity);

    void cleanupExpiredEffects();

    std::vector<GlitchEffect> activeGlitches_;
    float currentIntensity_ = 0.0f;
    float paranoiaLevel_ = 0.0f;
    bool enabled_ = true;
    std::mt19937 rng_;

    std::function<void()> breakdownCallback_;
};

}

#endif
