// AngleCalculator.h
#ifndef ANGLE_CALCULATOR_H
#define ANGLE_CALCULATOR_H

#include <SDL.h>

class AngleCalculator {
public:
    // 计算从源点指向目标点的角度（度）
    static float calculateAngle(float sourceX, float sourceY, float targetX, float targetY);

    // 计算从源点指向目标点的归一化方向向量
    static SDL_FPoint calculateDirection(float sourceX, float sourceY, float targetX, float targetY);

    // 计算两点之间的距离
    static float calculateDistance(float x1, float y1, float x2, float y2);
};

#endif // ANGLE_CALCULATOR_H
