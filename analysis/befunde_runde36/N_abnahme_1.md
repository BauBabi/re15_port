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

### 1.5 Etagenwahl "wie in RE2" (Batch F, Spielstand ROOM1140 Cut 5, Skript `S0.3,W2,Q0.3,W2,D0.2,W4`, Abzug inv-Bild 200)
Ohne 2F-Plan bleibt RUNTER auf "POLICE STATION 3F" (nur 1140 gefuellt, 2F nie betreten); mit Bit 88 (2F-Plan)
wechselt RUNTER auf "POLICE STATION 2F" mit allen 2F-Raeumen als Umriss (`f_sheet.png`) = RE2 Bank 35
(Set(35,n) bei der Aufnahme room20B0 @0x037E6-EE, gelesen @0x8006D934/@0x8006D998, selbst im RE2-Dump gelesen:
`8006d934 jal FUN_80077360` + `8006d938 _addiu a0,..,0x4d94`).

### 1.6 Nebenwirkung des Slot-Umzugs in ROOM1030 (Batch G, Spielstand vor der Karte, `RE15_AOT_DUMP=1 RE15_STATE_LOG`)
AOT-Dump beim Raumstart: `[aot] 8 GENERIC Mitte(-1692,-23280) halb(500,500) ev=28` (Karte) und `[aot] 39 MESSAGE
Mitte(-2208,-23844) halb(2048,2048) ev=3` (der unveraendert umgezogene Telefon-Satz). Skript `A0.1,W3,A0.1,W3,
A0.1,W2,A0.1,W4,A0.1,W3`; state.log: F31 `msg(a=1 fsm=1 id=30)` (Frage), F128 `id=31` (Meldung), F279 — vierter
Druck an derselben Stelle — `msg(a=1 fsm=1 id=3)` = "A payphone..." (Nachricht 3 des RDT): nach der Aufnahme
antwortet dort wieder das Telefon (Aot_reset(8) im Port-Programm, Slot 39 bleibt).

## 2. Urteil je Nutzer-Punkt (gegen den Wortlaut AUFTRAG.md Z. 73-77)

| # | Wortlaut (kurz) | Urteil | Beleg |
|---|---|---|---|
| 1 | Karte01/02 extrahieren, beispielhaft an die Stelle von map3F.bmp haengen, damit 3F freischalten wie in RE2, mit Text | **erfuellt** | 1.0 (map3F.bmp = ROOM1140 Cut 5, 8,5 gegen >= 113,8 fuer ROOM1150), 1.1 (Blatt in der Marke, AUSSEN 0, Palettenfarben), 1.2 (Frage "A police 3F map. / Will you take it?" Ja/Nein, Meldung "You've taken the / police 3F map.", Blatt weg), 1.5 (ganze Etage + Etagenwahl = RE2 Bank 33/35); Modell = karte01 (MD1 md5 99dae133 / TIM bf0fbcc7 laut Generator-Kopf, Unit-Test T prueft TIM 16928 B + MD1-Bytes) |
| 2a | 2F in ROOM10D0 bei map2F.bmp | **erfuellt** | 1.0 (10D0 Cut 6: 2,0), 1.1, 1.2, 1.4 (Elza-Figur) |
| 2b | 1F in ROOM1030 bei map1F.bmp | **erfuellt** | 1.0 (1030 Cut 3: 12,8), 1.1 (Cut 3 und Cut 0), 1.2, 1.6 (Telefon nach der Aufnahme) |
| 2c | B2 in ROOM1200 bei mapB2.bmp | **erfuellt** | 1.0 (1200 Cut 2: 3,7), 1.1, 1.2 |
| 2d | B1 in ROOM11D0 bei mapB1.bmp | **erfuellt** | 1.0 (11D0 Cut 0: 1,3), 1.1, 1.2, 1.3 (ECHTER Tuer-Weg 1180 -> 11D0 mit Aufnahme) |

## 3. Gates

