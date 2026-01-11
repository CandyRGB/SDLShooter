// SceneResult.h
#ifndef SCENE_RESULT_H
#define SCENE_RESULT_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "IScene.h"

class SceneResult : public IScene
{
public:
    SceneResult(int finalScore);
    virtual void init() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event* event) override;

private:
    int finalScore;             // 本局最终得分
    int selectedOption;         // 当前选中的选项：0=再来一次，1=退出游戏
    float timer;                // 计时器，用于闪烁效果
    Mix_Music* bgm;             // 背景音乐（可选）
    bool optionVisible;         // 选项可见性，用于闪烁效果
};

#endif // SCENE_RESULT_H