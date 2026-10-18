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
target_compile_definitions(test_r39_optik PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_optik PRIVATE m)
endif()
foreach(_teil kappen ecke leon elza)
    add_test(NAME unit_r39_optik_${_teil} COMMAND test_r39_optik ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_optik_${_teil} PROPERTIES TIMEOUT 120)
endforeach()
