# Runde 41 Spur N "nachtrag30" - unabhaengige Abnahme 1

Baum `.claude/worktrees/r41_nachtrag30`, Zweig `r41/nachtrag30`, Kopf 82db331e9, Merge-Basis 6e213a5a5 (master ist 91 Commits weiter). Datum 2026-10-31.
Bau: `local_build.sh configure` + `build` exit 0, "ninja: no work to do", exe 07:24:27 (nach f74597a3e; 648d15f1b und 82db331e9 aendern nur Dossier und Testdatei).
Messordner (nicht committet): scratchpad `n1/`, Laeufe `n1/l/<name>/`, Treiber `n1/run.py`: eigene exe-Kopie `re15_pc_abn1n.exe`, eigener Laufordner, SDL_RENDER_DRIVER=direct3d (kein RE15_SOFTWARE_RENDER), `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1`, Bilder per RE15_FRAMEDUMP; Gegenprobe am echten Fenster per PrintWindow (`n1/fenster.py`, kein Readback).

## Ergebnis

(Zwischenstand) Im Spurbaum alle sieben Punkte erfuellt, die fuenf Maengel aus Abnahme 0 behoben. Am Stand von master (Merge mit Spur O) verpufft das erste USE der Sicherung am geschlossenen Schrank, siehe "Merge mit master".

## Punkt 1: Wasser 2010

**Wortlaut:** "in room 2010 soll - bis zum ablassen der Wassers, auch ein Wasser Effekt die Leiter runter kommen und ueber den Weg bis zum Wasser Pool in der Mitte laufen." Nutzer-Wahl: bis zur Flut in 2030.

* **Raum:** Der Nutzer selbst schreibt im Vorab-Design (AUFTRAG.md Zeile 11, Wortlaut): "in ROOM 2000 die Leiter zu ROOM 2050 mit herunterlaufenden Wasserstrom zu blockieren ... Nach dem Steigen des Wassers dort, sollte die Leiter frei sein, also kein Wasser mehr runter laufen." Das stuetzt die Wahl ROOM2000/2001 staerker als die Herleitung im Dossier. ROOM2010 hat keine Leiter (Abnahme 0, alle Cuts).
* **Lauf:** `w10` (Cut 10, F430..F461) gegen `w10aus` (Bit 100), Spaltenprofil der Differenz, eigene Auswertung `n1/fluss2.py`: Verschiebung 2 / 5 / 8 / 12 / 14 Bildpunkte fuer 1..5 Bilder Abstand, Korrelation 0,77 .. 0,45; Bedeckung der Flaeche x 72..124, y 146..179: 44 %, 65 % der Punkte wechseln mindestens fuenfmal. Cut 9 (`w9`/`w9aus`, x 90..149, y 120..149): 1 / 3 / 5 / 7 / 9 Bildpunkte, Bedeckung 13 %. Das Wasser wandert nach rechts zum Kanal; die Zahlen des Bau-Agenten stimmen (Cut 10 2/5/8 exakt, Cut 9 je 1..2 Bildpunkte weniger durch die andere Methode).
* **Bild:** `n1/w10_zoom.png`, `n1/sh_wgeh.png`. Strom ueber die ganze Leiter, Spritzer von der Leiter bis zur Kante, Einschlaege im Kanal. Echtes Fenster `n1/g_w.png` (28 Aufnahmen): gleiches Bild.
* **Echter Weg:** `wgeh`: `RE15_FIRE_AOT=0@300#2010`, Wegplan zu Fuss: `[wasserlauf41] ROOM2000: 6 Strahlen`, Kamera Cut 14 -> 9 -> 10. Elza: `w10e` (ROOM2001, direct3d), Bank ROOM2001 traegt 0x0F (ids 05 06 07 0b 0c 0d 0f @0x14A20). CONTINUE: Riegel `_laden_*` gruen, je genau eine Zeile `[wasserlauf41]` im Log (keine doppelte Installation).
* **Ende:** Riegel `_wasser_*` Bit 100, unit Bit 103. Log `_ton` Lauf sa: `Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100`.
* **Effekt-Pool:** `fxan`/`fxaus` (900 Bilder): Spitze 86 von 96 gegen 31 ohne Wasserlauf, das Dossier nennt hoechstens 80. Kein Sattlauf, nur die Zahl im Dossier ist zu niedrig.
* Der grau gefuellte Kanal in den Riegel-Bildern ist der Software-Renderer des Riegels, unter direct3d ist er schwarz (`w10e`).

**Urteil: erfuellt.**

## Punkt 2: Schrank 2010

