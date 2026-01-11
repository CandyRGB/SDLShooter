// SceneMain.h
#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H

#include <list>
#include <unordered_map>
#include <random>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "Game.h"
#include "IScene.h"
#include "Object.h"
#include "EntityManager.h"

class SceneMain : public IScene {
public:
    SceneMain();
    ~SceneMain();

    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event* event) override;
    void init() override;
    void clean() override;

private:
    Game &game;
    Player player;
    Mix_Music* bgm;
    bool isDead = false;
    bool shouldSwitchToResult = false;
    Bullet bulletTemplate;
    EntityManager<Bullet> bullets;
    SDL_Texture* uiHealth;
    SDL_Texture* uiShield;
    SDL_Texture* shieldOverlay;
    SDL_Texture* energyBar;
    TTF_Font* scoreFont;
    int score = 0;

    // 随机数相关成员
    std::mt19937 gen;                          // 随机数生成器
    std::uniform_real_distribution<float> dis; // 随机数分布器

    // 敌机相关成员
    Enemy enemyTemplate;                      // 敌机模板
    EntityManager<Enemy> enemies;                // 存储活动敌机的列表

    // Boss相关成员
    BossEnemy boss;
    int bossSpawnScore = 500;  // Boss第一次出现分数阈值
    int lastBossSpawnScore = 0;  // 上次Boss出现的分数
    bool bossHasSpawned = false;  // Boss是否已生成

    // 追踪自爆敌机相关成员
    KamikazeEnemy kamikazeTemplate;           // 追踪自爆敌机模板
    EntityManager<KamikazeEnemy> kamikazes;       // 存储活动追踪自爆敌机的列表

    // 敌机子弹相关成员
    BulletEnemy bulletEnemyTemplate;
    EntityManager<BulletEnemy> bulletsEnemy;

    // 爆炸动画相关成员
    Explosion explosionTemplate;
    EntityManager<Explosion> explosions;

    // 物品相关成员
    Item itemLifeTemplate;
    Item itemBulletTemplate;
    Item itemShieldTemplate;
    Item itemEnergyTemplate;
    Item itemSpeedTemplate;
    Item itemDamageTemplate;
    EntityManager<Item> items;

    void keyboardControl(float deltaTime);
    void updateVelocity(Player::Velocity& velocity, float deltaTime);

    void shot();                            // 发射子弹
    void updateBullet(float deltaTime);     // 更新子弹
    void renderBullet();                    // 渲染子弹

    void spawEnemy();                         // 生成敌机
    void updateEnemies(float deltaTime);      // 更新敌机
    void renderEnemies();                     // 渲染敌机

    void spawnKamikaze();                    // 生成追踪自爆敌机
    void updateKamikazes(float deltaTime);   // 更新追踪自爆敌机
    void renderKamikazes();                  // 渲染追踪自爆敌机
    void kamikazeExplode(std::shared_ptr<KamikazeEnemy> kamikaze);  // 自爆敌机爆炸

    void renderEnemyBullets();                  // 渲染敌机子弹
    void updateEnemyBullets(float deltaTime);   // 更新敌机子弹
    void shotEnemy(std::shared_ptr<Enemy> enemy);               // 敌机发射子弹

    // Boss相关方法
    void spawnBoss();                       // 生成Boss
    void updateBoss(float deltaTime);          // 更新Boss
    void renderBoss();                      // 渲染Boss
    void bossShot();                        // Boss发射散弹
    void bossShotCircle();                  // Boss发射环形弹幕
    void bossExplode();                     // Boss爆炸
    bool checkBossCollision();               // 检查玩家与Boss的碰撞

    void updatePlayer(float deltaTime);     // 更新玩家
    void playerExplode();                   // 玩家爆炸
    void enemyExplode(std::shared_ptr<Enemy> enemy);    // 敌机爆炸

    void updateExplosions(float deltaTime);  // 更新爆炸动画
    void renderExplosions();                 // 渲染爆炸动画

    void dropItem(std::shared_ptr<Enemy> enemy);
    void updateItems(float deltaTime);
    void playerGetItem(std::shared_ptr<Item> item);
    void renderItems();
    void renderUI();
    void updateUltimate(float deltaTime);  // 更新大招状态
    void activateUltimate();               // 激活大招

    // 动态难度系统
    DifficultySystem difficultySystem;
    void updateDifficulty();              // 更新难度系数
    float calculateTargetDifficulty();    // 计算目标难度
    float getSmoothFactor(float baseValue, float difficultyScale, float maxScale);

};

#endif // SCENE_MAIN_H