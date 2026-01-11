# CODEBUDDY.md

本文档为 CodeBuddy 提供本代码仓库中代码的开发指引。

## 项目概述

这是一款基于 C++17 和 SDL2 构建的 2D 太空射击游戏。游戏采用基于场景的架构设计，包含标题界面和主游戏场景，实现了玩家/敌人战斗、道具增益（Power-ups）、大招系统以及视差背景滚动效果。

核心技术栈：
- C++17（含标准库特性）
- SDL2：用于图形渲染、窗口管理和输入处理
- SDL2_image：用于 PNG 图片加载
- SDL2_mixer：用于音频播放（背景音乐和音效）
- SDL2_ttf：用于 TrueType 字体渲染

## 构建系统

本项目使用 CMake（3.10+）并搭配 Visual Studio 2022 预设，适用于 Windows 开发环境。

- 主配置文件：`CMakeLists.txt`
- 构建预设：`CMakePresets.json`
- 目标可执行文件：`SDLShooter-Windows.exe`
- 输出目录：
  - 调试版（Debug）：`bin/Debug/SDLShooter-Windows.exe`
  - 发布版（Release）：`bin/Release/SDLShooter-Windows.exe`

依赖项（通过 `find_package` 查找）：
- SDL2
- SDL2_image
- SDL2_mixer
- SDL2_ttf

## 常用命令

### 构建项目
```bash
# 使用 CMake 预设（推荐方式）
cmake --preset "Visual Studio 生成工具 2022 Release - amd64"
cmake --build --preset "Visual Studio 生成工具 2022 Release - amd64-debug"

# 或使用 VS Code 任务：
# 执行「CMake: build」任务（快捷键 Ctrl+Shift+B）
```

### 调试
- 使用 VS Code 启动配置「MSVC Debug」
- 调试前会通过 preLaunchTask 自动构建项目
- PATH 环境变量包含来自 `C:/Users/23076/.cmake/` 的 SDL2 DLL 目录

### 运行
```bash
# 构建完成后，从输出目录运行
./bin/Debug/SDLShooter-Windows.exe
```

### 清理构建产物
```bash
# 删除构建生成的文件
rm -rf build/ out/ bin/
```

## 架构设计

### 核心组件

1. **游戏单例（Game Singleton）**（`Game.h`/`Game.cpp`）
   - 单例模式确保全局仅有一个实例
   - 管理固定 30 FPS 时间步长的主游戏循环
   - 处理 SDL 的初始化/清理以及场景切换逻辑
   - 提供渲染上下文（窗口、渲染器）
   - 实现视差背景滚动效果

2. **场景系统（Scene System）**（`IScene.h`）
   - 游戏场景的抽象接口
   - 核心方法：`init()`（初始化）、`update()`（更新）、`render()`（渲染）、`clean()`（清理）、`handleEvent()`（事件处理）
   - 具体实现：
     - `SceneTitle`：标题界面（含背景音乐和开始提示）
     - `SceneMain`：主游戏场景（含战斗核心逻辑）

3. **游戏对象系统（Game Object System）**（`Object.h`）
   - 基于结构体的组件化系统，定义游戏实体：
     - `Player`：玩家飞船（含生命值、护盾、子弹数、能量、大招机制）
     - `Bullet`：玩家发射的子弹
     - `Enemy`：普通敌机（含追踪玩家、旋转朝向、射击逻辑）
     - `KamikazeEnemy`：自爆敌机（含追踪、自爆倒计时、爆炸范围）
     - `BulletEnemy`：敌方发射的子弹
     - `Explosion`：爆炸动画效果（含范围伤害）
     - `Item`：增益道具（生命、护盾、子弹、能量）
     - `Background`：视差滚动的星空背景

4. **工具组件系统（Utility Components）**（新增）
   - `AngleCalculator`（`AngleCalculator.h/cpp`）：角度和方向计算
   - `HealthSystem`（`HealthSystem.h/cpp`）：生命值和护盾管理
   - `EntityManager`（`EntityManager.h`）：通用实体列表管理
   - `SoundManager`（`SoundManager.h/cpp`）：音效加载和播放（单例）
   - `TextureManager`（`TextureManager.h/cpp`）：纹理加载和管理（单例）

5. **主游戏逻辑（Main Gameplay）**（`SceneMain.h`/`SceneMain.cpp`）
   - 管理玩家、敌人、子弹、道具、爆炸效果的生命周期
   - 实现碰撞检测逻辑
   - 处理计分和 UI 渲染
   - 使用随机数生成实现敌人随机生成
   - 通过 SDL_mixer 管理音效播放
   - 自动攻击系统：玩家无需按键即可自动射击
   - 大招系统：能量满后按K键释放无敌技能

### 程序入口
- `main.cpp`：程序入口 → `Game::getInstance().init()` → `Game::run()`
- 游戏启动后默认加载 `SceneTitle`，按下 J 键切换至 `SceneMain`

### 设计模式
- 单例模式（Singleton）：Game 类、SoundManager、TextureManager
- 场景模式（Scene）：用于游戏状态管理的抽象场景接口
- 基于组件的实体（Component-based Entities）：基于结构体的游戏对象设计
- 资源管理（Resource Management）：SDL 负责纹理、音频、字体的生命周期管理
- 模板设计（Template）：EntityManager 支持多种实体类型

