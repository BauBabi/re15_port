# Runde 39 (2026-10-18), Spur L "optik" — Riegel, zaehlen zur Suite. Dossier: analysis/befunde_runde39/L_optik.md.
#
# Punkt 1 (AUFTRAG.md): "Ich habe noch mal einen Marker mit F9 gemacht. Leon ist immer noch vom Boden ueberdeckt an den
# einen Streifen." Marke befund_1100_F741_marke1.bmp: ROOM1100 Cut 1, Leon (-16007,0,-23658) im Quergang hinter der
# Ecke der Westwand; die Port-Masken (Westwand als Quader bis zur SCA-Zelle z -23200) reichen 5..7 Bildspalten ueber die
# GEMALTE Ecke hinaus und malen dort Boden/Rueckwand des hinteren Gangs ueber ihn. Fix: optik_r39.c kappt die Rechtecke
# von Cut 1/2 beim Laden an der gemalten Wand (Fenster am Hintergrund gemessen).
#
#   unit_r39_optik_kappen  Mechanik des Kappens an einer kuenstlichen Sektion (Teilung je Zeilenband, Atlas-Versatz,
#                          Rechtecke ganz jenseits entfallen, draw_count folgt, fremde Raeume/Cuts unveraendert).
#   unit_r39_optik_ecke    die Fenster gegen die am Hintergrund (Engine-Dekoder) NEU gemessene Eckkante; gekappte
#                          Deckung == rohe Deckung geschnitten mit der gemalten Wand (Gegenprobe: roh 474 / 372 Punkte
#                          jenseits der Ecke).
#   unit_r39_optik_leon    Leon an der Lage der Marke F741 ueber die Kamera von Cut 1 gegen die Masken (Original-Urteil
#                          re15_pri_mask_occludes @0x8002565c / ZSF3 @0x80066c70 / Masken-OT @0x80039650-60): jenseits
#                          der gemalten Ecke deckt nichts mehr (roh: > 50 Punkte), hinter der Wand bleibt er verdeckt.
#   unit_r39_optik_elza    ROOM1101 (Elza) zeichnet die Masken von ROOM1100 (re15_pri_masken_raum) -> dieselben Fenster.
add_executable(test_r39_optik ${CMAKE_CURRENT_LIST_DIR}/../test_r39_optik.c)
target_link_libraries(test_r39_optik PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_optik PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_optik PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_RE2_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon")
if(NOT WIN32)
    target_link_libraries(test_r39_optik PRIVATE m)
endif()
#   unit_r39_optik_schluessel  Punkt 2 (Schluessel 10F0/10F1 "kleiner/subtiler"): Bildlaenge = Median der RE2-Bildlaenge
#                          desselben Modells an seinen RE2-Platzierungen (aus info/re2leon mit der Engine-Kamera
#                          nachgerechnet == gen/itemorte_r38_lage.inc), Massstab < Runde 38, Huelle 3..9 px, MD1-Spanne/s =
#                          RE2-Modell 152, Lage/Drehung/Aufhebe-Zone unveraendert.
foreach(_teil kappen ecke leon elza schluessel)
    add_test(NAME unit_r39_optik_${_teil} COMMAND test_r39_optik ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_optik_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

#   integration_r39_optik_marke  (nur mit re15_pc) die echte exe an der Lage der Marke F741: im Streifen jenseits der
#                          gemalten Ecke (x 166..172) ist Leon gezeichnet (A gegen C > 0) und die Masken malen dort nichts
#                          ueber ihn (A gegen B <= 1); in der Wand (x 173..180) sind die Masken an (A gegen B > 0) und Leon
#                          bleibt verdeckt (A gegen C == 0). Vorher: 134 Streifenpunkte der Maske ueber Leon.
if(TARGET re15_pc AND TARGET probe_r36_ppm_diff)
    add_test(NAME integration_r39_optik_marke
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r36_ppm_diff>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_optik_marke_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_optik_marke.cmake)
    set_tests_properties(integration_r39_optik_marke PROPERTIES TIMEOUT 600)
endif()
