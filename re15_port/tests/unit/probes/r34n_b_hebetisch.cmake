# Runde 34 Nacht / Spur B — Hebetisch ROOM1150/1151: Cursor-Bedienung statt Direktoeffnung.
# Dossier: analysis/befunde_runde34_nacht/B_hebetisch.md
#
#   probe_r34n_b_messung   MESS-SONDE (kein add_test): Halte-Stelle For 15 @0x0FC0/@0x0F9E in sub04,
#                          Cut_old-Ziel mit/ohne vorgeschaltetes Cut 4, Kuppel-Huelle unter Cut 4 mit
#                          Engine-Projektion, 11F0-Cursor unter Cut 10 von ROOM11F0.
add_executable(probe_r34n_b_messung ${CMAKE_CURRENT_LIST_DIR}/../probe_r34n_b_messung.c)
target_link_libraries(probe_r34n_b_messung PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_b_messung PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r34n_b_messung PRIVATE m)
endif()
