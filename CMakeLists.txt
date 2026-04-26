cmake_minimum_required(VERSION 3.16)
project(UmbraDaciei VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build test suite" ON)

include(FetchContent)

set(SDL_WERROR OFF CACHE BOOL "" FORCE)
set(SDL_PIPEWIRE OFF CACHE BOOL "" FORCE)
set(SDL_PIPEWIRE_SHARED OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.2
)

FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-2.22.0
)

FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-2.8.2
)

FetchContent_Declare(
    SDL2_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG release-2.8.0
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(SDL2 SDL2_ttf SDL2_image SDL2_mixer nlohmann_json)

foreach(sdl2_target SDL2 SDL2-static SDL2main SDL2_test)
    if(TARGET ${sdl2_target})
        target_compile_options(${sdl2_target} PRIVATE -w)
    endif()
endforeach()

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})

set(HEADER_FILES
    src/core/Game.h
    src/core/Renderer.h
    src/core/AudioManager.h
    src/core/InputHandler.h
    src/core/SaveSystem.h
    src/systems/ConspiracyWeb.h
    src/systems/ParanoiaSystem.h
    src/systems/FolkloreEvents.h
    src/systems/GlitchRenderer.h
    src/scenes/SceneManager.h
    src/scenes/MainMenuScene.h
    src/scenes/ApartmentScene.h
    src/scenes/ForumScene.h
    src/scenes/FieldScene.h
    src/scenes/ShadowRealmScene.h
    src/scenes/EndingScene.h
    src/ui/ConspiracyBoard.h
    src/ui/Terminal.h
    src/ui/DialogueSystem.h
    src/ui/HUD.h
    src/entities/Player.h
    src/entities/NPC.h
    src/entities/Strigoi.h
    src/Config.h
)

set(SOURCE_FILES
    src/core/Game.cpp
    src/core/Renderer.cpp
    src/core/AudioManager.cpp
    src/core/InputHandler.cpp
    src/core/SaveSystem.cpp
    src/systems/ConspiracyWeb.cpp
    src/systems/ParanoiaSystem.cpp
    src/systems/FolkloreEvents.cpp
    src/systems/GlitchRenderer.cpp
    src/scenes/SceneManager.cpp
    src/scenes/MainMenuScene.cpp
    src/scenes/ApartmentScene.cpp
    src/scenes/ForumScene.cpp
    src/scenes/FieldScene.cpp
    src/scenes/ShadowRealmScene.cpp
    src/scenes/EndingScene.cpp
    src/ui/ConspiracyBoard.cpp
    src/ui/Terminal.cpp
    src/ui/DialogueSystem.cpp
    src/ui/HUD.cpp
    src/entities/Player.cpp
    src/entities/NPC.cpp
    src/entities/Strigoi.cpp
    src/main.cpp
)

add_executable(umbra_daciei ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(umbra_daciei
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    nlohmann_json::nlohmann_json
)

target_compile_definitions(umbra_daciei PRIVATE SDL_MAIN_HANDLED)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umbra_daciei PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(umbra_daciei PRIVATE /W4 /WX)
endif()

if(BUILD_TESTS)
    add_executable(test_conspiracy_web tests/test_conspiracy_web.cpp src/systems/ConspiracyWeb.cpp)
    target_link_libraries(test_conspiracy_web nlohmann_json::nlohmann_json pthread)
    enable_testing()
    add_test(NAME ConspiracyWebTest COMMAND test_conspiracy_web)

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_conspiracy_web PRIVATE -Wall -Wextra -Werror)
    elseif(MSVC)
        target_compile_options(test_conspiracy_web PRIVATE /W4 /WX)
    endif()
endif()