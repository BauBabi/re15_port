# Runde 36 Spur N "kartenitems" — Abnahme 1 (unabhaengig, nach Nachbesserung 1 "M1 Licht")

Baum `.claude/worktrees/r36_kartenitems`, Zweig `r36/kartenitems`, HEAD 7f69b0b2 (Code-Stand 8de33b9a 12:33,
danach nur Dossier), Basis 78a4a748. Gemessen am SELBST gebauten Stand (`local_build.sh configure` + `build`:
"LOCAL-BUILD-OK", ninja "no work to do", exe 12:35 md5 ab178a50 = Messkopie `build/platform/pc/
re15_pc_abn1n_kartenitems.exe`; alte exe der Abnahme 0 `re15_pc_abn0n_kartenitems.exe` md5 6f16291b als
Gegenprobe). Scratch `%TEMP%/claude/.../scratchpad/N_abn1/` (lauf.sh, batch_a..f.sh, eval_a.txt,
marken_abn1.txt, runs/, zoom_*.png, b_*_sheet.png, weg_*.png, e_sheet1.png, d_elza_sheet.png, bg/).
Bericht fortlaufend geschrieben (Sitzungslimit).

## 0. Gelesen
AUFTRAG.md Z. 73-77 (Wortlaut), VERTRAG.md (N: Bits 87-91, Ereignis 28, IDs je Raum 2 ab 28 / Reserve 30/31,
Pfade, Haken-Regel), Dossier N_kartenitems.md (1-8, inkl. Abschnitt 8 Nachbesserung 1), N_abnahme_0.md,
`git log master..HEAD` (18 Commits, davon 4 Code-/Test-Commits der Nachbesserung 0a13946d, 6c7daec4, 8de33b9a,
7f69b0b2), `git diff 78a4a748` (17 Dateien; gegen master sind die uebrigen Unterschiede nur der Vorsprung von
master). Code-Diff vollstaendig gelesen: kartenitems_r36.c/.h, gen/kartenitems_r36.inc (Kopf, Tabellen, Texte),
scd_vm.c (Ereignis-Kette + `attr = pc[8] | pc[9]<<8`), re15_scd.h (Feld `attr`), scd_room_setup.c (Installer),
re15_map_owned.c, map_hint_common.c, platform/pc/main.c (Installer CONTINUE-Weg, Prop-Lader, Flach-Zweig
Dreieck + Viereck `(attr & 0x12) == 0x10 -> 0x80`), Tests, Generator.

## 1. Messung je Nutzer-Punkt (eigene Laeufe, Messkopie, RE15_WINDOW_SCALE=1)

