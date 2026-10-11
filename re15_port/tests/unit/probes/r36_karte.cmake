# Runde 36 Spur M "karte" — Treppen 10A0 auf B1, Marker ROOM11D0 an der Tuer, ROOM1190 in zwei
# Bereichen, Tuer 11B0<->11C0 waagerecht, Teilbereiche ROOM1110 erst nach Betreten besucht,
# Fahrstuhl-Kabine nur auf der Etage besucht, von der aus man eingestiegen ist.
# Dossier: analysis/befunde_runde36/M_karte.md
#
#   probe_r36_karte messung   MESS-SONDE (kein add_test): echter Kartenpfad je Punkt
add_executable(probe_r36_karte ${CMAKE_CURRENT_LIST_DIR}/../test_r36_karte.c)
target_link_libraries(probe_r36_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_karte PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r36_karte PRIVATE m)
endif()
