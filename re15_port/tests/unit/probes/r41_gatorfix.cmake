# Runde 41 Spur G2 "gatorfix" - Riegel und Werkzeuge. Dossier: analysis/befunde_runde41/G2_gatorfix.md.
add_executable(test_r41_gatorfix ${CMAKE_CURRENT_LIST_DIR}/../test_r41_gatorfix.c)
target_link_libraries(test_r41_gatorfix PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_gatorfix PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r41_gatorfix PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_gatorfix PRIVATE m)
endif()
#   unit_r41_gatorfix_maul(_elza)  Punkt 1: in allen Haltebildern (Koppelbild + 26 .. Wurf, Fang .. weg) kein Leichen-Scheitel ueber dem Schaedel
#                                  (Part 7) oder unter dem Unterkiefer (Part 6), Laengsfenster +-150; Gegenprobe Leiche wie vor G2 (Pruefhaken)
#   unit_r41_gatorfix_wurf(_elza)  Punkt 2: Wurf Clip 5 Bild 57, hochgeworfen (>= H/2 ueber der Maullage), Fang Bild 75 im Maul (<= 40), weg Bild 77
#                                  samt Schluessel; Gegenprobe Leiche wie vor G2: kein Wurf
foreach(_teil maul maul_elza wurf wurf_elza)
    add_test(NAME unit_r41_gatorfix_${_teil} COMMAND test_r41_gatorfix ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_gatorfix_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
