# Runde 26: PIN fuer die RE1.5-Kaefig-Freigabe (ROOM11D0/11D1/3060/3061) — Zustandswort 0x0C01
# @0x8011172c landet in der RE1.5-ACT[12] 0x801101E4 (Clip 8, effektfrei) statt im
# RE2-Post-Latch @0x80102608, und die Fehlstelle C1 (@0x80100600-08) raeumt den Pose-Riegel ab.
add_executable(test_r26_kaefig_release0c ${CMAKE_CURRENT_SOURCE_DIR}/test_r26_kaefig_release0c.c)
target_link_libraries(test_r26_kaefig_release0c PRIVATE re15_engine re15_test_support)
target_include_directories(test_r26_kaefig_release0c PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r26_kaefig_release0c PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME test_r26_kaefig_release0c COMMAND test_r26_kaefig_release0c)
set_tests_properties(test_r26_kaefig_release0c PROPERTIES TIMEOUT 30)
