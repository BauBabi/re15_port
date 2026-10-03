# Runde 35 Spur G "karte" — Fahrstuhl-Cursor ROOM1080, ROOM11F0/1200 auf der Karte,
# ROOM1230 zeigt die Karte von ROOM11E0, ROOM1210 Korridor + Tueren.
# Dossier: analysis/befunde_runde35/G_karte.md
#
#   probe_r35_karte messung   MESS-SONDE (kein add_test): echter Kartenpfad je Raum/Position
add_executable(probe_r35_karte ${CMAKE_CURRENT_LIST_DIR}/../test_r35_karte.c)
target_link_libraries(probe_r35_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r35_karte PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r35_karte PRIVATE m)
endif()