## 游戏特性

### 核心玩法
1. **自动攻击**：玩家无需按键即可自动发射子弹，冷却时间 250ms
2. **动态追踪**：所有敌机实时追踪玩家位置，并计算角度朝向
3. **多样化敌人**：
   - 普通敌机：追踪玩家，发射子弹，击杀获得10分
   - 自爆敌机：追踪玩家，接近后3秒自爆倒计时，爆炸范围伤害2点，击杀获得15分
4. **道具系统**：
   - 生命道具：恢复1点生命值
   - 护盾道具：增加1点护盾，护盾优先承受伤害
   - 子弹道具：增加子弹数量，最多50发
   - 能量道具：增加能量值，满3能量可释放大招
5. **大招机制**：收集3个能量后按K键释放，获得5秒无敌状态，无敌时可直接撞击摧毁敌人

### 伤害系统
- **护盾优先**：玩家受到伤害时，护盾优先承受，护盾耗尽才扣除生命值
- **无敌保护**：大招无敌期间不受任何伤害，且碰撞敌人直接摧毁敌人
- **范围伤害**：自爆敌机爆炸有范围效果，在爆炸半径内的玩家受到伤害

### UI 显示
- 生命值：屏幕左上角，红色心形图标
- 护盾值：屏幕左上角生命下方，蓝色心形图标
- 能量条：屏幕左上角护盾下方，动态填充的进度条，满时显示提示文字
- 分数：屏幕右上角，白色文字显示

## 开发文档

- **GAME_RULES.md**：详细的游戏规则说明书，包含操作、角色、机制、技巧等
- **REFACTOR.md**：代码重构方案，包含新增的5个工具组件和使用说明

## 资源文件结构

资源文件存储在可执行文件相对路径下的 `assets/` 目录中：

```
assets/
├── image/     # 游戏精灵和纹理
│   ├── SpaceShip.png（玩家飞船）
│   ├── insect-2.png（普通敌机）
│   ├── insect-1.png（自爆敌机）
│   ├── laser-1.png（玩家子弹）
│   ├── laser-2.png（敌机子弹）
│   ├── explosion.png（爆炸效果）
│   ├── Stars-A.png（近处星星背景）
│   ├── Stars-B.png（远处星星背景）
│   ├── Health UI Black.png（生命值UI）
│   ├── Shield UI Black.png（护盾UI）
│   ├── shield.png（护盾层）
│   ├── bonus_life.png（生命道具）
│   ├── bonus_bullet.png（子弹道具）
│   ├── bonus_shield.png（护盾道具）
│   ├── bonus_energy.png（能量道具）
│   └── energy-bar.png（能量条）
├── font/      # UI 用 TrueType 字体（VonwaonBitmap）
│   ├── VonwaonBitmap-16px.ttf（标题字体）
│   └── VonwaonBitmap-12px.ttf（UI字体）
├── music/     # 背景音乐（.ogg 格式）
│   └── 03_Racing_Through_Asteroids_Loop.ogg
└── sound/     # 音效文件
    ├── laser_shoot4.wav（玩家射击）
    ├── xs_laser.wav（敌机射击）
    ├── explosion1.wav（玩家爆炸）
    ├── explosion3.wav（敌机爆炸）
    ├── eff11.wav（击中音效）
    └── eff5.wav（拾取音效）
```

## 开发配置

### VS Code 配置（`.vscode/`）
- `settings.json`：C++ 文件关联和 CMake 源码目录配置
- `tasks.json`：使用 Visual Studio 2022 预设的 CMake 构建任务
- `launch.json`：调试配置（含 SDL DLL 的 PATH 环境变量配置）
- `c_cpp_properties.json`：C++ 智能提示（IntelliSense）配置

### SDL2 依赖项
SDL2 库预期安装在以下路径：
- `C:/Users/23076/.cmake/SDL2-2.32.10/`
- `C:/Users/23076/.cmake/SDL2_image-2.8.8/`
- `C:/Users/23076/.cmake/SDL2_mixer-2.8.1/`
- `C:/Users/23076/.cmake/SDL2_ttf-2.24.0/`

运行时要求对应的 DLL 文件存在于 PATH 环境变量中，或可执行文件同级目录下。

## 核心实现说明

- 游戏对象使用 `std::list<std::shared_ptr<Entity>>` 进行管理
- `Game::run()` 中实现固定时间步长的游戏循环，目标帧率 30 FPS
- 碰撞检测采用简单的轴对齐包围盒（AABB, axis-aligned bounding boxes）算法
- 敌方随机生成使用 `std::mt19937` 随机数生成器和 `std::uniform_real_distribution` 均匀分布
- 角度计算使用 `atan2()` 和三角函数，支持实体朝向追踪
- 音频处理：`Mix_Music*` 用于背景音乐，`Mix_Chunk*` 用于音效
- 文本渲染通过 `TTF_Font*` 实现，`Game::renderTextCentered()` 提供居中文字渲染工具函数
- 伤害系统：统一的生命值和护盾处理逻辑，支持无敌状态和回调函数