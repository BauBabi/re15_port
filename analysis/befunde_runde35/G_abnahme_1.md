# Runde 35 Spur G "karte" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Baum `.claude/worktrees/r35_karte`, Zweig `r35/karte`, HEAD **e6c0121b** (Basis 154a73c1,
`git status` leer). Gebaut: `bash re15_port/tools/local_build.sh configure` + `build` ->
`ninja: no work to do`, `=== LOCAL-BUILD-OK (build)`. Die exe (02:09) ist juenger als der letzte
Code-Commit 88e9516a (02:08, nur Kommentar in re15_map_zones.h); 1d106dee/e6c0121b aendern nur das
Dossier. Die gemessene exe ist also der Stand HEAD.
Gegen master 87cc8575 (v0.8.22): `git merge-tree --write-tree master HEAD` konfliktfrei
(master hat re15_inv_screen.c ueber Spur K 0908b3df geaendert, kein Ueberlapp).

**Ergebnis: BESTANDEN** — alle vier Punkte am echten Tuerweg gemessen erfuellt, Gates halten
(Suite siehe unten). Punkt 1, der in Abnahme 0 durchfiel, bewegt sich jetzt 7 px je Achse auf
allen drei Etagen; vorher (master-exe, gleicher Weg) 5 x 2 px UNTER der Kabine.

---

## Messwerkzeug (eigenes, nicht das des Bau-Agenten)

Scratch `abn1/lauf.sh <name> "<kartenargs>" <skript> <raum> <open_n> <exit_n>`:
1. Spielstand Slot 0 mit `probe_r35_karte karte re15_card.mcr <raum> <x> <z> <yaw> besucht:... flag:b:i`
   (schreibt nur den Spielstand; was danach geschah, steht im debug.log der exe).
2. Eigene exe-Kopie `build/platform/pc/re15_pc_abn1g_<name>.exe` (danach geloescht), Umgebung
   `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
   RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
   RE15_INPUT_SCRIPT=<skript> RE15_INV_OPEN_AT=<n>#<raum> RE15_INV_FB_SHOT=karte.bmp
   RE15_INV_FB_SHOT_AT=60 RE15_EXIT_AT=<m>#<raum>` (Semantik main.c:5291-5367: Statusschirm
   oeffnen, 2 Bilder spaeter L1 = MAP, gezaehlt erst, wenn der Raum spielbar ist).
3. Auswertung mit eigenem `abn1/ana.py` (PIL): `marker` = Bbox der Pixel ohne Kartenfarbe im Fenster
   (Ring gemessen (16,16,0), 13 Pixel), `rot` = aktuelle Fuellung (R40..56, G<=12, B40..72),
   `gelbsegmente` = zusammenhaengende Tuerbalken (224,168,40). Spielerlage = letzte Zeile
   `[walk] ... pl pos` im debug.log (geprueft: das Laufen endet vor dem Oeffnen der Karte, z.B.
   run_F1_r: F210 `pl pos=(-15282,0,-518)`, Karte ab F~230, F420 unveraendert).

ECHTER WEG ueberall: der Spielstand steht im NACHBARRAUM vor der Tuer, `W1,A0.3,W1,A0.3` loest die
Tuer-AOT aus, die Karte geht erst im Zielraum auf (`[aot] DOOR FIRE` + `[room] PC loaded` im Log).

Bilder: `scratchpad/abn1/run_<name>/karte.bmp` (+ PNG/Crops), nicht im Baum.

---

## Punkt 1 — "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht"

**Urteil: erfuellt.**

### Nachher (HEAD e6c0121b), 12 Laeufe, Ring-Mitte im Kartenabzug

