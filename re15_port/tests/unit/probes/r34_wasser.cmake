# Runde 34 (Granaten) — Integration W7 (analysis/befunde_runde34_granaten/integration.md, bau_c.md N1.1).
#
#   unit_r34_wasser   ESP-Routinen 41 (@0x80018ef4) und 42 (@0x80018f98) am Raum-Effekt 0x0b
#                     (Wasserstrahl ROOM2000/2001/20B0/20B1), echte Engine + echte ROOM2000-Daten:
#                     Freigabe nach Halten 5k, Zaehler bis 25, defH +768 ab Zaehler 16, Schleife
#                     (Zeile 0, xlat 0, Satz 1 / Satz[1].Byte2), RNG-Weg ueber das neue Zeilenwort
#                     +0x18, Treffer-Test FUN_8002b7e8 (Radius 0x2d) -> Physik-Stopp, kein Bodenklemmen.
add_executable(probe_r34_wasser ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_wasser.c)
target_link_libraries(probe_r34_wasser PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_wasser PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34_wasser PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r34_wasser PRIVATE m)
endif()
add_test(NAME unit_r34_wasser COMMAND probe_r34_wasser)
set_tests_properties(unit_r34_wasser PROPERTIES TIMEOUT 120)
