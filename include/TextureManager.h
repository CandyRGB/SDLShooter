// TextureManager.h
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <unordered_map>
#include <string>
#include <SDL.h>
#include <functional>

class TextureManager {
public:
    void setRenderer(SDL_Renderer* r);
    void loadTexture(const std::string& name, const char* path, SDL_Renderer* renderer);
    SDL_Texture* get(const std::string& name);
    void cleanup();

    // 单例模式
    static TextureManager& getInstance();

private:
    TextureManager();
    ~TextureManager();
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    std::unordered_map<std::string, SDL_Texture*> textures;
    SDL_Renderer* renderer;
};

#endif // TEXTURE_MANAGER_H
