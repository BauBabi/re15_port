# Runde 39 Spur G "karten" — Karten 1F/2F/3F/B2 nach den Nutzer-Vorlagen (map*_correct.png),
# B1 neu, Marker ROOM1090 im Raum. Dossier: analysis/befunde_runde39/G_karten.md
#
#   probe_r39_karten ops <seite> <hexraum> <x> <z> [band]   MESS-SONDE (kein add_test)
add_executable(probe_r39_karten ${CMAKE_CURRENT_LIST_DIR}/../test_r39_karten.c)
target_link_libraries(probe_r39_karten PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r39_karten PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r39_karten PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r39_karten PRIVATE m)
endif()