* **Aufklappen, Bild:** `s_a`, `s_geh` (zu Fuss, Klick F233, Cut_chg(3) ohne Flackern, F235..F251 Tuer, dann Text). Bild neben fuse_box.bmp `n1/cmp_fuse.png`, Zoom `n1/s_a_zoom.png`. Elza: `_schrank_elza`, Bilder `n1/sh_se.png`.
* **Cursor-Schritt (Mangel 1):** `s_ja` F640 gegen `m2060` F500: **0 von 76800 Bildpunkten verschieden**. Bahn `c2010`/`c2060` mit gleicher Eingabe (je 15 Bilder hoch, links, rechts, runter): Mitte des Stempels in jeder Stichprobe gleich auf 0,5 Bildpunkte (`n1/cursorbahn.py`). Zonen-Raender `z10_*`/`z60_*`: rechts 11 aussen, rechts 10 innen, hoch 2 aussen, hoch 1 innen, runter 9 aussen, runter 8 innen, beide Raeume gleich. Kein Abbruch: ROOM2060 sub01 enthaelt nur die Polls @0x012C0/D8/F0/@0x01308/@0x0132E (Bytes `51 01 ..`), kein CROSS.
* **Folge:** Log `s_ja`: Frage 41 -> `Nahansicht ROOM2060 Cut 8, Cursor obj 4 bei (19571,-22542)` -> `Cut 9, Cursor (19771,-22542) auf Zone 7` -> Bit 125 -> Text 42 -> Standbild + Ton -> Cut_old. Klick von Norden `s_n`: Cut 4 -> Cut_chg(3) -> Tuer -> Text -> Cursor -> Standbild -> Cut 4, Bilder `n1/sh_sn.png`. Echtes Fenster `n1/g_s.png`: Cursor wandert, Text 42 auf Cut 9.
* **Cut 4 (Mangel 4):** `s_geh`: nach dem Klick zu Fuss nach Cut 4 und zurueck, Tuer nur unter Cut 3, `n1/sh_sg2.png`.

**Urteil im Spurbaum: erfuellt.** Merge-Lage siehe Abschnitt "Merge".

## Punkt 3: Panel 2030

* Lauf `p` (Riegel-Skript, eigene Bilder `n1/sh_p.png`): Frage 40, `offen: A 100 B 0 C 0`, A->B, B->C, C->A, `Anzeige 80 erreicht`, `GELOEST`, Text 43, Ja, Cut_chg(4), Motor F1250, Halt F1310, Text 44.
* Gegen `panel_entwurf_a.png` (F1060, Stand 50/20/30, Quelle C): mittlere Abweichung 3,3, 0,6 % der Punkte ueber 24. EXIT-Pfeil Entwurf (62,110,68), Spiel (60,110,67): Mangel 5 behoben.
* `p_ex` EXIT, `p_bed` Text 42, `p_voll` Text 47: wie Nachtrag 37. `p_keys`: SELECT oeffnet das Debug-Menue darueber (Original-Werkzeug, sichtbar), R1 und Dreieck tun nichts, die Tasten bleiben im Panel, CROSS verlaesst es. `p_tot`: RE15_KILL_AT im Panel: Tod laeuft, "YOU DIED" liegt ueber dem Panel, danach Rueckkehr zum Titel, kein Haenger (Gift kann das nicht ausloesen, es klemmt bei 1 HP @0x8003c174).
* START (Mangel 3): Riegel `_panel_*` Lauf st, Cursor `s_ja` START@660: kein `Inventar-Ton`.
* Echtes Fenster `n1/g_p.png` (32 Aufnahmen): Cursor wandert, Text ueber dem Band; die statische Textur von re15_render_pc_show_fmv wird also auch ohne Readback aktualisiert.

**Urteil: erfuellt.**

## Punkt 4: Notiz

* FILE31 selbst dekodiert (`n1/tim_beide.png`): "They mixed up / green and red, and on top of" + `that they swapped the / numbers of "black" and / "green"! What are they? ...`. Kein "black" in der Vertauschung, Anfuehrungszeichen sichtbar.
* Plakat im Spiel Leon + Elza (`n_pl`, `n_pl_e`, `n1/sh_nl.png`): 1. GREEN 01, 2. RED 02, 3. BLACK 09, 4. RED 08.
* Folge: Riegel `_weg`, `_weg_elza` (siehe Tests).

**Urteil: erfuellt.**

## Punkt 5: RE2-Ton der Plattform

