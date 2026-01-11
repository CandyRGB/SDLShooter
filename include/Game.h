// Game.h
#ifndef GAME_H
#define GAME_H

#include <memory>
#include <string>
#include <SDL.h>

#include "IScene.h"
#include "Object.h"


class Game
{
public:
    static Game& getInstance() {
        static Game instance;
        return instance;
    }
    void run();
    void init();
    void clean();
    void changeScene(std::unique_ptr<IScene> scene);

    void handleEvent(SDL_Event *event);
    void update();
    void render();

    SDL_Window* getWindow() { return window; }
    SDL_Renderer* getRenderer() { return renderer; }
    int getWidth() { return width; }
    int getHeight() { return height; }
    void backgroundUpdate(float deltaTime);
    void renderBackground();
    void renderTextCentered(std::string text, float posY, bool isTitle);
    
private:
    Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    ~Game();
    bool isRunning = true;
    TTF_Font* titleFont;
    TTF_Font* textFont;
    std::unique_ptr<IScene> currentScene;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int width = 700;
    int height = 700;
    float FPS = 30.0f;
    float deltaTime;
    Background nearStars;  // 近处的星星
    Background farStars;   // 远处的星星
};

#endif