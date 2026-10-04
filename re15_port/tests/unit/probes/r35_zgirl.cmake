# Runde 35 Spur C — Zombie-Maedchen (Typ 0x13, EM013), einziger Raum ROOM4050/4051.
# Dossier: analysis/befunde_runde35/C_zgirl.md
#
#   unit_r35_zgirl_zensus    T1 main00 je Eintritts-Cut 0..14: nur Cut 9/14 -> 0x13 an der RDT-Lage
#                            (@0x01eb4 / @0x01f5c), Cut 6/11 -> 0x18, sonst nichts (4050 UND 4051)
#   unit_r35_zgirl_killflag  T2 Kill-Flag 0xa0/0x7d in Zone 8 unterdrueckt den Spawn (@0x80042120-38)
#   unit_r35_zgirl_tuer      T3 Selbst-Tuer 6/7 -> Szenario 9/14 -> Wiedereintritt spawnt sie
#                            (@0x8001d968/@0x8001d988), Kreuz-Raum-Tuer 0 unveraendert, Stage-1-Gegenprobe
#   unit_r35_zgirl_ki_re15   T4 RE1.5-KI: INIT-HP 50..81, Annaeherung, Griff -10/-5, Ansprung nie scharf
#   unit_r35_zgirl_ki_re2    T5 RE2-KI (Spiel-Default): Annaeherung, Griff, Schaden
#   unit_r35_zgirl_tod       T6 Tod -> Leiche -> Kill-Flag -> kein Wiederauftauchen
#   unit_r35_zgirl_messer    T7 Messer (Original-Zeile 0 = jalr 0) -> Standard-Zurueckzucken, kein Haenger
#   integration_r35_zgirl    echte exe: Aktionstaste an Tuer 6 (Cut 9) und Tuer 7 (Cut 14) -> Spawn,
#                            Annaeherung, Griff (tests/integration/test_r35_zgirl.cmake)
add_executable(test_r35_zgirl ${CMAKE_CURRENT_LIST_DIR}/../test_r35_zgirl.c)
target_link_libraries(test_r35_zgirl PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_zgirl PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r35_zgirl PRIVATE m)
endif()
foreach(_r35c_teil zensus killflag tuer ki_re15 ki_re2 tod messer selbsttueren)
    add_test(NAME unit_r35_zgirl_${_r35c_teil} COMMAND test_r35_zgirl ${_r35c_teil})
    set_tests_properties(unit_r35_zgirl_${_r35c_teil} PROPERTIES TIMEOUT 120)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r35_zgirl
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_zgirl_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_zgirl.cmake)
    set_tests_properties(integration_r35_zgirl PROPERTIES TIMEOUT 900)
endif()
