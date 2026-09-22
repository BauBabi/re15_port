# Runde 21, Nutzer-Auftrag 2026-09-21: "bei KEy Items, die verwendet wurden, und nicht mehr
# im Spiel danach benoetigt werden genauso wie in Resident Evil 2 fragst - This Item is not
# used anymore - drop it? Yes No".
#
# Wortlaut: RE1.5 hat einen EIGENEN — "You don't need this key any more. Discard it?"
# (DEBUG.BIN @0x800C508B, Prompt-Skript [6] der Tabelle @0x800C4FC6). Regelform byte-true
# von RE2 uebernommen (LAB_80051718: Zaehler @0x80051810, Frage @0x80051834, Nein-Zweig
# @0x800517C4). Herleitung: include/re15_item_discard.h.
#
# ABDECKUNG: alle 16 erzeugten Benutzungsstellen (= 9 Gegenstaende x ihre Raumvarianten,
# engine/src/gen/discard_sites.inc; ROOM4001 msg 2 ist seit Runde 22 draussen - der Text
# steht im Nachrichtenblock, aber KEIN Message_on-Opcode gibt ihn aus, Aufnahmebedingung D
# des Generators), je 3 Faelle (Ja / Nein / Gegenstand nicht getragen) plus je eine
# GEGENPROBE mit einer anderen Nachricht desselben Raums.
#
# TEIL G (Runde 22): kein Bild mit VORGEMERKT und PAD FREI und UNSICHTBAR - die Lage, in
# der RE2 waehrend derselben Spanne ist (0xFF000000 von der ausloesenden Nachricht
# gehalten: @0x80051650 -> @0x8002fe90 -> FUN_8003027c case 0, zurueck erst LAB_800307e0,
# sofort wieder @0x80051850). Gegenprobe im selben Lauf gegen den Stand davor.
# TEIL H (Runde 22): Raumwechsel im Wartefenster ueber den ECHTEN Ladeweg
# (scd_room_reenter) - die Vormerkung ueberlebt, genau wie RE2s DAT_800d4498 (sechs
# Schreibstellen, keine auf dem Raum-Pfad). Gegenprobe: das alte, unbedingte
# re15_discard_reset() verliert sie.
# SACKGASSEN-RIEGEL: Vollzensus ueber alle 206 RDTs mit Header (+34 Stummel) und ~40 700
# Opcodes, gelaufen mit dem Laengen-Vorschub des Motors selbst (scd_opcode_size_at):
# 0 x Keep_Item_ck (der einzige Inventar-Leser), 0 x Zone-9-Bit geloescht. Gegenprobe
# gegen einen stehenbleibenden Walker: die 164 Item_aot_set und die drei Kartenleser-Tore
# Ck(9,52)/Ck(9,136)/Ck(9,138) muessen gesehen werden.
# Dossier: analysis/befunde_2026-09-22/discard-loch.md (Runde 22),
#          analysis/befunde_2026-09-21/discard-umsetzung.md (Ursprung)
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), s. probes/README.md.
add_executable(r21_discard_wegwerfen
    ${CMAKE_CURRENT_SOURCE_DIR}/r21_discard_wegwerfen.c)
target_link_libraries(r21_discard_wegwerfen PRIVATE re15_engine re15_test_support)
target_include_directories(r21_discard_wegwerfen PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(r21_discard_wegwerfen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r21_discard_wegwerfen COMMAND r21_discard_wegwerfen)
set_tests_properties(unit_r21_discard_wegwerfen PROPERTIES TIMEOUT 120)