### 3.1 RE-Gate — Stichproben SELBST disassembliert (richtige Binaerdatei, Sprungziele gelesen)
Nachbesserung 1 (die neuen Konstanten 0x10 / 0x2 / 0x0010 / 0x80):

| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| FUN_8002c18c `lw v0,0xc(s1)` @0x8002c2ec, `andi 0x10` @0x8002c2f4, `beq` @0x8002c2f8 | ghidra1_V2.txt Z. 119032-119036: `8002c2ec 0c 00 22 8e lw v0,0xc(s1)=>DAT_800b3fa4` / `8002c2f4 10 00 42 30 andi v0,v0,0x10` / `8002c2f8 19 00 40 10 beq v0,zero,LAB_8002c360` / Delay-Slot `8002c2fc _addiu s0,s1,0x70` | stimmt |
| Flach-Zweig FUN_80022f0c @0x8002c320 / FUN_80023098 @0x8002c350 mit a2 = obj+0x70 | `8002c300 move a2=>DAT_800b4008,s0` (s0 = s1+0x70), `8002c304 move a3,s2`, `8002c320 c3 8b 00 0c jal FUN_80022f0c`, `8002c350 26 8c 00 0c jal FUN_80023098`; NCCT-Zweig LAB_8002c360: `8002c380 jal FUN_800254a0`, `8002c3b0 jal FUN_800256b0` | stimmt |
| Bit 0x2 = Halbtransparenz @0x8002c2b4 (Parameter 4) | `8002c2ac lw v0,0xc(s1)` / `8002c2b4 02 00 42 30 andi v0,v0,0x2` / `8002c2b8 sltu s2,zero,v0` -> a3 = s2 in beiden Zweigen; Typ 3 `ori v0,zero,0x3` / `bne v1,v0` @0x8002c2c0 -> `jal FUN_8003e64c` @0x8002c2c8 | stimmt |
| Obj_model_set pc[8..9] -> Pool+0x0c @0x800409a8/@0x800409b0, beide Zweige | ghidra1_V2.txt Z. 150758-150760: `800409a8 08 00 c2 84 lh v0,0x8(a2)` / `800409b0 0c 00 a2 ac sw v0,0xc(a1)=>DAT_800b3fa4`; a2 = `lw a2,0x1c(t0)` (Thread-pc), a1 = Pool + id*0x94; der Zweig `beq v0,zero,LAB_80040990` @0x80040980 fuehrt ueber `lhu v0,0x6(a2)`/`ori 1`/`sw 0x0(a1)` @0x80040990-a4 nach 0x800409a8, der andere ueber `j LAB_800409a8` @0x80040988 mit `_sw zero,0x0(a1)` | stimmt |
| obj+0x70 = 0x808080 (FUN_8002b898.c:16) | RE_15_Quellcode_V2/FUN_8002b898.c: `*(undefined4 *)(&DAT_800b4008 + iVar1) = 0x808080;` mit iVar1 = id*0x94, DAT_800b4008 - DAT_800b3f98 = 0x70 | stimmt |
| FUN_80022f0c: Farbe = Konstante, kein NCCT | FUN_80022f0c.c Z. 24-26/41: `param_3->cd = param_4 << 1 \| 0x34; gte_ldrgb(param_3);` ... `*(CVECTOR *)(param_2 + 1) = *param_3;` | stimmt |
| RE2 room20B0 obj 8 @0x03158 / room2130 obj 0 @0x00FDC = 0x0010 | `xxd`: `2d08 0000 0000 0000 0000 0a00 1000 204e 204e 204e ...` / `2d00 0000 0000 0000 0000 0a00 1000 fcbd 50fb 60d7 ...` (Wort vor der Position = 0x0010) | stimmt (positionell, wie das Dossier sagt) |
| RE2 FUN_800363f0.c:41-54 | `if ((puVar4[4] & 0x10) == 0) { FUN_80027bec; FUN_80027dbc } else { FUN_8002d0e8; FUN_8002d244 }`, `uVar5 = puVar4[4] >> 1 & 1`; FUN_80035710.c:16 `(&DAT_800d03a0)[param_1 * 0x7e] = 0x808080;` | stimmt |
| RE1.5 ROOM1200 obj 0/1 @0x007DA/@0x00804 = 0x0010, ROOM1140 obj 0 @0x00B28 = 0x0000 | `xxd` (shared_assets md5 = info/Re1.5): `2d00 0000 0100 0a00 1000 bab2 ...`, `2d01 0000 0100 0a00 1000 a29d ...`, `2d00 0000 0100 0a00 0000 2ce4 ...`; dazu SCD-Dumps 1210 obj 0 `0a 00 10 00`, 11E0 obj 0 `fc 03 10 00` | stimmt |
| 0x80 im SDL-Weg = Textur unveraendert | render_pc.c:2616 `psx_prim_to_sdl_vert`: `(v*0xFF + 0x40) / 0x80` -> 0x80 -> 255 (= Texel x 1) | stimmt |

