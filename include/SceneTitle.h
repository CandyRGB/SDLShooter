// SceneTitle.h
#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "IScene.h"

class SceneTitle : public IScene
{
public:
    virtual void init() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event* event) override;

private:
    Mix_Music* bgm; // 背景音乐
    float timer = 0.0f; // 计时器
};

#endif // SCENE_TITLE_H