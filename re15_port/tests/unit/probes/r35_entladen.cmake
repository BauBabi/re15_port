# Runde 35 / Spur I — Entladen aller Raum-Assets (Raumwechsel, Spielstart, Tod).
# Dossier: analysis/befunde_runde35/I_entladen.md, Schnittstelle: include/re15_entladen.h
#
#   unit_r35_entladen_beleg    Original-Bytes in info/Re1.5/PSX.EXE: Arena-Reset @0x80039738/40,
#                              Spielmodul-Init @0x8001d5a0, genau zwei jal 0x800396fc
#                              (@0x8001d5ac/@0x8001d988), genau ein jal 0x80039590 (@0x8001ce54),
#                              jal 0x80039270 @0x800399cc, jal 0x8001b3f8 @0x80039a08
#   unit_r35_entladen_gegner   Generation je Gegnerbank: fremd nach der Grenze, leer nach Reset
#   probe_r35_entladen_karte   WERKZEUG (kein add_test): Speicherkarte mit Spielstand am
#                              Raum-Eintritt (fuer Lauf C)
#   integration_r35_entladen_{a,b,c,d}   echte exe (test_r35_entladen.cmake):
#       a  Tod -> NEW GAME          b  Raumwechsel 1020 -> 1030
#       c  Tod -> LOAD im Todesraum d  Cinematic-Bank 1170 ueber den Tod (240 Bilder/s)
add_executable(test_r35_entladen ${CMAKE_CURRENT_LIST_DIR}/../test_r35_entladen.c)
target_link_libraries(test_r35_entladen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_entladen PRIVATE ${CMAKE_SOURCE_DIR}/include)
get_filename_component(_r35i_repo_root "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
target_compile_definitions(test_r35_entladen PRIVATE RE15_REPO_ROOT="${_r35i_repo_root}")
if(NOT WIN32)
    target_link_libraries(test_r35_entladen PRIVATE m)
endif()
foreach(_r35i_teil beleg gegner)
    add_test(NAME unit_r35_entladen_${_r35i_teil} COMMAND test_r35_entladen ${_r35i_teil})
    set_tests_properties(unit_r35_entladen_${_r35i_teil} PROPERTIES TIMEOUT 60)
endforeach()

add_executable(probe_r35_entladen_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r35_entladen_karte.c)
target_link_libraries(probe_r35_entladen_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_entladen_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r35_entladen_karte PRIVATE m)
endif()

if(TARGET re15_pc)
    foreach(_r35i_lauf A B C D)
        string(TOLOWER "${_r35i_lauf}" _r35i_klein)
        add_test(NAME integration_r35_entladen_${_r35i_klein}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r35_entladen_karte>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_entladen_wd
                         -DTEIL=${_r35i_lauf}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_entladen.cmake)
        set_tests_properties(integration_r35_entladen_${_r35i_klein} PROPERTIES TIMEOUT 600)
    endforeach()
endif()
