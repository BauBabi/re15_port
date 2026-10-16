# Runde 38 (2026-10-16), Spur E "elzaintro" — Elzas Vorspann: dieselbe Stimme wie Leon in der Montage ROOM1241,
# Erzaehler-Schwarz ROOM1031 Cut 13 ohne durchscheinenden Lageplan. Dossier analysis/befunde_runde38/E_elzaintro.md,
# Regel include/re15_elzaintro_r38.h, Tabellen engine/src/elzaintro_r38.c (Haken audio_pc.c re15_voice_load_clip,
# main.c Netz s_scene_black, beide "Runde 38 Spur E").
#
# RIEGEL (Unit), messen die Zuordnung gegen die ausgelieferten Daten:
#   unit_r38_elzaintro_stimme   ROOM1240/1241 msg 0..5 byte-gleich, Message_on 0..5 an den Offsets beider sub02,
#                               Zuordnung (1241, 0..5) -> room1240 und nichts sonst, Ersatzpfad, die sechs WAVs da
#   unit_r38_elzaintro_schwarz  Erzaehler-Tabelle: je Zeile `29 cut` am Offset = einziges Cut_chg auf den Schnitt
#                               (Opcode-Walk aller Regionen), keine RVD-Zone fuehrt hinein, Kamera = Leons Cut 7,
#                               Standbild = ROOM1170 BG07; Gate nur fuer genau diese zwei (Raum, Cut)
add_executable(test_r38_elzaintro ${CMAKE_CURRENT_LIST_DIR}/../test_r38_elzaintro.c)
target_link_libraries(test_r38_elzaintro PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_elzaintro PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_elzaintro PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_SYNCHRO_DIR="${CMAKE_SOURCE_DIR}/../synchro")
foreach(_teil stimme schwarz)
    add_test(NAME unit_r38_elzaintro_${_teil} COMMAND test_r38_elzaintro ${_teil})
    set_tests_properties(unit_r38_elzaintro_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

# AUSWERTE-WERKZEUG (kein add_test): RMS je Bildfenster eines RE15_AUDIO_CAP_SYNC-Mitschnitts + Bildvergleich.
add_executable(probe_r38_elzaintro_auswertung ${CMAKE_CURRENT_LIST_DIR}/../probe_r38_elzaintro_auswertung.c)
if(NOT WIN32)
    target_link_libraries(probe_r38_elzaintro_auswertung PRIVATE m)   # sqrt (Linux-Release-Bau)
endif()

# INTEGRATION (echte exe, Neues Spiel ueber Titel + Charakterwahl = der echte Weg, je Leon UND Elza):
#   integration_r38_elzaintro_stimme   Elza laedt msg 0..5 aus room1240, Mitschnitt je Zeilenfenster = Leon (+-10 %),
#                                      Montage-Ablauf = Leon (msg 5 geparkt, Uebergabe im selben Bild)
#   integration_r38_elzaintro_schwarz  Erzaehlerbilder F60/F230/F400 Elza (ROOM1031) = Leon (ROOM1170) pixelgleich,
#                                      obwohl der Lageplan in ROOM1031 angelegt ist; Feld x 91..98 / y 107..112 schwarz
if(TARGET re15_pc)
    foreach(_teil stimme schwarz)
        add_test(NAME integration_r38_elzaintro_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_AUSWERT_TOOL=$<TARGET_FILE:probe_r38_elzaintro_auswertung>
                -DWORKDIR=${CMAKE_BINARY_DIR}/r38_elzaintro_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_elzaintro.cmake)
        set_tests_properties(integration_r38_elzaintro_${_teil} PROPERTIES TIMEOUT 1200)
    endforeach()
endif()
