# Runde 39 (2026-10-18), Spur K "heliport1171" — Johns Animationen in Elzas Dachszene ROOM1171.
# NUTZER: "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink
# Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen."
# Ursache: lazy RBJ-Kanal-Cache (engine/src/enemy_common.c, 2 Plaetze) von den Ereignis-Kraehen belegt -> John posierte
# seine Sub-0-Gesten aus Bank 0 (Clip 2 = Kletter-Clip +0x94 = 2 @0x800526a0) statt aus RBJ rec1 (FUN_8001b3f8 bindet
# jedes Marker-Bit ohne Grenze @0x8001b470-c4; Sub 0 spielt +0x180/+0x184 @0x80050d40/48).
# Dossier analysis/befunde_runde39/K_heliport1171.md.
#
# RIEGEL (Unit, echte RDTs/Archive, Bilder in main.c-Reihenfolge + RBJ-Abfrage des Renderers):
#   unit_r39_heliport1171_banken    Johns Baenke + ROOM1171-RBJ (rec1 Marker 0x2 {20,30,96})
#   unit_r39_heliport1171_binder    Slots ohne Record verdraengen den Record nicht (1171 John, 10D0 Marvin, 1170 Elliot aus)
#   unit_r39_heliport1171_szene     Johns Kanal + Clip-Laenge je Plc_motion-Schritt = Original, "Elza, hurry up!" = 96
#   unit_r39_heliport1171_leon1170  Gegenprobe Leon ROOM1170: keine Bindung ueber den Cache (wie vorher)
# "Auch sonst" (Dossier Abschnitt 6 — jede Szenen-Animation Johns gegen das Original):
#   unit_r39_heliport1171_initpose  K2 INIT-Pose = Loco-Paar Clip 2 Bild 0 bis zum ersten Plc_motion (@0x8011debc-c4)
#   unit_r39_heliport1171_eskorte   K2-Gegenprobe: Eskorte-EXEC ohne Gegner posiert ab dem 1. Takt die eigene Bank
#   unit_r39_heliport1171_saatbild  K3 Plc_motion-Saattakt posiert Bild 0 / fc-1 (@0x80050cec-d4c), Halten ab fc-1
#   unit_r39_heliport1171_schub     K4 Schub-Bit-Takt posiert noch Clip 0x12 (@0x80052af4 vor @0x80052b44)
# NACHBESSERUNG 1 (Abnahme 0, M1-M3; Dossier Abschnitt 12):
#   unit_r39_heliport1171_elza      M1 Elzas Sub 6 0x800517f0: Clip 1 Bild 0..15 ab Phase 0 (f314 @0x8005188c), M3a kein
#                                   Sub-6-Bild zwischen zwei Plc_dest (Dispatcher @0x8001ce0c nach dem SCD @0x8001cdec),
#                                   Ankunftsbild = Gang-/Dreh-Pose, M2 Spieler-Schub Bit-Takt Clip 0x12 / Phase 6 / Phase 7
#   unit_r39_heliport1171_kraehen   M3b Ereignis-Kraehen zeichnen das zuletzt posierte Bild (0x80115d94 posiert nicht):
#                                   INIT Bild 0, ORIENT Clip 8 ab Bild 0, Landebild = Fall-Pose, FINISH 0..11, Leiche Bild 11
# NACHBESSERUNG 2 (Abnahme 1, M1/M2 + eigener Befund N1; Dossier Abschnitt 13):
#   unit_r39_heliport1171_flug      M1 HOVER 0x80115130 Phase 0 endet den Takt (`j 0x801152cc` @0x801151d8): Bild, Lage, Gier
#                                   stehen, Zyklus 11; M2 SPIN 0x801153ac Gier +60 nur Phase 0/1 (@0x80115460-6c)
#   unit_r39_heliport1171_wand      N1 Wand-Pass FUN_8003b0a4 der Kraehen-Wurzel nach JEDEM Zustand (@0x80112204): die
#                                   Ereignis-Kraehe aus der Westwand-Zelle wird im INIT-Takt hinausgeschoben
add_executable(test_r39_heliport1171 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_heliport1171.c)
target_link_libraries(test_r39_heliport1171 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_heliport1171 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_heliport1171 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_heliport1171 PRIVATE m)
endif()
foreach(_teil banken binder szene leon1170 initpose eskorte saatbild schub elza kraehen flug wand)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_heliport1171_${_teil}_wd)
    add_test(NAME unit_r39_heliport1171_${_teil} COMMAND test_r39_heliport1171 ${_teil})
    set_tests_properties(unit_r39_heliport1171_${_teil} PROPERTIES TIMEOUT 300
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_heliport1171_${_teil}_wd)
endforeach()

# INTEGRATION (echte exe, Elza): der Pose-Strom RE15_ANIM_TRACE (anim_select_common.c = die Stelle, an der der Renderer
# die Pose aufloest) traegt die Bildzahl des GERENDERTEN Clips. Johns Gesten muessen aus RBJ rec1 kommen.
#   weg    echter Weg: Spielstand ROOM1131 (Elza, (3,0xC2)=1) vor der Flurtuer Slot 3, Aktionstaste -> ROOM1171, Selbst-Tuer
#          Slot 5 (RE15_FIRE_AOT, derselbe Tuerpfad wie die Aktionstaste) -> Szene -> ROOM10B1
#   laden  Spielstand direkt in ROOM1171 + CONTINUE (Lade-Weg)
foreach(_teil weg laden)
    add_test(NAME integration_r39_heliport1171_${_teil}
        COMMAND ${CMAKE_COMMAND}
            -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
            -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_elza_karte>
            -DWORKDIR=${CMAKE_BINARY_DIR}/r39_heliport1171_wd
            -DTEIL=${_teil}
            -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_heliport1171.cmake)
    set_tests_properties(integration_r39_heliport1171_${_teil} PROPERTIES TIMEOUT 900)
endforeach()
