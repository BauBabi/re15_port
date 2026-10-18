# Spur B (Runde 39) "toene" — Schubladen-Ton ROOM11F0/11F1 (RE2 ROOM1130 Satz 0x28), Lueftungsgitter-Toene der
# Hunde-Szene ROOM1190/1191 (RE1.5 ROOM2060 snd0 0x0E/0x0D, die Schachtgitter der Spinnen) und Schrank-Ton nach dem
# Zielscheiben-Raetsel (RE2 ROOM1120 Satz 0x0C, die Regale der Bibliothek 2F). Dossier analysis/befunde_runde39/B_toene.md,
# Modul engine/src/toene_r39.c + platform/pc/src/toene_r39_pc.c, Bank shared_assets/RE2/TOENE39.VBS.
#
# RIEGEL (Unit): misst die MECHANIK am echten VM (RDT laden, scd_room_reenter, Anker-Scan, Opcode-Haken, Requisiten).
#   schublade gitter schrank bank signatur
add_executable(test_r39_toene ${CMAKE_CURRENT_LIST_DIR}/../test_r39_toene.c)
target_link_libraries(test_r39_toene PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_toene PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_toene PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil schublade gitter schrank bank signatur)
    add_test(NAME unit_r39_toene_${_teil} COMMAND test_r39_toene ${_teil})
    set_tests_properties(unit_r39_toene_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

# AUSWERTE-WERKZEUG (kein add_test): Kreuzkorrelation eines TOENE39.VBS-Satzes (oder Raumbank-Satzes) mit einem
# RE15_AUDIO_CAP_SYNC-Mitschnitt an einem Raum-Bild (Verfahren probe_r38_elzatoene_kreuz.c).
add_executable(probe_r39_toene_kreuz ${CMAKE_CURRENT_LIST_DIR}/../probe_r39_toene_kreuz.c)
target_link_libraries(probe_r39_toene_kreuz PRIVATE re15_engine m)   # m: sqrt (glibc-libm; Linux-Release v0.8.26 brach ohne ab)
target_include_directories(probe_r39_toene_kreuz PRIVATE ${CMAKE_SOURCE_DIR}/include)

# INTEGRATION (echte exe, Spielstand + CONTINUE + Tuer per Viereck-Druck am Tuersatz): je Nutzer-Punkt und Figur ein Lauf,
# gemessen in debug.log ([toene39]-Anker, Cut_chg) und im RE15_AUDIO_CAP_SYNC-Mitschnitt (Kreuzkorrelation je Ton).
#   schublade / schublade_elza  ROOM11E0/11E1 -> Tuer 0 -> ROOM11F0/11F1, Schreibtisch AOT 13 -> sub19 (Punkt 1)
#   szene / szene_elza          ROOM1180/1181 -> Tuer 2 -> ROOM1190/1191, Raetsel geloest -> sub10 (Punkte 2 + 3)
if(TARGET re15_pc)
    foreach(_teil schublade schublade_elza szene szene_elza)
        add_test(NAME integration_r39_toene_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_elza_karte>
                -DRE15_KREUZ_TOOL=$<TARGET_FILE:probe_r39_toene_kreuz>
                -DRE15_TOENE39=${CMAKE_SOURCE_DIR}/shared_assets/RE2/TOENE39.VBS
                -DWORKDIR=${CMAKE_BINARY_DIR}/r39_toene_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_toene.cmake)
        set_tests_properties(integration_r39_toene_${_teil} PROPERTIES TIMEOUT 1200)
    endforeach()
endif()
