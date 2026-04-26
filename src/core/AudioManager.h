#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL2/SDL_mixer.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace core {

struct AudioChunk {
    Mix_Chunk* chunk = nullptr;
    std::string name;

    AudioChunk() = default;
    AudioChunk(Mix_Chunk* c, const std::string& n) : chunk(c), name(n) {}

    ~AudioChunk() {
        if (chunk) Mix_FreeChunk(chunk);
    }
};

struct MusicTrack {
    Mix_Music* music = nullptr;
    std::string name;

    MusicTrack() = default;
    MusicTrack(Mix_Music* m, const std::string& n) : music(m), name(n) {}

    ~MusicTrack() {
        if (music) Mix_FreeMusic(music);
    }
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void init();
    void shutdown();

    void loadSFX(const std::string& name, const std::string& path);
    void loadMusic(const std::string& name, const std::string& path);
    void generateSineWave(const std::string& name, float frequency, float duration);

    void playSFX(const std::string& name, int loops = 0);
    void playMusic(const std::string& name, int loops = -1);
    void stopMusic();

    void crossfade(const std::string& from, const std::string& to, int ms);

    void setSFXVolume(int volume);
    void setMusicVolume(int volume);

    void setParanoia(float paranoia);

    bool isPlaying(const std::string& name) const;

private:
    void applyDistortion(Mix_Chunk* chunk, float paranoia);

    std::unordered_map<std::string, std::unique_ptr<AudioChunk>> sfx_;
    std::unordered_map<std::string, std::unique_ptr<MusicTrack>> music_;

    std::string currentMusic_;
    float paranoia_ = 0.0f;
};

}

#endif