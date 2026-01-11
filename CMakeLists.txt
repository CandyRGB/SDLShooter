# 标题
cmake_minimum_required(VERSION 3.10)
project(SDLShooter VERSION 0.1.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置编译选项
#if (MSVC)
#    add_compile_options(/EHsc)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic)
#endif()

# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release)

# 目标名称设置: SDLShooter-Windows
set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

# 引入库
# include_directories("C:/Users/23076/.cmake/SDL2/include")

# 链接库位置
# link_directories("C:/Users/23076/.cmake/SDL2/lib/x64")

# 如果库里有cmake文件，查找并载入Cmake预设
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories("include")

aux_source_directory(src SRC_LIST)

# 添加组件源文件
set(COMPONENT_SRC
    src/AngleCalculator.cpp
    src/HealthSystem.cpp
    src/SoundManager.cpp
    src/TextureManager.cpp
)

list(APPEND SRC_LIST ${COMPONENT_SRC})

# 添加可执行文件
add_executable(${TARGET} 
    ${SRC_LIST}
    resource.rc
)

# 链接库
target_link_libraries(${TARGET}
    ${SDL2_LIBRARIES}
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
)

# ============================================
# DLL 依赖处理
# ============================================

# 查找 SDL2 DLL 文件
find_file(SDL2_DLL
  NAMES SDL2.dll
  PATHS
    "C:/Users/23076/.cmake/SDL2-2.32.10/lib/x64"
    "$ENV{SDL2_DIR}/lib/x64"
    "$ENV{VCPKG_ROOT}/installed/x64-windows/bin"
  NO_DEFAULT_PATH
)

find_file(SDL2_IMAGE_DLL
  NAMES SDL2_image.dll
  PATHS
    "C:/Users/23076/.cmake/SDL2_image-2.8.8/lib/x64"
    "$ENV{SDL2_image_DIR}/lib/x64"
    "$ENV{VCPKG_ROOT}/installed/x64-windows/bin"
  NO_DEFAULT_PATH
)

find_file(SDL2_MIXER_DLL
  NAMES SDL2_mixer.dll
  PATHS
    "C:/Users/23076/.cmake/SDL2_mixer-2.8.1/lib/x64"
    "$ENV{SDL2_mixer_DIR}/lib/x64"
    "$ENV{VCPKG_ROOT}/installed/x64-windows/bin"
  NO_DEFAULT_PATH
)

find_file(SDL2_TTF_DLL
  NAMES SDL2_ttf.dll
  PATHS
    "C:/Users/23076/.cmake/SDL2_ttf-2.24.0/lib/x64"
    "$ENV{SDL2_ttf_DIR}/lib/x64"
    "$ENV{VCPKG_ROOT}/installed/x64-windows/bin"
  NO_DEFAULT_PATH
)

# 复制 DLL 到构建输出目录
if(SDL2_DLL AND SDL2_IMAGE_DLL AND SDL2_MIXER_DLL AND SDL2_TTF_DLL)
  add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SDL2_DLL}"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/SDL2.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SDL2_IMAGE_DLL}"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/SDL2_image.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SDL2_MIXER_DLL}"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/SDL2_mixer.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SDL2_TTF_DLL}"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/SDL2_ttf.dll"
    COMMENT "复制 SDL2 DLL 文件到输出目录"
  )

  # 为 Release 构建也复制
  add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SDL2_DLL}"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/SDL2.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SDL2_IMAGE_DLL}"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/SDL2_image.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SDL2_MIXER_DLL}"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/SDL2_mixer.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SDL2_TTF_DLL}"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/SDL2_ttf.dll"
    COMMENT "复制 SDL2 DLL 文件到 Release 输出目录"
  )

  # 安装 DLL 文件
  install(FILES
    "${SDL2_DLL}"
    "${SDL2_IMAGE_DLL}"
    "${SDL2_MIXER_DLL}"
    "${SDL2_TTF_DLL}"
    DESTINATION bin
    COMPONENT runtime
  )

  message(STATUS "SDL2 DLL 文件找到并配置完成")
else()
  message(WARNING "部分 SDL2 DLL 文件未找到，安装包可能缺少运行时库")
endif()

# ============================================
# CPack 配置 - 安装包生成
# ============================================

# 设置版本信息
set(CPACK_PACKAGE_NAME "SDL Shooter")
set(CPACK_PACKAGE_VENDOR "Tangchuan")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "2D太空射击游戏")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 0)

