// SceneTitle.cpp
#include <string>

#include "SceneTitle.h"
#include "SceneMain.h"
#include "Game.h"

void SceneTitle::init()
{
    //载入并播放背景音乐
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (bgm == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
}

// SceneTitle.cpp
void SceneTitle::update(float deltaTime)
{
    timer += deltaTime;
    if (timer > 1.0f){
        timer -= 1.0f;
    }
}

// SceneTitle.cpp
void SceneTitle::render()
{
    //渲染标题文字
    std::string titleText = "SDL太空战机";
    Game::getInstance().renderTextCentered(titleText, 0.4, true);

    //渲染普通文字
    if (timer < 0.5f){
        std::string instructions = "开始游戏";
        Game::getInstance().renderTextCentered(instructions, 0.8, false);
    }
}

// SceneTitle.cpp
void SceneTitle::handleEvent(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN){
        if (event->key.keysym.scancode == SDL_SCANCODE_J){
            auto sceneMain = std::make_unique<SceneMain>();
            Game::getInstance().changeScene(std::move(sceneMain));
        }
    }
}

// SceneTitle.cpp
void SceneTitle::clean()
{
    // 清理背景音乐
    if (bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}