Urspruenglicher Bau (Stichproben, zusaetzlich zur Abnahme 0): ROOM5030 sub02 @0x010AC `2b00 ffff 0200 0600 1e00
210c 1f00 4605 .. 090a 0100 2203 7301 2b01 ffff 0200 0800 0100` und sub00 @0x0108A `2c05 03b1 ..`; RE2 room20B0
sub10 @0x037BA..0x0381A (`2b00 .. 2208 5301 2221 0201 .. 2223 0401 2e04 0800 3200 204e 204e 204e 0200 2b00 1100
00ff 0700 1200 3604 0501 .. 0100`); RE2 FUN_8006E120 `8006e66c jal FUN_80077360` mit a0 = 0x800D4924 (Bank 33),
`8006e744 beq v0,zero,LAB_8006e768`; ROOM1030 Telefon @0x01D2E `2c08 0131 0000 60ef dc9a 0010 0010 0300`. Alle
stimmen.

Der Fix ERKLAERT den Befund M1: die Karte war mit NCCT in den Ambient-Raeumen (1140/10D0/11D0: Vertexfarbe
41/128) fast schwarz bzw. mit der Tint-Konstante blau; das Original zeichnet ein Objekt mit Attribut-Bit 0x10
ohne Raumlicht mit 0x808080 — genau das Bild, das jetzt gemessen ist (jeder Kartenpixel Palettenfarbe, Summe
296..310 in sechs Ansichten bei Raumlicht 36..89). Wortsuche im Code-Diff (deferred/tunable/interim/for now/
faithful/plausib/TODO/FIXME/getenv/approx): 0 Treffer; kein neuer Env-Schalter. Die Licht-Spalte, BG_REF und
"hellste 15 %" sind aus Header, Tabelle, Generator und Code entfernt (Generator-Diff: nur noch der Hinweis
"keine Spalte mehr"). Einzige nicht adressierte Zahlen: Lage/Massstab/Rechteck/Slots (PORT-WAHL aus den Marken,
gekennzeichnet) und die Port-Entscheidung `(attr & 0x12) == 0x10` (Abschnitt 3.4).

### 3.2 Vertrag
* Bank-9-Bits 87..91, Ereignis 28, Nachrichten 30/31 (VERTRAG 1.2 Reserve) — unveraendert gegenueber Abnahme 0.
* Pfade (`git diff 78a4a748 --name-only`, 17 Dateien): engine/src (kartenitems_r36.c, gen/kartenitems_r36.inc,
  map_hint_common.c, re15_map_owned.c, scd_room_setup.c, scd_vm.c), include (re15_kartenitems_r36.h, re15_scd.h),
  platform/pc/main.c, tests/unit/probes/r36_kartenitems.cmake (GLOB), eigene Tests/Auswerter, tools/r36_kartenitems,
  Dossier + Abnahme 0. Kein release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists.
