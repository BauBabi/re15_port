# Runde 39 (2026-10-18), Spur M "schluessel2" — Riegel, zaehlen zur Suite. Dossier: analysis/befunde_runde39/M_schluessel2.md.
#
# NUTZER-VORGABE (AUFTRAG.md Nachtrag 5, woertlich): "Und die Schlüssel im Communicationsroom sind immer noch zu groß, die
# sollten maximal halb so groß sein." Parking Lot Key (Leon ROOM10F0) / Prison Key (Elza ROOM10F1) an der Pinnwand: der
# groesste Massstab, bei dem die gezeichnete Huelle in Breite UND Hoehe hoechstens 0,5 x Stand Spur L ist
# (tools/r38_itemorte/itemorte_export.py NUTZER_FAKTOR_M -> gen/itemorte_r38_lage.inc RE15_R39M_* + gen/itemorte_r38_md1.inc).
# Die Zahl 0,5 ist eine Nutzer-Zahl (kein RE-Wert); die FORM (Bild linear im Massstab: GTE RTPS, H = fov >> 7
# @0x80021e70 vor SetGeomScreen @0x80066c30) steht im Dossier.
#
# Zwei Grenzen, je in Breite UND Hoehe <= 0,5 x Spur L: (1) stetige gezeichnete Huelle, (2) GTE-Raster = das Viereck der
# GANZZAHLIGEN Bildschirm-Ecken (FUN_80022f0c RTPT @0x80022fb0, SXY ins GPU-Polygon @0x80022fd8; Port objpoly_r38.c).
#
#   unit_r39_schluessel2_halb    (1) am gelieferten Netz (MD1 + Prop-Lage + Engine-Kamera Cut 5, jeder sichtbare Texel
#                                abgetastet) gegen Stand Spur L (RE2-Modell aus info/re2leon, Ecken wie das Werkzeug):
#                                Breite UND Hoehe <= 0,5 x; Mitte unveraendert; dieselben Abtastpunkte; in der Marke;
#                                geliefertes MD1 == RE2-Modell mit genau diesen Ecken.
#   unit_r39_schluessel2_raster  (2) mit den Engine-Funktionen (re15_camera_compose_view_bone, re15_objpoly_r38_tri_mesh):
#                                GTE-Viereck Spur L 6 x 3 / 6 x 4, geliefert 2 x 1 / 3 x 2 (== Werkzeug == RE15_TRILOG),
#                                <= 0,5 x; knapp darueber verletzt eine Grenze (groesstes zulaessiges Viereck); an beiden
#                                Intervall-Enden dieselben Ecken (dasselbe Bild).
#   unit_r39_schluessel2_konst   Konstanten: Faktor 50/100 (NUTZER-VORGABE), Basis == Spur-L-Abnahme, Lage/Drehung/
#                                Aufhebe-Zone/Slot/Licht unveraendert.
add_executable(test_r39_schluessel2 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_schluessel2.c)
target_link_libraries(test_r39_schluessel2 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_schluessel2 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r39_schluessel2 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_RE2_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon")
if(NOT WIN32)
    target_link_libraries(test_r39_schluessel2 PRIVATE m)
endif()
foreach(_teil halb raster konst)
    add_test(NAME unit_r39_schluessel2_${_teil} COMMAND test_r39_schluessel2 ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_schluessel2_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

#   integration_r39_schluessel2  (nur mit re15_pc) ECHTER Lade-Weg (Spielstand + CONTINUE, Kartenwerkzeug
#                                probe_r37_feuerloescher_karte), Fensterskala 3, Leon ROOM10F0 / Elza ROOM10F1 Cut 5:
#                                RE15_TRILOG-Ecken == GTE-Viereck des Kopfes (2 x 1 / 3 x 2); Unterschied mit/ohne Taken-Bit
#                                (probe_r38_itemorte_kontrast): Huelle <= 7 x 4 bzw. 8 x 5 (= halbe Spur-L-Messung 14 x 8 /
#                                16 x 10), sichtbar (Pixel + Sum|dL| >= ~60 % der Messung), nichts ausserhalb.
if(TARGET re15_pc AND TARGET probe_r38_itemorte_kontrast AND TARGET probe_r37_feuerloescher_karte)
    add_test(NAME integration_r39_schluessel2
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KONTRAST_TOOL=$<TARGET_FILE:probe_r38_itemorte_kontrast>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r37_feuerloescher_karte>
                     -DRE15_LAGE_INC=${CMAKE_SOURCE_DIR}/engine/src/gen/itemorte_r38_lage.inc
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_schluessel2_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_schluessel2.cmake)
    set_tests_properties(integration_r39_schluessel2 PROPERTIES TIMEOUT 900)
endif()
