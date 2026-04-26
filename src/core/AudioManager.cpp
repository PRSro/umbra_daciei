#include "AudioManager.h"
#include <cmath>
#include <iostream>
#include <SDL2/SDL_mixer.h>

namespace core {

AudioManager::AudioManager() = default;

AudioManager::~AudioManager() {
    shutdown();
}

void AudioManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "[Audio] Failed to open audio: " << Mix_GetError() << "\n";
        return;
    }

    Mix_AllocateChannels(16);
    setSFXVolume(80);
    setMusicVolume(70);
}

void AudioManager::shutdown() {
    sfx_.clear();
    music_.clear();
    Mix_CloseAudio();
}

void AudioManager::loadSFX(const std::string& name, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (chunk) {
        sfx_[name] = std::make_unique<AudioChunk>(chunk, name);
    }
}

void AudioManager::loadMusic(const std::string& name, const std::string& path) {
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (music) {
        music_[name] = std::make_unique<MusicTrack>(music, name);
    }
}

void AudioManager::generateSineWave(const std::string& name, float frequency, float duration) {
    const int sampleRate = 44100;
    const int channels = 2;
    const int samples = static_cast<int>(sampleRate * duration);
    const int bufferSize = samples * channels * sizeof(Sint16);

    Uint8* buffer = static_cast<Uint8*>(SDL_malloc(bufferSize));
    if (!buffer) return;

    Sint16* samplesData = reinterpret_cast<Sint16*>(buffer);

    for (int i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float amplitude = 8000.0f * std::sin(2.0f * 3.14159f * frequency * t);
        Sint16 sample = static_cast<Sint16>(amplitude);

        samplesData[i * channels] = sample;
        samplesData[i * channels + 1] = sample;
    }

    Mix_Chunk* chunk = Mix_QuickLoad_RAW(buffer, bufferSize);
    if (chunk) {
        sfx_[name] = std::make_unique<AudioChunk>(chunk, name);
    }
}

void AudioManager::playSFX(const std::string& name, int loops) {
    auto it = sfx_.find(name);
    if (it != sfx_.end() && it->second->chunk) {
        Mix_PlayChannel(-1, it->second->chunk, loops);
    }
}

void AudioManager::playMusic(const std::string& name, int loops) {
    auto it = music_.find(name);
    if (it != music_.end() && it->second->music) {
        Mix_PlayMusic(it->second->music, loops);
        currentMusic_ = name;
    }
}

void AudioManager::stopMusic() {
    Mix_HaltMusic();
    currentMusic_.clear();
}

void AudioManager::crossfade(const std::string& from, const std::string& to, int ms) {
    (void)from;
    Mix_FadeOutMusic(ms);
    auto it = music_.find(to);
    if (it != music_.end() && it->second->music) {
        Mix_FadeInMusic(it->second->music, -1, ms);
        currentMusic_ = to;
    }
}

void AudioManager::setSFXVolume(int volume) {
    Mix_Volume(-1, volume);
}

void AudioManager::setMusicVolume(int volume) {
    Mix_VolumeMusic(volume);
}

void AudioManager::setParanoia(float paranoia) {
    paranoia_ = paranoia;
}

bool AudioManager::isPlaying(const std::string& name) const {
    (void)name;
    return Mix_Playing(-1) != 0;
}

void AudioManager::applyDistortion(Mix_Chunk* chunk, float paranoia) {
    if (!chunk || paranoia < 0.3f) return;
}

}