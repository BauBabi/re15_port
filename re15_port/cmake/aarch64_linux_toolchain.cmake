# =============================================================================
# CMake-Toolchain: Linux/aarch64 (ARM64) — Cross-Compile von x86_64 aus
# =============================================================================
# Gegenstueck zu cmake/psx_toolchain.cmake (MIPS). Erwartet das Debian-Paket
#   gcc-aarch64-linux-gnu  (+ die :arm64-Multiarch-Dev-Pakete fuer SDL2)
# Tests laufen ueber qemu-user; CMAKE_CROSSCOMPILING_EMULATOR sorgt dafuer,
# dass ctest die aarch64-Binaries direkt starten kann.
# =============================================================================
set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER   aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_AR           aarch64-linux-gnu-ar)
set(CMAKE_RANLIB       aarch64-linux-gnu-ranlib)
set(CMAKE_STRIP        aarch64-linux-gnu-strip)

# Multiarch: Header liegen in /usr/include(+/aarch64-linux-gnu),
# Libs in /usr/lib/aarch64-linux-gnu -> BOTH, nicht ONLY.
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

if(EXISTS /usr/bin/qemu-aarch64-static)
    set(CMAKE_CROSSCOMPILING_EMULATOR /usr/bin/qemu-aarch64-static)
elseif(EXISTS /usr/bin/qemu-aarch64)
    set(CMAKE_CROSSCOMPILING_EMULATOR /usr/bin/qemu-aarch64)
endif()
