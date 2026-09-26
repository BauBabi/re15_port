# Sonden: die beiden Fahrstuehle des Spiels — GANZER WEG bis in den GELADENEN Raum.
#
# Gemessener ALT-Stand (vor dem Fix vom 2026-09-26, beide Sonden Exit 1 = ROT):
#   ROOM1080  1F 0x1040 OK | 2F Soll 0x10C0 Ist 0x1040 FALSCH | 3F Soll 0x1120 Ist 0x1040 FALSCH
#   ROOM4020  1F 0x4000 OK | 2F Soll 0x5000 Ist 0x4000 FALSCH | 3F Soll 0x50C0 Ist 0x4000 FALSCH
# Nur 1F war "richtig", und auch das nur zufaellig: der port-eigene Zweig `is_auto_door`
# in aot_common.c feuerte im selben Bild Tuer-Slot 0 nach und ueberschrieb die per Aot_on
# angemeldete Etage. Slot 0 IST 1F. Ein Riegel, der nur 1F prueft, waere gruen geblieben —
# deshalb pinnen diese Sonden alle DREI Etagen.
#
# GEGENPROBE nachgefahren (2026-09-26): den Zweig `is_auto_door` samt seinen drei
# Verwendungen wieder eingebaut, neu gebaut, `ctest -R unit_fahrstuhl` gelaufen ->
#   287 - unit_fahrstuhl_1080_etagen (Failed)
#   288 - unit_fahrstuhl_4020_etagen (Failed)
# danach den Fix wiederhergestellt -> beide gruen. Die Riegel messen also wirklich
# diesen Defekt und nicht bloss sich selbst.
add_executable(probe_elevator_1080_full probe_elevator_1080_full.c)
target_link_libraries(probe_elevator_1080_full PRIVATE re15_engine re15_test_support)
target_include_directories(probe_elevator_1080_full PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Der ZWEITE Fahrstuhl (ROOM4020 -> 0x4000 / 0x5000 / 0x50C0). Gleiche Fehlerklasse,
# laut Zensus messung-fahrstuhl.md §5 einer von vier Raeumen mit mehr als einer
# Null-Rect-Cross-Room-Tuer.
add_executable(probe_elevator_4020_full probe_elevator_4020_full.c)
target_link_libraries(probe_elevator_4020_full PRIVATE re15_engine re15_test_support)
target_include_directories(probe_elevator_4020_full PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME unit_fahrstuhl_1080_etagen COMMAND probe_elevator_1080_full)
add_test(NAME unit_fahrstuhl_4020_etagen COMMAND probe_elevator_4020_full)