* BRUECKE4080.EDT/VH/VB bytegleich RE2 ROOM4080.RDT @0xA3C0/@0xA480/@0xB2A0, @0x031F2 `36 02 0f 00`, @0x03314 `36 02 10 01`, EDT @0xA3FC `00 01 53 16`, @0xA400 `00 01 64 16` (eigener Vergleich).
* Zwilling `_ton` Lauf ba/bb, eigene Auswertung: Differenz ab capTick 668, RMS 1400..2150 bis zum Halt 727, dann Ausklang bis 806. Log: `se=15 vag=12` Stimme 6, `se=16 vag=13` Stimme 6.

**Urteil: erfuellt.**

## Punkt 6: RE2-Alarmton

* SUB_34.BGM bytegleich info/re2leon/COMMON/SOUND/SUB_34.BGM; MAIN25 == MAIN11 (md5 52613db8..); Tabelle 0x80074828[0x62] = 0xFF65; ROOM6000 @0x00D52 `54 00 01 00 46 40`; RE2 ROOM6040 @0x02360 `57 00 00 06 11 ff 00 00`.
* Zwilling Lauf sa/sb, eigene Auswertung: Sirene ab capTick 674, RMS(A-B) 2146..2232 in jedem 30-Bilder-Fenster bis zum Verlassen 2724, Gesamtpegel 2240..2750. Reihenfolge Warntext/Sirene/Flut: `integration_r41_ventilkette_alarm` (siehe Tests).

**Urteil: erfuellt.**

## Punkt 7: Lautstaerke

* Mitschnitte `_laut` eigener Lauf, eigene Auswertung: 2 499 000 Werte, 0 falsch fuer 50 (q 0x2000) und 10 (q 327), RMS-Verhaeltnis 0,25000 und 0,00998, Spitze 16681 -> 4170 -> 166; Log `[tuer] Sequenz Archiv 2 DOOR26`, `Schliesston 1`, Landung (6363,0,-14032).
* Nur ein Ausgabeweg: audio_pc.c:2005, Endstufe audio_pc.c:528 nach der Mischung.

**Urteil: erfuellt.**

## Merge mit master (Spur O ist seit 91cd9ec82 in master)

`git merge-tree --write-tree HEAD master` = Baum c0b70826f, Textkonflikt nur `test_r41_ventilkette_weg.cmake`, keine verwaisten Bezeichner (VENTIL13, 2030_v13). Baum exportiert und gebaut (scratchpad `merge/`, `local_build.sh`, exit 0, exe 10:51; Konfliktdatei ist nur ein Test), Laeufe `m_*`:

* **Mangel 1, gemessen.** Echter Weg, Inventar: Lauf `m_real` (`RE15_DEBUG_JUMP=2010@240 RE15_SET_FLAG=3:108 RE15_EQUIP=3 RE15_PLAYER_POS=4950,-2770,0,0 RE15_KEYITEMS_LOG=1 RE15_PRESS=start@260,square@285,down@300,square@310,square@325`), Leon steht vor dem geschlossenen Schrank, START, Fuse, USE:
  ```
  [keyitems] USE 0x40 ROOM2010 Slot 8 Ereignis 20 -> Inventar schliesst
  [ventilkette41] ROOM2010 Ereignis 20 -> Port-Programm 2010_kasten_frage
  [keyitems] USE-Lauf ROOM2010 Ereignis 20 Frage 41
  [schrank2010] Tuer klappt auf
  [keyitems] USE-Lauf ohne Frage beendet (Ereignis 20)
  [schrank2010] Tuer auf (rot_y 1024)
  [msgtext] room=2010 id=40 "A fuse box for the valve controls.One of the fuses is missing..."
  ```
  Dann `Frage 41 entfaellt: Nein (Aktionstaste)` (Lauf `m_b`), kein Cursor, keine Sicherung, Bild `n1/sh_mr.png`. Elza gleich (`m_be`, ROOM2011). Ursache: `re15_keyitems_r41_tick` (master keyitems_r41.c:386) beendet den USE-Lauf nach zwei Takten ("die Frage faellt in allen Stellen im ersten Skripttakt"); der Vorspann AUF von Spur N schiebt die Frage um die 16 Bilder Aufklappen dahinter. Mit offener Tuer geht es: `m_a` (erst Klick, dann USE) und `m_c` (Klick von Norden, USE unter Cut 4): `Frage 41 entfaellt: Ja (USE)` -> Cursor -> Bit 125 -> Text 42 -> Standbild. Kein Haenger, ein zweites USE wirkt, die Sicherung bleibt im Inventar. Aber der erste Griff zur Sicherung am geschlossenen Schrank, also der Weg, den Nachtrag 34 fuer Key Items verlangt, verpufft mit dem Satz "One of the fuses is missing...".
