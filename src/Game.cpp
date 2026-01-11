// Game.cpp
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "Game.h"
#include "SceneTitle.h"

Game::Game()
{
}

Game::~Game()
{
    clean();
}

void Game::run()
{
    float frameTime = 1000.0f / FPS;
    while (isRunning)
    {
        Uint32 frameStart = SDL_GetTicks();
        SDL_Event event;
        handleEvent(&event);
        
        update();

        render();
        
        Uint32 diff = SDL_GetTicks() - frameStart;
        if (diff < frameTime) {
            SDL_Delay(frameTime - diff);
            deltaTime = frameTime / 1000.0f;
        } else {
            deltaTime = diff / 1000.0f;
        }
    }
}

void Game::init()
{
    // SDL 初始化
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    // 初始化SDL_image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        isRunning = false;
    }

    // 初始化SDL_mixer
    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        isRunning = false;
    }

    // 打开音频设备
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        isRunning = false;
    }

    // 设置音频设备的通道数
    Mix_AllocateChannels(16);

    // 设置音量
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    Mix_Volume(-1, MIX_MAX_VOLUME / 4);

    // 初始化SDL_ttf
    if (TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        isRunning = false;
    }

    // 创建窗口
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
   
    // 初始化背景卷轴
    nearStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-A.png");
    SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);   
    nearStars.height /= 2;
    nearStars.width /= 2;

    farStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-B.png");
    SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
    farStars.height /= 2;
    farStars.width /= 2;
    farStars.speed = 20;  // 远处的星星移动速度较慢

    // 载入字体
    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
    if (titleFont == nullptr || textFont == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont: %s\n", TTF_GetError());
        isRunning = false;
    }

    currentScene = std::make_unique<SceneTitle>();
    currentScene->init();
}

void Game::clean()
{
    // 清理资源
    IMG_Quit();

    Mix_Quit();

    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    if (nearStars.texture != nullptr) {
        SDL_DestroyTexture(nearStars.texture);
    }
    if (farStars.texture != nullptr) {
        SDL_DestroyTexture(farStars.texture);
    }

    SDL_Quit();
}

void Game::changeScene(std::unique_ptr<IScene> scene)
{
    currentScene->clean();
    currentScene = std::move(scene);
    currentScene->init();
}

void Game::handleEvent(SDL_Event *event)
{
    while (SDL_PollEvent(event))
    {
        if (event->type == SDL_QUIT)
        {
            isRunning = false;
        }
        currentScene->handleEvent(event);
    }
}

void Game::update()
{
    backgroundUpdate(deltaTime);
    currentScene->update(deltaTime);
}

void Game::render()
{
    // 清空
    SDL_RenderClear(renderer);

    // 渲染背景
    renderBackground();

    currentScene->render();
    // 显示更新
    SDL_RenderPresent(renderer);
}

void Game::backgroundUpdate(float deltaTime)
{
    nearStars.offset += nearStars.speed * deltaTime;
    if (nearStars.offset >= 0) {
        nearStars.offset -= nearStars.height;
    }

    farStars.offset += farStars.speed * deltaTime;
    if (farStars.offset >= 0) {
        farStars.offset -= farStars.height;
    }
}

// Game.cpp
void Game::renderBackground()
{
    // 渲染远处的星星
    for (int posY = static_cast<int>(farStars.offset); posY < getHeight(); posY += farStars.height){
        for (int posX = 0; posX < getWidth(); posX += farStars.width){
            SDL_Rect ds = {posX, posY, farStars.width, farStars.height};
            SDL_RenderCopy(renderer, farStars.texture, NULL, &ds);
        }
    }
    // 渲染近处的星星
    for (int posY = static_cast<int>(nearStars.offset); posY < getHeight(); posY += nearStars.height)
    {
        for (int posX = 0; posX < getWidth(); posX += nearStars.width)
        {
            SDL_Rect dstRect = {posX, posY, nearStars.width, nearStars.height};
            SDL_RenderCopy(renderer, nearStars.texture, nullptr, &dstRect);
        }   
    }
}

// Game.cpp
void Game::renderTextCentered(std::string text, float posY, bool isTitle)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface;
    if (isTitle){
        surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
    }else{
        surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int y = static_cast<int>((getHeight() - surface->h) * posY);
    SDL_Rect rect = {getWidth() / 2 - surface->w / 2,
                     y,
                     surface->w,
                     surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}