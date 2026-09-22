# Runde 21, Nutzer-Auftrag 2026-09-21: "bei KEy Items, die verwendet wurden, und nicht mehr
# im Spiel danach benoetigt werden genauso wie in Resident Evil 2 fragst - This Item is not
# used anymore - drop it? Yes No".
#
# Wortlaut: RE1.5 hat einen EIGENEN - "You don't need this key any more. Discard it?"
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
# TEIL L (Runde 24): DAS BESITZ-GATE, gefahren an den AUSGELIEFERTEN Unterprogrammen
# (nicht an der 6-Byte-Ersatzfolge aus TEIL A) mit einem Spieler, der jede Ja/Nein-Frage
# des Skripts mit JA beantwortet. Gemessen: 10 von 16 Stellen im Pruefstand erreicht,
# davon 0 mit Abfrage OHNE Besitz und 10 mit Abfrage MIT Besitz (das ist die Gegenprobe:
# ohne sie stuende der Riegel auch gruen, wenn die Abfrage nie kaeme). Dazu die Zaehler des
# Gates selbst - 20 Vorentscheide, 10 mit Treffer - und die KETTENPOSITION: 0 von 20
# Vorentscheiden fielen, waehrend das Nachrichtensystem schon belegt war. RE2 entscheidet
# die Besitzfrage VOR dem Oeffnen der Nachricht: @0x80051628 `jal FUN_800696cc` sucht den
# Platz, @0x80051634 `bltz s1,LAB_800516a0` verzweigt ohne Treffer, erst @0x8005164C
# `jal FUN_8002fe38` oeffnet die Zeile und @0x80051670 haengt die Fortsetzung ein; der
# Nicht-Treffer-Zweig haengt NICHTS ein (@0x800516C0 `j LAB_800516f8`).
# RUECKBAU dieser Zahl (Aufruf zurueck in re15_discard_notice_message verlegt, also hinter
# das Oeffnen): 20 von 20 - die Zahl ist also keine Tautologie.
#
# TEIL N (Runde 24): ROOM4000 und ROOM1090 in BEIDEN Faellen. Ausgewiesen wird auch das
# Tuer-/Tor-Flag des Skripts, nicht nur die Abfrage: ROOM4000 sub02 hat KEINE Besitz-
# pruefung (@0x0142A `Ck(3,32,0)`, @0x01446 `Ck(12,31,0)` = die Ja-Antwort, @0x01450
# `Set(3,32,1)`), und die Blue Master Keycard 0x47 wird im ganzen Spiel nur in ROOM4010
# ausgegeben - also HINTER dieser Tuer. Ohne Besitz faellt `Set(3,32,1)` deshalb
# byte-true, und das MUSS so sein, sonst kaeme niemand an die Karte. Was der Port
# verantwortet und hier haelt: keine Abfrage ohne Besitz.
# TEIL C6 dazu: flag(3,32) hat im ganzen ausgelieferten Bestand GENAU EINEN Schreiber und
# keinen Loescher - die Stelle ist einmalig und endgueltig. Vollstaendige Liste aller
# 16 Stellen: tools/discard_verlustwege.py (10 ohne Besitztor im Skript, 1 mit endgueltig
# geschlossenem Tor).
#
# TEIL M (Runde 24): der NACHHALL-UEBERLAPP. RE2 kennt den Zustand "Text steht, Freeze
# geloest" nicht (@0x800307e8 `andi v0,v0,0x7f` und @0x800307f4 `sw v1,DAT_800cfbdc` sind
# ein Paar im selben Block; und die Abfrage wird mit DEMSELBEN Aufruf geoeffnet wie die
# Zeile davor, @0x80051834 gegen @0x8005164C). Der Untertitel-Nachhall des Ports
# (msg-FSM Zustand 7, Nutzer-Entscheidung 2026-09-20) BLEIBT, ueberlappt die Abfrage aber
# nicht mehr: gemessen 4 Bilder im Rueckbau, 0 Bilder jetzt.
#
# TEIL I (Runde 23, KORRIGIERT in Runde 24): die Spanne "vorgemerkt" wird je
# Benutzungsstelle in ihre ANTEILE zerlegt (Nachricht / Szene / Faden-Ende / Folge-
# Nachricht) und muss GENAU dort enden, wo das Belegt-Bit des Nachrichtensystems frei wird
# - RE2s einzige Schranke, @0x800517e0 lbu DAT_800e873c / @0x800517f0 andi 0x80 /
# @0x800517f4 bne. Gemessen: 10 von 16 Stellen gefahren, 6 mit Grund ausgelassen; der
# UEBERHANG des Runde-22-Modells betrug 887 Bilder, davon 191 mit freiem Pad (groesstes
# Loch ROOM1100 msg 4 mit 80 Bildern) - zwei PRUEFE erzwingen, dass dieser Ueberhang
# messbar war, sonst wuerde der Riegel nichts pruefen.
# ⛔ KORREKTUR DES ZEUGEN: gepinnt wird jetzt RE15_PAUSE_PLAYER 0x80000000 (@0x80031c54
# `lw a0,g_pauseflags` / @0x80031c78 `bltz a0,0x80031da8`), nicht mehr RE15_PAUSE_PAD
# 0x01000000. Das Pad-Bit maskiert nur die SCD-Pad-Woerter (game_step_common.c:1061-65)
# und wird zusaetzlich vom Szenen-Fenster gehalten - deshalb meldete der Riegel 4
# Bruecken-Bilder. Mit dem richtigen Zeugen sind es 1 Bild an 10 von 10 Stellen.
# ⛔ UND DER ABSOLUTSATZ IST WEG: das Fenster ist ABGESICHERT, nicht verschwunden. Aus 161
# Bildern Ueberhang (ROOM1100/1101) ist 1 Bild je Stelle geworden, gehalten von der
# Port-Zeile re15_discard_frozen(); RE2 hat hier 0 Bilder (@0x80051810 dekrementiert,
# @0x80051844 `lui v1,0xff00` friert im SELBEN Aufruf wieder ein).
#
# TEIL J (Runde 23, ERWEITERT in Runde 24): WAS DER SPIELER SPUERT - das px/pz-Delta
# hinter dem ECHTEN re15_game_step, mit gehaltenem VORWAERTS. Vier Spalten im selben Lauf:
# JETZT 0 / RUECKBAU des einen Bruecken-Bildes 680 Einheiten an 8 von 10 Stellen (davon
# 605 an den 9 Stellen mit Ueberhang-Spanne und 75 in ROOM1090) / GEGENPROBE 6357 Einheiten
# ueber die Ueberhang-Spanne / STUB (ohne re15_game_step) 0.
# ⛔ ROOM1090 WIRD JETZT AUSGEWIESEN, nicht stillschweigend uebersprungen: sein Faden endet
# im Pruefstand nicht, weil sub03 `Gosub 5` macht und sub05 eine Warteschleife ist
# (@0x026F4 `Do`, @0x026FC `Ck(5,33,0)`, @0x02700 `Evt_end`). Bis Runde 23 fiel die Stelle
# per `continue` heraus und zaehlte damit als Erfolg.
# ⛔ Der Vorgaenger (TEIL G) war eine TAUTOLOGIE: re15_discard_pad_locked() gab woertlich
# dasselbe zurueck wie re15_discard_active(), die Lochbedingung war damit `x && !x` = 0.
# TEIL K (Runde 23): der SICHTBARE Prompt friert das Skript ein (@0x80051844 lui v1,0xff00 /
# @0x80051850 sw DAT_800cfbdc; SCD-Gate @0x8003f044) und es laeuft nach der ANTWORT weiter -
# ROOM1090 sub03 parkt 200 Bilder auf Datei-0x2507 und laeuft danach bis 0x259F, das
# Szenen-Fenster @0x2508 geht auf.
# SACKGASSEN-RIEGEL: Vollzensus ueber alle 206 RDTs mit Header (+34 Stummel) und ~40 700
# Opcodes, gelaufen mit dem Laengen-Vorschub des Motors selbst (scd_opcode_size_at):
# 0 x Keep_Item_ck (der einzige Inventar-Leser), 0 x Zone-9-Bit geloescht. Gegenprobe
# gegen einen stehenbleibenden Walker: die 164 Item_aot_set und die drei Kartenleser-Tore
# Ck(9,52)/Ck(9,136)/Ck(9,138) muessen gesehen werden.
# Dossier: analysis/befunde_2026-09-22/discard-besitz.md   (Runde 24, GUELTIG),
#          analysis/befunde_2026-09-22/discard-fenster.md  (Runde 23),
#          analysis/befunde_2026-09-22/discard-loch.md     (Runde 22),
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
set_tests_properties(unit_r21_discard_wegwerfen PROPERTIES TIMEOUT 240)
