# Runde 41 (2026-11-01), Spur W "wasserblink" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde41/W_wasserblink.md. Code: engine/src/raumlicht.c (Fall ROOM2000/2001 entfernt).
#
#   unit_raumlicht                      (tests/unit/CMakeLists.txt) Tabelle: 2000/2001 in 6000 Bildern kein Licht, 50B0 drei Scheine
#   integration_r41_wasserblink_leon    echte exe, Leon ROOM2000 ueber die RVD-Zonen 0->1->2 auf Cut 2: Kanal ohne Flackern ueber
#                                       120 Bilder, Bild gleich dem Lauf mit RE15_KEIN_RAUMLICHT, Wasserlauf der Spur N laeuft
#   integration_r41_wasserblink_elza    dasselbe fuer Elza ROOM2001
#   integration_r41_wasserblink_gegen   Gegenprobe: ROOM50B0 Cut 2 zeigt den Raumlicht-Schein (Rot-Zuwachs gegen KEIN_RAUMLICHT),
#                                       der Messweg sieht also, was der Raumlicht-Pfad zeichnet
add_executable(test_r41_wasserblink ${CMAKE_CURRENT_LIST_DIR}/../test_r41_wasserblink.c)
add_test(NAME unit_r41_wasserblink_werkzeug COMMAND test_r41_wasserblink)
set_tests_properties(unit_r41_wasserblink_werkzeug PROPERTIES WILL_FAIL TRUE TIMEOUT 30)   # ohne Argumente: Rueckgabe 2

if(TARGET re15_pc)
    foreach(_teil leon elza gegen)
        add_test(NAME integration_r41_wasserblink_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_TOOL=$<TARGET_FILE:test_r41_wasserblink>
                         -DTEIL=${_teil}
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_wasserblink_${_teil}_wd
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_wasserblink.cmake)
        set_tests_properties(integration_r41_wasserblink_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
