# Runde 30, Nachschliff "sicherung-nein" — Riegel fuer die Antwort "No" im Sicherungs-Modal.
#
# Dossier: analysis/befunde_runde30/nachschliff-sicherung-nein.md
#
#   unit_r30_sicherung_nein   ROOM1150 UND ROOM1151, je zwei Faelle:
#       A  Fahrt 1 "No" -> genau EIN Modal, Sicherung bleibt liegen, Parklage -20224;
#          Fahrt 2 -> das Modal geht WIEDER auf, "Yes" -> Flag (9,53), Prop weg, Item 0x40
#          im Inventar; Fahrt 3 -> kein Modal
#       B  Fahrt 1 "Yes" -> Fahrt 2 ohne Modal, keine zweite Aufnahme, Prop bleibt weg
#   Original-Regel (abgelehnte Aufnahme laesst die Zone scharf): RE1.5 @0x8001e090 /
#   @0x8001e0ec, RE2 @0x800720cc / @0x80072298 — Kopf von probe_r30_sicherung_nein.c.
add_executable(probe_r30_sicherung_nein
    ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_nein.c)
target_link_libraries(probe_r30_sicherung_nein PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_sicherung_nein PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_r30_sicherung_nein COMMAND probe_r30_sicherung_nein)
set_tests_properties(unit_r30_sicherung_nein PROPERTIES TIMEOUT 60)
