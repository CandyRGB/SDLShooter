// SceneResult.cpp
#include <string>

#include "SceneResult.h"
#include "SceneMain.h"
#include "SceneTitle.h"
#include "Game.h"

SceneResult::SceneResult(int finalScore)
    : finalScore(finalScore), selectedOption(0), timer(0.0f), bgm(nullptr), optionVisible(true)
{
}

void SceneResult::init()
{
    // 可以加载背景音乐，但非必需
    // bgm = Mix_LoadMUS("assets/music/...");
    // if (bgm) Mix_PlayMusic(bgm, -1);
}

void SceneResult::update(float deltaTime)
{
    timer += deltaTime;
    // 每0.5秒切换一次选项可见性，用于闪烁效果
    if (timer > 0.5f) {
        timer -= 0.5f;
        optionVisible = !optionVisible;
    }
}

void SceneResult::render()
{
    // 渲染最终得分
    std::string scoreText = "本局得分: " + std::to_string(finalScore);
    Game::getInstance().renderTextCentered(scoreText, 0.3, true);

    // 渲染选项
    std::string option1Text = "再来一次";
    std::string option2Text = "返回主菜单";

    // 计算选项位置
    float option1Y = 0.6f;
    float option2Y = 0.7f;

    // 渲染第一个选项
    if (selectedOption == 0) {
        // 高亮显示选中的选项（根据可见性闪烁）
        if (optionVisible) {
            Game::getInstance().renderTextCentered("> " + option1Text + " <", option1Y, false);
        } else {
            Game::getInstance().renderTextCentered(option1Text, option1Y, false);
        }
        // 第二个选项正常显示
        Game::getInstance().renderTextCentered(option2Text, option2Y, false);
    } else {
        // 第一个选项正常显示
        Game::getInstance().renderTextCentered(option1Text, option1Y, false);
        // 高亮显示选中的选项（根据可见性闪烁）
        if (optionVisible) {
            Game::getInstance().renderTextCentered("> " + option2Text + " <", option2Y, false);
        } else {
            Game::getInstance().renderTextCentered(option2Text, option2Y, false);
        }
    }
}

void SceneResult::handleEvent(SDL_Event* event)
{
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_W:
                selectedOption = 0;
                break;
            case SDL_SCANCODE_S:
                selectedOption = 1;
                break;
            case SDL_SCANCODE_J:
                if (selectedOption == 0) {
                    // 再来一次：切换到SceneMain重新开始游戏
                    auto sceneMain = std::make_unique<SceneMain>();
                    Game::getInstance().changeScene(std::move(sceneMain));
                } else {
                    // 返回主菜单：切换到标题界面
                    auto sceneTitle = std::make_unique<SceneTitle>();
                    Game::getInstance().changeScene(std::move(sceneTitle));
                }
                break;
            case SDL_SCANCODE_ESCAPE:
                // ESC键直接退出游戏
                Game::getInstance().changeScene(std::make_unique<SceneTitle>());
                break;
            default:
                break;
        }
    }
}

void SceneResult::clean()
{
    // 清理背景音乐
    if (bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }
}