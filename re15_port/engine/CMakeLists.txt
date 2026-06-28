# =============================================================================
# Engine-Library — Gemeinsamer Code für PSX und PC
# =============================================================================
# Neue .c-Dateien in engine/src/ werden automatisch beim nächsten
# CMake-Configure erfasst (GLOB-Mechanismus, Requirement 1.4).
# =============================================================================

file(GLOB ENGINE_SOURCES "src/*.c")

add_library(re15_engine STATIC ${ENGINE_SOURCES})
target_include_directories(re15_engine PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Plattform-Definition AN die Engine-Lib geben. Die psx_dev-Engine-Quellen
# schalten plattformspezifische Includes/Pfade per RE15_PLATFORM_PC/_PSX
# (z.B. <stdio.h>-Debug-Ausgaben nur auf PC). Da die Engine hier eine separate
# Lib ist (nicht flach ins Target kompiliert wie in psx_dev), muss die Definition
# explizit gesetzt werden — passend zum aktiven Target. PUBLIC, damit sie auch an
# die Plattform-Backends propagiert, die die Lib linken.
if(RE15_BUILD_PC)
    target_compile_definitions(re15_engine PUBLIC RE15_PLATFORM_PC=1)
endif()
if(RE15_BUILD_PSX)
    target_compile_definitions(re15_engine PUBLIC RE15_PLATFORM_PSX=1)
endif()

# Pass RE15_ASSETS_PATH as compile-time define if set
if(DEFINED RE15_ASSETS_PATH AND NOT RE15_ASSETS_PATH STREQUAL "")
    target_compile_definitions(re15_engine PUBLIC
        RE15_ASSETS_PATH=${RE15_ASSETS_PATH}
    )
endif()
