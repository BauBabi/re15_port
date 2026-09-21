# Runde 21, Nutzer-Auftrag 2026-09-21: "bei KEy Items, die verwendet wurden, und nicht mehr
# im Spiel danach benoetigt werden genauso wie in Resident Evil 2 fragst - This Item is not
# used anymore - drop it? Yes No".
#
# Wortlaut: RE1.5 hat einen EIGENEN — "You don't need this key any more. Discard it?"
# (DEBUG.BIN @0x800C508B, Prompt-Skript [6] der Tabelle @0x800C4FC6). Regelform byte-true
# von RE2 uebernommen (LAB_80051718: Zaehler @0x80051810, Frage @0x80051834, Nein-Zweig
# @0x800517C4). Herleitung: include/re15_item_discard.h.
#
# ABDECKUNG: alle 17 erzeugten Benutzungsstellen (= 9 Gegenstaende x ihre Raumvarianten,
# engine/src/gen/discard_sites.inc), je 3 Faelle (Ja / Nein / Gegenstand nicht getragen)
# plus je eine GEGENPROBE mit einer anderen Nachricht desselben Raums.
# SACKGASSEN-RIEGEL: Vollzensus ueber alle 206 RDTs mit Header (+34 Stummel) und ~40 700
# Opcodes, gelaufen mit dem Laengen-Vorschub des Motors selbst (scd_opcode_size_at):
# 0 x Keep_Item_ck (der einzige Inventar-Leser), 0 x Zone-9-Bit geloescht. Gegenprobe
# gegen einen stehenbleibenden Walker: die 164 Item_aot_set und die drei Kartenleser-Tore
# Ck(9,52)/Ck(9,136)/Ck(9,138) muessen gesehen werden.
# Dossier: analysis/befunde_2026-09-21/discard-umsetzung.md
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
set_tests_properties(unit_r21_discard_wegwerfen PROPERTIES TIMEOUT 60)
