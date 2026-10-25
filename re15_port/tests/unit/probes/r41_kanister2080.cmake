# Runde 41 Spur G "kanister2080" — ROOM2080/2081 nach der 2090-Szene (Bit 110): Knall (Nachtrag 17) + 8 Zombies oben (Nachtrag 20), der RE2-Gaskanister
# an der Reling (Gas_Can.bmp = Cut 5) und die Marvin/John-Szene (Kopfschuesse, Kanister, RE2-Wurf ROOM7020 sub12/sub13) mit dem
# Kanister im 2090-Becken (Spur-B-API re15_gr2_kanister_setzen). Dossier analysis/befunde_runde41/G_kanister2080.md, Code
# engine/src/kanister2080_r41.c + include/re15_kanister2080_r41.h + platform/pc/src/kanister2080_pc.c, Daten
# shared_assets/RE2/KANISTER2080.VBS (RE2 ROOM40A0 Satz 11).
#
#   unit_r41_kanister2080_lage    Kanister-Fuss in Cut 5 in der roten Marke (x 159..175, y 149..163), Rueckprojektion (167,156)
#                                 trifft die Konstante; 8 Zombies in Cut 2 und 7 ganz im Bild, paarweise >= 800, frei von
#                                 Band-8-Wandzellen, Blick auf den Treppenabsatz
#   unit_r41_kanister2080_spawn   8 Sce_em_set Typ 0x16 grid 0x02 Etage 8 Tot-Bit 236+i (Form ROOM2020 @0x0090A)
#   unit_r41_kanister2080_szene   NPC 0x40/0x49, ROOM1021-Schussform je lebendem Zombie, Schnitte 2 -> 5 -> 7, RE2-Wurfform
#                                 ROOM7020 sub12, Rueckkehr-Tuer nach 2090; mit 3 Toten nur 5 Schuesse
#   unit_r41_kanister2080_start   Start-Programm (Riegel + Tuer -> 2080 Cut 2 Parkplatz) und Ende (RE2 sub18-Form)
#   unit_r41_kanister2080_bahn    RE2 sub13 Fall (v/a, Stop y >= Wasser - 800) unabhaengig nachgerechnet; Landepunkt im Becken
#   unit_r41_kanister2080_landung Nachbesserung 1 (FG-E3/FG-E4): Nennbahn endet im 2090-Wasser, >= 10 Fall-Bilder in 2090 Cut 12,
#                                 Installer legt den Kanister bei Bit 116 an die Aufschlagstelle (2090 und 2091)
#                                 Nachbesserung 2 (N3): Nord- UND Suedwurf, Wahl des weiteren Landepunkts nach der Lage
#                                 des Alligators, Bit 117
#   unit_r41_kanister2080_kippen  Nachtrag 26: Kipp-Bruch je Bild = RE2 ROOM40A0 sub10 (a -4 @0x066E, 23 Bilder @0x0676, -1024 @0x0682)
#   unit_r41_kanister2080_hals    Nachtrag 26: RE2-Halsfuehrung FUN_800177c0 (Schritt 20, Klemmen 560/312, Akku -> 0 danach)
#   unit_r41_kanister2080_anschwimmen  Nachbesserung 2 (N3): echter Boss-Takt nach der Szene, fuenf Lagen beim Ausloesen
#                                 (Abnahme-1-Laeufe): Aufnahme in 1500 Bildern, kein Biss davor; Gegenprobe ohne Szene
#   unit_r41_kanister2080_texte   Id 42 "Marvin: Hey, Leon, try with this!" / "John: Hey, Elza, try with this!" in Vorbildform
#   unit_r41_kanister2080_ebene   Nachtrag 20: Band-8-Freiflaeche ab den Zombies geschlossen, nichts auf der Treppe @0x0BE2
#                                 (Absatzwand @0x05C4 x -16650), Treppenfuss Band 5 unerreichbar
#   unit_r41_kanister2080_blick   Nachbesserung 3 (M2, G-E2): Adas Versatz aus RE2 ROOM7020 (@0x0740/@0x0748, RID @0x144) nachgerechnet,
#                                 Werfer in 2080/2081 Cut 7 der Kamera zugewandt (frontal wie Ada 0,985), Nord/Sued gleicher Dir_set
#   unit_r41_kanister2080_fx_*    Nachbesserung 3 (M3, Nachtrag 28): Raum-ESP ROOM40A0 + Ops 4/6/37/38/41/42/69/78 der RE2-Kanister-
#                                 Explosion gegen die selbst gelesene Disassembly (test_r41_kanister2080_fx.c)
#   integration_r41_kanister2080  echte exe (Laeufe ankunft / kanister / szene / elza), s. Kopf des Skripts
#   integration_r41_kanister2080_kampf  Nachbesserung 1 (M2): die ganze Folge im echten Ablauf OHNE RE15_GB_TEST/RE15_GATOR_KANISTER,
#                                 Leon + Elza: Ausloeser -> Szene (Hinknien) -> Fall in Cut 12 (FG-E4) -> derselbe Alligator
#                                 (FG-E3, kein neues Auftauchen/Intro) -> Aufnahme an der Aufschlagstelle -> R1-Tod
add_executable(test_r41_kanister2080 ${CMAKE_CURRENT_SOURCE_DIR}/test_r41_kanister2080.c)
target_link_libraries(test_r41_kanister2080 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_kanister2080 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_kanister2080 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_kanister2080 PRIVATE m)
endif()
foreach(_teil lage spawn szene start bahn texte ebene landung kippen hals blick)
    add_test(NAME unit_r41_kanister2080_${_teil} COMMAND test_r41_kanister2080 ${_teil})
    set_tests_properties(unit_r41_kanister2080_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

# Nachbesserung 2 (N3): echter Boss-Takt, Kanister nach der Szene, fuenf Lagen beim Ausloesen (Abnahme-1-Laeufe) + Gegenprobe
add_executable(test_r41_kanister2080_anschwimmen ${CMAKE_CURRENT_SOURCE_DIR}/test_r41_kanister2080_anschwimmen.c)
target_link_libraries(test_r41_kanister2080_anschwimmen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_kanister2080_anschwimmen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_kanister2080_anschwimmen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX" RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_kanister2080_anschwimmen PRIVATE m)
endif()
add_test(NAME unit_r41_kanister2080_anschwimmen COMMAND test_r41_kanister2080_anschwimmen)
set_tests_properties(unit_r41_kanister2080_anschwimmen PROPERTIES TIMEOUT 300)

# Nachbesserung 3 (M3, Nachtrag 28): Ops der RE2-Kanister-Explosion (Raum-ESP ROOM40A0 = shared_assets/RE2/GATOR40A0.ESP)
add_executable(test_r41_kanister2080_fx ${CMAKE_CURRENT_SOURCE_DIR}/test_r41_kanister2080_fx.c)
target_link_libraries(test_r41_kanister2080_fx PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_kanister2080_fx PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r41_kanister2080_fx PRIVATE m)
endif()
foreach(_teil register op69 op41 op4_6 op37_78)
    add_test(NAME unit_r41_kanister2080_fx_${_teil} COMMAND test_r41_kanister2080_fx ${_teil})
    set_tests_properties(unit_r41_kanister2080_fx_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r41_kanister2080
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_kanister2080_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_kanister2080.cmake)
    set_tests_properties(integration_r41_kanister2080 PROPERTIES TIMEOUT 1500)
    add_test(NAME integration_r41_kanister2080_kampf
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_kanister2080_kampf_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_kanister2080_kampf.cmake)
    set_tests_properties(integration_r41_kanister2080_kampf PROPERTIES TIMEOUT 3000)
endif()