| Lauf | Weg (Tuer-Log) | Skript nach der Tuer | pl pos (debug.log) | Blatt (Bild) | Ring-Mitte | Ring-Bbox |
|---|---|---|---|---|---|---|
| F1_steh | 1040 `DOOR FIRE slot=0 rect=(-21786,-10300,hw=1150,hh=700) spawn=(-13650,0,-900)` | — | (-13650,-900) | 1F | (113,141) | (111,139)-(115,143) |
| F1_vor1 | dto. | U1 | (-13650,-3150) | 1F | (113,136) | (111,134)-(115,138) |
| F1_vor2 | dto. | U2 | (-13650,-3682) | 1F | (113,135) | (111,133)-(115,137) |
| F1_r | dto. | R0.6,U2 | (-15282,-518) | 1F | (117,142) | (115,140)-(119,144) |
| F1_l | dto. | L0.6,U2 | (-12018,-518) | 1F | (110,142) | (108,140)-(112,144) |
| F2_steh | 10C0 `DOOR FIRE slot=1 rect=(1500,6400,hw=1000,hh=500)` | — | (-13650,-900) | **2F** | (113,141) | |
| F2_vor2 | dto. | U2 | (-13650,-3682) | 2F | (113,135) | |
| F2_r | dto. | R0.6,U2 | (-15282,-518) | 2F | (117,142) | |
| F3_steh | 1120 `DOOR FIRE slot=1 rect=(1300,6400,hw=1000,hh=500)` | — | (-13650,-900) | **3F** | (131,144) | |
| F3_vor2 | dto. | U2 | (-13650,-3682) | 3F | (131,138) | |
| F3_l | dto. | L0.6,U2 | (-12018,-518) | 3F | (128,145) | |
| F3_r | dto. | R0.6,U2 | (-15282,-518) | 3F | (135,145) | (133,143)-(137,147) |
| F2_l | 10C0 wie oben | L0.6,U2 | (-12018,-518) | 2F | (110,142) | |
| F1_diag | 1040 wie oben | U1,R0.6,U0.5 | (-14175,-2160) | 1F | (114,138) | (112,136)-(116,140) |

Kabine rot in jedem Lauf `rot bbox=(110,135)-(117,142)` (1F/2F) bzw. `(128,138)-(135,145)` (3F) =
der gemalte Innenraum (Kachel selbst gelesen, s. RE-Gate 4). Blattbeschriftung "POLICE STATION
1F/2F/3F" im Bild abgelesen (F1_steh.png, F2_steh.png, F3_l.png).

=> Die Ring-Mitte wandert **x 110..117 (7 px) und y 135..142 (7 px)** auf 1F und 2F, auf 3F
**x 128..135 (7 px) / y 138..145 (7 px)**; jede Lage ist eine andere Ring-Lage, und die
Zwischenlagen F1_vor1 (z -3150 -> y 136) und F1_diag (x -14175 / z -2160 -> (114,138)) liegen
zwischen den Endlagen - die Bewegung ist stetig in beiden Achsen, nicht nur zwei Endpunkte. Richtung: vorwaerts (-z) = Karte oben, Welt-West (rechts gedreht) = Karte
rechts (180 Grad, wie der Raum gegen ROOM1040 liegt). Rechnerisch nachgeprueft mit der Zeile
(155,73,2330,2330, flip 1,1): z -900 -> tz=(31100*23300)>>20=691 -> my=69+73=142, Ring-Mitte
my-1 = 141 = gemessen; z -3150 -> 137 -> 136 = gemessen.

### Vorher (eigener Lauf, ohne Spur G)

exe des Hauptbaums (`C:/workspace/git/reAi_v2/re15_port/build/platform/pc/re15_pc.exe`, 01:05,
Kopie in den Scratch, Hauptbaum nicht beruehrt; die 1080-Zeilen sind auf master unveraendert
`78,214,2080,2320` / Blatt 3/4 ohne Zeile - `git show master:re15_port/engine/src/re15_map_zones.h`
Z. 78/354/356), gleicher Weg, gleiche Lagen:

| Lauf | pl pos | Ring-Mitte |
|---|---|---|
| V1_steh | (-13650,-900) | (113,144) |
| V1_vor2 | (-13650,-3682) | (113,145) |
| V1_r | (-15282,-518) | (112,143) |
| V1_l | (-12018,-518) | (117,143) |
| V3_steh (aus 1120) | (-13650,-900) | (113,144) **auf Blatt 1F**, nicht 3F (kein Gelb/Gruen von 1120 im Bild) |