# 复制资源文件到构建目录
file(COPY "${CMAKE_SOURCE_DIR}/assets" 
     DESTINATION "${CMAKE_BINARY_DIR}")

# 定义开始菜单快捷方式图标（引用安装后的文件）
# set(CPACK_NSIS_INSTALLED_ICON_NAME "$INSTDIR\\\\assets\\\\image\\\\icon.ico")

# 使用 NSIS 生成器
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_PACKAGE_NAME "SDL Shooter")
set(CPACK_NSIS_DISPLAY_NAME "SDL Shooter - 2D太空射击游戏")
set(CPACK_NSIS_HELP_LINK "https://github.com/your-username/SDLShooter")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/your-username/SDLShooter")
set(CPACK_NSIS_CONTACT "tangchuan@example.com")
set(CPACK_NSIS_MODIFY_PATH ON)  # 允许用户添加程序到PATH

# 设置构建配置为 Release
set(CPACK_BUILD_CONFIG "Release")

# 自定义 NSIS 配置来处理图标文件
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/assets/image/icon.ico")
set(CPACK_NSIS_MUI_UNICON "${CMAKE_SOURCE_DIR}/assets/image/icon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\SDLShooter-Windows.exe")

# 安装目录设置
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SDL Shooter")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")

# 配置 Visual C++ Redistributable
# 设置 VC++ Redistributable 安装选项
# Note: 如果用户需要，可以手动安装 VC++ Redistributable
# 设置完成后显示运行选项
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "$INSTDIR\\bin\\SDLShooter-Windows.exe")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN_TEXT "运行 SDL Shooter")

# 可选：添加 VC++ Redistributable 安装（需要网络连接）
# 注意：由于网络依赖和可能的中文编码问题，暂时禁用自动安装
# 用户可以从微软官网手动安装：https://aka.ms/vs/17/release/vc_redist.x64.exe
# set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
#   ; 可选：下载并安装 Visual C++ Redistributable
#   ; 由于需要网络连接，这里只是示例代码
#   ; InitPluginsDir
#   ; SetOutPath '$PLUGINSDIR'
#   ; NSISdl::download https://aka.ms/vs/17/release/vc_redist.x64.exe '$PLUGINSDIR\\\\vc_redist.x64.exe'
#   ; Pop $0
#   ; StrCmp $0 success 0 +2
#   ;   MessageBox MB_OK 'Could not download Visual C++ Redistributable. Manual installation may be required.'
#   ; success:
#   ;   ExecWait '$PLUGINSDIR\\\\vc_redist.x64.exe /install /quiet /norestart'
# ")

# 创建开始菜单项
set(CPACK_NSIS_CREATE_ICONS_EXTRA "
  CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\SDL Shooter.lnk' '$INSTDIR\\\\bin\\\\SDLShooter-Windows.exe'
  CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Uninstall.lnk' '$INSTDIR\\\\uninstall.exe'
")

set(CPACK_NSIS_DELETE_ICONS_EXTRA "
  Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\SDL Shooter.lnk'
  Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Uninstall.lnk'
")

# 添加桌面快捷方式（可选）
# set(CPACK_NSIS_CREATE_ICONS_EXTRA "
#   CreateShortCut '$DESKTOP\\\\SDL Shooter.lnk' '$INSTDIR\\\\bin\\\\SDLShooter-Windows.exe'
# ")

# 设置安装组件
set(CPACK_COMPONENTS_ALL game runtime resources)

# 游戏可执行文件组件
install(TARGETS ${TARGET}
  RUNTIME DESTINATION bin
  COMPONENT game
)

# 资源文件安装
install(DIRECTORY assets/
  DESTINATION assets
  COMPONENT resources
  PATTERN ".DS_Store" EXCLUDE
  PATTERN "*.tmp" EXCLUDE
)

# 安装文档
install(FILES
  README.md
  GAME_RULES.md
  LICENSE.md
  DESTINATION docs
  COMPONENT resources
)

# 包含 CPack 模块
include(CPack)

# 为每个组件创建安装规则
cpack_add_component(game
  DISPLAY_NAME "游戏可执行文件"
  DESCRIPTION "SDL Shooter 主程序"
  REQUIRED
)

cpack_add_component(runtime
  DISPLAY_NAME "运行时库"
  DESCRIPTION "Visual C++ Redistributable 和 SDL2 库"
  REQUIRED
)

cpack_add_component(resources
  DISPLAY_NAME "游戏资源"
  DESCRIPTION "图像、音频、字体等游戏资源文件"
  REQUIRED
)