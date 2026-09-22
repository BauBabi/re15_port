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
# TEIL I (Runde 23): die Spanne "vorgemerkt" wird je Benutzungsstelle in ihre ANTEILE
# zerlegt (Nachricht / Szene / Faden-Ende / Folge-Nachricht) und muss GENAU dort enden, wo
# das Belegt-Bit des Nachrichtensystems frei wird - RE2s einzige Schranke, @0x800517e0 lbu
# DAT_800e873c / @0x800517f0 andi 0x80 / @0x800517f4 bne. Gemessen: 10 von 16 Stellen
# gefahren, 6 mit Grund ausgelassen; der UEBERHANG des Runde-22-Modells betrug 887 Bilder,
# davon 191 mit freiem Pad (groesstes Loch ROOM1100 msg 4 mit 80 Bildern) - zwei PRUEFE
# erzwingen, dass dieser Ueberhang messbar war, sonst wuerde der Riegel nichts pruefen.
# TEIL J (Runde 23): WAS DER SPIELER SPUERT - das px/pz-Delta hinter dem ECHTEN
# re15_game_step, mit gehaltenem VORWAERTS, in genau den Bildern mit vorgemerkter,
# unsichtbarer Abfrage. Drei Spalten im selben Lauf: JETZT 0 / GEGENPROBE 6357 Einheiten
# ueber die Ueberhang-Spanne / STUB (ohne re15_game_step) 0. Je Stelle zusaetzlich: die
# Abfrage geht auf, und nach der Antwort laeuft der Spieler wieder (oder es ist belegt, DASS
# Szene oder Folge-Nachricht ihn halten - nie die Abfrage).
# ⛔ Der Vorgaenger (TEIL G) war eine TAUTOLOGIE: re15_discard_pad_locked() gab woertlich
# dasselbe zurueck wie re15_discard_active(), die Lochbedingung war damit `x && !x` = 0.
# TEIL K (Runde 23): der SICHTBARE Prompt friert das Skript ein (@0x80051844 lui v1,0xff00 /
# @0x80051850 sw DAT_800cfbdc; SCD-Gate @0x8003f044) und es laeuft nach der ANTWORT weiter -
# ROOM1090 sub03 parkt 200 Bilder auf Datei-0x2507 und laeuft danach bis 0x259F, das
# Szenen-Fenster @0x2508 geht auf. Genau der Fall, an dem Runde 21 sich irrte (dort wurde
# die Frage nie beantwortet).
# SACKGASSEN-RIEGEL: Vollzensus ueber alle 206 RDTs mit Header (+34 Stummel) und ~40 700
# Opcodes, gelaufen mit dem Laengen-Vorschub des Motors selbst (scd_opcode_size_at):
# 0 x Keep_Item_ck (der einzige Inventar-Leser), 0 x Zone-9-Bit geloescht. Gegenprobe
# gegen einen stehenbleibenden Walker: die 164 Item_aot_set und die drei Kartenleser-Tore
# Ck(9,52)/Ck(9,136)/Ck(9,138) muessen gesehen werden.
# Dossier: analysis/befunde_2026-09-22/discard-fenster.md (Runde 23),
#          analysis/befunde_2026-09-22/discard-loch.md (Runde 22),
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
