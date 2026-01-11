# SDL Shooter 安装包打包指南

本文档介绍如何为 SDL Shooter 项目生成 Windows 安装包。

## 打包工具选择

我们使用 **CPack + NSIS** 方案生成专业的 Windows 安装程序，具有以下特点：
- 完整的安装向导界面
- 开始菜单快捷方式
- 卸载程序
- 安装完成后运行游戏选项
- 包含所有游戏资源文件
- 可选的桌面快捷方式
- 注意：Visual C++ Redistributable 需要用户手动安装

## 系统要求

### 必需软件
1. **CMake 3.10+** - 构建系统
2. **Visual Studio 2022** - C++ 编译器（或 MSBuild 工具）
3. **NSIS (Nullsoft Scriptable Install System) 3.0+** - 安装包生成器

### 可选软件
4. **vcpkg** - 用于管理 SDL2 依赖（如果未手动安装 SDL2）

## 安装 NSIS

### 方法1：使用安装程序
1. 访问 [NSIS 官网](https://nsis.sourceforge.io/Download)
2. 下载最新版本（推荐 3.09+）
3. 运行安装程序，选择默认选项
4. **重要**：确保将 NSIS 安装路径添加到系统 PATH 环境变量

### 方法2：使用 Chocolatey（推荐）
```powershell
# 以管理员身份运行 PowerShell
choco install nsis
```

### 验证安装
```bash
makensis /VERSION
```

## 打包步骤

### 1. 准备构建环境

确保 SDL2 依赖库已正确安装。项目支持以下安装方式：

#### 选项A：使用项目配置的路径（推荐）
SDL2 库应安装在：`C:/Users/<用户名>/.cmake/`
- `SDL2-2.32.10/`
- `SDL2_image-2.8.8/`
- `SDL2_mixer-2.8.1/`
- `SDL2_ttf-2.24.0/`

#### 选项B：使用 vcpkg
```bash
vcpkg install sdl2:x64-windows sdl2-image:x64-windows sdl2-mixer:x64-windows sdl2-ttf:x64-windows
```

### 2. 配置项目

```bash
# 使用 CMake 预设（推荐）
cmake --preset "Visual Studio 生成工具 2022 Release - amd64"

# 或手动配置
cmake -B build -S . -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

### 3. 构建项目

```bash
# 构建 Release 版本
cmake --build build --config Release
```

### 4. 生成安装包

```bash
# 进入构建目录
cd build

# 使用 CPack 生成安装包
cpack -C Release

# 或指定生成器
cpack -C Release -G NSIS
```

### 5. 安装包输出

安装包将生成在构建目录中：
- `SDLShooter-0.1.0-win64.exe` - NSIS 安装程序
- `SDLShooter-0.1.0-win64.zip` - 便携版 ZIP 包

## 安装包内容

生成的安装包包含：

### 文件结构
```
安装目录/
├── bin/
│   ├── SDLShooter-Windows.exe     # 游戏主程序
│   ├── SDL2.dll                   # SDL2 核心库
│   ├── SDL2_image.dll             # 图像支持库
│   ├── SDL2_mixer.dll             # 音频支持库
│   └── SDL2_ttf.dll               # 字体支持库
├── assets/                        # 游戏资源文件
│   ├── image/                     # 图像资源
│   ├── font/                      # 字体文件
│   ├── music/                     # 背景音乐
│   ├── sound/                     # 音效文件
│   └── effect/                    # 特效资源
├── docs/                          # 文档
│   ├── README.md                  # 项目说明
│   ├── GAME_RULES.md              # 游戏规则
│   └── LICENSE.md                 # 许可证文件
└── uninstall.exe                  # 卸载程序
```

### 安装功能
1. **运行游戏选项** - 安装完成后提供运行游戏选项
2. **开始菜单快捷方式** - 在开始菜单创建游戏和卸载程序快捷方式
3. **可选桌面快捷方式** - 用户可选择是否创建桌面快捷方式
4. **程序目录选择** - 用户可自定义安装路径（默认：`C:\Program Files\SDL Shooter\`）
5. **完整的卸载功能** - 通过控制面板或开始菜单卸载程序可完全移除
6. **手动 VC++ Redistributable 安装** - 由于网络依赖和编码问题，自动安装已禁用，用户可从微软官网手动安装

## 高级配置

### 自定义安装包

编辑 `CMakeLists.txt` 中的 CPack 配置部分：

#### 修改版本信息
```cmake
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
```

#### 添加快捷方式图标
1. 将图标文件（.ico 格式）放入 `assets/image/` 目录
2. 取消注释并修改以下配置：
```cmake
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/assets/image/game-icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/assets/image/game-icon.ico")
```

#### 添加桌面快捷方式
取消注释以下配置：
```cmake
set(CPACK_NSIS_CREATE_ICONS_EXTRA "
  CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\SDL Shooter.lnk' '$INSTDIR\\\\bin\\\\SDLShooter-Windows.exe'
  CreateShortCut '$DESKTOP\\\\SDL Shooter.lnk' '$INSTDIR\\\\bin\\\\SDLShooter-Windows.exe'
")
```

### 多配置构建

```bash
# 生成 Debug 和 Release 版本
cmake -B build -S . -G "Visual Studio 17 2022" -A x64

# 构建所有配置
cmake --build build --config Release
cmake --build build --config Debug

# 为特定配置生成安装包
cd build
cpack -C Release
cpack -C Debug
```

## 故障排除

### 常见问题

#### 1. NSIS 未找到错误
```
Could not find NSIS compiler makensis: likely it is not installed, or not in your PATH
```
**解决方法**：确保 NSIS 已安装并添加到系统 PATH 环境变量。

#### 2. DLL 文件未找到
```
Could not find SDL2.dll
```
**解决方法**：
- 确保 SDL2 库已正确安装
- 检查 `C:/Users/<用户名>/.cmake/` 目录是否存在 SDL2 库
- 或使用 vcpkg 安装 SDL2 依赖

#### 3. 构建类型不匹配
确保使用 Release 配置构建和打包：
```bash
cmake --build build --config Release
cpack -C Release
```

#### 4. 图标文件错误
如果缺少图标文件，注释掉相关配置或提供正确的 .ico 文件。

#### 5. 中文编码和批处理脚本错误
```
安装包生成可能失败 was unexpected at this time.
```
**原因**：Windows 批处理脚本中的中文标点符号（如中文逗号"，"、中文句号"。"、中文冒号"："）可能导致语法错误。

**解决方法**：
1. 检查 `scripts/package.bat` 文件中是否使用了中文标点符号
2. 将所有中文标点替换为英文标点：
   - 中文逗号"，" → 英文逗号","
   - 中文句号"。" → 英文句号"."
   - 中文冒号"：" → 英文冒号":"
   - 中文问号"？" → 英文问号"?"
3. 确保 NSIS 脚本中避免使用中文字符串（已默认禁用自动 VC++ 安装功能）

**示例修复**：
```batch
:: 错误示例
echo [警告] 未找到 NSIS (makensis)，安装包生成可能失败。

:: 正确示例
echo [警告] 未找到 NSIS (makensis), 安装包生成可能失败.
```

### 调试信息

启用详细输出查看打包过程：
```bash
cpack -C Release --verbose
```

检查生成的 NSIS 脚本：
```bash
# 在构建目录中查找
find . -name "*.nsi"
```

## 自动化打包

### GitHub Actions 集成

项目已配置 GitHub Actions 工作流（`.github/workflows/cmake-windows.yml`），在推送代码时自动构建和测试。

要扩展为自动打包，可在工作流中添加：
```yaml
- name: 生成安装包
  run: |
    cd build
    cpack -C Release
```

### 本地脚本

项目已提供完整的打包脚本 `scripts/package.bat`，包含以下功能：

#### 脚本功能
1. **环境检查**：自动检查 CMake 和 NSIS 是否安装
2. **中文编码处理**：已修复中文标点符号问题，避免语法错误
3. **分步执行**：清晰的 5 步打包流程
4. **错误处理**：详细的错误信息和解决方案提示
5. **构建产物验证**：自动检查生成的安装包

#### 使用方式
```bash
# 直接运行打包脚本
scripts\package.bat

# 或手动执行各步骤
cmake -B build-package -S . -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build-package --config Release
cd build-package && cpack -C Release
```

#### 脚本特点
- 使用 `build-package` 目录避免与常规构建冲突
- 支持用户交互（如 NSIS 未安装时询问是否继续）
- 详细的进度报告和错误诊断
- 修复了中文编码问题，确保在中文 Windows 环境稳定运行

## 发布检查清单

在发布安装包前，请检查：

- [ ] 版本号正确
- [ ] 安装程序运行正常
- [ ] 游戏可以启动并运行
- [ ] 所有资源文件正确包含
- [ ] 开始菜单快捷方式正常工作
- [ ] 卸载程序完全移除文件
- [ ] 在不同 Windows 版本测试（Win10/11）
- [ ] 防病毒软件不误报
- [ ] 文件大小合理（预计：15-20MB）

## 相关文档

- [CMake CPack 文档](https://cmake.org/cmake/help/latest/module/CPack.html)
- [NSIS 用户手册](https://nsis.sourceforge.io/Docs/)
- [SDL2 官网](https://www.libsdl.org/)
- [Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## 许可证

打包脚本和配置遵循与项目相同的 MIT 许可证。SDL2 库遵循其各自的许可证。