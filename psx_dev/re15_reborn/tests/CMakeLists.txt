# RE1.5 Rebuilt — Unit tests (host PC build, no PSX emulator needed)
#
# Tests cover the platform-agnostic code (tim_common, scd_vm) so we have
# fast feedback on every subsystem change.
#
# Run:
#   cd psx_dev/re15_reborn/tests && mkdir build && cd build
#   cmake .. && cmake --build . --config Release
#   ctest -C Release --output-on-failure

cmake_minimum_required(VERSION 3.21)

project(
    re15_reborn_tests
    LANGUAGES C
    VERSION 0.1.0
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Tests on PC — define RE15_PLATFORM_PC just like the PC build, so any
# platform-conditional code follows the same path tests verify.
add_definitions(-DRE15_PLATFORM_PC=1 -DRE15_TESTING=1)

# Shared engine sources (platform-agnostic)
set(SHARED_DIR ${CMAKE_SOURCE_DIR}/..)

enable_testing()

# -- test_tim ----------------------------------------------------------------
add_executable(test_tim
    test_tim.c
    ${SHARED_DIR}/src/tim_common.c
)
target_include_directories(test_tim PRIVATE ${SHARED_DIR}/include)
add_test(NAME tim_parser COMMAND test_tim)

# -- test_scd ----------------------------------------------------------------
# Stub out re15_debug_text since scd_vm.c calls it via re15_engine.h
add_executable(test_scd
    test_scd.c
    test_stubs.c
    ${SHARED_DIR}/src/scd_vm.c
)
target_include_directories(test_scd PRIVATE ${SHARED_DIR}/include)
add_test(NAME scd_vm COMMAND test_scd)

# -- test_md1 ----------------------------------------------------------------
add_executable(test_md1
    test_md1.c
    ${SHARED_DIR}/src/md1_common.c
)
target_include_directories(test_md1 PRIVATE ${SHARED_DIR}/include)
add_test(NAME md1_parser COMMAND test_md1)

# -- test_backface (Phase 4.5.4) ---------------------------------------------
# Pure math test, no engine deps. Verifies the IS_BACKFACE convention used
# in re15_reborn_pc/main.c matches the PSX gte_nclip sign convention.
add_executable(test_backface test_backface.c)
add_test(NAME backface_culling COMMAND test_backface)

# -- test_bss (Phase 4.5.6.1 + 4.5.6.2) -------------------------------------
# BSS chunk-header parser + VLC decoder. Real-asset integration test: reads
# the bundled assets/test.bss (ROOM11000.bs first chunk) and verifies the
# decoder produces the expected coefficient count + preamble. Path is
# baked in at configure time so the test runs from any CTest working dir.
add_executable(test_bss
    test_bss.c
    ${SHARED_DIR}/src/bss_common.c
    ${SHARED_DIR}/src/bss_vlc.c
    ${SHARED_DIR}/src/bss_mdec.c
)
target_include_directories(test_bss PRIVATE ${SHARED_DIR}/include)
target_compile_definitions(test_bss PRIVATE
    RE15_TEST_BSS_PATH="${SHARED_DIR}/assets/test.bss"
)
add_test(NAME bss_parser COMMAND test_bss)

# -- test_vab (Phase 4.6.2) -------------------------------------------------
# VAB header parser + PSX ADPCM decoder. Reads bundled DOOR00 VH+VB.
add_executable(test_vab
    test_vab.c
    ${SHARED_DIR}/src/vab_common.c
)
target_include_directories(test_vab PRIVATE ${SHARED_DIR}/include)
target_compile_definitions(test_vab PRIVATE
    RE15_TEST_VH_PATH="${SHARED_DIR}/assets/test.vh"
    RE15_TEST_VB_PATH="${SHARED_DIR}/assets/test.vb"
)
add_test(NAME vab_parser COMMAND test_vab)

# -- test_emd (Phase 4.5.7.1) ------------------------------------------------
# EDD animation table + EMR skeleton+keyframes parser. Real-asset
# integration test with bundled PL00.edd + PL00.emr (Leon player model).
add_executable(test_emd
    test_emd.c
    ${SHARED_DIR}/src/emd_common.c
)
target_include_directories(test_emd PRIVATE ${SHARED_DIR}/include)
target_compile_definitions(test_emd PRIVATE
    RE15_TEST_EDD_PATH="${SHARED_DIR}/assets/test.edd"
    RE15_TEST_EMR_PATH="${SHARED_DIR}/assets/test.emr"
)
add_test(NAME emd_parser COMMAND test_emd)

# -- test_skeleton (Phase 4.5.7.2) -------------------------------------------
# Skeleton pose math: trig identities, identity-rotation skeleton,
# parent-rotation-rotates-child-offset case. Uses the PC trig backend.
add_executable(test_skeleton
    test_skeleton.c
    ${SHARED_DIR}/src/skeleton_common.c
    ${SHARED_DIR}/src/emd_common.c
    ${CMAKE_SOURCE_DIR}/../../re15_reborn_pc/src/skeleton_trig_pc.c
)
target_include_directories(test_skeleton PRIVATE ${SHARED_DIR}/include)
if(MSVC)
    # MSVC links math statically.
else()
    target_link_libraries(test_skeleton PRIVATE m)
endif()
add_test(NAME skeleton_pose COMMAND test_skeleton)
