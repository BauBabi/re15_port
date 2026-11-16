# Runde 41 "zielhilfe" — Nutzer 2026-11-16: Waffe heben, Gegner 180 Grad hinter Leon -> Leon dreht nur teilweise.
# Bericht: scratchpad zielhilfe/BERICHT_r1.md; Code: engine/src/player_common.c (Heben-Zweig der Zielhilfe).
#
#   unit_r41_zielhilfe_hinten  180 Grad (und -179,9) -> nach dem Heben Restwinkel 0 (Pistole, Schrotflinte, Messer ziehen,
#                              Messer wiederheben, MG). Rot gegen master (Pistole Rest 248 = 21,8 Grad).
#   unit_r41_zielhilfe_vorne   0 Grad -> Blick jedes Bild bitgleich auf der Peilung (wie master).
#   unit_r41_zielhilfe_wand    Zombie hinter der Wand (ROOM1100) -> kein Ziel, keine Drehung (Runde 39 Spur D unveraendert).
#   unit_r41_zielhilfe_mess    reine Messung 0/45/90/135/180/-90/-135 Grad je Waffe (immer gruen, Protokoll im Log).
add_executable(test_r41_zielhilfe ${CMAKE_CURRENT_LIST_DIR}/../test_r41_zielhilfe.c)
target_link_libraries(test_r41_zielhilfe PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_zielhilfe PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_zielhilfe PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_zielhilfe PRIVATE m)
endif()
foreach(_t hinten vorne wand mess)
    add_test(NAME unit_r41_zielhilfe_${_t} COMMAND test_r41_zielhilfe ${_t})
    set_tests_properties(unit_r41_zielhilfe_${_t} PROPERTIES TIMEOUT 120)
endforeach()
