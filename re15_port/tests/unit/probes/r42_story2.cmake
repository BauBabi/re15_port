# Runde 42 "story" (v0.8.31), Stufe STAGE2 - Riegel. Konstanten/Belege include/re15_story2_r42.h, Code engine/src/story2_r42.c,
# Texte tools/r42_story/texte_stage2.py -> engine/src/gen/story2_r42_texte.inc. PORT-STORY nach dem Konzept (3.2 L1/L1b, 4.2 E1,
# 6b; K-01..K-03, K-07, K-08), Bits/Ids aus analysis/befunde_runde42/VERTRAG.md.
#
#   unit_r42_story2_texte        2070/2071 msg 1 = ROOM11A0 msg 2 @0x0142C bytegleich; Zeilen 43..54 je Fassung vorhanden
#   unit_r42_story2_programme    Szenen-Programme: Zeilen in Reihenfolge, Set(9,118,1), Evt_end
#   unit_r42_story2_ablass       K-01: Rechner 2080 -> (3,245) und der Hinweis msg 52
#   unit_r42_story2_loch(_elza)  K-01/L1/E1/K-07: Schluesselloch 2080/2081, Szene einmal, Begleiter wartet, nach Bit 120 nicht mehr
#   unit_r42_story2_tor2090      K-01: Tor 2090 nimmt den Schluessel nicht mehr; nach (9,113) offen
#   unit_r42_story2_tueren(_elza) K-02/K-03: 2070 von der anderen Seite verriegelt, 11A0 und 2040 schliessen auf, danach Abkuerzung
#   unit_r42_story2_uhr          K-08/F-19: Uhr erst nach dem Tod, neben dem Schluessel im Bild, Puls; Gravur bei Leon
#   unit_r42_story2_altstand(_elza)  Pruefung 1 Befund 5: Tor schon offen (v0.8.30) -> kein Schluesselloch, Szene am Rechner
#   unit_r42_story2_begleiter    Pruefung 1 Befund 8: Begleiter in 11A0/11A1 nicht, solange sie in 2080/2081 warten
#   unit_r42_story2_inszenierung(_elza)  Pruefung 1 Befund 1/4: Lagen P/N/S, Reihenfolge, Sherry aus der Nische nach dem Treppenweg
#   integration_r42_story2_*     echte exe (tests/integration/test_r42_story2.cmake): kette/kette_elza = EIN Durchlauf je Figur
#                                (Ablassen, echter Kampf, Schluessel + Uhr, Szene, Tor, 20A0, 11A0, Abkuerzung); tueren(_elza) = Zeitbombe
add_executable(test_r42_story2 ${CMAKE_CURRENT_LIST_DIR}/../test_r42_story2.c)
target_link_libraries(test_r42_story2 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_story2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_story2 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_story2 PRIVATE m)
endif()
foreach(_teil texte programme ablass loch loch_elza tor2090 tueren tueren_elza uhr altstand altstand_elza begleiter inszenierung
              inszenierung_elza)
    add_test(NAME unit_r42_story2_${_teil} COMMAND test_r42_story2 ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_story2_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story2.cmake)
    foreach(_teil kette kette_elza tueren tueren_elza)
        add_test(NAME integration_r42_story2_${_teil}
                 COMMAND ${CMAKE_COMMAND} -DRE15_PC_EXE=$<TARGET_FILE:re15_pc> -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/r42_story2_${_teil}
                         -DTEIL=${_teil} -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story2.cmake)
        if(_teil MATCHES "^kette")
            set_tests_properties(integration_r42_story2_${_teil} PROPERTIES TIMEOUT 2700)   # Glied 1 Ablassen bis 20A0 (bis 2300 s) + Glied 2 (600 s)
        else()
            set_tests_properties(integration_r42_story2_${_teil} PROPERTIES TIMEOUT 900)
        endif()
    endforeach()
endif()
