# Sonde: Fahrstuhl ROOM1080 — GANZER WEG bis in den geladenen Raum (Messung 2026-09-26).
# Bewusst KEIN add_test: die Sonde belegt derzeit einen OFFENEN Defekt und wuerde die
# Suite rot faerben. Sie wird gepinnt, sobald der Fix drin ist.
add_executable(probe_elevator_1080_full probe_elevator_1080_full.c)
target_link_libraries(probe_elevator_1080_full PRIVATE re15_engine re15_test_support)
target_include_directories(probe_elevator_1080_full PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Der ZWEITE Fahrstuhl (ROOM4020 -> 0x4000 / 0x5000 / 0x50C0). Gleiche Fehlerklasse,
# laut Zensus messung-fahrstuhl.md §5 einer von vier Raeumen mit mehr als einer
# Null-Rect-Cross-Room-Tuer.
add_executable(probe_elevator_4020_full probe_elevator_4020_full.c)
target_link_libraries(probe_elevator_4020_full PRIVATE re15_engine re15_test_support)
target_include_directories(probe_elevator_4020_full PRIVATE ${CMAKE_SOURCE_DIR}/include)