= x 5 px / y 2 px, ganz UNTER der gemalten Kabine (Innenraum y135..142), vorwaerts sogar nach
unten; auf 3F das falsche Blatt. Das ist exakt der Nutzerbefund und die Vorbedingung, die das
Dossier fuer den Fix nennt (Messung vorher 154a73c1 "x 113..118 (5 px), y 144..146 (2 px)").

### Erklaert der Fix den Befund?

Ja. Der Cursor stand still, weil (a) die Abbildung den Innenraum unter die Kabine legte und die
Klemmung Rect+4 (re15_inv_screen.c) davon nur einen Rest uebrig liess, (b) nach Nachbesserung 0 der
frei gewaehlte Massstab 795/805 den Innenraum auf 3 px stauchte. Der Fix nimmt den RE2-Massstab
1/450 (RE1.5-Zeile @0x800768f0 ist der Stub {0,0,1,1} - selbst gelesen) und ein Klemmfenster =
gemalter Innenraum. Gemessen: genau die vorhergesagten Pixel (Dossier N1 "steh (113,141), vor
(113,135), rechts (117,142), links (110,142)").

Hinweis (kein Mangel, steht im Dossier OFFEN): in den Ecklagen liegt der 5x5-Ring mit seinem
Rand auf der gemalten Wand und 1 px darueber hinaus (F1_l Bbox x108, Wand x109; F1_r x119, Wand
x118; F1_vor2 y133, Wand y134). RE1.5 und RE2 klemmen den Marker nicht (RE-Gate 1/2).

## Punkt 2 — "ROOM 11F0 / ROOM 1200 taucht nicht auf der Karte auf, wenn man drin ist"

**Urteil: erfuellt.**

| Lauf | Weg | Karte in diesem Raum |
|---|---|---|
| P2_11F0 | 11E0 (-24707,-9442) Yaw 1024 -> `DOOR FIRE slot=0 rect=(-25100,-9550,hw=2000,hh=750) spawn=(250,0,250)`, `PC loaded room11f0.rdt` | Blatt "POLICE STATION B2", **rot 1392 px, Bbox (101,102)-(132,154)** = rect 1 (100,101,40,56), sonst nirgends rot; Ring (105,110) |
| P2_11F0_w | dto. + `W1,U3` -> pl pos (250,-6500) | rot unveraendert rect 1 (1387 px), Ring (105,125) - wandert mit |
| P2_1200 | 11E0 (2000,-24250) Yaw 3072 -> `DOOR FIRE slot=1 ... spawn=(-20154,0,-25245)`, `PC loaded room1200.rdt` | Blatt B2, **rot 995 px, Bbox (142,102)-(169,137)** = rect 2 (141,101,32,40); Ring (164,134) an der Tuernische |
| P2_1200_w | dto. + `W1,U3` -> pl pos (-20220,-18667) | rot rect 2 (995 px), Ring (164,120) |

Gegenprobe im selben Bild: 11E0 (besucht) gruen, die jeweils andere Kachel (nicht besucht)
ungezeichnet. Vorher (Dossier + Abnahme 0): 11E0/11F0/1200 alle auf rect 0 (Garage).
RE-Beleg selbst gelesen: Zeile 1200 @0x800769b0 = [129,150,3168,2305], 11F0 @0x800769a8 = Stub
[0,0,1,1].

## Punkt 3 — "In ROOM 1230 bekomme ich die Map von ROOM 11E0"

**Urteil: erfuellt.**

| Lauf | Weg | Karte |
|---|---|---|
| P3_1230 | 11D0 (-300,-17100) Yaw 3072, flag 4:243 -> `DOOR FIRE slot=0 rect=(-300,-16500,hw=1000,hh=500) spawn=(-4729,0,-16018)`, `PC loaded room1230.rdt` | Blatt **"POLICE STATION B1"** (nicht B2 = Blatt von 11E0); **rot 1032 px, Bbox (121,61)-(167,123)** = Gang rect 0 (120,60,56,72); 11D0 gruen darunter; Ring (130,122) an der 11D0-Tuer |
| P3_1230_w | dto. + `W1,U3` -> pl pos (-4729,-9166) | rot unveraendert, Ring (130,118) |

RE-Pruefung: Seiten-Setzer selbst disassembliert - `lh v1,0x0fe2` @0x8004b56c, `sltiu v0,v1,0x26`
@0x8004b574, Tabelle @0x8001103c [35] -> 0x8004b854, `ori v0,zero,0x1` @0x8004b884,
`sb v0,0x260e` @0x8004b88c: das Original schickt 1230 auf B2 (Stub-Zeile @0x800769c8 [0,0,1,1]
selbst gelesen) = exakt der Nutzerbefund. Der Fix weicht bewusst ab ("ROOM1230 IST ROOM1180",
im Code gekennzeichnet); Beleg selbst nachgeprueft: die fuenf Tuer-Datensaetze 1180
@0x9EE/0xA12/0xA34/0xA54/0xA74 und 1230 @0xAEE/0xB12/0xB34/0xB54/0xB74 sind ab Byte 1 bitgleich
(32 B je Satz verglichen).

Hinweis (kein Mangel, Dossier OFFEN 3): der Gang ist schematisch gemalt; im Suedgang-Abschnitt
(idx3, sy=547) wandert der Ring bei 6850 Einheiten Nordlauf nur 4 px (122 -> 118).

## Punkt 4 — "In ROOM 1210 ist der Korridor falsch und so gut wie alle Tueren fehlen"

**Urteil: erfuellt.**

P4_1210: 11E0 (10450,5500) Yaw 3072, flag 3:139 -> `DOOR FIRE slot=3 rect=(10450,5500,hw=750,hh=1000)
spawn=(-26400,0,-2200)`, `PC loaded room1210.rdt`. Karte: Blatt B2, **rot 763 px, Bbox
(188,71)-(225,128)** = T-Korridor rect 3 (187,70,64,80) (vorher 1210 auf rect 4 = eine Zelle).
Gelbe Balken (eigene Segmentierung):

| Balken | Tuer-Datensatz ROOM1210.RDT (selbst geparst) | Projektion mit Zeile @0x800769b8 [177,140,2496,2250] |
|---|---|---|
| x187 y72..76 | @0x1CC6 slot 0 r(-28000,-3500,1500,2200) -> Raum 0x1E (11E0) | (188,76) |
| x201 y80..84 | @0x1CE6 slot 1 -> 0x22 (1220) | (202,84) |
| x212 y89..93 | @0x1D06 slot 2 -> 1220 | (212,92) |
| x201 y97..101 | @0x1D26 slot 3 -> 1220 | (202,101) |
| x212 y106..110 | @0x1D46 slot 4 -> 1220 | (212,108) |
| x212 y123..127 | @0x1D66 slot 5 -> 1220 | (212,126) |

= alle sechs Tueren des Raums (Suche nach Door_aot_set im RDT: genau diese 6), jede als Balken auf
der Korridorwand, je <= 1 px neben der Original-Projektion. P4_1210_w (+`W1,U3`, pl pos
(-19650,-2200)): Ring von (190,75) nach (205,75) entlang des Querbalkens, rot unveraendert.
Bild: `abn1/P4_crop.png`.

---

## Gate: Suite

Dossier: woertlich `=== LOCAL-BUILD-OK (all) — Tests 484/484`; `ctest -N` = `Total Tests: 484`
(Basis 478 + unit_r35_karte_fahrstuhl/_b2/_r1230/_r1210 + integration_r35_karte +
integration_r35_karte_fahrstuhl) - damit ist M3 aus Abnahme 0 aufgeloest.
Eigener Lauf `bash re15_port/tools/local_build.sh test` (ohne Neubau, 2026-10-04 02:42-03:06,
unter Last anderer Baeume): **`100% tests passed, 0 tests failed out of 484`** /
**`=== LOCAL-BUILD-OK (test) — Tests 484/484`** (1393,5 s). Keiner der fuenf Fenster-Haken musste
nachgefahren werden (boot_bg_pin 17,7 s, dark_start_pin 17,7 s, relatch_pin 23,0 s,
save_counter_pin 18,3 s, weste_load_pin 5,8 s - alle Passed). integration_map_raum_live,
integration_map_uebergang, unit_map_etagenzeile Passed.

## Gate: @0x / RE-Belege — Stichproben SELBST disassembliert / gelesen

1. **RE2 FUN_8006e120** (`re2_disasm.py dis 0x8006e1c0 0x58`, info/re2leon/PSX.EXE):
   `lui v0,0x91a2` @0x8006e1dc / `ori v0,v0,0xb3c5` @0x8006e1e8, `lw a0,-976(a0)` (0x800cfc30
   Spieler x), `addiu a0,a0,28000` @0x8006e1ec, `mfhi v1; addu v1,v1,a0; sra v1,v1,8` @0x8006e204-20c,
   `sra a0,a0,31; subu` @0x8006e214/218 = (x+28000)/450; z ebenso, `subu v0,zero,v0` @0x8006e268;
   `lhu a1,8(v1)` @0x8006e2cc / `lhu v1,10(v1)` @0x8006e2e4 (Versatz je Raum), `jal 0x8008f918`
   @0x8006e2f0 mit `sh v0,10(s4)` im Delay-Slot - **kein min/max dazwischen**. Sprungziel selbst
   disassembliert: 0x8008f918 = `lui a2,0xff; ori a2,0xffff; lui a3,0xff00; lw v1,0(a1); lw v0,0(a0);
   and/or; sw v1,0(a1)` = AddPrim (Tag-Verkettung). Behauptung stimmt; 2^20/450 = 2330,2 -> 2330.
2. **RE1.5 Marker** (`re15_disasm.py dis 0x80047528 48`): `addiu a0,t0,-4` @0x80047554,
   `addiu a1,t0,4` @0x80047564, `ori v1,zero,0xfffc` @0x80047578, `addiu v0,v0,4` @0x800475ac/
   0x800475c0, `jal 0x8006b538` @0x800475d8; Sprungziel 0x8006b538 = dieselbe AddPrim-Folge;
   0x80047480-0x80047528 ohne slt/Klemmung. "RE1.5 klemmt nicht" stimmt.
3. **TEX.TIM @0x14910** (eigener TIM-Parser): CLUT 1548 B, Bildblock @0x614, Daten @0x620, 320
   Halbworte breit (4bpp) -> Zeile v=129 @0x148a0, u=224/225 @0x14910; Texel u225..229 v129..133 =
   Ring `a a a / a . . . a / a . a . a / ...` mit Mittelpunkt (227,131) = Quad-Ursprung (224,128)+3
   -> Ring-Mitte mx-1/my-1. Stimmt.
4. **MAP03.PIX / MAP05.PIX @0x1454** (256x256 4bpp, 128 B/Zeile; v=40 -> 0x1400 + u168/2 = 0x1454):
   Kachel uv(168,40) = 10x10, Index 4 in Spalte/Zeile 0 und 9, Index 1 in 1..8 - auf beiden
   Blaettern. KF_INNEN_LO 1 / KF_INNEN_HI 8 stimmen.
5. Zeilen @0x800768b0+8*Idx (`re15_disasm.py read ... 4 --w 2 --signed`): 0x800768f0 [0,0,1,1],
   0x800769a8 [0,0,1,1], 0x800769b0 [129,150,3168,2305], 0x800769b8 [177,140,2496,2250],
   0x800769c0/0x800769c8 [0,0,1,1]. Seiten-Setzer `ori v0,zero,0x2` @0x8004b684 (nach
   `j 0x8004b888`), `sb v0,9742(at)` = 0x800b260e @0x8004b88c. Stimmt.
6. **G1** SCA ROOM1220.RDT (Zeiger @0x20 = 0x530), 11 Eintraege selbst gelesen: @0x05C0
   x -28186..-27611, @0x05CC x -21836..-21561, @0x0590 x -17275..-17075 z -28650..-4300, @0x065C
   x -11311..-10736, @0x0560 z -4500..-4300, @0x0578 z -4525..-4300, @0x062C/@0x0668
   z -11925..-11725, @0x05B4/@0x0698 z -19875..-19675, @0x05A8 z -28750..-26750 - alle wie im
   Kommentar re15_map_zones.h.

Diff-Suche (154a73c1..HEAD, nur `+`-Zeilen unter re15_port) nach deferred/tunable/interim/
for now/faithful/plausib/TODO/vermutlich/getenv/ungefaehr/geschaetzt: **0 Treffer**. Keine
Env-Schalter.

Konstanten ohne @0x, aber gekennzeichnet: Versatz ox/oy 155/73 bzw. 173/76 der 1080-Zeilen
("Versatz (ox,oy) = PORT-WAHL aus der Kunst", re15_map_zones.h); die Blattwahl der Kabine nach dem
Vorraum ("⛔ PORT-WAHL (Abnahme 0, G2)", karte_fahrstuhl_1080.c - G2 erledigt); die fuenf
1180/1230-Abschnitte (PORT-WAHL, aus Runde 1 unveraendert). Commit-Messages 11d1d8ea/e6c0121b
tragen @0x8006e1dc / @0x1454 / @0x14910 / @0x8006e2f0 / @0x800475d8.

## Gate: Vertrag / Pfade

`git diff --name-only 154a73c1 HEAD`: analysis/befunde_runde35/G_* (Dossier, Abnahme 0, Bilder,
G_karte_zeilen.py), engine/src/karte_fahrstuhl_1080.c (neu), include/re15_karte_fahrstuhl.h
(neu), engine/src/re15_inv_screen.c (+3: include, Kommentar, Aufruf), engine/src/re15_map_zones.c
(include + Haken + 2x Filter + angehaengte Tabellenzeilen), engine/src/re15_map_zones.h,
tests/unit/probes/r35_karte.cmake, tests/unit/test_r35_karte.c, tests/integration/
test_r35_karte.cmake, tests/integration/test_r35_karte_fahrstuhl.cmake,
tests/unit/test_map_etagenzeile.c. **Keine** release/, platform/android/, shared_assets/PSX/,
tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt. Keine Bank-9-Bits, keine
Nachrichten-IDs, keine AOT-Slots/Ereignisse (Spur G hat laut VERTRAG §1.1-1.3 keine). Neue
Kartendaten: zid 102..109, Besucht-Zusatzbits 242..250 von 256 (kartenintern).

Hinweise (wie Abnahme 0, keine Ausschluesse): re15_map_zones.h ist gemeinsame Datei mit ~120
Datenzeilen statt 1-5 Zeilen Haken (ohne sie ist der Auftrag nicht loesbar, jede Stelle traegt
"Runde 35 Spur G", kein anderer Zweig fasst die Datei an - merge-tree gegen master konfliktfrei);
re15_inv_screen.c steht nicht in der VERTRAG-Liste, der Haken ist 1 Aufruf + 1 include, markiert;
der fremde Pin test_map_etagenzeile.c ist gelockert (begruendet im Pin, Wirkungsprobe bleibt).

## Gate: Tests

Vorhanden und messend:
* Punkt 1: `unit_r35_karte_fahrstuhl` Teil (c) - 6 begehbare Lagen je Etage, Spanne >= 6 px je
  Achse UND Ring-Mitte auf Kachel-Index 1 (test_r35_karte.c:328-356); Teil (d) - die 4 SCA-Waende
  landen auf Index 4 (:357-378). `integration_r35_karte_fahrstuhl` - echte exe, echter Tuerweg
  1040/1120, 6 Laeufe, Spanne >= 6 px (1F x/y, 3F x), 180 Grad
  (test_r35_karte_fahrstuhl.cmake:111-157). Beide wuerden am alten Stand rot: meine
  Vorher-Messung (x 5 / y 2 px) und der 8fee1bb4-Stand (2/2 px, Abnahme 0) unterschreiten 6 px.
  Damit ist M2 aus Abnahme 0 aufgeloest.
* Punkte 2-4: `unit_r35_karte_b2`, `_r1230`, `_r1210`, `integration_r35_karte` (unveraendert aus
  Abnahme 0, dort als messend gewertet).
Lauf im Suite-Lauf: unit_r35_karte_fahrstuhl / _b2 / _r1230 / _r1210 Passed (#457-460),
integration_r35_karte Passed (70,8 s), integration_r35_karte_fahrstuhl Passed (168,8 s).
Zusaetzlich die Sonde direkt: `probe_r35_karte fahrstuhl` -> "Kabine 1F/2F: x 110..117 (7 px),
y 135..142 (7 px)", "3F: x 128..135, y 138..145", Waende W(118,138) O(109,138) N(113,134)
S(113,143) auf Index 4 - deckungsgleich mit meinen exe-Messungen; b2/r1230/r1210 OK.

---

## Urteile

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Fahrstuhl-Cursor 1080 | **erfuellt** | Tuerweg 1040/10C0/1120, 14 Laeufe: Ring x 110..117 / y 135..142 (7 x 7 px, 1F+2F), 3F x 128..135 / y 138..145 (7 x 7 px), Zwischenlagen stetig; vorher (master-exe) 5 x 2 px unter der Kabine, 3F falsches Blatt |
| 2 11F0 / 1200 erscheinen | erfuellt | Tuerweg aus 11E0: rot nur rect 1 (1392 px) bzw. rect 2 (995 px), Blatt B2 |
| 3 1230 zeigt 11E0-Karte | erfuellt | Tuerweg aus 11D0: Blatt B1, rot nur Gang rect 0 (1032 px) |
| 4 1210 Korridor + Tueren | erfuellt | Tuerweg aus 11E0: rot T-Korridor rect 3 (763 px), 6 gelbe Balken = 6 Door_aot_set, je <= 1 px an der Original-Projektion |

Maengel aus Abnahme 0: M1 (Bewegung) behoben (gemessen), M2 (Riegel) behoben, M3 (484) stimmt
jetzt (ctest -N 484), M4 (Dossier) korrigiert (G_karte.md Z. 165-167, 240-241, 251-254), G1
nachgetragen und nachgelesen, G2 gekennzeichnet.

## Maengel

**Keine.** Alle vier Punkte sind erfuellt; Suite 484/484 im eigenen Lauf, das @0x-Gate haelt
(6 Stichproben selbst disassembliert bzw. gelesen, Spruenge bis zum Ziel verfolgt, keine
Rate-Woerter), das Pfad-Gate haelt, die Tests messen jeden Punkt.

Hinweise ohne Ausschluss (fuer den Orchestrator bzw. eine spaetere Runde, alle im Dossier OFFEN):
* H1 - Ring in den Ecklagen der Kabine 1 px ueber der gemalten Wand (F1_l Bbox x108 vs. Wand
  x109, F1_r x119 vs. x118, F1_vor2 y133 vs. y134); Original klemmt nicht (RE1.5 @0x800475d8,
  RE2 @0x8006e2f0).
* H2 - Gang 1180/1230 schematisch: im Suedgang-Abschnitt (idx3, sy=547) 4 px Markerweg auf
  6850 Einheiten (P3_1230 -> P3_1230_w, y 122 -> 118).
* H3 - Gemessen am Zweigstand (Basis 154a73c1). master ist bei 87cc8575 (v0.8.22, enthaelt Spur K
  mit eigener Aenderung an re15_inv_screen.c); merge-tree konfliktfrei - nach dem Zusammenfuehren
  die Karte im Hauptbaum (Fahrstuhl 1F/3F, 1210) noch einmal am echten Weg pruefen.
* H4 - Unveraendert offen laut Dossier: Blatt 0 ROOM1190/11A0 vermutlich vertauscht
  (@0x80076978/@0x80076980), ROOM5020 Blatt 9 ohne sichtbares Rot, Fahrstuhl-Etage nach Laden in
  der Kabine aus Bank 3 Bit 54/55/56.
