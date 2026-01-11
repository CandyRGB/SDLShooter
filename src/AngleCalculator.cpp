// AngleCalculator.cpp
#include "AngleCalculator.h"
#include <cmath>

float AngleCalculator::calculateAngle(float sourceX, float sourceY, float targetX, float targetY) {
    float dx = targetX - sourceX;
    float dy = targetY - sourceY;
    return atan2(dy, dx) * 180.0f / M_PI - 90.0f;
}

SDL_FPoint AngleCalculator::calculateDirection(float sourceX, float sourceY, float targetX, float targetY) {
    float dx = targetX - sourceX;
    float dy = targetY - sourceY;
    float length = sqrt(dx * dx + dy * dy);

    if (length > 0) {
        dx /= length;
        dy /= length;
    }

    return SDL_FPoint{dx, dy};
}

float AngleCalculator::calculateDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}