* **Mangel 2, gemessen.** Die Aussage des Bau-Agenten "Konflikt nur in test_r41_ventilkette_weg.cmake k1" reicht nicht: `test_r41_nachtrag30.cmake` Teil schrank faellt auf der gemergten exe durch (`cmake -P`, Zeile 176 `_danach "[schrank2010] Tuer auf" ... id=41 "Will you use the Fuse?"`), denn O laesst die Frage entfallen, sobald die Sicherung im Inventar liegt. Teil schrank Lauf b (Leon, Elza) muss auf USE umgestellt werden, das Dossier ("Bedienung Sicherung 2010: nach Yes ...", OFFEN 1) ebenso.
* Teil panel auf der gemergten exe gruen (Lauf `m_p` und `cmake -P`), Teile wasser und laden siehe Tests.

## Gates

**RE-Gate.** Grep im Diff (Basis 6e213a5a5) nach deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/geschaetzt/ungefaehr/vorlaeufig/Platzhalter: 0 Treffer. Neue getenv: RE15_SE_DEBUG (Log) und der Messhaken der Lautstaerke. Jede neue Konstante der Koepfe traegt @0x.., Byte-Offset, NUTZER-VORGABE, PORT-WAHL oder "gemessen". Selbst geprueft:
1. STAGE2.BIN @0x801109b8 `lui v0,0xf00`, `or a0,a0,v0`, @0x801109cc `jal 0x80019700`; Helfer 0x80110988 mit `jal`-Aufrufern ab 0x8010f1ac (EM024-Raster): Effekt 0x0F wie zitiert.
2. PSX.EXE @0x8001cd14..@0x8001cd20 `lw v0,g_pauseflags` / `and v0,v0,v1` (v1 0x01000000 @0x8001cd0c) / `bne`: START-Poll. @0x80019e28..@0x80019e40 g_pauseflags & 0x10000000. @0x800402e4 `sb a1,0x3f7b(at)`: alter Cut, Quelle von Cut_old (scd_vm.c).
3. ROOM2000 EFF 0x0F @0x15D64 `24 00 11 00`; Zeilen selbst dekodiert: Sub 3 vel (100,0,0) acc 0, Sub 6 vel (42,-18,0) acc (0,10,0), je Zeile 0 A 10. ids @0x14608, ROOM2001 @0x14A20 gleich.
4. ROOM2060: @0x00F12 Lage 0x4C73 / 0xA7F2 = (19571,-22542), @0x0100C `2c 07 05 44 .. f4 4c 14 a1 02 08 02 08`, Polls @0x012C0/D8/F0/@0x01308, @0x01324 `3e 00 0f 00 07 00`, @0x0132E `51 01 40 00`, sub14..17 `2f 02 c8 00` / `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff`, sub18 @0x0168C, sub19 @0x0169E, @0x016A2 `29 09`, @0x016C5 `4b 06 0a`, @0x016C8 `2b 05 ff ff 02 00`. ROOM2030 @0x01E60 `29 0a`, ROOM2000 @0x01AD4, ROOM11F0 @0x01332/@0x01336, RE2 ROOM4080 wie Punkt 5.
5. Rahmen-Werte (9418, 29532, -29132, -15968): `probe_r36_g2_cursor_rahmen.c` Zeile 226..229 und G2_belege/orig_zensus/log.txt.
Ungenau, kein Mangel: "ROOM2050 sub01 @0x021FA Set(2,0)=1" zeigt auf den Block (`06 00 0e 00 21 05 06 01`), das Set steht @0x02202. **Haelt.**

**Vertrag.** `git diff 6e213a5a5..HEAD --name-only`: keine release/, android/, shared_assets/PSX, pri/, synchro/, keine Test-CMakeLists (die Liste von `git diff master` enthaelt nur den Vorlauf von master). Bits: keine neuen, 101 umbenannt, k_port[] nachgezogen. Ids 2030 40/41/42/47, 2010 40..44. Bank 5 in 2010/2011/2030/2031: keine `21 05`/`22 05` im Raumskript. Gemeinsame Dateien: game_step_common.c 2 Zeilen, main.c 4, render_pc.c 4, audio_pc.c 23 (benannt). Paket-Gate im Dossier genannt, make_package.sh prueft die neuen Dateien noch nicht (Integration). **Haelt.**

**Riegel.** Aenderungen an alten Riegeln seit Abnahme 0 gelesen: k1 und unit Teil Kasten strenger (Cursor, Reihenfolge), nichts aufgeweicht. Veralteter Kommentar `test_r41_nachtrag30.cmake` "Modell der nassen Spur".

**Merge.** Haelt nicht, siehe oben.
