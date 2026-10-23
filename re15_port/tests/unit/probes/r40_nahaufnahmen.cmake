# Runde 40 "nahaufnahmen" — nie gezeigte STAGE1-Nahaufnahmen beim Untersuchen (11D0 Pult, 1110 Spinde/Zeitbombe,
# 1100 VUS INVALID/TRY AGAIN, 11F0 Notstrom-Pult). Herleitung include/re15_nahaufnahmen_r40.h.
#
# Mechanik-Riegel: echte RDTs (Leon + Elza), Raumaufbau wie im Spiel, Aktion vor jeder Stelle, angezeigter Cut je Bild
# (re15_cam_present_tick), Rueckweg der Kamera, Original-Teile der Programme Byte fuer Byte gegen die RDT.
add_executable(test_r40_nahaufnahmen test_r40_nahaufnahmen.c)
target_link_libraries(test_r40_nahaufnahmen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r40_nahaufnahmen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r40_nahaufnahmen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r40_nahaufnahmen COMMAND test_r40_nahaufnahmen)
set_tests_properties(unit_r40_nahaufnahmen PROPERTIES TIMEOUT 120)