* Gemeinsame Dateien: scd_vm.c +3 Zeilen (include, Ereignis-Kette, `attr`), re15_scd.h +1 Feld (+Kommentar),
  scd_room_setup.c +2, re15_map_owned.c +4, map_hint_common.c +2, main.c: include, Installer 1 Zeile, Prop-Lader
  6 Zeilen (r35-Muster), Flach-Zweig 2+2 Zeilen (Dreieck/Viereck) — der frueher dort stehende 3-Zeilen-Karten-Haken
  ist weg. `g_scd.props` wird nirgends serialisiert (re15_savedata.c/re15_savepoint.c: 0 Treffer) — das neue Feld
  aendert kein Spielstand-Layout.
* Tests je Punkt vorhanden und messend: `unit_r36_kartenitems` (217 Pruefungen: Tabelle/Texte, Raumstart aller
  10 Raeume inkl. Prop-Attribut 0x0010 und RDT-Attribute 1200/1140, echter Druck 1140 Ja/Nein, 10D1 Elza, 1030 mit
  Telefon danach, Kartenblatt, Spielstand) und `integration_r36_kartenitems_bild` (echte exe: 5 Raeume + 1030
  Cut 0, Marken-Fenster, Palettenpruefung, Cut-3/Cut-0-Helligkeit, Kartenschirm, echter Weg 1140) — Ergebnis
  Abschnitt 3.3.

### 3.4 Nebenwirkung des generischen Flach-Zweigs (Batch C: alte exe 6f16291b gegen neue ab178a50, Debug-Sprung,
`RE15_FORCE_CUT`, F700, `eval_c.py` |dR|+|dG|+|dB| > 30, Bilder `c_<raum>_<cut>.png` alt | neu)
Der Zweig gilt fuer JEDES Objekt mit Attribut-Wort `(attr & 0x12) == 0x10` — so hatte es die Abnahme 0 verlangt
("Zweig `& 0x10` im Prop-Zeichner"), und so arbeitet der Original-Dispatcher FUN_8002c18c fuer jeden Pool-Eintrag
(3.1). Selbst gemessen (Zahlen = Dossier 8.7, unabhaengig reproduziert):

| Raum (Cut) | Objekt (attr aus dem SCD-Dump) | Pixel | Mittel alt -> neu | Bild |
|---|---|---|---|---|
| 1210 (6) | obj 0 Zellentuer `0a 00 10 00` @0x01E4E | 4341 | (21,19,14) S55 -> (52,46,35) S134 | vorher dunkelbraune Flaeche, nachher die Tuer mit Gitterfenster in der Helligkeit des gemalten Hintergrunds |
| 11E0 (5) | obj 0 Rolltor `fc 03 10 00` @0x016E6 | 5152 | (5,5,6) S16 -> (20,22,24) S67 | vorher fast schwarzer Block im Gang, nachher das gewellte Tor mit Lamellen |
| 1200 (2) | obj 0/1 Schubladen-Fronten `0a 00 10 00` @0x007DA/@0x00804 | 2204 | (18,18,12) S49 -> (44,43,30) S119 | vorher fast schwarze Kaesten vor den gemalten Schraenken, nachher gleiche Helligkeit wie der Hintergrund |
| 11B0 (5) | obj 2 (0x0010); die Kiste obj 0 (0x0012, halbtransparent) | 192 (Huelle x209..221 y160..177) | (15,18,13) S47 -> (33,40,28) S102 | nur das kleine Objekt obj 2 aendert sich; die Kiste (Bit 0x2) bleibt wie bisher — die Port-Entscheidung `(attr & 0x12) == 0x10` greift |

(Der erste Lauf von 11B0 mit der neuen exe blieb in der Initialisierung haengen — debug.log 409 Byte, kein Sprung,
exit 124 nach 240 s; das ist das Start-Flattern unter Last aus VERTRAG 1.5 (zeitgleich liefen Integrations-Suite
und Spur-F-Laeufe anderer Sitzungen, 7 re15_pc-Prozesse); der Nachlauf lief im ersten Versuch in 47 s durch.)

