# Runde 42 Strang "namen" (2026-11-18), Stufe 1: Sprechernamen auf jeder Seite gesprochener Untertitel (Annette im Birkin-Film
# ROOM6000, Leon ROOM6030 msg 4). Belege im Kopf von tests/unit/test_r42_namen.c, Werkzeuge tools/r42_birkinfilm/szene_bauen.py und
# tools/r42_story/texte_stage6.py.
#
#   unit_r42_namen_texte  6000/6030 wie im Spiel geladen: jede Seite einer Nachricht mit Namenskopf traegt den Kopf
#   unit_r42_namen_seite  6030 msg 4 in der Schreibmaschine: Seite 2 nach `02 63` ohne Taste, Fenster beginnt mit "Leon:"
#   unit_r42_namen_fortsetzung  Stufe 2: Mod-Fortsetzungszeilen 10D0/1150/2000 am Satzende der Aufnahme, jede mit Namen; Elza-
#                         Raeume 10D1/1151/2001 mitgeprueft (include/re15_namen_r42.h, tools/r42_namen/fortsetzung_bauen.py)
add_executable(test_r42_namen ${CMAKE_CURRENT_LIST_DIR}/../test_r42_namen.c)
target_link_libraries(test_r42_namen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_namen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_namen PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_SYNCHRO_DIR="${CMAKE_SOURCE_DIR}/../synchro")
if(NOT WIN32)
    target_link_libraries(test_r42_namen PRIVATE m)
endif()
foreach(_teil texte seite fortsetzung)
    add_test(NAME unit_r42_namen_${_teil} COMMAND test_r42_namen ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_namen_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
