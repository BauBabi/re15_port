# Runde 39 Spur E "gegner" — Gorillas 0x27 (HP x2, Flinch erst nach 3 x 12 Energie; seit Nachbesserung 2 fuer ALLE
# Gorillas, Nutzer-Nachtrag 3), kein paralleler
# Hundebiss (RE2-Ein-Angreifer-Riegel PL+0x1D3), liegende Zombies kriechen (RE1.5-Wecker im RE2-WAIT),
# Gorilla-/Affen-Schatten auf +0x1ba. Dossier analysis/befunde_runde39/E_gegner.md,
# Code engine/src/gegner_r39.c + include/re15_gegner_r39.h (Haken: enemy_ai_common.c, enemy_ai_re2_dog.c,
# enemy_ai_re2_zombie.c, platform/pc/main.c).
#
#   unit_r39_gegner_affen_hp        ROOM11C0: beide 0x27 mit 360 (vorher 180 @0x8011f514); Regel am Typ, nicht am Raum
#   unit_r39_gegner_affen_zucken    Resolver-Treffer 12 (@0x8006ee44): Flinch (Clip 7) erst beim 3. Treffer, Zustand/
#                                   Unterzustaende bleiben, +0x93 frei; Schrotflinte 38 reagiert sofort
#   unit_r39_gegner_affen_schuss    dieselbe Kette ueber den echten Pistolenschuss (Pad R1 + Kreuz), 360 -> 324
#   unit_r39_gegner_hund_riegel     zwei RE2-Hunde im Flug: nur der erste beisst (-20), PL+0x1D3 |= 0x80
#                                   (@0x801051f8-0x80105200), Tor @0x80104e5c-68; nach der Reaktion frei (@0x8003f8b4)
#   unit_r39_gegner_kriecher_1010   ROOM1010 Cut 4, RE2-KI: 0x81-Kriecher liegt fern, kriecht ab < 3000 in Sicht
#                                   (RE1.5 Block B @0x80104718-48)
#   unit_r39_gegner_kriecher_1220   dasselbe fuer die 0x16-Kriecher in ROOM1220/1221
#   unit_r39_gegner_hund_re15       RE1.5-Flavor: zwei Hunde im Biss-Bild, nur einer zieht ab (acae7-Tor @0x8010f294, Original nativ)
#   unit_r39_gegner_kriecher_*_re15 dieselbe Kriecher-Messung im RE1.5-Flavor (Block B nativ, Sub 5 -> Wort 1)
#   unit_r39_gegner_schatten        Quad-Hoehe 0x27/0x29 = +0x1ba (@0x80116f30 / @0x80110c78), andere Typen unveraendert;
#                                   Szenen-Gorillas grid 0x30 ebenso (Schwanz @0x80116f20-38 auf jedem Pfad); Region-Viereck
#                                   des RDT je Cut == Original-RAM DAT_800ac790, Punkt-im-Viereck (FUN_80014368) wie Original
#   integration_r39_gegner_schatten echte exe ROOM11C0: waehrend des Gorilla-Sprungs (Sub 7) Quad-Y = Boden-Y je Bild
#                                   (gegner_r39.log), Koerper-Y darueber; Szenen-Gorillas: Quad auf -3600, Cut-13-Fenster
#                                   leer, Cut-12-Fenster nur der Wagen-Gorilla (Region-Cull wie FUN_8001b064 @0x8001b224)
#   integration_r39_gegner_hunde1190 echte exe ROOM1190 (Hunde-Luke, Szene sub10): kein zweiter HP-Abzug durch einen
#                                   Hundebiss innerhalb der 22 Bilder Spieler-Flinch nach einem Biss (state.log)
#   Nachbesserung 2 (test_r39_gegner_1141.c, ROOM1141 mit Elza = der zweite 0x27-Raum des Zensus):
#   unit_r39_gegner_1141_hp1141     Kampf-Lage sub00 @0x00B54/@0x00B68: HP 180 -> 360; Regel haengt am Typ (11C0/1141/2000)
#   unit_r39_gegner_1141_zucken1141 Elzas SIG P228 (Waffe 4, 12 @0x8006ee48), drei echte Schuesse: Flinch erst beim dritten
#   unit_r39_gegner_1141_ablauf1141 Phase H -> Gemaelde -> BEFREIUNG -> Kampf bis zum Tod beider Gorillas (vorher 16 / nachher
#                                   31 Treffer je Gorilla) -> Kill-Flags (7,0xCB)/(7,0xCC) -> sub01 (4,0xF2)=0 -> Johns Abgang
#   unit_r39_gegner_1141_affe29     Gegenprobe kleiner Affe 0x29 (ROOM3040): HP und Treffer-Antwort gleich mit Haken aus/an
add_executable(test_r39_gegner ${CMAKE_CURRENT_SOURCE_DIR}/test_r39_gegner.c)
target_link_libraries(test_r39_gegner PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_gegner PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_gegner PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_gegner PRIVATE m)
endif()
foreach(_teil affen_hp affen_zucken affen_schuss hund_riegel hund_re15 kriecher_1010 kriecher_1220 kriecher_1010_re15 kriecher_1220_re15 schatten)
    add_test(NAME unit_r39_gegner_${_teil} COMMAND test_r39_gegner ${_teil})
    set_tests_properties(unit_r39_gegner_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

add_executable(test_r39_gegner_1141 ${CMAKE_CURRENT_SOURCE_DIR}/test_r39_gegner_1141.c)
target_link_libraries(test_r39_gegner_1141 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_gegner_1141 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_gegner_1141 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_gegner_1141 PRIVATE m)
endif()
foreach(_teil hp1141 zucken1141 ablauf1141 affe29)
    add_test(NAME unit_r39_gegner_1141_${_teil} COMMAND test_r39_gegner_1141 ${_teil})
    set_tests_properties(unit_r39_gegner_1141_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r39_gegner_schatten
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_gegner_schatten_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_gegner_schatten.cmake)
    set_tests_properties(integration_r39_gegner_schatten PROPERTIES TIMEOUT 900)
    add_test(NAME integration_r39_gegner_hunde1190
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_gegner_hunde1190_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_gegner_hunde1190.cmake)
    set_tests_properties(integration_r39_gegner_hunde1190 PROPERTIES TIMEOUT 900)
endif()
