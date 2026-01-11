@echo off
setlocal enabledelayedexpansion

echo ========================================
echo SDL Shooter 安装包生成脚本
echo ========================================
echo.

:: 设置颜色
color 0A

:: 检查 CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [错误] "未找到 CMake, 请先安装 CMake 3.10+"
    echo 下载地址: https://cmake.org/download/
    pause
    exit /b 1
)

:: 检查 NSIS
where makensis >nul 2>nul
if %errorlevel% neq 0 (
    echo [警告] "未找到 NSIS (makensis), 安装包生成可能失败"
    echo 建议安装 NSIS: https://nsis.sourceforge.io/Download
    echo.
    set /p continue="是否继续? (y/N): "
    if /i not "!continue!"=="y" (
        exit /b 1
    )
)

:: 设置路径
set "SOURCE_DIR=%~dp0.."
set "BUILD_DIR=%SOURCE_DIR%\build-package"
set "PROJECT_NAME=SDL Shooter"
set "VERSION=0.1.0"

echo 项目目录: %SOURCE_DIR%
echo 构建目录: %BUILD_DIR%
echo.

:: 清理旧构建
echo [1/5] 清理旧构建文件...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo 已清理旧构建目录
)

:: 配置项目
echo [2/5] 配置 CMake 项目...
cmake -B "%BUILD_DIR%" -S "%SOURCE_DIR%" ^
    -G "Visual Studio 17 2022" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo [错误] CMake 配置失败
    pause
    exit /b 1
)

:: 构建项目
echo [3/5] "构建项目 (Release 配置)..."
cmake --build "%BUILD_DIR%" --config Release
if %errorlevel% neq 0 (
    echo [错误] 构建失败
    pause
    exit /b 1
)

:: 生成安装包
echo [4/5] 生成安装包...
cd "%BUILD_DIR%"
cpack -C Release
if %errorlevel% neq 0 (
    echo [错误] 安装包生成失败
    echo.
    echo 可能的原因:
    echo 1. NSIS 未安装或不在 PATH 中
    echo 2. SDL2 DLL 文件未找到
    echo 3. 构建目录权限问题
    echo.
    pause
    exit /b 1
)

:: 查找生成的安装包
echo [5/5] 查找生成的安装包...
cd "%BUILD_DIR%"
dir /b *.exe 2>nul >nul
if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo 安装包生成成功！
    echo ========================================
    echo.

    for /f "tokens=*" %%f in ('dir /b *.exe 2^>nul') do (
        echo 安装包: %BUILD_DIR%\%%f
        echo 文件大小:
        for /f "tokens=3" %%s in ('dir "%%f" ^| find "%%f"') do echo        %%s
    )

    echo.
    for /f "tokens=*" %%f in ('dir /b *.zip 2^>nul') do (
        echo 便携版: %BUILD_DIR%\%%f
    )
) else (
    echo [错误] 未找到生成的安装包
    dir /b *.*
)

echo.
echo 操作完成！
echo.
echo 安装包位置: %BUILD_DIR%\
echo.
echo 下一步:
echo 1. 运行安装包测试安装过程
echo 2. 检查游戏是否正常运行
echo 3. 测试卸载功能
echo.

pause