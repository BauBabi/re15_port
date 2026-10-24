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
#   unit_r41_kanister2080_texte   Id 42 "Marvin: Hey, Leon, try with this!" / "John: Hey, Elza, try with this!" in Vorbildform
#   unit_r41_kanister2080_ebene   Nachtrag 20: Band-8-Freiflaeche ab den Zombies geschlossen, nichts auf der Treppe @0x0BE2
#                                 (Absatzwand @0x05C4 x -16650), Treppenfuss Band 5 unerreichbar
#   integration_r41_kanister2080  echte exe (Laeufe ankunft / kanister / szene / elza), s. Kopf des Skripts
add_executable(test_r41_kanister2080 ${CMAKE_CURRENT_SOURCE_DIR}/test_r41_kanister2080.c)
target_link_libraries(test_r41_kanister2080 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_kanister2080 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_kanister2080 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_kanister2080 PRIVATE m)
endif()
foreach(_teil lage spawn szene start bahn texte ebene)
    add_test(NAME unit_r41_kanister2080_${_teil} COMMAND test_r41_kanister2080 ${_teil})
    set_tests_properties(unit_r41_kanister2080_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r41_kanister2080
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_kanister2080_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_kanister2080.cmake)
    set_tests_properties(integration_r41_kanister2080 PROPERTIES TIMEOUT 1500)
endif()
