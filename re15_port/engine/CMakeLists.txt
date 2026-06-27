# =============================================================================
# Engine-Library — Gemeinsamer Code für PSX und PC
# =============================================================================
# Neue .c-Dateien in engine/src/ werden automatisch beim nächsten
# CMake-Configure erfasst (GLOB-Mechanismus, Requirement 1.4).
# =============================================================================

file(GLOB ENGINE_SOURCES "src/*.c")

add_library(re15_engine STATIC ${ENGINE_SOURCES})
target_include_directories(re15_engine PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Pass RE15_ASSETS_PATH as compile-time define if set
if(DEFINED RE15_ASSETS_PATH AND NOT RE15_ASSETS_PATH STREQUAL "")
    target_compile_definitions(re15_engine PUBLIC
        RE15_ASSETS_PATH=${RE15_ASSETS_PATH}
    )
endif()
