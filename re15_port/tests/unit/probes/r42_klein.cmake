# Runde 42 "story" (v0.8.31), Stufe Kleinkram - Riegel. Konstanten/Belege include/re15_story_klein_r42.h, Code
# engine/src/story_klein_r42.c, Text tools/r42_story/texte_klein.py -> engine/src/gen/story_klein_r42_texte.inc.
# PORT-STORY nach dem Konzept (5 T-08 [B22], T-09 [B39]). Nachlauf der Untertitel (Pruefhinweis H4): tools/r42_story/nachlauf_r42.py,
# Riegel in unit_r42_story3a_texte.
#
#   unit_r42_klein_belege   Original-Bytes ROOM6020 sub00, ROOM50B0 Set(3,0x62), ROOM4010 Slot 9, DEBUG.BIN Beschreibung 0x47
#   unit_r42_klein_ada      6020: Ada nur, solange (3,0x62)=0; Marvin immer
#   unit_r42_klein_elza     6021: Original-Saetze 0x49/0x4B unveraendert
#   unit_r42_klein_karte    Beschreibung 0x47 "Michael" (3 Zeilen, 1 Seite); 0x46 unveraendert
#   integration_r42_klein_szene / _lebt / _tot   Durchlaeufe: Adas Szene 50B0; 5130 -> 6020 (tests/integration/test_r42_klein.cmake)
#   integration_r42_klein_glyphe_karte / _uhr    Bildpruefung CHECK 0x47 / 0x3F Seite 2 (Anfuehrungszeichen 0x61, Schriftseite 256 Zeilen)
add_executable(test_r42_klein ${CMAKE_CURRENT_LIST_DIR}/../test_r42_klein.c)
target_link_libraries(test_r42_klein PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_klein PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r42_klein PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_klein PRIVATE m)
endif()
foreach(_teil belege ada elza karte)
    add_test(NAME unit_r42_klein_${_teil} COMMAND test_r42_klein ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_klein_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_klein.cmake)
    foreach(_teil szene lebt tot glyphe_karte glyphe_uhr)
        add_test(NAME integration_r42_klein_${_teil}
                 COMMAND ${CMAKE_COMMAND} -DRE15_PC_EXE=$<TARGET_FILE:re15_pc> -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/r42_klein_${_teil}
                         -DTEIL=${_teil} -DPRUEFER=$<TARGET_FILE:test_r42_klein>
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_klein.cmake)
        set_tests_properties(integration_r42_klein_${_teil} PROPERTIES TIMEOUT 3000)
    endforeach()
endif()
