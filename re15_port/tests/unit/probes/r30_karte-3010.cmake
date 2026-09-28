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

# ---- RIEGEL (Bau-Agent Runde 30, Kartenhinweis nach der Irons-Szene) ----------------------
# EIN Programm, fuenf Riegel (argv[1]); Belege im Kopf von test_r30_hinweis.c und in
# include/re15_map_hint.h. Die RE2-Quelle ROOM3010.RDT dient nur dem Wellenvergleich im
# Riegel "bank" (fehlt sie, wird dieser eine Vergleich gemeldet und uebersprungen).
add_executable(test_r30_hinweis ${CMAKE_CURRENT_SOURCE_DIR}/test_r30_hinweis.c)
target_link_libraries(test_r30_hinweis PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_hinweis PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r30_hinweis PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_RE2_RDT_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon/PL0/RDT")
foreach(_r30h anker fsm spurlos zeichner bank)
    add_test(NAME unit_r30_hinweis_${_r30h} COMMAND test_r30_hinweis ${_r30h})
    set_tests_properties(unit_r30_hinweis_${_r30h} PROPERTIES TIMEOUT 60 SKIP_RETURN_CODE 77)
endforeach()
