# Runde 36 Spur C — der allgemeine Schusspfad: Sichtlinie (RE1.5 FUN_8001b9b4 @0x80012168-74), Fresser-Sperre
# (RE2 FUN_800410CC @0x80041270-9c), Blut-Anker, Kraehe gegen Magnum (RE2 EMOVL21_S0.BIN GIB @0x80102CA0).
#
# Dossier: analysis/befunde_runde36/C_schuss.md; Code: engine/src/schuss_r36.c, include/re15_schuss_r36.h,
# Haken in re15_damage.c (Sichtlinie), enemy_ai_re2_zombie.c (Fresser), enemy_ai_common.c (Export).
#
#   unit_r36_schuss           Mechanik ohne exe: Sichtlinie ueber den Konferenztisch ROOM1140 (echte RDT, alle
#                             Waffenklassen), Fresser-Sperre je Gate (RE2-KI), Aufstehen gibt frei.
add_executable(test_r36_schuss ${CMAKE_CURRENT_LIST_DIR}/../test_r36_schuss.c)
target_link_libraries(test_r36_schuss PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_schuss PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r36_schuss PRIVATE m)
endif()
add_test(NAME unit_r36_schuss COMMAND test_r36_schuss)
set_tests_properties(unit_r36_schuss PROPERTIES TIMEOUT 120)

#   integration_r36_schuss    echte exe (ROOM1140, RE15_DEBUG_JUMP, Messkopie re15_pc_r36c_haken_*): Python ueber den Konferenztisch
#                             trifft NICHT (Plaetze 4/5 hp 250 bleiben), Fresser vom Tuerpunkt (> 4000) werden
#                             NICHT getroffen, Zombie hinter dem Tisch wird getroffen (Gegenprobe); dazu unter RE1.5-KI
#                             (Fresser treffbar) frei_re15 trifft Platz 4/5, tisch_re15 / gl_tisch_re15 sperren (M4).
if(TARGET re15_pc)
    add_test(NAME integration_r36_schuss
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_schuss_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_schuss.cmake)
    set_tests_properties(integration_r36_schuss PROPERTIES TIMEOUT 900)
endif()

#   probe_r36_blut            Messsonde + Pin fuer den Blut-Anker des RE2-Zombies (ROOM1140, echter game_step,
#                             RE2-Bank), Schuss bis zum TOD + 80 Bilder: JEDER Spawn aus dem Mess-Ring
#                             (re15_schuss_r36_fx_*) gegen Anker.R*a3 + Anker.T aus der gezeichneten Pose —
#                             HURT @0x80105650-708, Stagger @0x80105D14-DBC, Flinch @0x801050B0-158, Nachbluten
#                             @0x80105F5C-FF8, Aufsteh-Treffer @0x80107B14-30, TOD @0x8010864C-6B4, Tropfblut
#                             @0x8010887C-D4, Tod des Liegenden @0x80109B30-60, Kriecher-Tod @0x80108A84-D4
#                             (Nachbesserung 1: Abnahme 0 M1/M2/M3).
add_executable(probe_r36_blut ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_blut.c)
target_link_libraries(probe_r36_blut PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_blut PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_blut PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r36_blut PRIVATE m)
endif()
add_test(NAME unit_r36_blut_pin_eben COMMAND probe_r36_blut pin eben)
add_test(NAME unit_r36_blut_pin_tief COMMAND probe_r36_blut pin tief)
add_test(NAME unit_r36_blut_pin_mg   COMMAND probe_r36_blut pin mg)
#                             Nachbesserung 2 (Abnahme 1 M1): "Zeichner" der Sonde = Zwilling des Port-Renderers (Bankwahl,
#                             Crossfade, Unterhaelfte, Ragdoll-Anker, fliegende Teile); Pruefung (G) fuer JEDEN Spawn (auch
#                             a3 = 0, Zensus-Gruppe B) gegen die gezeichnete Matrix: rip = SPAS (RIP-Kopf @0x80108E28, RIP-P1
#                             @0x80109108/3C, Aufschlag-Blut @0x80028C4C), arm = Remington tief (@0x80107704), burst = Remington
#                             (Ragdoll-Emitter @0x80106D7C), glied = Ingram Dauerfeuer (Gliedabriss @0x80105368/@0x801053D0 +
#                             Aufschlag-Blut des Glieds).
add_test(NAME unit_r36_blut_pin_rip   COMMAND probe_r36_blut pin rip)
add_test(NAME unit_r36_blut_pin_arm   COMMAND probe_r36_blut pin arm)
add_test(NAME unit_r36_blut_pin_burst COMMAND probe_r36_blut pin burst)
add_test(NAME unit_r36_blut_pin_glied COMMAND probe_r36_blut pin glied)
#                             Nachbesserung 3 (Abnahme 2 M1): KRIECHER-KOPFABRISS (EMZ0 P5 Leon-Weiche): Kopfabriss
#                             @0x80102B24 (Part 0, a3 (0,-600,0) @0x80102B10-1C, Id 0x08001000 @0x80102AD4-DC) und
#                             Hals-Stumpf @0x80102BBC (Part 8, a3 (0,100,0) @0x80102B8C-A0, Id 4096 @0x80102BA4) an der
#                             gezeichneten Matrix; Bank wie die exe (Hybrid). kriecher = Ingram, Platz 2 (Typ 0x10);
#                             kriecher5 = MC51 von +z, Platz 5 (Typ 0x11): die SCA-Klemme holt den Kriecher im Kopfab-Bild
#                             zurueck -> pinnt die Nachfuehrung re15_schuss_r36_nachfuehren (RE2-Root `jal 0x8003567c`
#                             @0x80100638 nach dem Handler).
add_test(NAME unit_r36_blut_pin_kriecher  COMMAND probe_r36_blut pin kriecher)
add_test(NAME unit_r36_blut_pin_kriecher5 COMMAND probe_r36_blut pin kriecher5)
set_tests_properties(unit_r36_blut_pin_eben unit_r36_blut_pin_tief unit_r36_blut_pin_mg
                     unit_r36_blut_pin_rip unit_r36_blut_pin_arm unit_r36_blut_pin_burst unit_r36_blut_pin_glied
                     unit_r36_blut_pin_kriecher unit_r36_blut_pin_kriecher5
                     PROPERTIES TIMEOUT 120)

#   unit_r36_schuss_kraehe    Punkt 3: Kraehe gegen Super Redhawk = RE2 GIB 0x80102CA0 (13 Teile Wurf/Wirbel, Draws,
#                             Physik bis zum Verschwinden, Dispatch @0x80104A18).
add_executable(test_r36_schuss_kraehe ${CMAKE_CURRENT_LIST_DIR}/../test_r36_schuss_kraehe.c)
target_link_libraries(test_r36_schuss_kraehe PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_schuss_kraehe PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r36_schuss_kraehe PRIVATE m)
endif()
add_test(NAME unit_r36_schuss_kraehe COMMAND test_r36_schuss_kraehe)
set_tests_properties(unit_r36_schuss_kraehe PROPERTIES TIMEOUT 120)
