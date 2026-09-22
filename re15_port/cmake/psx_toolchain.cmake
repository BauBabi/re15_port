# =============================================================================
# PSn00bSDK Toolchain-Datei für CMake Cross-Compilation (MIPS I / PSX)
# =============================================================================
# Verwendung:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/psx_toolchain.cmake -DPSN00BSDK_PATH=/path/to/sdk ..
# =============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mipsel)

# PSn00bSDK Pfad muss gesetzt sein
if(NOT DEFINED PSN00BSDK_PATH)
    if(DEFINED ENV{PSN00BSDK_PATH})
        set(PSN00BSDK_PATH "$ENV{PSN00BSDK_PATH}")
    else()
        message(FATAL_ERROR
            "PSN00BSDK_PATH nicht definiert.\n"
            "Setze PSN00BSDK_PATH auf das PSn00bSDK-Installationsverzeichnis.\n"
            "Installationsanleitung: https://github.com/Lameguy64/PSn00bSDK"
        )
    endif()
endif()

# Compiler-Pfade (PSn00bSDK gcc-mipsel)
set(PSN00BSDK_TC "${PSN00BSDK_PATH}/bin")

# Auf Windows liegen die Werkzeuge als .exe. CMake prueft CMAKE_ASM_COMPILER anders als
# CMAKE_C_COMPILER: die C-Pruefung findet "mipsel-none-elf-gcc" noch (GNU 12.3.0 wird
# erkannt), die ASM-Pruefung verlangt einen VOLLEN Pfad auf eine EXISTIERENDE Datei und
# bricht mit "is not a full path to an existing compiler tool" ab. Deshalb wird die
# Endung hier bestimmt, statt sie zu raten. (Gemessen 2026-09-22 mit PSn00bSDK 0.24-win32
# unter C:/PSn00bSDK/sdk/PSn00bSDK-0.24-win32.)
set(_psx_exe "")
if(EXISTS "${PSN00BSDK_TC}/mipsel-none-elf-gcc.exe")
    set(_psx_exe ".exe")
endif()

set(CMAKE_C_COMPILER   "${PSN00BSDK_TC}/mipsel-none-elf-gcc${_psx_exe}")
set(CMAKE_ASM_COMPILER "${PSN00BSDK_TC}/mipsel-none-elf-gcc${_psx_exe}")
set(CMAKE_AR           "${PSN00BSDK_TC}/mipsel-none-elf-ar${_psx_exe}")
set(CMAKE_RANLIB       "${PSN00BSDK_TC}/mipsel-none-elf-ranlib${_psx_exe}")
set(CMAKE_LINKER       "${PSN00BSDK_TC}/mipsel-none-elf-ld${_psx_exe}")
set(CMAKE_OBJCOPY      "${PSN00BSDK_TC}/mipsel-none-elf-objcopy${_psx_exe}")

# PSX-spezifische Compiler-Flags
set(CMAKE_C_FLAGS_INIT "-march=r3000 -mabi=32 -mno-abicalls -fno-pic -msoft-float -fno-builtin")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g")

# Keine Standard-Bibliotheken (bare-metal)
set(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_C_STANDARD_LIBRARIES_INIT "")

# Suchpfade auf SDK beschränken
set(CMAKE_FIND_ROOT_PATH "${PSN00BSDK_PATH}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# PSn00bSDK Include- und Library-Pfade
set(PSN00BSDK_INCLUDE "${PSN00BSDK_PATH}/include")
set(PSN00BSDK_LIBDIR  "${PSN00BSDK_PATH}/lib")

# Plattform-Define für Compile-Time-Branching
add_definitions(-DRE15_PLATFORM_PSX)
