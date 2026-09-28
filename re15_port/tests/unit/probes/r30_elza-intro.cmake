# Messsonde Runde 30, Thema G: Elza-Intro (Lobby-Bild im Vorspann, Abbruch spielt die
# Montage noch einmal, Leon statt Elza in ROOM1031, Lobby-Szene startet nicht).
#
# Dossier: analysis/befunde_runde30/elza-intro.md
#
# Die MESSSONDE probe_r30_elza_intro hat KEIN add_test (sie beschreibt Zustaende, auch den
# defekten). Die zwei RIEGEL des Baus stehen am Ende dieser Datei (unit_r30_elza_selbsttuer,
# integration_elza_vollstart).
#
#   M1  ROOM1031 main00 mit flag(3,193)==0 -> sub12 -> Aot_on(18) -> zurueck nach ROOM1241
#       (ROOM1031.RDT @0x0204E Ck / @0x02052 Door_aot_set 18 / @0x02976 sub12)
#   M2  ROOM1031 mit flag(3,193)==1 -> Erzaehler sub15 -> Aot_on(19) = SELBST-Tuer
#       (@0x02082 Door_aot_set 19, Ziel Stage 0 / Raum 0x03, Cut 6) — steigt der Port neu ein?
#       Original: FUN_8001d600 `jal 0x800396fc` @0x8001d988 unbedingt, SCD-Raum-Init
#       `jal 0x8003ef6c` @0x80039a00.
#   M3  Gegenprobe: mit erzwungenem Neueinstieg laeuft main00 zum dritten Mal und startet
#       ueber Ck(3,207,1) @0x020AE die Lobby-Szene sub13.
#   M4  scd_vm_init() nullt work_vars[0x10] (DAT_800B0FF0, @0x8001d558 / @0x80039768).
add_executable(probe_r30_elza_intro probe_r30_elza-intro.c)
target_link_libraries(probe_r30_elza_intro PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_elza_intro PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_elza_intro PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# =============================================================================================
# RIEGEL (Bau Runde 30, Dossier §5.4). Die Messsonde oben bleibt ohne add_test; die zwei Riegel
# sind eigene Ziele und pruefen den GEBAUTEN Zustand (P1 main.c Vorlauf (3,193) auf
# RE15_ROOM_BASE, P2 main.c work_vars[0x10] nach scd_vm_init, P3 aot_common.c Selbst-Tuer mit
# Variantenziffer).
#
# (ii) Engine ohne Fenster: ROOM1031 mit flag(3,193)=1 -> Erzaehler sub15 -> Aot_on(19) @0x02A9E
#      -> Selbst-Tuer Slot 19 (@0x02082, Raum 0x03) muss neu einsteigen (@0x8001d988 jal
#      0x800396fc unbedingt; Datei = Basis + Elza-Bit @0x800397e4/@0x800397ec), danach laeuft
#      sub13 (@0x020B2) und schliesst (3,207)/(2,7) binnen 600 Bildern (gemessen 550).
add_executable(test_r30_elza_selbsttuer test_r30_elza_selbsttuer.c)
target_link_libraries(test_r30_elza_selbsttuer PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_elza_selbsttuer PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r30_elza_selbsttuer PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r30_elza_selbsttuer COMMAND test_r30_elza_selbsttuer)
set_tests_properties(unit_r30_elza_selbsttuer PROPERTIES TIMEOUT 60)

# (i) Integration, ECHTE exe: Titel -> NEW GAME -> Elza -> Montage ROOM1241 -> Abbruch mit
#     Quadrat im Tastenfenster -> ROOM1031. Prueft am debug.log: erster Hintergrund in 1031 =
#     Cut 13 (kein Lobby-Bild), ROOM1241 kein zweites Mal geladen, sub15 statt sub12, kein
#     PL00-Tausch, nach Tuer 19 startet sub13 und die Szene endet ("letterbox closed").
#     Laufzeit ~100 s (Zeitablauf des Spiels ist der erwartete Ausgang, geprueft wird das Log).
#     GUI-Haken: flattert unter parallelen Agenten (Memory reai-v2-gui-tests-flattern...) —
#     ein rotes Ergebnis EINZELN wiederholen (ctest -R integration_elza_vollstart).
if(TARGET re15_pc)
    add_test(NAME integration_elza_vollstart
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/elza_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_elza_vollstart.cmake)
    set_tests_properties(integration_elza_vollstart PROPERTIES TIMEOUT 240)
endif()
