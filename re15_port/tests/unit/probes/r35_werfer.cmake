# Runde 35 Spur B — Werfer-Klasse (Granatwerfer 15/16/17, Raketenwerfer 18), Flammenwerfer 14,
# Colt Python 20 nach RE2 Retail (Beta -> Retail).
#
# Dossier: analysis/befunde_runde35/B_werfer.md; Code: engine/src/werfer_r35.c, include/re15_werfer.h,
# engine/src/re2_fx.c (Block "RUNDE 35 SPUR B"), re15_damage.c (Applier Zeilen 16/17, Arten 6..9).
#
#   unit_r35_werfer           Mechanik ohne exe: Magazine/Munition, Bank-/Clip-Umsetzung, GL-Runde
#                             (Op 17/22/15/47/48/49), Rakete (Op 23/24/47 Sub 13), Flammenstrahl (Op 70),
#                             Fuel-Takt, Python/Rakete/GL/Flamme gegen den Zombie.
#   integration_r35_werfer    echte exe (ROOM1140, RE15_DEBUG_JUMP): je Waffe RE2-Spawns im Rueckstoss-
#                             bild 1, Aufschlag-/Explosions-Toene, Gegner-HP, kein Haenger
#                             (tests/integration/test_r35_werfer.cmake, Muster test_r34_granaten.cmake).
add_executable(test_r35_werfer ${CMAKE_CURRENT_LIST_DIR}/../test_r35_werfer.c)
target_link_libraries(test_r35_werfer PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_werfer PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r35_werfer PRIVATE m)
endif()
add_test(NAME unit_r35_werfer COMMAND test_r35_werfer)
set_tests_properties(unit_r35_werfer PROPERTIES TIMEOUT 120)

if(TARGET re15_pc)
    add_test(NAME integration_r35_werfer
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_werfer_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_werfer.cmake)
    set_tests_properties(integration_r35_werfer PROPERTIES TIMEOUT 900)
endif()
