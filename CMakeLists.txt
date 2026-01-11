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
)

# 链接库
target_link_libraries(${TARGET}
    ${SDL2_LIBRARIES}
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
)