# Runde 41 Spur Form, Nachbesserung 4 (Abnahme 2 M-A/M-B): Kamera der Rueckstoss-Szene im echten Wagenweg.
# Dossier analysis/befunde_runde41/form/STUFE3.md Abschnitt "Nachbesserung 4 (Rueckstoss-Kamera)".
#
# MESS-WERKZEUG (kein add_test): Anteil fast schwarzer Bildpunkte je Framedump-Bild (Luma < 24, wie abn2/werkzeug/hell.py).
add_executable(probe_r41_form_kamera_bild ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_form_kamera_bild.c)

# INTEGRATION (echte exe, tests/integration/test_r41_form_wagenweg.cmake), bisher in tests/unit/CMakeLists.txt registriert:
#   integration_r41_form_wagenweg               Leon, Wagenweg A -> B -> C (Nachbesserung 3) + Kamera der Szene (Nachbesserung 4)
#   integration_r41_form_wagenweg_elza          dasselbe mit Elza (ROOM5091)
#   integration_r41_form_wagenweg_zurueck_leon  Leon weicht nach dem Kampfstart 2 s zurueck (Abnahme 2 Lauf v5090)
#   integration_r41_form_wagenweg_zurueck_elza  Elza dasselbe (Abnahme 2 Lauf v5091)
#   integration_r41_form_wagenweg_warten_leon   Leon wartet nach dem Kampfstart 24 s (Abnahme 2/3 Lauf k5090), Pruefung 7
#                                               Kampfkamera (Nachbesserung 4 Runde 2, Abnahme 3 N1)
#   integration_r41_form_wagenweg_warten_elza   Elza dasselbe
# Lauf A2 (Integration Runde 41 Welle 2, Form-HP 1000): leon/elza/zurueck_* toeten die Form mit dem Raketenwerfer (4 x 301,
# RE2-G4 @0x800A5D24) nach ihrem Ansturm; Laufende und Framedump-Ende = F2000 (Szenenende gemessen F1705 / F1779), warten_* wie bisher
# Pistole bis F1931. Laufzeit je Teil gemessen 104-115 s, das TIMEOUT 600 bleibt. Schranken und Gegenproben: Kopf des Skripts.
if(TARGET re15_pc)
    foreach(_teil leon elza zurueck_leon zurueck_elza warten_leon warten_elza)
        if(_teil STREQUAL "leon")
            set(_name integration_r41_form_wagenweg)
        else()
            set(_name integration_r41_form_wagenweg_${_teil})
        endif()
        add_test(NAME ${_name}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_form_wagenweg_${_teil}_wd
                         -DRE15_FORM_HP_HEADER=${CMAKE_SOURCE_DIR}/include/re15_birkin_formen.h
                         -DRE15_BILD_TOOL=$<TARGET_FILE:probe_r41_form_kamera_bild>
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_form_wagenweg.cmake)
        set_tests_properties(${_name} PROPERTIES TIMEOUT 600)
    endforeach()
endif()