### 1.0 Welcher Raum/Cut liegt unter den Marken? (eigene Rechnung, `marken_abn1.py`)
Eigener Hintergrund-Dump aller 428 STAGE1-Cuts (`probe_bg_dump` nach `N_abn1/bg/`, "Bilder geschrieben 428 |
Fehlschlaege 0"), Mass = mittlere |dR|+|dG|+|dB| je Pixel ausserhalb der roten Marke:

| Marke | rote Pixel / Huelle | bester Cut | zweiter | bester ROOM1150-Cut |
|---|---|---|---|---|
| map3F.bmp | 110 / x231..241 y72..81 | **ROOM1140 Cut 5: 8,5** | ROOM1230 Cut 2: 75,2 | Cut 4: 113,8 |
| map2F.bmp | 320 / x270..289 y120..135 | ROOM10D0 Cut 6: 2,0 | ROOM1210 Cut 1: 70,1 | Cut 0: 79,6 |
| map1F.bmp | 110 / x155..164 y103..113 | ROOM1030 Cut 3: 12,8 | ROOM1030 Cut 10: 15,1 | Cut 7: 89,7 |
| mapB2.bmp | 238 / x58..74 y80..93 | ROOM1200 Cut 2: 3,7 | ROOM11E0 Cut 9: 54,0 | Cut 5: 77,7 |
| mapB1.bmp | 696 / x227..250 y65..93 | ROOM11D0 Cut 0: 1,3 | ROOM10A0 Cut 0: 57,1 | Cut 5: 65,6 |

map3F.bmp ist der Konferenzraum ROOM1140 (Blatt 4 = 3F), nicht ROOM1150 wie AUFTRAG.md Z. 82 annahm — die Spur
folgt dem Bild des Nutzers; richtig. Marken-Kopien `analysis/befunde_runde36/marken/map*.bmp` md5-gleich mit
den Nutzer-Dateien in der Repo-Wurzel (52f9e14c/e0686732/796b37ec/a822e0d3/3eeae95d).

### 1.1 Karte an der Marke, Lade-Weg CONTINUE, MIT/OHNE Plan-Bit (Batch A, 12 Laeufe, F100, Nutzer-Cut)
Spielstand je Raum mit `probe_r35_inhalt_karte` (Spieler abseits der Karte), `RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_FORCE_CUT=<cut> RE15_FRAMEDUMP=100:raum.ppm RE15_EXIT_AT=101#<raum>`;
Auswerter `probe_r34n_e_bild` (Fenster = Marken-Huelle +-3, |d| > 30) und `probe_r36_kartenitems_licht`
(jeder Kartenpixel exakt eine Farbe der 256er-CLUT der RE2-TIM, 5 Bit << 3). Alle 12 Laeufe: "CONTINUE: resumed"
1x; die Zeile `[kartenitems] ROOMxxxx: Karte Blatt n (Bit 9,8x) Prop obj 15 ... Zone Slot 40` (1030: `Slot 8
(Muenztelefon -> Slot 39)`) steht nur in den Laeufen OHNE Bit (`eval_a.txt`):

| Raum (Cut) | Pixel im Fenster (Soll) | Huelle der Aenderung | Nutzer-Marke | ausserhalb | Mittel RGB (Summe) | Palette |
|---|---|---|---|---|---|---|
| 1140 (5) | 73 (>= 36) | x230..241 y72..80 | x231..241 y72..81 | 0 | (105,107,96) 310 | 73 von 73 |
| 10D0 (6) | 219 (>= 110) | x270..289 y120..134 | x270..289 y120..135 | 0 | (101,104,94) 299 | 219 von 219 |
| 1030 (3) | 88 (>= 40) | x154..164 y104..111 | x155..164 y103..113 | 0 | (101,102,93) 297 | 88 von 88 |
| 1200 (2) | 201 (>= 88) | x57..73 y79..92 | x58..74 y80..93 | 0 | (100,102,93) 296 | 201 von 201 |
| 11D0 (0) | 221 (>= 84) | x233..245 y65..93 | x227..250 y65..93 | 0 | (102,105,96) 304 | 221 von 221 |
| 1030 (0) | 48 (>= 24) | x91..98 y107..112 | (anderer Cut) | 0 | (105,107,96) 310 | 48 von 48 |

Zoom-Streifen (`zoom_3f/2f/1f/b2/b1/1f0.png`: mit | ohne | Marke, 6x) selbst angesehen: 3F ein Blatt auf der
Tafel genau in der Marke, 2F auf der Pinnwand unter dem Foto, 1F an der Rueckwand zwischen den Aushaengen, B2 auf
der Ziegelwand rechts der Vitrine, B1 als perspektivisch verzerrtes Blatt auf der schraegen Ziegelwand, 1030 Cut 0
an der Rueckwand. Keine Aenderung ausserhalb des Marken-Fensters (AUSSEN 0 in allen sechs Paaren).
Licht (M1 der Abnahme 0): in allen sechs Ansichten ist JEDER Kartenpixel eine Palettenfarbe (Summe 296..310
bei Raumlicht 36..89/128 nach Abnahme 0 Abschnitt 4; 1030 Cut 3 gegen Cut 0: 297 gegen 310 bei 1,65-fachem
Raumlicht) — die Karte wird ohne Raumlicht gezeichnet, wie der Flach-Weg des Originals (Abschnitt 3.1).

### 1.2 Aufnahme am echten Aktionsdruck, alle fuenf Raeume (Batch B, Leon, CONTINUE vor der Karte)
Spielstand vor der Karte (1140 (-2050,-9900) r3071 / 10D0 (2133,22544) r1024 / 1030 (-1691,-22660) r1024 /
1200 (-25410,-15682) r2048 / 11D0 (776,-18819) r225), `RE15_FORCE_CUT=<Nutzer-Cut>`, Skript (Basis spiel, Start
30) `A0.1,W3,A0.1,W3,A0.1,W2,S0.3,W2,Q0.3,W4` (Aktion, Ja, Meldung weg, START, L1), Framedumps F30..F330/10,
`RE15_INV_FB_SHOT=karte.bmp` bei inv-Bild 140. Kontaktbogen je Raum `b_w3f/w2f/w1f/wb2/wb1_sheet.png` und
Zoom-Streifen `weg_*.png` (Marke +-10, F40/110/130/170/240) selbst angesehen:

| Raum | debug.log | F60/F110 | F170 | Blatt an der Wand | Kartenschirm (karte.bmp) |
|---|---|---|---|---|---|
| 1140 | `Ereignis 28 -> Port-Programm Karte Blatt 4` | "A police 3F map. / Will you take it? > Yes No" (Name gruen) | "You've taken the / police 3F map." | F40/F110 da, ab F130 weg | POLICE STATION 3F: alle 3F-Raeume als Umriss, 1140 gefuellt |
| 10D0 | `... Blatt 3` | dito "police 2F map" | dito | da -> weg | 2F komplett, 10D0 gefuellt |
| 1030 | `... Blatt 2` | dito "police 1F map" | dito | da -> weg | 1F komplett, 1030 gefuellt |
| 1200 | `... Blatt 1` | dito "police B2 map" | dito | da -> weg | B2 komplett, 1200 gefuellt |
| 11D0 | `... Blatt 0` | dito "police B1 map" | dito | da -> weg | B1 komplett, 11D0 gefuellt |

Das Weltmodell verschwindet mit dem Ja (RE2 room20B0 Pos_set(20000,..) @0x037F6 vor der Meldung @0x03800): in
allen fuenf Zoom-Streifen ist das Blatt bei F40 und F110 (Frage offen) da und ab F130 nicht mehr.

### 1.3 ECHTER Tuer-Weg Leon ROOM1180 -> ROOM11D0 mit Aufnahme (Batch E)
Spielstand ROOM1180 (-4836,-15916) rot 1024 (vor Tuer-Slot 0, main00 @0x009EE/@0x00A12: Ziel 0x16 = 1160 bzw.
mit Ck(4,243) 0x1D = 11D0), `RE15_SET_FLAG=4:243`, Skript `A0.1,W1,L0.15,W0.3,U0.75,W1,L0.1,W0.5,A0.1,W3,A0.1,W3,
A0.1,W2,S0.3,W2,Q0.3,W4`, Framedumps F0..F800/10, kein FORCE_CUT (natuerlicher Eintritts-Cut). debug.log:
Z. 42 `CONTINUE: resumed in room 1180`, Z. 43 `[setflag] flag(4,243)`, Z. 57 `[tuer] Sequenz Archiv 2 DOOR1A ...
(9 Skripte)`, Z. 71 `[kartenitems] ROOM11D0: Karte Blatt 0 (Bit 9,91) Prop obj 15 bei (1694,-2199,-19149), Zone
Slot 40`, Z. 162 `[kartenitems] ROOM11D0 Ereignis 28 -> Port-Programm Karte Blatt 0`, Z. 259 `EXIT_AT: Bild 800
in Raum 11d0`. Bilder (`e_sheet1.png`, `weg_e.png`): Leon kommt durch die Tuer (F40..F100), das Blatt haengt im
Eintritts-Cut 0 an der Wand rechts der Tuer (= Nutzer-Marke), F150/F160/F200 "A police B1 map. / Will you take",
F250..F300 "You've taken the / police B1 map.", Blatt bis F240 da, ab F260 weg; `karte.bmp` = POLICE STATION
B1 mit allen B1-Raeumen als Umriss und 11D0 gefuellt (`e_karte.png`).

### 1.4 Elza (Batch D, `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, Debug-Sprung 10D1@250, PLAYER_POS vor dem Aushang)
debug.log: Z. 25 `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, Z. 62 `AUTO-JUMP -> ROOM10D1`, Z. 64
`JUMP -> 10d 2F CORRIDOR (ROOM10D0)` — das Debug-Menue dieses Baums laedt fuer Elza noch Leons RDT (Spur I,
nicht Spur N; Installer arbeitet auf `room_id & 0xFFFE`), Z. 73 `[kartenitems] ROOM10D0: Karte Blatt 3 ...`,
Z. 125 `Ereignis 28 -> Port-Programm Karte Blatt 3`. Bilder (`d_elza_sheet.png`): Elza (rote Jacke) vor dem
Aushang, F450/F500 "A police 2F map. / Will you take it? > Yes No", F560 "You've taken the / police 2F map.",
F630 Blatt weg, Kartenschirm POLICE STATION 2F komplett. Die xxx1-RDTs selbst (1141/10D1/1031/1201/11D1 Raumstart
+ Aufnahme in 10D1 und 1031 am Bild-Ablauf re15_game_step) deckt `unit_r36_kartenitems` Teile R/D ab (selbst
gelaufen, Abschnitt 3.3). Beobachtung ausserhalb der Spur: der Statusschirm zeigt mit Elza Leons Dienstausweis
(vorbestehend, nicht Teil des Auftrags).
