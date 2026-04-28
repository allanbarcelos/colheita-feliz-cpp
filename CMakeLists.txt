cmake_minimum_required(VERSION 3.20)

project(ColheitaFeliz LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(nlohmann_json REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(game SHARED
    src/game.cpp
)

set_target_properties(game PROPERTIES
    PREFIX ""
    OUTPUT_NAME "game"
)

target_link_libraries(game PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    nlohmann_json::nlohmann_json
)

add_executable(ColheitaFeliz WIN32
    src/main.cpp
)

target_link_libraries(ColheitaFeliz PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    nlohmann_json::nlohmann_json
)

add_dependencies(ColheitaFeliz game)
