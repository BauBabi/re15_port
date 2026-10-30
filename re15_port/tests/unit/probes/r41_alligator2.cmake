# Runde 41 (2026-10-30), Spur P "alligator2" - Riegel. Dossier: analysis/befunde_runde41/P_alligator2.md.
#   Punkte: Nachtrag 31 Punkt 2 (Kampfbeginn nicht vor der Linse), FG-E7 Option (a) (Schluessel-Lagewahl sucht weiter), Lagespruenge
#   mit dem Kanister im Maul (Kopf-Klemme <-> Koerper-Klemme), Riegel Uebergabe Ankunftsszene 2090 -> Knall + 8 Zombies in 2080.
#   unit_r41_alligator2_kamera(_elza)   Abstand der Gator-Teile zur Kamera Cut 0 ueber 150 Bilder ab dem Kampfbeginn, Gegenprobe Lage der Nachbesserung 5
#   unit_r41_alligator2_schluessel      Lagewahl sucht weiter (Stufe >= 1), Becken-Scan: nie unter 10,4 Bildpunkten
#   unit_r41_alligator2_tragen          echter Boss-Takt mit Kanister: Bildschritt <= 340, Gegenprobe harte Klemme
#   integration_r41_alligator2_kamera(_elza)     exe: Bildanteil rechte Bildhaelfte (Median-Hintergrund), Wurzel >= 3800 von der Kamera
#   integration_r41_alligator2_uebergabe(_elza)  exe: Leiter -> Szene -> Leiter -> Knall -> 8 Zombies oben, ohne RE15_SET_FLAG=9:110
#   integration_r41_alligator2_k3(_elza)         exe: K3-Wandlage mit Kopf-Klemme, Schluessel >= 10,4 Bildpunkte und sichtbar (bilddiff)
#   integration_r41_alligator2_tragen(_elza)     exe: natuerlicher Kampf, Wurzelschritt <= 340 zwischen Aufnahme und R1, Kanister beim R1 innerhalb
#   Werkzeuge (kein Riegel): test_r41_alligator2 lagesuche2 | schluessel_scan | trag_scan | trag_eins | bildanteil | tragelog
add_executable(test_r41_alligator2 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_alligator2.c)
target_link_libraries(test_r41_alligator2 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_alligator2 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r41_alligator2 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_alligator2 PRIVATE m)
endif()
foreach(_teil kamera kamera_elza schluessel tragen)
    add_test(NAME unit_r41_alligator2_${_teil} COMMAND test_r41_alligator2 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_alligator2_${_teil} PROPERTIES TIMEOUT 600)
endforeach()

if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_alligator2.cmake)
    foreach(_teil kamera kamera_elza uebergabe uebergabe_elza k3 k3_elza tragen tragen_elza)
        add_test(NAME integration_r41_alligator2_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_TOOL=$<TARGET_FILE:test_r41_alligator2>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_alligator2_${_teil}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_alligator2.cmake)
        set_tests_properties(integration_r41_alligator2_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
