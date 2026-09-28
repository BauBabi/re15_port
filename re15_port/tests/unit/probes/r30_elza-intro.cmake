# Messsonde Runde 30, Thema G: Elza-Intro (Lobby-Bild im Vorspann, Abbruch spielt die
# Montage noch einmal, Leon statt Elza in ROOM1031, Lobby-Szene startet nicht).
#
# Dossier: analysis/befunde_runde30/elza-intro.md
#
# REINE MESSSONDE, KEIN add_test: die Sonde beschreibt den heutigen (defekten) Zustand und
# wuerde als Haken die Suite rot machen. Der Bau-Agent macht daraus den Riegel, sobald die
# drei Stellen stehen (Plan im Dossier, Abschnitt 5).
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
