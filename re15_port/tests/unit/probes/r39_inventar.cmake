# Runde 39 Spur C "inventar" — Spind ROOM1000 (erst Handgun-Munition, dann "Nothing unusual."), Stapelregel "alles mit
# einer Zahl" (Memory Card, Granaten), Status-Bild je Figur (Elza: STPIC_04), Wegwerf-Abfrage der Yellow Keycard bei
# Elza (ROOM11E1). Dossier analysis/befunde_runde39/C_inventar.md, Belege include/re15_inventar_r39.h.
#
# RIEGEL (Unit, je Nutzer-Punkt die MECHANIK am echten Raum-/VM-Weg):
#   unit_r39_inventar_spind    ROOM1000: Slot 3 Aufnahme 0x15 x30 Bit (9,69), Text msg 0 in Slot 11; Druck -> Modal ->
#                              Ja -> +15, Bit; zweiter Druck -> msg 0; Wiederbetreten: Text in Slot 3; Elza 1001 Original
#   unit_r39_inventar_stapeln  Zensus der Stapelregel gegen @0x80074DA8, Gutschrift, Paar-Matcher, ITEM BOX, echter
#                              Menue-Weg COMBINE 21:3 + 21:2 -> 21:5
#   unit_r39_inventar_status   STPIC-Index/Pfad je Figur, Dateien + TIM-Geometrie, 04 != 00, Varianten gleich
#   unit_r39_inventar_discard  discard_sites.inc (11E1 msg 5 Gate (3,139), 11E0 gestrichen), Vorentscheid, echter Weg
#                              Elza 11E1 (Leser -> Code -> msg 5 -> Abfrage -> Ja -> Karte weg), Leon 11E0 Gegenprobe
add_executable(test_r39_inventar ${CMAKE_CURRENT_LIST_DIR}/../test_r39_inventar.c)
target_link_libraries(test_r39_inventar PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_inventar PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r39_inventar PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_inventar PRIVATE m)
endif()
foreach(_teil spind stapeln status discard)
    add_test(NAME unit_r39_inventar_${_teil} COMMAND test_r39_inventar ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_inventar_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

# MESS-WERKZEUG (kein add_test): Ausweis-Block eines Framedumps gegen eine STPIC_<k>.TIM.
add_executable(probe_r39_inventar_bild ${CMAKE_CURRENT_LIST_DIR}/../probe_r39_inventar_bild.c)

# INTEGRATION (echte exe, tests/integration/test_r39_inventar.cmake), je Teil eigener Laufordner + exe-Kopie:
#   integration_r39_inventar_spind    Leon ROOM1000: Aufnahme 0x15 (15:50 -> 15:65), danach msg 0; mit (9,69): nur msg 0
#   integration_r39_inventar_stapeln  Leon ROOM1010: Memory Card 21:3 + Aufnahme -> 21:6 EIN Platz; COMBINE 21:3+21:2 -> 21:5
#   integration_r39_inventar_status   Elza ROOM1001 Statusschirm: STPIC_04 geladen + Bild == Driver License; Leon: STPIC_00
#   integration_r39_inventar_discard  Elza ROOM11E1: Leser -> Code -> msg 5 -> Abfrage -> Ja -> 0x39 weg; Leon 11E0: nichts
if(TARGET re15_pc AND TARGET probe_r37_feuerloescher_karte)
    foreach(_teil spind stapeln status discard)
        add_test(NAME integration_r39_inventar_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r37_feuerloescher_karte>
                -DRE15_BILD_TOOL=$<TARGET_FILE:probe_r39_inventar_bild>
                -DRE15_ASSET_PSX_DIR=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                -DWORKDIR=${CMAKE_BINARY_DIR}/r39_inventar_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_inventar.cmake)
        set_tests_properties(integration_r39_inventar_${_teil} PROPERTIES TIMEOUT 1200)
    endforeach()
endif()