Das ist die Folge des Original-Mechanismus (Objekte mit Bit 0x10 ohne Raumlicht, Farbe 0x808080), nicht eine
Spur-Konstante; die Spur hat die vorbestehende Abweichung (Port zeichnete alle 672 Obj_model_set ueber NCCT)
fuer die 169 deckenden Flach-Objekte mit behoben. Release-Notes-Hinweis (Dossier 8.9) ist angemessen.

### 3.3 Spur-Tests und Regressionsgruppen (gezielt, KEINE volle Suite — VERTRAG 1.5)
* `unit_r36_kartenitems` direkt ausgefuehrt (`build/tests/unit/test_r36_kartenitems.exe`, `N_abn1/unit_out.txt`):
  **217 PASS, 0 FAIL, exit 0** — darin Prop-Attribut 0x0010 der Karte in allen 10 Raeumen, ROOM1200 obj 0/1 =
  0x0010, ROOM1140 obj 0 = 0x0000, echter Druck 1140 Ja/Nein, 10D1 (Elza) Ja, 1030 Ja + Telefon danach.
* `integration_r36_kartenitems_bild`: dieselbe Messung habe ich mit der Messkopie eigenstaendig gefahren
  (Batch A = Teil 1 a/b/d inkl. 1030 Cut 0 und Cut-3/Cut-0-Abstand 13 <= 29, Batch B/E = Teil 2 echter Weg) —
  Abschnitt 1.1/1.2/1.3, alle Schwellen des Tests erfuellt (73/219/88/201/221/48 Pixel gegen Soll 36/110/40/88/84/24,
  AUSSEN 0, Palette n von n). Die Laufverzeichnisse des Bau-Agenten (`build/tests/integration/
  r36_kartenitems_bild_wd/lauf_f1ceb6f7_*`) liegen vor.
* Regressionsgruppen: `ctest --test-dir re15_port/build -R "karte|map|besitz|hinweis|r34n_e|r35_inhalt|r34n_a|
  r34n_d|fenster|cut10f0|r35_cut|1030|1140|10d0|1200|11d0|item_stack|r36_kartenitems|irons_tisch|aot|msg_|action_msg|
  prop_|boot_bg|dark_start|relatch|save_counter|weste_load|savepoint|continue|1040|1090|1190|11a0|11b0|11c0|11e0|1210|
  1260|climb|obj_model|scd_|r34n_b|affen|gitter|espr" --timeout 900 --output-on-failure` (196 Tests, Log
  `scratchpad/ctest_regr_abn1.log`), gestartet 13:03 parallel zur Integrations-Suite und den Spur-F-Laeufen
  anderer Sitzungen. Stand beim Abschluss dieses Berichts (13:34, der Orchestrator erzwang die Abgabe):
  **187 von 196 gruen, 0 rot, 0 Timeouts**, darunter alle bis dahin gelaufenen exe-Haken im ERSTEN Lauf:
  integration_weste_load_pin 6,7 s, r34n_e_dokumente_bild 95,7 s, r35_cut10f0 241,2 s, **r35_cut1150 507,4 s**,
  r35_fenster 26,2 s, r35_karte 74,2 s, boot_bg/dark_start/relatch/save_counter gruen; noch im Lauf:
  integration_r35_karte_fahrstuhl und danach die letzten 8 (map_uebergang, map_raum_live, unit_r36_kartenitems,
  integration_r36_kartenitems_bild, ...). Die beiden Spur-Tests sind durch die Direktausfuehrung (217/217) und die
  eigene exe-Messung (1.1-1.3) abgedeckt; der Lauf schreibt seine Summenzeile nach `ctest_regr_abn1.log`
  (`CTEST_EXIT=`), der Orchestrator faehrt die volle Suite im Integrationsbaum.

## 4. Maengel

Keine Maengel gegen den Wortlaut der Punkte 1-2 und keine gegen die Gates. M1 der Abnahme 0 ist behoben: die
Licht-Konstante je Raum (Spalte `licht`, BG_REF "hellste 15 %", Haken `re15_kartenitems_r36_licht`) ist weg; die
Karte traegt das Attribut-Wort 0x0010 und nimmt den Original-Flachweg (3.1), gemessen als Palettenfarben in sechs
Ansichten (1.1).

