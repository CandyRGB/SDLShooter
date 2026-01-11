// SceneMain.cpp
#include <SDL_image.h>
#include <string>
#include <cmath>


#include "SceneMain.h"
#include "SceneResult.h"
#include "AngleCalculator.h"
#include "HealthSystem.h"
#include "SoundManager.h"

SceneMain::SceneMain() : game(Game::getInstance())
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);  // 在每次更新时处理键盘输入
    updateDifficulty();          // 更新难度系数
    updateBullet(deltaTime);
    updateEnemyBullets(deltaTime);
    spawEnemy();                 // 生成敌机
    updateEnemies(deltaTime);    // 更新敌机
    spawnKamikaze();             // 生成追踪自爆敌机
    updateKamikazes(deltaTime);  // 更新追踪自爆敌机
    spawnBoss();                 // 检查是否生成Boss
    updateBoss(deltaTime);        // 更新Boss
    updatePlayer(deltaTime);
    updateExplosions(deltaTime);
    updateItems(deltaTime);
    updateUltimate(deltaTime);   // 更新大招状态

    // 检查是否需要切换到结算场景
    if (shouldSwitchToResult) {
        shouldSwitchToResult = false;
        auto sceneResult = std::make_unique<SceneResult>(score);
        Game::getInstance().changeScene(std::move(sceneResult));
    }
}

void SceneMain::render()
{
    // 绘制子弹
    renderBullet();
    // 绘制敌机子弹
    renderEnemyBullets();
    // 绘制Boss
    renderBoss();
    // 绘制敌机
    renderEnemies();
    // 绘制追踪自爆敌机
    renderKamikazes();
    // 绘制玩家
    if (!isDead) {
        // 无敌状态下闪烁显示
        bool shouldRenderPlayer = true;
        if (player.isInvincible) {
            auto currentTime = SDL_GetTicks();
            int blinkInterval = 100;  // 闪烁间隔（毫秒）
            shouldRenderPlayer = ((currentTime / blinkInterval) % 2 == 0);
        }
        if (shouldRenderPlayer) {
            SDL_Rect playerRect = { static_cast<int>(player.position.x),
                            static_cast<int>(player.position.y),
                            player.width,
                            player.height };
            SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);
        }
        // 绘制护盾层（如果有护盾）
        if (player.shield > 0) {
            SDL_Rect shieldRect = { static_cast<int>(player.position.x - 5),
                                   static_cast<int>(player.position.y - 10),
                                   player.width + 10,
                                   player.width + 10};
            SDL_RenderCopy(game.getRenderer(), shieldOverlay, NULL, &shieldRect);
        }
    }
    // 绘制爆炸动画
    renderExplosions();
    // 绘制物品
    renderItems();
    // 渲染UI
    renderUI();
}

void SceneMain::handleEvent(SDL_Event *event)
{
}

void SceneMain::init()
{
    // 读取并播放背景音乐
    bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    if (bgm == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load background music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);

    // 读取音效资源
    SoundManager::getInstance().loadSound("player_shot", "assets/sound/laser_shoot4.wav");
    SoundManager::getInstance().loadSound("enemy_shot", "assets/sound/xs_laser.wav");
    SoundManager::getInstance().loadSound("player_explosion", "assets/sound/explosion1.wav");
    SoundManager::getInstance().loadSound("enemy_explosion", "assets/sound/explosion3.wav");
    SoundManager::getInstance().loadSound("hit", "assets/sound/eff11.wav");
    SoundManager::getInstance().loadSound("get_item", "assets/sound/eff5.wav");

    // 读取UI资源
    uiHealth = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png");
    uiShield = IMG_LoadTexture(game.getRenderer(), "assets/image/Shield UI Black.png");
    shieldOverlay = IMG_LoadTexture(game.getRenderer(), "assets/image/shield.png");
    energyBar = IMG_LoadTexture(game.getRenderer(), "assets/image/energy-bar.png");

    // 载入字体
    scoreFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);

    // 初始化随机数生成器
    std::random_device rd;  // 获取真随机数作为种子
    gen = std::mt19937(rd());  // 用种子初始化梅森旋转引擎
    dis = std::uniform_real_distribution<float>(0.0f, 1.0f);  // 设置[0,1]均匀分布

    player.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
    if (player.texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load player texture: %s", SDL_GetError());
    }
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
    player.width /= 4;
    player.height /= 4;
    player.position.x = game.getWidth() / 2 - player.width / 2;
    player.position.y = game.getHeight() - player.height;
    // 初始化子弹模板
    bulletTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
    SDL_QueryTexture(bulletTemplate.texture, NULL, NULL, &bulletTemplate.width, &bulletTemplate.height);
    bulletTemplate.width /= 2;
    bulletTemplate.height /= 2;

    // 初始化敌机模板
    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 4;
    enemyTemplate.height /= 4;

    // 初始化追踪自爆敌机模板
    kamikazeTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-1.png");
    SDL_QueryTexture(kamikazeTemplate.texture, NULL, NULL, &kamikazeTemplate.width, &kamikazeTemplate.height);
    kamikazeTemplate.width /= 4;
    kamikazeTemplate.height /= 4;

    // 初始化敌机子弹模板
    bulletEnemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-2.png");
    SDL_QueryTexture(bulletEnemyTemplate.texture, NULL, NULL, &bulletEnemyTemplate.width, &bulletEnemyTemplate.height);
    bulletEnemyTemplate.width /= 2;  // 调大尺寸：从/4改为/2
    bulletEnemyTemplate.height /= 2;

    // 初始化爆炸动画模板
    explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
    explosionTemplate.totlaFrame = explosionTemplate.width / explosionTemplate.height;
    explosionTemplate.width = explosionTemplate.height;

    // 初始化物品模板
    itemLifeTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_life.png");
    SDL_QueryTexture(itemLifeTemplate.texture, NULL, NULL, &itemLifeTemplate.width, &itemLifeTemplate.height);
    itemLifeTemplate.width /= 3;  // 调大尺寸：从/4改为/3
    itemLifeTemplate.height /= 3;
    itemLifeTemplate.type = ItemType::Life;

    itemBulletTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_bullet.png");
    SDL_QueryTexture(itemBulletTemplate.texture, NULL, NULL, &itemBulletTemplate.width, &itemBulletTemplate.height);
    itemBulletTemplate.width /= 3;  // 调大尺寸：从/4改为/3
    itemBulletTemplate.height /= 3;
    itemBulletTemplate.type = ItemType::Bullet;

    itemShieldTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_shield.png");
    SDL_QueryTexture(itemShieldTemplate.texture, NULL, NULL, &itemShieldTemplate.width, &itemShieldTemplate.height);
    itemShieldTemplate.width /= 3;  // 调大尺寸：从/4改为/3
    itemShieldTemplate.height /= 3;
    itemShieldTemplate.type = ItemType::Shield;

    itemEnergyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_energy.png");
    SDL_QueryTexture(itemEnergyTemplate.texture, NULL, NULL, &itemEnergyTemplate.width, &itemEnergyTemplate.height);
    itemEnergyTemplate.width /= 3;  // 调大尺寸：从/4改为/3
    itemEnergyTemplate.height /= 3;
    itemEnergyTemplate.type = ItemType::Energy;

    itemSpeedTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_speed.png");
    SDL_QueryTexture(itemSpeedTemplate.texture, NULL, NULL, &itemSpeedTemplate.width, &itemSpeedTemplate.height);
    itemSpeedTemplate.width /= 3;
    itemSpeedTemplate.height /= 3;
    itemSpeedTemplate.type = ItemType::Speed;

    itemDamageTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_atk.png");
    SDL_QueryTexture(itemDamageTemplate.texture, NULL, NULL, &itemDamageTemplate.width, &itemDamageTemplate.height);
    itemDamageTemplate.width /= 3;
    itemDamageTemplate.height /= 3;
    itemDamageTemplate.type = ItemType::Damage;

    // 初始化Boss
    boss.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-boss.png");
    SDL_QueryTexture(boss.texture, NULL, NULL, &boss.width, &boss.height);
    boss.width /= 3;
    boss.height /= 3;
    boss.health = 10;
    boss.maxHealth = 10;
    boss.isActive = false;
    boss.isDefeated = false;
    boss.attackPattern = 0;
}

