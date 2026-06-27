# =============================================================================
# PSX-Target — PSn00bSDK Cross-Compile (MIPS I)
# =============================================================================
# Konfiguration für das PSX-Target. Wird nur gebaut wenn RE15_BUILD_PSX=ON.
# Benötigt: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/psx_toolchain.cmake
#           -DPSN00BSDK_PATH=/path/to/sdk ..
# =============================================================================

# -----------------------------------------------------------------------------
# PSX Executable
# -----------------------------------------------------------------------------
file(GLOB PSX_SOURCES "src/*.c")
add_executable(re15_psx main.c ${PSX_SOURCES})

target_compile_definitions(re15_psx PRIVATE RE15_PLATFORM_PSX)

# PSn00bSDK Include-Pfade
target_include_directories(re15_psx PRIVATE
    "${PSN00BSDK_PATH}/include"
)

# PSn00bSDK Library-Pfade
target_link_directories(re15_psx PRIVATE
    "${PSN00BSDK_PATH}/lib"
)

# Linke gegen Engine und PSn00bSDK-Bibliotheken
target_link_libraries(re15_psx
    PRIVATE
        re15_engine
        psxgpu
        psxgte
        psxspu
        psxcd
        psxpad
        c
)

# -----------------------------------------------------------------------------
# Post-Build: ELF → PS-EXE Konvertierung
# -----------------------------------------------------------------------------
add_custom_command(TARGET re15_psx POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
        "$<TARGET_FILE:re15_psx>"
        "${CMAKE_CURRENT_BINARY_DIR}/RE15.EXE"
    COMMENT "Konvertiere ELF → PS-EXE (RE15.EXE)"
)

# -----------------------------------------------------------------------------
# ISO-Image Generierung (optional, via mkpsxiso)
# -----------------------------------------------------------------------------
find_program(MKPSXISO mkpsxiso)

if(MKPSXISO)
    add_custom_target(re15_iso
        COMMAND ${MKPSXISO} -y "${CMAKE_CURRENT_SOURCE_DIR}/iso.xml"
            -o "${CMAKE_CURRENT_BINARY_DIR}/RE15.bin"
            -c "${CMAKE_CURRENT_BINARY_DIR}/RE15.cue"
        DEPENDS re15_psx
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Erstelle PSX ISO-Image (RE15.bin/cue)"
    )
else()
    message(STATUS "mkpsxiso nicht gefunden — ISO-Image-Target deaktiviert")
    message(STATUS "  Installation: https://github.com/Lameguy64/mkpsxiso")
endif()
