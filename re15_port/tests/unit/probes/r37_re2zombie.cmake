# Runde 37 (2026-10-12), Spur L "re2zombie" — RE2-Zombie-KI: die drei 0x201-Produzenten (Arme-hoch-Gang EXEC[2]),
# DECISION[0] Block 3, EXEC[0] P1. Dossier analysis/befunde_runde37/L_re2zombie.md, Schalter include/re15_re2zombie_r37.h.
add_executable(test_r37_re2zombie ${CMAKE_CURRENT_LIST_DIR}/../test_r37_re2zombie.c)
target_link_libraries(test_r37_re2zombie PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_re2zombie PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_re2zombie PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r37_re2zombie PRIVATE m)
endif()
