#ifndef HUD_H
#define HUD_H

#include <string>

namespace core { class Renderer; }
namespace systems { class ParanoiaSystem; }

namespace ui {

class HUD {
public:
    HUD();
    ~HUD() = default;

    void init();
    void update(float dt);
    void render(core::Renderer* renderer, systems::ParanoiaSystem* ps);

    void setDay(int day) { day_ = day; }
    int getDay() const { return day_; }

    void setParanoia(float paranoia) { paranoia_ = paranoia; }
    float getParanoia() const { return paranoia_; }

    void setAwakening(float awakening) { awakening_ = awakening; }
    float getAwakening() const { return awakening_; }

    void setTimeOfDay(float t) { timeOfDay_ = t; }
    float getTimeOfDay() const { return timeOfDay_; }

    void setEvidence(int evidence) { evidence_ = evidence; }
    int getEvidence() const { return evidence_; }

    void setMessage(const std::string& message);
    void clearMessage();

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

private:
    int day_ = 1;
    float paranoia_ = 0.0f;
    float awakening_ = 0.0f;
    float timeOfDay_ = 8.0f;
    int evidence_ = 0;
    std::string message_;
    std::string messageDisplay_;
    float messageTimer_ = 0.0f;
    bool visible_ = true;
};

}

#endif