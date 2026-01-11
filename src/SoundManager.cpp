// SoundManager.cpp
#include "SoundManager.h"
#include <SDL_log.h>

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager() {}

SoundManager::~SoundManager() {
    cleanup();
}

void SoundManager::loadSound(const std::string& name, const char* path) {
    Mix_Chunk* sound = Mix_LoadWAV(path);
    if (sound == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load sound %s: %s", path, Mix_GetError());
        return;
    }
    sounds[name] = sound;
}

void SoundManager::play(const std::string& name, int channel) {
    auto it = sounds.find(name);
    if (it != sounds.end() && it->second != nullptr) {
        Mix_PlayChannel(channel, it->second, 0);
    }
}

void SoundManager::cleanup() {
    for (auto& pair : sounds) {
        if (pair.second != nullptr) {
            Mix_FreeChunk(pair.second);
        }
    }
    sounds.clear();
}
