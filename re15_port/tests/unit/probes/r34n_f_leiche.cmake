# Runde 34 Nacht, Spur F (Leichen ROOM1110 / ROOM1230): neuer Untersuchen-Text + einmal Handgun-Munition.
# Dossier: analysis/befunde_runde34_nacht/F_leichen.md (Abschnitte 2, 4, 6, 9).
#
# MESS-SONDE probe_r34n_f_leiche (kein Pin): Ist-Zeitlinie der zwei Original-Leichen-Ereignisse
# (ROOM1110 Slot 5 -> sub02, ROOM1230 Slot 18 -> sub21) und die Simulation des Bauplans.
add_executable(probe_r34n_f_leiche ${CMAKE_CURRENT_SOURCE_DIR}/probe_r34n_f_leiche.c)
target_link_libraries(probe_r34n_f_leiche PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_f_leiche PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34n_f_leiche PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# RIEGEL test_r34n_f_leiche — die ECHTEN Haken (re15_leiche_message_on in op_message_on,
# re15_leiche_tick in re15_game_step), je Teil ein ctest-Eintrag:
#   texte      eingebackene Texte = RDT-Belegstellen (ROOM1110 @0x0D68/@0x0DA1/@0x0DD3,
#              ROOM1230 @0x16F4/@0x1720/@0x1752, ROOM1011 @0x012A3)
#   nein_*     Druck -> Nachricht 20/22 statt 0/10, Modal im Schliess-Bild, "No" -> Bit 0, wiederholbar
#   ja_*       "Yes" -> Bit (9,61)/(9,62), +7 H. Gun Bullets gestapelt, danach Nachricht 21/23 ohne Modal
#   voll       "can't carry", Bit 0, wiederholbar
#   varianten  ROOM1111 / ROOM1231
#   laden      Bit vor dem Raumaufbau -> kurzer Text; Speicherstand traegt Bit 61/62
#   andere     alle uebrigen Nachrichten beider Raeume unveraendert (u.a. ROOM1230 msg 0 = Tastenfeld)
#   stimme     Haken HINTER dem Stimmen-Riegel (Gegenpruefung Auflage 1)
#   nachhall   eigene Aufnahme laenger als der Text -> Modal trotzdem im Freeze-Bild (ein Kanal,
#              FUN_80027e68 Open-Guard @0x80027e74..80)
#   latch      offener Text verschwindet ohne Schliessen (Raum-Neuaufbau / fremde Raum-Basis) -> kein
#              Modal (Gegenpruefung Auflage 5)
add_executable(test_r34n_f_leiche ${CMAKE_CURRENT_SOURCE_DIR}/test_r34n_f_leiche.c)
target_link_libraries(test_r34n_f_leiche PRIVATE re15_engine re15_test_support)
target_include_directories(test_r34n_f_leiche PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r34n_f_leiche PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil texte nein_1110 nein_1230 ja_1110 ja_1230 voll varianten laden andere stimme nachhall latch)
    add_test(NAME unit_r34n_f_leiche_${_teil} COMMAND test_r34n_f_leiche ${_teil})
    set_tests_properties(unit_r34n_f_leiche_${_teil} PROPERTIES TIMEOUT 120)
endforeach()
