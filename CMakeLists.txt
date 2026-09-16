cmake_minimum_required(VERSION 3.20)

project(FazendaDosSonhos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan (non-MSVC)" OFF)
if(ENABLE_SANITIZERS AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

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

# NOVO (fase 11): Windows resource script — embeda app_icon.ico no exe.
# Em Linux/macOS APP_RC_FILE fica vazia, add_executable ignora silently.
if(WIN32)
    set(APP_RC_FILE src/app.rc)
endif()

add_executable(FazendaDosSonhos WIN32
    src/main.cpp
    ${APP_RC_FILE}
)

target_link_libraries(FazendaDosSonhos PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    nlohmann_json::nlohmann_json
)

# dlopen/dlsym/dlclose estão em libdl no Linux; no macOS já estão no SDK.
if(UNIX)
    target_link_libraries(FazendaDosSonhos PRIVATE ${CMAKE_DL_LIBS})
endif()

add_dependencies(FazendaDosSonhos game)

enable_testing()
add_executable(test_logic tests/test_logic.cpp)
target_include_directories(test_logic PRIVATE src)
target_compile_definitions(test_logic PRIVATE SDL_MAIN_HANDLED)
target_link_libraries(test_logic PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    nlohmann_json::nlohmann_json
)
# SDL2::SDL2 no MinGW injeta -mwindows (subsistema GUI → WinMain).
# O teste é um main() de console; força -mconsole depois das libs SDL.
set_target_properties(test_logic PROPERTIES WIN32_EXECUTABLE FALSE)
if(MINGW)
    target_link_options(test_logic PRIVATE -mconsole)
endif()
add_test(NAME logic COMMAND test_logic)
