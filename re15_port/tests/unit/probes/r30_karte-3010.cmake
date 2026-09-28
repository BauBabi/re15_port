# Runde 30, Thema B — Karte oeffnet sich nach der Irons-Cutscene (RE2 ROOM3010-Mechanismus).
# MESSSONDE, kein Riegel: was tut der Port heute am Ende von ROOM1150 sub08?
# Eigene Datei, damit parallel arbeitende Agenten nicht dieselbe CMakeLists editieren.
add_executable(probe_r30_karte-3010 ${CMAKE_CURRENT_SOURCE_DIR}/probe_r30_karte-3010.c)
target_link_libraries(probe_r30_karte-3010 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_karte-3010 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_karte-3010 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# Zweite Messsonde: Lage und Zustand des Zielraums ROOM10F0 auf der Karte des Ports.
add_executable(probe_r30_karte-3010_ziel ${CMAKE_CURRENT_SOURCE_DIR}/probe_r30_karte-3010_ziel.c)
target_link_libraries(probe_r30_karte-3010_ziel PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_karte-3010_ziel PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Dritte Messsonde: die aus RE2 geschnittene Mini-Bank HINTSE.VBS durch den echten VAB-Pfad.
add_executable(probe_r30_karte-3010_ton ${CMAKE_CURRENT_SOURCE_DIR}/probe_r30_karte-3010_ton.c)
target_link_libraries(probe_r30_karte-3010_ton PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_karte-3010_ton PRIVATE ${CMAKE_SOURCE_DIR}/include)
