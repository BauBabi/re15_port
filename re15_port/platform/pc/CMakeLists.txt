# =============================================================================
# PC-Target — SDL2/OpenGL via FetchContent
# =============================================================================
# SDL2 wird automatisch via FetchContent heruntergeladen (keine manuelle
# Installation auf Windows nötig). Auf Linux: libSDL2-dev, libGL, libX11
# müssen installiert sein.
# =============================================================================

include(FetchContent)

# -----------------------------------------------------------------------------
# SDL2 via FetchContent (Version 2.28.5)
# -----------------------------------------------------------------------------
FetchContent_Declare(
    SDL2
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.tar.gz
    URL_HASH SHA256=332cb37d0be20cb9541739c61f79bae5a477427d79ae85e352089afdaf6666e4
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(SDL2)

# Prüfe ob SDL2 erfolgreich verfügbar ist
if(NOT TARGET SDL2::SDL2)
    message(FATAL_ERROR
        "SDL2 konnte nicht heruntergeladen oder konfiguriert werden.\n"
        "Mögliche Ursachen:\n"
        "  - Keine Netzwerkverbindung (FetchContent benötigt Download)\n"
        "  - GitHub nicht erreichbar\n"
        "Abhängigkeit: SDL2 2.28.5\n"
        "Manuelle Alternative: SDL2-Entwicklungsbibliothek installieren\n"
        "  Windows: vcpkg install sdl2\n"
        "  Linux:   sudo apt install libsdl2-dev"
    )
endif()

# -----------------------------------------------------------------------------
# Plattform-spezifische Quellen (src/)
# -----------------------------------------------------------------------------
file(GLOB PC_PLATFORM_SOURCES "src/*.c")

# -----------------------------------------------------------------------------
# PC-Executable
# -----------------------------------------------------------------------------
add_executable(re15_pc main.c ${PC_PLATFORM_SOURCES})

target_compile_definitions(re15_pc PRIVATE RE15_PLATFORM_PC)

target_include_directories(re15_pc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(re15_pc
    PRIVATE
        re15_engine
        SDL2::SDL2main
        SDL2::SDL2
)
