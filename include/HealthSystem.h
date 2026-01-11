// HealthSystem.h
#ifndef HEALTH_SYSTEM_H
#define HEALTH_SYSTEM_H

#include <functional>

class HealthSystem {
public:
    // 处理伤害，先扣除护盾再扣除生命值
    // 参数：生命值引用，护盾引用，伤害值，无敌状态，无敌时的回调
    static void takeDamage(int& health, int& shield, int damage, bool isInvincible,
                      std::function<void()> onHit);

    // 治疗生命值
    static void heal(int& health, int maxHealth, int amount);

    // 增加护盾
    static void addShield(int& shield, int amount);
};

#endif // HEALTH_SYSTEM_H
