// HealthSystem.cpp
#include "HealthSystem.h"

void HealthSystem::takeDamage(int& health, int& shield, int damage, bool isInvincible,
                           std::function<void()> onHit) {
    if (isInvincible) {
        // 无敌状态，调用回调但不扣血
        if (onHit) {
            onHit();
        }
        return;
    }

    if (shield > 0) {
        // 护盾优先承受伤害
        shield -= damage;
        if (shield < 0) {
            health += shield;  // 剩余伤害转给生命值
            shield = 0;
        }
    } else {
        health -= damage;
    }

    if (onHit) {
        onHit();
    }
}

void HealthSystem::heal(int& health, int maxHealth, int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

void HealthSystem::addShield(int& shield, int amount) {
    shield += amount;
}
