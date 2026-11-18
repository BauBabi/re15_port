# Runde 42 Strang "gore", Stufe 1 (BAUPLAN gore_matrix U1/U10/U17, Nutzerentscheide 16./17.11.2026):
#   Granatwerfer trennt den Oberkoerper (Beine stehen, kippen, Oberkoerper kriecht), Handgranate zerteilt;
#   Handgranaten-Schaden = hoechste Zombie-HP + 1, jeder Zombie im Wirkbereich stirbt und wird zerteilt;
#   Schrot-Kriecher: die Beine bleiben am Platz.
# Code: enemy_ai_re2_zombie.c re2z_gl_oberkoerper_ab / re2z_death / re15_re2z_lower_body_tick, zombiegore_r15.c
#       re15_zgore_tod, re15_damage.c re15_enemy_take_damage_at (re2z_hg).
#   unit_r42_gore       [u1] [u10] [u17]
#   unit_r42_gore_aus   eigener Prozess mit RE15_KEIN_80PROZENT_GORE=1
add_executable(test_r42_gore
    ${CMAKE_CURRENT_LIST_DIR}/../test_r42_gore.c)
target_link_libraries(test_r42_gore PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_gore PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(test_r42_gore PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_gore PRIVATE m)
endif()
add_test(NAME unit_r42_gore COMMAND test_r42_gore alle)
set_tests_properties(unit_r42_gore PROPERTIES TIMEOUT 300)
add_test(NAME unit_r42_gore_aus COMMAND test_r42_gore aus)
set_tests_properties(unit_r42_gore_aus PROPERTIES TIMEOUT 120)
