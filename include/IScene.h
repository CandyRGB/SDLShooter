// Scene.h
#ifndef ISCENE_H
#define ISCENE_H

#include <SDL.h>

class IScene{
public:
    IScene() = default;
    virtual ~IScene() = default;

    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void clean() = 0;
    virtual void handleEvent(SDL_Event* event) = 0;
};

#endif // ISCENE_H