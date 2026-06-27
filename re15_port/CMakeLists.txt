cmake_minimum_required(VERSION 3.21)
project(re15_port LANGUAGES C ASM)

# =============================================================================
# Build-Optionen
# =============================================================================
option(RE15_BUILD_PSX "Build PSX target via PSn00bSDK" OFF)
option(RE15_BUILD_PC  "Build PC target (SDL2/OpenGL)"  ON)

# =============================================================================
# CMake Module Path
# =============================================================================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# =============================================================================
# Asset-Pfad-Konfiguration (RE15_ASSETS_PATH)
# =============================================================================
# Resolve from environment variable if not set as CMake variable
if(NOT DEFINED RE15_ASSETS_PATH AND DEFINED ENV{RE15_ASSETS_PATH})
    set(RE15_ASSETS_PATH "$ENV{RE15_ASSETS_PATH}" CACHE PATH
        "Pfad zum extrahierten CD-Image (Original-Assets)")
endif()

# Default: das in-repo kanonische Asset-Verzeichnis (shared_assets/PSX). Alle
# Spiel-Assets liegen genau hier; nichts wird von ausserhalb gelesen.
if(NOT DEFINED RE15_ASSETS_PATH AND EXISTS "${CMAKE_SOURCE_DIR}/shared_assets/PSX")
    set(RE15_ASSETS_PATH "${CMAKE_SOURCE_DIR}/shared_assets/PSX" CACHE PATH
        "Pfad zum extrahierten CD-Image (Original-Assets)")
endif()

# Validate: Must be set
if(NOT DEFINED RE15_ASSETS_PATH OR RE15_ASSETS_PATH STREQUAL "")
    message(FATAL_ERROR
        "RE15_ASSETS_PATH nicht gesetzt oder ungültig.\n"
        "Setze den Pfad zum extrahierten CD-Image:\n"
        "  cmake -DRE15_ASSETS_PATH=/path/to/extracted_cd ..\n"
        "Alternativ als Umgebungsvariable:\n"
        "  export RE15_ASSETS_PATH=/path/to/extracted_cd"
    )
endif()

# Validate: Path must exist
if(NOT EXISTS "${RE15_ASSETS_PATH}")
    message(FATAL_ERROR
        "RE15_ASSETS_PATH nicht gesetzt oder ungültig.\n"
        "Der angegebene Pfad existiert nicht: ${RE15_ASSETS_PATH}\n"
        "Beispiel: cmake -DRE15_ASSETS_PATH=/path/to/extracted_cd .."
    )
endif()

message(STATUS "RE15_ASSETS_PATH: ${RE15_ASSETS_PATH}")

# =============================================================================
# Gemeinsame Engine-Library
# =============================================================================
add_subdirectory(engine)

# =============================================================================
# PSX-Target
# =============================================================================
if(RE15_BUILD_PSX)
    if(NOT DEFINED PSN00BSDK_PATH AND NOT DEFINED ENV{PSN00BSDK_PATH})
        message(FATAL_ERROR
            "PSn00bSDK nicht gefunden.\n"
            "Setze PSN00BSDK_PATH als CMake-Variable oder Umgebungsvariable.\n"
            "Installationsanleitung: https://github.com/Lameguy64/PSn00bSDK"
        )
    endif()

    # Resolve from environment if not set as CMake variable
    if(NOT DEFINED PSN00BSDK_PATH)
        set(PSN00BSDK_PATH "$ENV{PSN00BSDK_PATH}")
    endif()

    find_package(PSn00bSDK REQUIRED)
    add_subdirectory(platform/psx)
endif()

# =============================================================================
# PC-Target
# =============================================================================
if(RE15_BUILD_PC)
    add_subdirectory(platform/pc)
endif()

# =============================================================================
# Asset-Pipeline Tools (nur PC)
# =============================================================================
if(RE15_BUILD_PC)
    add_subdirectory(tools)
endif()

# =============================================================================
# Tests (nur PC, kein GPU/Devkit nötig)
# =============================================================================
if(RE15_BUILD_PC)
    enable_testing()
    add_subdirectory(tests)
endif()
