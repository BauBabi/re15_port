# Runde 41 Spur B "re2gator" — RE2-Alligator (EM23) im ROOM2090-Boss. Dossier analysis/befunde_runde41/B_re2gator.md,
# Code engine/src/gator_re2_r41.c + include/re15_gator_re2_r41.h (Haken: enemy_ai_boss_gator.c, platform/pc/main.c,
# platform/pc/src/gator_re2_pc.c), Daten shared_assets/RE2/KANISTER40A0.{MD1,TIM} + GATOR40A0.VBS.
#
#   unit_r41_re2gator_clips      Verfolgung: Clip 1 (Paddeln) bei +0x1F0 >= 0x65 ohne Biss-Sperre, Clip 0 (Schwimmen)
#                                in der Biss-Sperre (+0x238 = 70/110); Vortrieb = authored Wurzelversatz off[f]-off[f-1]
#                                x Scale (FUN_80015e7c); Clip-1-Wrap nach 26 Bildern (@0x80100d08-10)
#   unit_r41_re2gator_dash       Zonenwechsel in Leons Zone -> SE 4 (@0x80100d64/84), Clip 2 (150) -> Clip 3 (150) ueber
#                                die authored Wurzelbahn, danach CHASE; Stroke-Woerter 0x28 (Clip 2 f50/f110) ->
#                                Ereigniskette: Rumble-Rampe (10,120,50), Bit 4 fuer 5 Bilder (Tabelle @0x8009db14),
#                                dann Bit 8 fuer 10 Bilder (@0x8009daf4) = Hintergrundversatz 0..3 px
#   unit_r41_re2gator_intro      Aggro -> Clip 6 (125) -> Clip 3 (150) -> Biss-Sperre 110 (@0x8010091c), dann CHASE Clip 0
#   unit_r41_re2gator_sockel     HP 25300; 100 Pistolentreffer toeten nicht (Sockel), geschwaecht sobald HP < 25000;
#                                Treffer-Reaktion nur 0 (SE 5) oder 4 (SE 0 + Clip 10, Sperre 120..183), nie beides
#   unit_r41_re2gator_kanister   RE15_GATOR_KANISTER-Lage vor der Schnauze -> Stufe 1..7 (13+6+6+7+7 Bilder, Kopf/Kiefer-az
#                                nach den Ease-Formeln 0x80101bac/0x80101c74, Anhaengen an Part 6, Lokallage -> (1750,-210));
#                                Treffer beim Tragen -> R1: SE 4 + Raum-Satz 15 + Rumble (8,3)/(50,255->0) + Clip 9 (37) +
#                                Kanister 9 Bilder spaeter weg + Leiche (Endpose), weitere Treffer wirkungslos
#   unit_r41_re2gator_putz       Dash-Start -> 6 x 9 Putz-Partikel (RE2 CORE00.ESP id 4, Typen 0x10/0x08/0x00) ueber
#                                re2fx ohne unbekannte Ops
add_executable(test_r41_re2gator ${CMAKE_CURRENT_SOURCE_DIR}/test_r41_re2gator.c)
target_link_libraries(test_r41_re2gator PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_re2gator PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_re2gator PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_re2gator PRIVATE m)
endif()
#   unit_r41_re2gator_modell     KANISTER40A0.MD1 (4964 B, 95 Quads, 104 Vertices, CLUT 0x7840) + .TIM (8 bpp 128x256,
#                                2 CLUTs) parsen wie der PC-Zeichner (re15_md1_parse / re15_tim_parse)
#   unit_r41_re2gator_kiefer     Treffer beim Tragen -> FUN_80104318 stempelt Part-Record 7 (Flagwort 0x4A, Tinte 0x202038,
#                                +0xA0 0x8000, +0x9A -10, +0x98 yaw+0x800); Part-Bruecke (re15_re2z_gore_part_matrix /
#                                _part_burst): 30 Scherben-Bilder, Skala 180..2790, Aufstieg -10/Bild, danach Flagwort 0
#   unit_r41_re2gator_wand       Nachbesserung 1 (Abnahme 0, M1): Intro vom Lauerplatz OHNE Bahn-Gate (Clip 6 125 ->
#                                Clip 3 150 -> yaw += 1024 @0x80100938-48, Biss-Sperre 110) und Dash im Westkanal nach
#                                SUEDEN auf der Klemmlinie (x = -7434) mit Leon in der Tuernische: Zonenwechsel 2 -> 0 ->
#                                SE 4, Clip 2/3 je 150, Gator bleibt im Becken (Dispatcher-Klemme nimmt den RE2-Korridor-
#                                Seitversatz -2775), laengs ~4188 voran, yaw += 1024 @0x80100eb0-c0
#   unit_r41_re2gator_kamera     Nachtrag 4 (RE2-Kameraschnitte ROOM40A0 -> ROOM2090): Intro DURCHBRUCH bei Clip 6
#                                Bild 0 -> SEITE bei Bild 70 (@0x80100894-a4) -> ANGRIFF am Clip-6-Ende (@0x801008bc-c8)
#                                -> zurueck + frei am Clip-3-Ende (@0x801008f8-918); R1 aus einem Cut mit Kopf im Bild =
#                                nur Sperre, ohne = EXPLOSION-Schnitt, zurueck bei Clip-9-Bild 36 (@0x801024a4-ec), frei in
#                                P3 (@0x80102688-94); FINISHER bei FRESSEN P0 (@0x80100fbc-ff4) mit Kopf + Kiefer im Bild;
#                                jede Wahl unabhaengig nachgerechnet (Projektion, frontal = cos Blickachse/Kamera)
foreach(_teil modell clips dash intro sockel kanister kiefer putz wand kamera)
    add_test(NAME unit_r41_re2gator_${_teil} COMMAND test_r41_re2gator ${_teil})
    set_tests_properties(unit_r41_re2gator_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

if(TARGET re15_pc)
    # echte exe, ROOM2090 per Sprung, RE15_GB_TEST=1 (Kampf sofort), RE15_GATOR_KANISTER am Suedufer:
    # Zustandslog zeigt Clip 1 (Paddeln) in der Verfolgung, gator_boss.log die RE2-Zeilen (INTRO/DASH/KANISTER),
    # keine HP-Abnahme unter den Sockel.
    add_test(NAME integration_r41_re2gator
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_re2gator_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_re2gator.cmake)
    set_tests_properties(integration_r41_re2gator PROPERTIES TIMEOUT 900)
endif()
