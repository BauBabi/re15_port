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
#                                      + Elliot Raum-Modell (Nachbesserung 1, M2)
#       e  Raum-Stimmen 1240 -> 1170 mit Ton (SDL_AUDIODRIVER=dummy; Nachbesserung 1, M1)
#   unit_r35_entladen_n1beleg  Nachbesserung 1: Sce_em_set-Modell in der Arena (@0x80042328),
#                              Spieler fest 0x801bd814 (@0x800314c8/cc), Arena-Basis-Schreiber
#                              @0x80039a58; RE2 Raumlader @0x8004a1c4 -> Setmode 0xA0 (XA aus)
#   unit_r35_entladen_n2beleg  Nachbesserung 2: Animationsblock = RDT+0x5C (@0x8001b3fc/@0x8001b404,
#                              Binder nur @0x80039a08); RE2 ENEMSE-Bank je Raum (@0x8004a33c -> @0x8005a108)
#   integration_r35_entladen_{f,g,h,i}  Nachbesserung 2: f Tuer 1170 -> 1130 (RBJ-Datei + bg_prev),
#       g Leihe Spur K 10F0 -> 1030, h Boot-Puffer (Karte 1170), i RE2-Raumbank TUERSE (Dummy-Ton),
#       j RE2-ENEMSE-Bank (Kraehe ROOM10C0) am Tod (Dummy-Ton)
#   integration_r35_entladen_{k,l}  Nachbesserung 3: Generator-Lampen ROOM11F0 (k Tod, l Tuer 11F0 -> 11E0)
#   unit_r35_entladen_n3beleg  Nachbesserung 3: RE2 Raum-ESP-TIM je Raumladen (@0x8004a2ec, @0x8001bc80/84/88)
add_executable(test_r35_entladen ${CMAKE_CURRENT_LIST_DIR}/../test_r35_entladen.c)
target_link_libraries(test_r35_entladen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_entladen PRIVATE ${CMAKE_SOURCE_DIR}/include)
get_filename_component(_r35i_repo_root "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
target_compile_definitions(test_r35_entladen PRIVATE RE15_REPO_ROOT="${_r35i_repo_root}")
if(NOT WIN32)
    target_link_libraries(test_r35_entladen PRIVATE m)
endif()
foreach(_r35i_teil beleg gegner n1beleg n2beleg n3beleg)
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
    foreach(_r35i_lauf A B C D E F G H I J K L)
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
