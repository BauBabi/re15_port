# =============================================================================
# Android-Target — libmain.so fuer SDLActivity (2026-09-19)
# =============================================================================
# Wird ueber re15_port/CMakeLists.txt (RE15_BUILD_ANDROID=ON, RE15_BUILD_PC=OFF)
# vom Gradle-Projekt platform/android/app/build.gradle konfiguriert (AGP ->
# externalNativeBuild -> NDK-Toolchain). Baut:
#   libSDL2.so  aus dem SDL2-2.28.5-Quellbaum (RE15_SDL2_SRC, Gradle-Task fetchSdl2)
#   libmain.so  = re15_engine (engine/src) + ALLE PC-Plattformquellen
#                 (platform/pc/src/*.c + platform/pc/main.c) + android_glue.c
# SDLActivity laedt "SDL2" und "main" (RE15Activity.getLibraries) und ruft in
# libmain.so das Symbol SDL_main — main.c benennt sein main() dafuer per
# <SDL_main.h> um (nur unter __ANDROID__).
# =============================================================================
cmake_minimum_required(VERSION 3.21)

if(NOT ANDROID)
    message(FATAL_ERROR "platform/android/jni ist nur mit der NDK-Toolchain baubar (ANDROID nicht gesetzt).")
endif()
if(NOT DEFINED RE15_SDL2_SRC OR NOT EXISTS "${RE15_SDL2_SRC}/CMakeLists.txt")
    message(FATAL_ERROR
        "RE15_SDL2_SRC zeigt nicht auf einen SDL2-Quellbaum: '${RE15_SDL2_SRC}'.\n"
        "Das Gradle-Projekt laedt ihn per Task fetchSdl2 nach platform/android/_deps/SDL2-2.28.5.")
endif()

# SDL2s hidapi (src/hidapi/android/hid.cpp) ist C++ — nur hier einschalten, das
# PC-Target bleibt reines C.
enable_language(CXX)

set(SDL_SHARED              ON  CACHE BOOL "" FORCE)
set(SDL_STATIC              OFF CACHE BOOL "" FORCE)
set(SDL_TEST                OFF CACHE BOOL "" FORCE)
set(SDL2_DISABLE_INSTALL    ON  CACHE BOOL "" FORCE)
set(SDL2_DISABLE_UNINSTALL  ON  CACHE BOOL "" FORCE)
set(SDL2_DISABLE_SDL2MAIN   ON  CACHE BOOL "" FORCE)   # Android braucht kein SDL2main (SDL_main wird per dlsym gefunden)
# Sensor-Subsystem AUS (2026-09-19, gemessen): SDL 2.28.5 src/sensor/android/SDL_androidsensor.c
# ruft ALooper_pollAll, das NDK r27 als entfernt markiert ("obsoleted in Android 1 ... Use
# ALooper_pollOnce") -> harter Compile-Fehler. SDL hat das erst in 2.30 umgestellt. Der Port
# braucht keinen Beschleunigungssensor (PSX-Digitalpad), also weg damit statt SDL zu patchen.
set(SDL_SENSOR              OFF CACHE BOOL "" FORCE)
add_subdirectory("${RE15_SDL2_SRC}" "${CMAKE_BINARY_DIR}/sdl2")

file(GLOB RE15_PC_PLATFORM_SOURCES "${CMAKE_SOURCE_DIR}/platform/pc/src/*.c")

add_library(main SHARED
    "${CMAKE_SOURCE_DIR}/platform/pc/main.c"
    ${RE15_PC_PLATFORM_SOURCES}
    android_glue.c)

target_compile_definitions(main PRIVATE
    RE15_PLATFORM_PC
    # Die einkompilierten Defaults zeigen auf den Bau-Host und existieren auf dem Geraet
    # nicht; die Wurzel-Aufloesung (asset_root_pc.c) findet die Assets ueber den
    # exe-Anker = App-Speicherordner (android_glue.c).
    RE15_ASSET_ROOT_DEFAULT="${RE15_ASSETS_PATH}"
    RE15_CD_ROOT_DEFAULT="${RE15_ASSETS_PATH}")

target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/platform/pc/src")

target_link_libraries(main PRIVATE re15_engine SDL2::SDL2 android log m)