Hinweise (keine Abnahme-Maengel, fuer Release Notes / Folgespuren):
* H1 — Halbtransparenz-Bit 0x2 des Objekt-Attributs (FUN_8002c18c @0x8002c2b4 -> FUN_80022f0c Parameter 4,
  Prim-Code 0x36, STP-Texel-Mischung) bleibt im Port fuer Objekte unportiert; die 91 flach+halbtransparenten
  Objekte (Dossier 8.8, mit Raumliste) laufen weiter ueber NCCT (vorbestehend, nicht durch die Spur eingefuehrt;
  gemessen 11B0 Kiste unveraendert). Mit Adresse und Messweg unter OFFEN gefuehrt (VERTRAG Regel 4) — eigene Spur.
* H2 — Sichtbare Aenderung ausserhalb der Karten: 169 deckende Objekte mit Flach-Attribut (Rolltor 11E0, Zellentuer
  1210, Schubladen 1200, Objekte in 1090/1190/11A0/11B0/11C0, Stages 2/4/5/6) erscheinen jetzt in Texturhelligkeit
  statt fast schwarz (3.4, Bilder `c_*.png`). Gehoert in die Release Notes (Dossier 8.9).
* H3 — Entscheidungsfragen an den Nutzer (Dossier 7, unveraendert): 3F-Karte haengt in ROOM1140 (map3F.bmp ist der
  Konferenzraum, nicht Irons' Buero); Groesse in die Marken eingepasst (RE2-Originalgroesse 1080 x 720 waere ~2,8x);
  ROOM1030: bis zur Aufnahme antwortet im Karten-Rechteck die Karte, daneben und danach das Telefon.
* H4 — Elza per Debug-Sprung landet in Leons 10D0 (Spur I, nicht N); mit Elza zeigt der Statusschirm Leons
  Dienstausweis (vorbestehend, ausserhalb des Auftrags).

## 5. Ergebnis
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| 1 Karte01/02 an map3F.bmp, 3F freischalten wie RE2, mit Text | erfuellt | 1.0 (ROOM1140 Cut 5), 1.1 (Blatt in der Marke, Palettenfarben), 1.2 (Frage/Ja/Meldung, Blatt weg, 3F komplett), 1.5 (Etagenwahl Bank 35) |
| 2a 2F ROOM10D0 (map2F.bmp) | erfuellt | 1.0, 1.1, 1.2, 1.4 (Elza-Figur) |
| 2b 1F ROOM1030 (map1F.bmp) | erfuellt | 1.0, 1.1 (Cut 3 + Cut 0), 1.2, 1.6 (Telefon danach) |
| 2c B2 ROOM1200 (mapB2.bmp) | erfuellt | 1.0, 1.1, 1.2 |
| 2d B1 ROOM11D0 (mapB1.bmp) | erfuellt | 1.0, 1.1, 1.2, 1.3 (echter Tuer-Weg 1180 -> 11D0) |

Gates: RE-Gate haelt (3.1: 0x10 @0x8002c2f4, 0x2 @0x8002c2b4, pc[8..9] @0x800409a8/@0x800409b0, 0x808080
FUN_8002b898, RE2 @0x03158/@0x00FDC, RE1.5 @0x007DA/@0x00804, alles selbst gelesen; keine Guess-Tells, kein
Env-Schalter; der Fix erklaert M1). Vertrag haelt (3.2). Regressionsgruppen ohne Rot (3.3, 187/196 beim Abbruch
durch den Orchestrator, Rest im Lauf), Spur-Tests gruen (217/217 direkt; Integration eigenstaendig nachgemessen).

**Abnahme 1: BESTANDEN.** Zusammenfuehren; Release-Notes-Hinweis H2 und Entscheidungsfragen H3 an den Nutzer.
