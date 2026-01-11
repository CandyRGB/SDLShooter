// TextureManager.cpp
#include "TextureManager.h"
#include <SDL_log.h>
#include <SDL_image.h>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

TextureManager::TextureManager() : renderer(nullptr) {}

TextureManager::~TextureManager() {
    cleanup();
}

void TextureManager::setRenderer(SDL_Renderer* r) {
    renderer = r;
}

void TextureManager::loadTexture(const std::string& name, const char* path, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load texture %s: %s", path, SDL_GetError());
        return;
    }
    textures[name] = texture;
}

SDL_Texture* TextureManager::get(const std::string& name) {
    auto it = textures.find(name);
    return (it != textures.end()) ? it->second : nullptr;
}

void TextureManager::cleanup() {
    for (auto& pair : textures) {
        if (pair.second != nullptr) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
}
