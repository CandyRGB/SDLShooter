// Object.h
#ifndef OBJECT_H
#define OBJECT_H
#include <SDL.h>

struct Player {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    float speed = 300.0f;
    int health = 3;
    int maxHealth = 3;
    int shield = 0;
    // 惯性
    struct Velocity {
        float current = 0.0f;  // 当前速度
        float target = 0.0f;   // 目标速度
    } velocityX, velocityY;

    float acceleration = 800.0f;  // 加速度
    float deceleration = 1200.0f; // 减速度

    // 射击
    Uint32 coolDown = 250;     // 冷却时间
    Uint32 minCoolDown = 100;   // 最小冷却时间
    Uint32 lastShotTime = 0;   // 上次射击时间
    Uint32 bulletCount = 1;    // 子弹数量（同时发射的子弹数）
    Uint32 maxBulletCount = 50; // 最大子弹数量
    int bulletDamage = 1;       // 子弹伤害

    // 大招
    int energy = 0;              // 当前能量值
    int maxEnergy = 3;           // 最大能量值
    bool isInvincible = false;   // 是否无敌
    Uint32 invincibleStartTime = 0;  // 无敌开始时间
    Uint32 invincibleDuration = 5000;  // 无敌持续时间（5秒）
};

// 子弹
struct Bullet {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    float speed = 500.0f;
    int damage = 1;
};

struct Enemy {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 150;
    int health = 2;
    Uint32 coolDown = 1000;     // 发射冷却时间（1秒）
    Uint32 lastShotTime = 0;   // 上次发射时间
    float angle = 0.0f;         // 旋转角度（朝向玩家）
};

// 追踪自爆敌机
struct KamikazeEnemy {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    float speed = 200.0f;
    int health = 2;
    bool isExploding = false;    // 是否正在倒计时自爆
    Uint32 explosionStartTime = 0;  // 自爆倒计时开始时间
    Uint32 explosionDelay = 3000;   // 自爆倒计时时间（3秒）
    float angle = 0.0f;         // 旋转角度（朝向玩家）
};

struct BulletEnemy {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    SDL_FPoint direction = {0, 0};
    int width = 0;
    int height = 0;
    float speed = 300.0f;
    int damage = 1;
};

struct Explosion{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int currentFrame = 0;       // 当前帧索引
    int totlaFrame = 0;         // 总帧数
    Uint32 startTime = 0;       // 动画开始时间
    Uint32 FPS = 10;            // 帧率（每秒播放帧数）
    int explosionDamage = 1;    // 爆炸伤害（用于自爆敌机）
    int explosionRange = 0;     // 爆炸范围（用于自爆敌机，0表示无范围伤害）
    bool hasDamagedPlayer = false;  // 是否已对玩家造成伤害（避免重复伤害）
};

// 物品类型枚举
enum class ItemType {
    Life,
    Shield,
    Time,
    Bullet,
    Energy,
    Speed,
    Damage
};

// 物品
struct Item {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    SDL_FPoint direction = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 200;
    int bounceCount = 3;
    ItemType type = ItemType::Life;
};

// 背景
struct Background {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    float offset = 0;
    int width = 0;
    int height = 0;
    int speed = 60;
};

// 动态难度系统
struct DifficultySystem {
    float difficultyFactor = 1.0f;     // 当前难度系数 (0.6 ~ 2.0)
    float targetFactor = 1.0f;         // 目标难度系数
    float smoothSpeed = 0.03f;         // 难度平滑过渡速度（降低以更平滑）
    int lastUpdateScore = 0;           // 上次更新时的分数
    float updateThreshold = 80.0f;     // 分数变化达到此值时更新难度（降低阈值以更频繁微调）
};

// Boss敌机
struct BossEnemy {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    float speed = 80.0f;
    int health = 50;
    int maxHealth = 50;
    Uint32 coolDown = 800;      // 发射冷却时间
    Uint32 lastShotTime = 0;
    Uint32 patternChangeTime = 0;   // 上次切换攻击模式的时间
    int attackPattern = 0;       // 当前攻击模式（0:散弹, 1:环形）
    float angle = 0.0f;
    bool isActive = false;       // Boss是否已激活
    bool isDefeated = false;    // Boss是否已被击败
    Uint32 spawnTime = 0;      // Boss出现时间
};
#endif // OBJECT_H