void SceneMain::clean()
{
    // 清理音效
    SoundManager::getInstance().cleanup();

    // 清理实体列表
    bullets.clear();
    enemies.clear();
    kamikazes.clear();
    bulletsEnemy.clear();
    explosions.clear();
    items.clear();

    if (player.texture != nullptr) {
        SDL_DestroyTexture(player.texture);
    }
    if (bulletTemplate.texture != nullptr) {
        SDL_DestroyTexture(bulletTemplate.texture);
    }
    if (enemyTemplate.texture != nullptr){
        SDL_DestroyTexture(enemyTemplate.texture);
    }
    if (kamikazeTemplate.texture != nullptr){
        SDL_DestroyTexture(kamikazeTemplate.texture);
    }
    if (bulletEnemyTemplate.texture != nullptr){
        SDL_DestroyTexture(bulletEnemyTemplate.texture);
    }
    if (explosionTemplate.texture != nullptr){
        SDL_DestroyTexture(explosionTemplate.texture);
    }
    if (itemLifeTemplate.texture != nullptr) {
        SDL_DestroyTexture(itemLifeTemplate.texture);
    }
    if (itemBulletTemplate.texture != nullptr) {
        SDL_DestroyTexture(itemBulletTemplate.texture);
    }
    if (itemShieldTemplate.texture != nullptr) {
        SDL_DestroyTexture(itemShieldTemplate.texture);
    }
    if (itemEnergyTemplate.texture != nullptr) {
        SDL_DestroyTexture(itemEnergyTemplate.texture);
    }
    if (itemSpeedTemplate.texture != nullptr) {
        SDL_DestroyTexture(itemSpeedTemplate.texture);
    }
    if (itemDamageTemplate.texture != nullptr) {
        SDL_DestroyTexture(itemDamageTemplate.texture);
    }
    if (boss.texture != nullptr) {
        SDL_DestroyTexture(boss.texture);
    }
    if (bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
    if (uiHealth != nullptr){
        SDL_DestroyTexture(uiHealth);
    }
    if (uiShield != nullptr){
        SDL_DestroyTexture(uiShield);
    }
    if (shieldOverlay != nullptr){
        SDL_DestroyTexture(shieldOverlay);
    }
    if (energyBar != nullptr){
        SDL_DestroyTexture(energyBar);
    }
    // 清理字体
    if (scoreFont != nullptr){
        TTF_CloseFont(scoreFont);
    }
}

void SceneMain::keyboardControl(float deltaTime) {
    auto keyboardState = SDL_GetKeyboardState(NULL);
    
    // 设置各方向目标速度
    player.velocityX.target = 0.0f;
    player.velocityY.target = 0.0f;
    
    if (keyboardState[SDL_SCANCODE_W]) player.velocityY.target = -player.speed;
    if (keyboardState[SDL_SCANCODE_S]) player.velocityY.target = player.speed;
    if (keyboardState[SDL_SCANCODE_A]) player.velocityX.target = -player.speed;
    if (keyboardState[SDL_SCANCODE_D]) player.velocityX.target = player.speed;
    
    // 处理加速键
    if (keyboardState[SDL_SCANCODE_SPACE]) {
        player.velocityX.target *= 1.5f;
        player.velocityY.target *= 1.5f;
    }
    
    // 更新X轴速度
    updateVelocity(player.velocityX, deltaTime);
    // 更新Y轴速度
    updateVelocity(player.velocityY, deltaTime);
    
    // 应用移动
    player.position.x += player.velocityX.current * deltaTime;
    player.position.y += player.velocityY.current * deltaTime;
    
    // 限制移动范围
    player.position.x = std::max(0.0f, std::min(player.position.x,
        static_cast<float>(game.getWidth() - player.width)));
    player.position.y = std::max(0.0f, std::min(player.position.y,
        static_cast<float>(game.getHeight() - player.height)));

    // 自动攻击
    auto currentTime = SDL_GetTicks();
    if (currentTime - player.lastShotTime > player.coolDown){
        shot();
        player.lastShotTime = currentTime;
    }

    // K键释放大招
    if (keyboardState[SDL_SCANCODE_K] && player.energy >= player.maxEnergy && !player.isInvincible) {
        activateUltimate();
    }
}

void SceneMain::updateVelocity(Player::Velocity& velocity, float deltaTime) {
    if (velocity.current < velocity.target) {
        velocity.current += player.acceleration * deltaTime;
        if (velocity.current > velocity.target) {
            velocity.current = velocity.target;
        }
    } else if (velocity.current > velocity.target) {
        velocity.current -= player.deceleration * deltaTime;
        if (velocity.current < velocity.target) {
            velocity.current = velocity.target;
        }
    }
}

void SceneMain::shot()
{
    // 根据子弹数量创建多颗子弹
    for (Uint32 i = 0; i < player.bulletCount; ++i) {
        auto bullet = std::make_unique<Bullet>(bulletTemplate);
        // 计算水平偏移：子弹对称分布在玩家中心两侧
        float spacing = bullet->width * 0.3f; // 子弹间距为子弹宽度的30%
        float totalWidth = (player.bulletCount - 1) * spacing;
        float offset = (i - (player.bulletCount - 1) / 2.0f) * spacing;
        // 定位在飞机顶部中央，加上水平偏移
        bullet->position.x = player.position.x + player.width / 2 - bullet->width / 2 + offset;
        bullet->position.y = player.position.y;
        // 添加到活动子弹列表
        bullets.add(std::move(bullet));
    }
    SoundManager::getInstance().play("player_shot");
}

void SceneMain::updateBullet(float deltaTime)
{
    int margin = 32; // 子弹超出屏幕外边界的距离
    for (auto it = bullets.begin(); it != bullets.end();) {
        auto bullet = *it;
        // 更新子弹位置
        bullet->position.y -= bullet->speed * deltaTime;
        // 检查子弹是否超出屏幕
        if (bullet->position.y + margin < 0) {
            it = bullets.erase(it);
        } else {
            // 检查子弹是否击中敌机
            bool isHit = false;
            for (auto enemy : enemies) {
                SDL_Rect bulletRect = {
                    static_cast<int>(bullet->position.x),
                    static_cast<int>(bullet->position.y),
                    bullet->width,
                    bullet->height
                };
                SDL_Rect enemyRect = {
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height
                };
                if (SDL_HasIntersection(&bulletRect, &enemyRect)) {
                    enemy->health -= player.bulletDamage;
                    isHit = true;
                    it = bullets.erase(it);
                    SoundManager::getInstance().play("hit");
                    break;
                }
            }
            // 检查子弹是否击中追踪自爆敌机
            if (!isHit) {
                for (auto kamikaze : kamikazes) {
                    SDL_Rect bulletRect = {
                        static_cast<int>(bullet->position.x),
                        static_cast<int>(bullet->position.y),
                        bullet->width,
                        bullet->height
                    };
                    SDL_Rect kamikazeRect = {
                        static_cast<int>(kamikaze->position.x),
                        static_cast<int>(kamikaze->position.y),
                        kamikaze->width,
                        kamikaze->height
                    };
                    if (SDL_HasIntersection(&bulletRect, &kamikazeRect)) {
                        kamikaze->health -= player.bulletDamage;
                        isHit = true;
                        it = bullets.erase(it);
                        SoundManager::getInstance().play("hit");
                        break;
                    }
                }
            }
            if (!isHit) {
                ++it;
            }

        }
    }

    // 检查子弹是否击中Boss
    for (auto it = bullets.begin(); it != bullets.end();) {
        auto bullet = *it;
        bool isHit = false;

        if (boss.isActive && !boss.isDefeated) {
            SDL_Rect bulletRect = {
                static_cast<int>(bullet->position.x),
                static_cast<int>(bullet->position.y),
                bullet->width,
                bullet->height
            };
            SDL_Rect bossRect = {
                static_cast<int>(boss.position.x),
                static_cast<int>(boss.position.y),
                boss.width,
                boss.height
            };
            if (SDL_HasIntersection(&bulletRect, &bossRect)) {
                boss.health -= player.bulletDamage;
                isHit = true;
                it = bullets.erase(it);
                SoundManager::getInstance().play("hit");
                break;
            }
        }
        if (!isHit) {
            ++it;
        }
    }
}

void SceneMain::renderBullet()
{
    for (auto bullet : bullets){
        SDL_Rect bulletRect = {
            static_cast<int>(bullet->position.x),
            static_cast<int>(bullet->position.y),
            bullet->width,
            bullet->height
        };
        SDL_RenderCopy(game.getRenderer(), bullet->texture, NULL, &bulletRect);
    }
}

void SceneMain::spawEnemy()
{
    // 根据难度调整生成概率（基础概率1/60，随难度增加）
    float spawnProbability = (1 / 60.0f) * difficultySystem.difficultyFactor;
    if (dis(gen) > spawnProbability){
        return;
    }

    // 创建新敌机
    auto enemy = std::make_unique<Enemy>(enemyTemplate);

    // 利用随机数决定敌机的水平位置
    enemy->position.x = dis(gen) * (game.getWidth() - enemy->width);

    // 垂直位置设在屏幕上方（刚好不可见）
    enemy->position.y = - enemy->height;

    // 根据难度调整敌机属性（保持核心属性稳定，仅微调）
    enemy->speed = 150 + (difficultySystem.difficultyFactor - 1.0f) * 30.0f;  // 速度范围：120 ~ 180（小幅调整）
    enemy->health = static_cast<int>(2.0f + (difficultySystem.difficultyFactor - 1.0f) * 1.5f + 0.5f);  // 生命值范围：2 ~ 4
    enemy->coolDown = static_cast<Uint32>(1000.0f - (difficultySystem.difficultyFactor - 1.0f) * 200.0f + 0.5f);  // 冷却范围：800 ~ 1200（适度变化）

    // 添加到活动敌机列表
    enemies.add(std::move(enemy));
}

void SceneMain::updateEnemies(float deltaTime)
{
    auto currentTime = SDL_GetTicks();
    const float stopDistance = 400.0f;
    for (auto it = enemies.begin(); it != enemies.end();) {
        auto enemy = *it;

        // 检查敌机是否完全进入屏幕
        bool fullyInsideScreen = enemy->position.y >= 0 &&
                                  enemy->position.x >= 0 &&
                                  enemy->position.x + enemy->width <= game.getWidth();

        if (!fullyInsideScreen) {
            // 屏幕外：只能向下移动，禁止射击
            enemy->position.y += enemy->speed * deltaTime;
            enemy->angle = AngleCalculator::calculateAngle(
                enemy->position.x + enemy->width / 2,
                enemy->position.y + enemy->height / 2,
                player.position.x + player.width / 2,
                player.position.y + player.height / 2
            );
        } else {
            // 屏幕内：正常移动和射击
            enemy->angle = AngleCalculator::calculateAngle(
                enemy->position.x + enemy->width / 2,
                enemy->position.y + enemy->height / 2,
                player.position.x + player.width / 2,
                player.position.y + player.height / 2
            );

            float distance = AngleCalculator::calculateDistance(
                enemy->position.x + enemy->width / 2,
                enemy->position.y + enemy->height / 2,
                player.position.x + player.width / 2,
                player.position.y + player.height / 2
            );

            if (distance > stopDistance && distance > 0) {
                auto direction = AngleCalculator::calculateDirection(
                    enemy->position.x + enemy->width / 2,
                    enemy->position.y + enemy->height / 2,
                    player.position.x + player.width / 2,
                    player.position.y + player.height / 2
                );
                enemy->position.x += direction.x * enemy->speed * deltaTime;
                enemy->position.y += direction.y * enemy->speed * deltaTime;
            }

            if (currentTime - enemy->lastShotTime > enemy->coolDown) {
                shotEnemy(enemy);
                enemy->lastShotTime = currentTime;
            }
        }

        // 检查是否超出屏幕或死亡
        if (enemy->position.y > game.getHeight() + 100 || enemy->position.x < -100 || enemy->position.x > game.getWidth() + 100 || enemy->health <= 0) {
            if (enemy->health <= 0) {
                enemyExplode(enemy);
            }
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
}

void SceneMain::renderEnemies()
{
    for (auto enemy : enemies){
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height
        };
        // 按照敌机朝向玩家的角度渲染
        SDL_RenderCopyEx(game.getRenderer(), enemy->texture, NULL, &enemyRect, enemy->angle, NULL, SDL_FLIP_NONE);
    }
}

void SceneMain::updateEnemyBullets(float deltaTime)
{
    auto margin = 32; // 屏幕边缘额外余量
    for (auto it = bulletsEnemy.begin(); it != bulletsEnemy.end();) {
        auto bullet = *it;
        
        // 根据方向向量更新子弹位置
        bullet->position.x += bullet->speed * bullet->direction.x * deltaTime;
        bullet->position.y += bullet->speed * bullet->direction.y * deltaTime;
        
        // 检查子弹是否飞出屏幕边界
        if (bullet->position.y > game.getHeight() + margin ||
            bullet->position.y < - margin ||
            bullet->position.x < - margin ||
            bullet->position.x > game.getWidth() + margin) {
            it = bulletsEnemy.erase(it);
        } else {
            // 检查子弹是否击中玩家
            SDL_Rect bulletRect = {
                static_cast<int>(bullet->position.x),
                static_cast<int>(bullet->position.y),
                bullet->width,
                bullet->height
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height
            };
            if (SDL_HasIntersection(&bulletRect, &playerRect) && isDead == false) {
                HealthSystem::takeDamage(player.health, player.shield, bullet->damage, player.isInvincible, [this]() {
                    SoundManager::getInstance().play("hit");
                });
                it = bulletsEnemy.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void SceneMain::renderEnemyBullets()
{
    for (auto bullet : bulletsEnemy){
        SDL_Rect bulletRect = {
            static_cast<int>(bullet->position.x),
            static_cast<int>(bullet->position.y),
            bullet->width,
            bullet->height
        };

        // 计算子弹旋转角度（从方向向量计算角度，并调整为正确的方向）
        // 假设子弹图片原始方向是向上，需要减去90度
        float angle = atan2(bullet->direction.y, bullet->direction.x) * 180 / M_PI - 90;

        // 使用带旋转参数的渲染函数
        SDL_RenderCopyEx(game.getRenderer(), bullet->texture, NULL, &bulletRect, angle, NULL, SDL_FLIP_NONE);
    }
}

void SceneMain::shotEnemy(std::shared_ptr<Enemy> enemy)
{
    // 创建新子弹
    auto bullet = std::make_unique<BulletEnemy>(bulletEnemyTemplate);

    // 设置子弹初始位置（从敌机中央发射）
    bullet->position.x = enemy->position.x + enemy->width / 2 - bullet->width / 2;
    bullet->position.y = enemy->position.y + enemy->height / 2 - bullet->height / 2;

    // 计算子弹飞向玩家的方向
    bullet->direction = AngleCalculator::calculateDirection(
        enemy->position.x + enemy->width / 2,
        enemy->position.y + enemy->height / 2,
        player.position.x + player.width / 2,
        player.position.y + player.height / 2
    );

    // 添加到敌机子弹列表
    bulletsEnemy.add(std::move(bullet));
    SoundManager::getInstance().play("enemy_shot");
}

void SceneMain::updatePlayer(float deltaTime)
{
    if (isDead) {
        return;
    }
    if (player.health <= 0) {
        // 玩家死亡处理
        isDead = true;
        playerExplode();
        shouldSwitchToResult = true;
        return;
    }
    for (auto enemy : enemies) {
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height
        };
        SDL_Rect playerRect = {
            static_cast<int>(player.position.x),
            static_cast<int>(player.position.y),
            player.width,
            player.height
        };
        if (SDL_HasIntersection(&playerRect, &enemyRect)) {
            if (player.isInvincible) {
                enemy->health = 0;  // 无敌状态下直接摧毁敌人
            } else {
                HealthSystem::takeDamage(player.health, player.shield, 1, player.isInvincible, nullptr);
                enemy->health = 0;
            }
        }
    }

    // 检查爆炸对玩家的伤害（仅处理自爆敌机的范围伤害）
    for (auto explosion : explosions) {
        if (explosion->explosionRange > 0 && !explosion->hasDamagedPlayer) {  // 只处理未造成过伤害的爆炸
            float distance = AngleCalculator::calculateDistance(
                explosion->position.x + explosion->width / 2,
                explosion->position.y + explosion->height / 2,
                player.position.x + player.width / 2,
                player.position.y + player.height / 2
            );

            // 爆炸范围内的玩家受到伤害
            if (distance <= explosion->explosionRange && isDead == false && !player.isInvincible) {
                HealthSystem::takeDamage(player.health, player.shield, explosion->explosionDamage, player.isInvincible, [this]() {
                    SoundManager::getInstance().play("hit");
                });
                explosion->hasDamagedPlayer = true;  // 标记已造成伤害，避免重复
            }
        }
    }
}

void SceneMain::playerExplode()
{
    auto currentTime = SDL_GetTicks();
    auto explosion = std::make_unique<Explosion>(explosionTemplate);
    explosion->position.x = player.position.x + player.width / 2 - explosion->width / 2;
    explosion->position.y = player.position.y + player.height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    explosions.add(std::move(explosion));
    SoundManager::getInstance().play("player_explosion");
}

void SceneMain::enemyExplode(std::shared_ptr<Enemy> enemy)
{
    auto currentTime = SDL_GetTicks();
    auto explosion = std::make_unique<Explosion>(explosionTemplate);
    explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2;
    explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    explosions.add(std::move(explosion));
    SoundManager::getInstance().play("enemy_explosion");

    // 添加50%概率掉落物品
    if (dis(gen) < 0.5f){
        dropItem(enemy);
    }
    score += 10;  // 击败敌机获得10分
}

void SceneMain::updateExplosions(float)
{
    auto currentTime = SDL_GetTicks();
    for (auto it = explosions.begin(); it != explosions.end();) {
        auto explosion = *it;
        // 根据时间差和帧率计算当前帧索引
        explosion->currentFrame = (currentTime - explosion->startTime) * explosion->FPS / 1000;
        // 检查动画是否播放完毕
        if (explosion->currentFrame >= explosion->totlaFrame) {
            it = explosions.erase(it);
        } else {
            ++it;
        }
    }
}

void SceneMain::renderExplosions()
{
    for (auto explosion : explosions)
    {
        // 定义源矩形（精灵图上的区域）
        SDL_Rect src = {explosion->currentFrame * explosion->width, 0, explosion->width, explosion->height};
        // 定义目标矩形（屏幕上的位置）
        SDL_Rect dst = {
            static_cast<int>(explosion->position.x), 
            static_cast<int>(explosion->position.y), 
            explosion->width, 
            explosion->height
        };
        // 绘制当前帧
        SDL_RenderCopy(game.getRenderer(), explosion->texture, &src, &dst);
    }
}

void SceneMain::dropItem(std::shared_ptr<Enemy> enemy)
{
    std::shared_ptr<Item> item;
    // 随机选择物品类型：25%生命，25%子弹，15%护盾，15%能量，10%速度，10%攻击
    float randValue = dis(gen);
    if (randValue < 0.25f) {
        item = std::make_shared<Item>(itemLifeTemplate);
    } else if (randValue < 0.5f) {
        item = std::make_shared<Item>(itemBulletTemplate);
    } else if (randValue < 0.65f) {
        item = std::make_shared<Item>(itemShieldTemplate);
    } else if (randValue < 0.8f) {
        item = std::make_shared<Item>(itemEnergyTemplate);
    } else if (randValue < 0.9f) {
        item = std::make_shared<Item>(itemSpeedTemplate);
    } else {
        item = std::make_shared<Item>(itemDamageTemplate);
    }
    item->position.x = enemy->position.x + enemy->width / 2 - item->width / 2;
    item->position.y = enemy->position.y + enemy->height / 2 - item->height / 2;

    // 生成随机方向
    float angle = dis(gen) * 2 * M_PI;
    item->direction.x = cos(angle);
    item->direction.y = sin(angle);

    items.add(std::move(item));
}

void SceneMain::updateItems(float deltaTime)
{
    for (auto it = items.begin(); it != items.end();)
    {
        auto item = *it;
        // 更新位置
        item->position.x += item->direction.x * item->speed * deltaTime;
        item->position.y += item->direction.y * item->speed * deltaTime;
        
        // 处理屏幕边缘反弹
        if (item->position.x < 0 && item->bounceCount > 0) {
            item->direction.x = -item->direction.x;
            item->bounceCount--;
        }
        if (item->position.x + item->width > game.getWidth() && item->bounceCount > 0) {
            item->direction.x = -item->direction.x;
            item->bounceCount--;
        }
        if (item->position.y < 0 && item->bounceCount > 0) {
            item->direction.y = -item->direction.y;
            item->bounceCount--;
        }
        if (item->position.y + item->height > game.getHeight() && item->bounceCount > 0) {
            item->direction.y = -item->direction.y;
            item->bounceCount--;
        }
        
        // 如果超出屏幕范围则删除
        if (item->position.x + item->width < 0 || 
            item->position.x > game.getWidth() ||
            item->position.y + item->height < 0 || 
            item->position.y > game.getHeight()){
            it = items.erase(it);
        } else{
            // 检测物品与玩家的碰撞
            SDL_Rect itemRect = {
                static_cast<int>(item->position.x), 
                static_cast<int>(item->position.y), 
                item->width, 
                item->height
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x), 
                static_cast<int>(player.position.y), 
                player.width, 
                player.height
            };
            
            if (SDL_HasIntersection(&itemRect, &playerRect) && isDead == false) {
                playerGetItem(item);
                it = items.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void SceneMain::playerGetItem(std::shared_ptr<Item> item)
{
    score += 5;  // 拾取物品获得5分
    switch (item->type) {
        case ItemType::Life:
            HealthSystem::heal(player.health, player.maxHealth, 1);
            break;
        case ItemType::Bullet:
            player.bulletCount += 1;
            if (player.bulletCount > player.maxBulletCount) {
                player.bulletCount = player.maxBulletCount;
            }
            break;
        case ItemType::Shield:
            HealthSystem::addShield(player.shield, 1);
            break;
        case ItemType::Energy:
            if (player.energy < player.maxEnergy) {
                player.energy += 1;
            }
            break;
        case ItemType::Speed:
            player.coolDown = std::max(player.minCoolDown, player.coolDown - 15);
            break;
        case ItemType::Damage:
            player.bulletDamage += 1;
            break;
        default:
            break;
    }
    SoundManager::getInstance().play("get_item");
}

void SceneMain::renderItems()
{
    for (auto &item : items)
    {
        SDL_Rect itemRect = {
            static_cast<int>(item->position.x), 
            static_cast<int>(item->position.y), 
            item->width, 
            item->height
        };
        SDL_RenderCopy(game.getRenderer(), item->texture, NULL, &itemRect);
    }   
}

void SceneMain::renderUI()
{
    int x = 10;
    int y = 10;
    int size = 32;
    int offset = 40;
    SDL_SetTextureColorMod(uiHealth, 100, 100, 100); // 颜色减淡
    for (int i = 0; i < player.maxHealth; i++)
    {
        SDL_Rect rect = {x + i * offset, y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &rect);
    }
    SDL_SetTextureColorMod(uiHealth, 255, 255, 255); // reset color
    for (int i = 0; i < player.health; i++)
    {
        SDL_Rect rect = {x + i * offset, y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &rect);
    }
    // 渲染护盾
    int shieldY = y + 50;
    for (int i = 0; i < player.shield; i++)
    {
        SDL_Rect rect = {x + i * offset, shieldY, size, size};
        SDL_RenderCopy(game.getRenderer(), uiShield, NULL, &rect);
    }
    // 渲染得分
    auto text = "SCORE:" + std::to_string(score);
    SDL_Color color = {255, 255, 255, 255};  // 白色
    SDL_Surface* surface = TTF_RenderUTF8_Solid(scoreFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game.getRenderer(), surface);
    SDL_Rect rect = {game.getWidth() - 10 - surface->w, 10, surface->w, surface->h};
    SDL_RenderCopy(game.getRenderer(), texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // 渲染动态能量条（大招）
    int energyY = shieldY + 50;
    int barWidth = 120;  // 能量条总宽度
    int barHeight = 20;  // 能量条高度
    int energyBarX = x;

    // 获取能量条纹理尺寸
    int textureWidth, textureHeight;
    SDL_QueryTexture(energyBar, NULL, NULL, &textureWidth, &textureHeight);

    // 计算填充比例（0.0到1.0）
    float energyRatio = static_cast<float>(player.energy) / player.maxEnergy;

    // 保存当前渲染颜色
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(game.getRenderer(), &r, &g, &b, &a);

    // 绘制能量条背景（白色边框，透明填充）
    SDL_Rect bgRect = {energyBarX, energyY, barWidth, barHeight};
    SDL_SetRenderDrawColor(game.getRenderer(), 255, 255, 255, 255);
    SDL_RenderDrawRect(game.getRenderer(), &bgRect);

    // 恢复之前的渲染颜色
    SDL_SetRenderDrawColor(game.getRenderer(), r, g, b, a);

    // 根据能量值填充能量条
    if (player.energy > 0) {
        // 计算源矩形（从纹理中截取相应宽度的部分）
        int sourceWidth = static_cast<int>(textureWidth * energyRatio);
        SDL_Rect srcRect = {0, 0, sourceWidth, textureHeight};

        // 计算目标矩形（按比例填充能量条）
        int fillWidth = static_cast<int>(barWidth * energyRatio);
        SDL_Rect dstRect = {energyBarX + 1, energyY + 1, fillWidth - 2, barHeight - 2};

        // 渲染填充部分
        SDL_RenderCopy(game.getRenderer(), energyBar, &srcRect, &dstRect);
    }

    // 渲染攻击力
    int statsY = energyY + 40;
    SDL_Color statsColor = {255, 200, 100, 255};  // 金色
    auto damageText = "DMG:" + std::to_string(player.bulletDamage);
    SDL_Surface* damageSurface = TTF_RenderUTF8_Solid(scoreFont, damageText.c_str(), statsColor);
    SDL_Texture* damageTexture = SDL_CreateTextureFromSurface(game.getRenderer(), damageSurface);
    SDL_Rect damageRect = {x, statsY, damageSurface->w, damageSurface->h};
    SDL_RenderCopy(game.getRenderer(), damageTexture, NULL, &damageRect);
    SDL_FreeSurface(damageSurface);
    SDL_DestroyTexture(damageTexture);

    // 渲染射速（显示为每秒射击次数）
    int fireRateY = statsY + 30;
    float fireRate = 1000.0f / player.coolDown;  // 转换为每秒射击次数
    auto fireRateText = "SPD:" + std::to_string(static_cast<int>(fireRate * 10) / 10.0f) + "/s";
    SDL_Surface* fireRateSurface = TTF_RenderUTF8_Solid(scoreFont, fireRateText.c_str(), statsColor);
    SDL_Texture* fireRateTexture = SDL_CreateTextureFromSurface(game.getRenderer(), fireRateSurface);
    SDL_Rect fireRateRect = {x, fireRateY, fireRateSurface->w, fireRateSurface->h};
    SDL_RenderCopy(game.getRenderer(), fireRateTexture, NULL, &fireRateRect);
    SDL_FreeSurface(fireRateSurface);
    SDL_DestroyTexture(fireRateTexture);
}

void SceneMain::updateUltimate(float deltaTime)
{
    if (player.isInvincible) {
        auto currentTime = SDL_GetTicks();
        if (currentTime - player.invincibleStartTime >= player.invincibleDuration) {
            player.isInvincible = false;
        }
    }
}

void SceneMain::activateUltimate()
{
    if (player.energy >= player.maxEnergy && !player.isInvincible) {
        player.energy = 0;
        player.isInvincible = true;
        player.invincibleStartTime = SDL_GetTicks();
        SoundManager::getInstance().play("get_item");  // 使用拾取音效作为大招音效
    }
}

void SceneMain::spawnKamikaze()
{
    // 根据难度调整生成概率（基础概率1/120，随难度增加）
    float spawnProbability = (1 / 120.0f) * difficultySystem.difficultyFactor * 0.8f;  // 自爆敌机略少
    if (dis(gen) > spawnProbability){
        return;
    }

    // 创建新追踪自爆敌机
    auto kamikaze = std::make_unique<KamikazeEnemy>(kamikazeTemplate);

    // 利用随机数决定敌机的水平位置
    kamikaze->position.x = dis(gen) * (game.getWidth() - kamikaze->width);

    // 垂直位置设在屏幕上方（刚好不可见）
    kamikaze->position.y = - kamikaze->height;

    // 根据难度调整自爆敌机属性（保持核心属性稳定，仅微调）
    kamikaze->speed = 200 + (difficultySystem.difficultyFactor - 1.0f) * 40.0f;  // 速度范围：160 ~ 240（小幅调整）
    kamikaze->health = static_cast<int>(2.0f + (difficultySystem.difficultyFactor - 1.0f) * 1.0f + 0.5f);  // 生命值范围：2 ~ 3
    kamikaze->explosionDelay = static_cast<Uint32>(3000.0f - (difficultySystem.difficultyFactor - 1.0f) * 300.0f + 0.5f);  // 倒计时范围：2700 ~ 3300（基本保持稳定）

    // 添加到活动追踪自爆敌机列表
    kamikazes.add(std::move(kamikaze));
}

void SceneMain::updateKamikazes(float deltaTime)
{
    auto currentTime = SDL_GetTicks();
    for (auto it = kamikazes.begin(); it != kamikazes.end();) {
        auto kamikaze = *it;

        // 更新自爆敌机朝向玩家的角度
        kamikaze->angle = AngleCalculator::calculateAngle(
            kamikaze->position.x + kamikaze->width / 2,
            kamikaze->position.y + kamikaze->height / 2,
            player.position.x + player.width / 2,
            player.position.y + player.height / 2
        );

        // 检查血量是否耗尽
        if (kamikaze->health <= 0) {
            kamikazeExplode(kamikaze);
            it = kamikazes.erase(it);
            continue;
        }

        // 如果正在倒计时自爆，继续追踪玩家并检查倒计时
        if (kamikaze->isExploding) {
            // 继续追踪玩家
            auto direction = AngleCalculator::calculateDirection(
                kamikaze->position.x + kamikaze->width / 2,
                kamikaze->position.y + kamikaze->height / 2,
                player.position.x + player.width / 2,
                player.position.y + player.height / 2
            );
            kamikaze->position.x += direction.x * kamikaze->speed * deltaTime;
            kamikaze->position.y += direction.y * kamikaze->speed * deltaTime;

            // 检查倒计时是否结束
            if (currentTime - kamikaze->explosionStartTime >= kamikaze->explosionDelay) {
                kamikazeExplode(kamikaze);
                it = kamikazes.erase(it);
                continue;
            } else {
                ++it;
                continue;
            }
        }

        // 计算到玩家的距离
        auto distance = AngleCalculator::calculateDistance(
            kamikaze->position.x + kamikaze->width / 2,
            kamikaze->position.y + kamikaze->height / 2,
            player.position.x + player.width / 2,
            player.position.y + player.height / 2
        );

        // 如果距离玩家小于等于200像素，触发自爆倒计时
        if (distance <= 200.0f) {
            kamikaze->isExploding = true;
            kamikaze->explosionStartTime = currentTime;
        } else {
            // 追踪玩家
            auto direction = AngleCalculator::calculateDirection(
                kamikaze->position.x + kamikaze->width / 2,
                kamikaze->position.y + kamikaze->height / 2,
                player.position.x + player.width / 2,
                player.position.y + player.height / 2
            );
            kamikaze->position.x += direction.x * kamikaze->speed * deltaTime;
            kamikaze->position.y += direction.y * kamikaze->speed * deltaTime;
        }

        // 检查是否击中玩家
        SDL_Rect kamikazeRect = {
            static_cast<int>(kamikaze->position.x),
            static_cast<int>(kamikaze->position.y),
            kamikaze->width,
            kamikaze->height
        };
        SDL_Rect playerRect = {
            static_cast<int>(player.position.x),
            static_cast<int>(player.position.y),
            player.width,
            player.height
        };
        if (SDL_HasIntersection(&kamikazeRect, &playerRect) && isDead == false) {
            if (player.isInvincible) {
                kamikazeExplode(kamikaze);  // 无敌状态下直接引爆
            } else {
                HealthSystem::takeDamage(player.health, player.shield, 1, player.isInvincible, nullptr);
                kamikazeExplode(kamikaze);
            }
            it = kamikazes.erase(it);
        } else if (kamikaze->position.y > game.getHeight() + 100 ||
                   kamikaze->position.y < -100 ||
                   kamikaze->position.x < -100 ||
                   kamikaze->position.x > game.getWidth() + 100) {
            // 超出屏幕范围则删除
            it = kamikazes.erase(it);
        } else {
            ++it;
        }
    }
}

void SceneMain::renderKamikazes()
{
    auto currentTime = SDL_GetTicks();
    for (auto kamikaze : kamikazes) {
        SDL_Rect kamikazeRect = {
            static_cast<int>(kamikaze->position.x),
            static_cast<int>(kamikaze->position.y),
            kamikaze->width,
            kamikaze->height
        };

        // 如果正在倒计时自爆，红色闪烁效果
        if (kamikaze->isExploding) {
            int blinkInterval = 150;  // 闪烁间隔（毫秒）
            if ((currentTime / blinkInterval) % 2 == 0) {
                SDL_SetTextureColorMod(kamikaze->texture, 255, 50, 50);  // 红色
            } else {
                SDL_SetTextureColorMod(kamikaze->texture, 255, 255, 255);  // 白色
            }
            // 按照自爆敌机朝向玩家的角度渲染
            SDL_RenderCopyEx(game.getRenderer(), kamikaze->texture, NULL, &kamikazeRect, kamikaze->angle, NULL, SDL_FLIP_NONE);
            SDL_SetTextureColorMod(kamikaze->texture, 255, 255, 255);  // 重置颜色
        } else {
            // 按照自爆敌机朝向玩家的角度渲染
            SDL_RenderCopyEx(game.getRenderer(), kamikaze->texture, NULL, &kamikazeRect, kamikaze->angle, NULL, SDL_FLIP_NONE);
        }
    }
}

void SceneMain::kamikazeExplode(std::shared_ptr<KamikazeEnemy> kamikaze)
{
    auto currentTime = SDL_GetTicks();
    auto explosion = std::make_unique<Explosion>(explosionTemplate);
    explosion->position.x = kamikaze->position.x + kamikaze->width / 2 - explosion->width / 2;
    explosion->position.y = kamikaze->position.y + kamikaze->height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    explosion->explosionDamage = player.bulletDamage + 1;  // 自爆伤害为玩家伤害+1，最少2
    explosion->explosionRange = kamikaze->width * 1.5f;  // 爆炸半径（像素）
    explosions.add(std::move(explosion));
    SoundManager::getInstance().play("enemy_explosion");

    score += 15;  // 击败追踪自爆敌机获得15分
}

void SceneMain::updateDifficulty()
{
    // 检查分数变化是否达到阈值
    if (score - difficultySystem.lastUpdateScore < difficultySystem.updateThreshold) {
        // 平滑过渡到目标难度
        float diff = difficultySystem.targetFactor - difficultySystem.difficultyFactor;
        if (fabs(diff) > 0.001f) {
            difficultySystem.difficultyFactor += diff * difficultySystem.smoothSpeed;
        }
        return;
    }

    // 更新并计算新的目标难度
    difficultySystem.lastUpdateScore = score;
    difficultySystem.targetFactor = calculateTargetDifficulty();
}

void SceneMain::spawnBoss()
{
    // 计算Boss血量公式：得分^2 * 1/2，限制最大值
    int maxBossHealth = static_cast<int>(0.1f * pow(score, 1.1f));

    // 计算玩家血量公式：3 + 得分 * 1/100
    int newPlayerMaxHealth = 3 + static_cast<int>(score / 100);
    // 只有当最大血量增加时才更新，同时增加当前血量
    if (newPlayerMaxHealth > player.maxHealth) {
        HealthSystem::heal(player.health, player.maxHealth, newPlayerMaxHealth - player.maxHealth);
        player.maxHealth = newPlayerMaxHealth;
    }

    // 检查是否需要生成新Boss（分数超过上次Boss出现分数且当前没有激活的Boss）
    if (score >= lastBossSpawnScore + 500 && !boss.isActive) {
        lastBossSpawnScore = score;
        boss.isActive = true;
        boss.isDefeated = false;
        boss.spawnTime = SDL_GetTicks();
        boss.maxHealth = maxBossHealth;
        boss.health = maxBossHealth;
        boss.position.x = game.getWidth() / 2 - boss.width / 2;
        boss.position.y = -boss.height;  // 从屏幕上方进入
        boss.attackPattern = 0;
        SoundManager::getInstance().play("enemy_explosion");
    }
}

void SceneMain::updateBoss(float deltaTime)
{
    if (!boss.isActive || boss.isDefeated) {
        return;
    }

    auto currentTime = SDL_GetTicks();

    // Boss进入屏幕后停止
    if (boss.position.y < 50) {
        boss.position.y += boss.speed * deltaTime;
        boss.angle = 0;
        return;
    }

    // 更新Boss朝向玩家的角度
    float distance = AngleCalculator::calculateDistance(
        boss.position.x + boss.width / 2,
        boss.position.y + boss.height / 2,
        player.position.x + player.width / 2,
        player.position.y + player.height / 2
    );
    boss.angle = AngleCalculator::calculateAngle(
        boss.position.x + boss.width / 2,
        boss.position.y + boss.height / 2,
        player.position.x + player.width / 2,
        player.position.y + player.height / 2
    );

    // 水平追踪玩家（缓慢移动）
    auto direction = AngleCalculator::calculateDirection(
        boss.position.x + boss.width / 2,
        boss.position.y + boss.height / 2,
        player.position.x + player.width / 2,
        player.position.y + player.height / 2
    );
    boss.position.x += direction.x * 30.0f * deltaTime;

    // 每5秒切换攻击模式
    if (currentTime - boss.patternChangeTime > 5000) {
        boss.patternChangeTime = currentTime;
        boss.attackPattern = (boss.attackPattern + 1) % 2;
    }

    // 检查冷却时间并攻击
    if (currentTime - boss.lastShotTime > boss.coolDown) {
        if (boss.attackPattern == 0) {
            bossShot();  // 散弹攻击
        } else {
            bossShotCircle();  // 环形弹幕
        }
        boss.lastShotTime = currentTime;
    }

    // 检查Boss碰撞（玩家直接撞到Boss）
    if (checkBossCollision()) {
        HealthSystem::takeDamage(player.health, player.shield, 1, player.isInvincible, [this]() {
            SoundManager::getInstance().play("hit");
        });
    }

    // 检查Boss死亡
    if (boss.health <= 0) {
        boss.isDefeated = true;
        bossExplode();
        boss.isActive = false;
        score += boss.maxHealth / 10;  // 击败Boss获得血量10%的分数
    }
}

void SceneMain::renderBoss()
{
    if (!boss.isActive || boss.isDefeated) {
        return;
    }

    SDL_Rect bossRect = {
        static_cast<int>(boss.position.x),
        static_cast<int>(boss.position.y),
        boss.width,
        boss.height
    };
    SDL_RenderCopyEx(game.getRenderer(), boss.texture, NULL, &bossRect, boss.angle, NULL, SDL_FLIP_NONE);

    // 绘制Boss血条
    int barWidth = boss.width;
    int barHeight = 8;
    int barX = static_cast<int>(boss.position.x);
    int barY = static_cast<int>(boss.position.y - 15);

    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(game.getRenderer(), &r, &g, &b, &a);

    // 血条背景
    SDL_Rect bgRect = {barX, barY, barWidth, barHeight};
    SDL_SetRenderDrawColor(game.getRenderer(), 100, 100, 100, 255);
    SDL_RenderFillRect(game.getRenderer(), &bgRect);

    // 血条填充
    float healthRatio = static_cast<float>(boss.health) / boss.maxHealth;
    SDL_Rect fillRect = {barX, barY, static_cast<int>(barWidth * healthRatio), barHeight};
    SDL_SetRenderDrawColor(game.getRenderer(), 255, 50, 50, 255);
    SDL_RenderFillRect(game.getRenderer(), &fillRect);

    SDL_SetRenderDrawColor(game.getRenderer(), r, g, b, a);
}

void SceneMain::bossShot()
{
    // 散弹攻击：向前方发射5发扇形子弹
    int bulletCount = 5;
    float spreadAngle = 30.0f;  // 散弹角度范围

    for (int i = 0; i < bulletCount; i++) {
        auto bullet = std::make_unique<BulletEnemy>(bulletEnemyTemplate);
        bullet->position.x = boss.position.x + boss.width / 2 - bullet->width / 2;
        bullet->position.y = boss.position.y + boss.height / 2 - bullet->height / 2;

        float angleOffset = (i - (bulletCount - 1) / 2.0f) * (spreadAngle / (bulletCount - 1));
        float angle = (boss.angle + 90) * M_PI / 180.0f + angleOffset * M_PI / 180.0f;

        bullet->direction.x = cos(angle);
        bullet->direction.y = sin(angle);
        bullet->speed = 250.0f;
        bullet->damage = 1;

        bulletsEnemy.add(std::move(bullet));
    }
    SoundManager::getInstance().play("enemy_shot");
}

void SceneMain::bossShotCircle()
{
    // 环形弹幕：向四周发射8发子弹
    int bulletCount = 8;
    for (int i = 0; i < bulletCount; i++) {
        auto bullet = std::make_unique<BulletEnemy>(bulletEnemyTemplate);
        bullet->position.x = boss.position.x + boss.width / 2 - bullet->width / 2;
        bullet->position.y = boss.position.y + boss.height / 2 - bullet->height / 2;

        float angle = i * (360.0f / bulletCount) * M_PI / 180.0f;
        bullet->direction.x = cos(angle);
        bullet->direction.y = sin(angle);
        bullet->speed = 200.0f;
        bullet->damage = 1;

        bulletsEnemy.add(std::move(bullet));
    }
    SoundManager::getInstance().play("enemy_shot");
}

void SceneMain::bossExplode()
{
    auto currentTime = SDL_GetTicks();
    auto explosion = std::make_unique<Explosion>(explosionTemplate);
    explosion->position.x = boss.position.x + boss.width / 2 - explosion->width / 2;
    explosion->position.y = boss.position.y + boss.height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    explosion->explosionDamage = 5;
    float explosionRange = boss.width * 2;  // 先保存爆炸范围
    explosion->explosionRange = explosionRange;
    explosions.add(std::move(explosion));
    SoundManager::getInstance().play("enemy_explosion");

    // 爆炸伤害玩家
    auto distance = AngleCalculator::calculateDistance(
        boss.position.x + boss.width / 2,
        boss.position.y + boss.height / 2,
        player.position.x + player.width / 2,
        player.position.y + player.height / 2
    );

    if (distance <= explosionRange && isDead == false && !player.isInvincible) {
        HealthSystem::takeDamage(player.health, player.shield, 5, player.isInvincible, [this]() {
            SoundManager::getInstance().play("hit");
        });
    }
}

bool SceneMain::checkBossCollision()
{
    SDL_Rect bossRect = {
        static_cast<int>(boss.position.x),
        static_cast<int>(boss.position.y),
        boss.width,
        boss.height
    };
    SDL_Rect playerRect = {
        static_cast<int>(player.position.x),
        static_cast<int>(player.position.y),
        player.width,
        player.height
    };
    return SDL_HasIntersection(&bossRect, &playerRect);
}

float SceneMain::calculateTargetDifficulty()
{
    // 计算装备评分：子弹数量 + 护盾 + 能量（降低影响权重）
    difficultySystem.lastUpdateScore = score;
    float equipmentScore = player.bulletCount * 1.5f + player.shield * 1.0f + player.energy * 0.5f;

    // 基础难度系数：根据分数计算（更加平滑的曲线）
    float baseDifficulty = 1.0f;
    if (score < 500) {
        // 前期（0-500分）：难度 0.6 ~ 1.0（提高下限，避免过简单）
        baseDifficulty = 0.6f + (score / 500.0f) * 0.4f;
    } else if (score < 2000) {
        // 中期（500-2000分）：难度 1.0 ~ 1.6（降低上限，避免过难）
        baseDifficulty = 1.0f + ((score - 500) / 1500.0f) * 0.6f;
    } else {
        // 后期（2000+分）：难度 1.6 ~ 2.0，缓慢逼近上限
        baseDifficulty = 1.6f + (1.0f - exp(-(score - 2000) / 3000.0f)) * 0.4f;
    }

    // 装备影响：装备越强，难度越高（降低影响幅度）
    float equipmentFactor = 1.0f + (equipmentScore / 30.0f) * 0.15f;  // 最多增加15%

    // 生命值影响：生命越低，难度略微降低（降低影响幅度，避免负反馈）
    float healthFactor = 0.9f + (player.health / static_cast<float>(player.maxHealth)) * 0.2f;  // 范围：0.9 ~ 1.1

    // 综合计算目标难度
    float target = baseDifficulty * equipmentFactor * healthFactor;

    // 限制在 0.6 ~ 2.0 范围内（收缩范围，减少极端情况）
    difficultySystem.targetFactor = std::max(0.6f, std::min(2.0f, target));
    return difficultySystem.targetFactor;
}

float SceneMain::getSmoothFactor(float baseValue, float difficultyScale, float maxScale)
{
    // 根据当前难度系数计算平滑的属性值
    float normalizedDiff = (difficultySystem.difficultyFactor - 0.5f) / 2.0f;  // 归一化到 0 ~ 1
    float scaleFactor = 1.0f + normalizedDiff * (maxScale - 1.0f) * difficultyScale;
    return baseValue * scaleFactor;
}