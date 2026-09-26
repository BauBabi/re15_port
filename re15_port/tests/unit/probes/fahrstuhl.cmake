# Sonde: Fahrstuhl ROOM1080 — GANZER WEG bis in den geladenen Raum (Messung 2026-09-26).
# Bewusst KEIN add_test: die Sonde belegt derzeit einen OFFENEN Defekt und wuerde die
# Suite rot faerben. Sie wird gepinnt, sobald der Fix drin ist.
add_executable(probe_elevator_1080_full probe_elevator_1080_full.c)
target_link_libraries(probe_elevator_1080_full PRIVATE re15_engine re15_test_support)
target_include_directories(probe_elevator_1080_full PRIVATE ${CMAKE_SOURCE_DIR}/include)
