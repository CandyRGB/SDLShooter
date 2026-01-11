// SoundManager.h
#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <unordered_map>
#include <string>
#include <SDL_mixer.h>
#include <functional>

class SoundManager {
public:
    void loadSound(const std::string& name, const char* path);
    void play(const std::string& name, int channel = -1);
    void cleanup();

    // 单例模式
    static SoundManager& getInstance();

private:
    SoundManager();
    ~SoundManager();
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    std::unordered_map<std::string, Mix_Chunk*> sounds;
};

#endif // SOUND_MANAGER_H
