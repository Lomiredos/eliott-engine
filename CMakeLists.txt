cmake_minimum_required(VERSION 3.20)
project(eliott-engine VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_TOOLCHAIN_FILE "C:/Dev/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_PREFIX_PATH "C:/Dev/vcpkg/installed/x64-mingw-static")


set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)

add_subdirectory(extern/eliott-audio)
add_subdirectory(extern/eliott-ecs)
add_subdirectory(extern/eliott-input)
add_subdirectory(extern/eliott-math)
add_subdirectory(extern/eliott-physics)
add_subdirectory(extern/eliott-renderer)
add_subdirectory(extern/eliott-tmx-parser)

add_library(eliott-engine STATIC
    src/Engine.cpp
    src/Timer.cpp
)

target_include_directories(eliott-engine PUBLIC
    include/
)

target_link_libraries(eliott-engine PUBLIC
    SDL3::SDL3
    eliott-audio
    eliott-ecs
    eliott-input
    eliott-math
    eliott-physics
    eliott-renderer
    eliott-tmx-parser
)