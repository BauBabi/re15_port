# Runde 37 — Spur R "kiste2feld": Abnahme 0 (unabhaengig, 2026-10-13)

Baum `.claude/worktrees/r37_kiste2feld`, Zweig `r37/kiste2feld`, Basis badd228f, HEAD 8c44a2e3.
Geprueft gegen AUFTRAG.md (Nachtrag 9 + Orchestrator-Auftrag R) und VERTRAG.md. Nichts vom Bau-Agenten uebernommen,
was hier nicht selbst gemessen ist.

## 0. Stand (fortlaufend)

- [x] AUFTRAG/VERTRAG/Dossier gelesen, Code-Diff gelesen
- [x] RE-Gate: Stichproben selbst disassembliert (§2)
- [x] Bau (local_build.sh configure + build; Quellen des Diffs per touch neu uebersetzt, 0 Warnungen)
- [x] Gezielte Tests (§4: eigene 2/2, Module 86/86)
- [x] Messung an der exe je Punkt (§3.2-§3.5: Kiste Leon/Elza, echter MC51-Tausch, Gitter, Ausruest-Kasten)
- [x] Urteil (§7): beide Punkte erfuellt, keine Maengel -> bestanden

## 1. Code-Diff (gelesen)

Geaendert (git diff badd228f --name-only): `engine/src/kiste2feld_r37.c` (neu), `include/re15_kiste2feld_r37.h` (neu),
`engine/src/re15_inv_screen.c` (box_cell_icon_sized: Op-Feld u = Spanne 40/80 statt 0), `include/re15_inv_screen.h`
(Enum-Notiz), `platform/pc/src/inv_render_pc.c` (ITEMTILE-Zweig: Breitbild 80x30 bei u == 80 oder w == 80, sonst
40x30-Kachel, beides mit `re15_kiste2feld_texel`; inv_compose_cells: Kopf schreibt Spalten 0..39, Schwanz 40..79 an
das eigene uv), Tests `tests/unit/probes/r37_kiste2feld.cmake`, `test_r37_kiste2feld.c`, zwei Probe-Werkzeuge,
`tests/integration/test_r37_kiste2feld.cmake`, Dossier + Belegbilder.

Gelesen und nachvollzogen:
- `box_cell_icon_sized` gibt jetzt im Op-Feld u die Quell-Spanne (2-Feld 80, schmal 40) mit; einziger Aufrufer
  Block 7b (re15_inv_screen.c:1828). `box_cell_icon` (40x30, u 0) hat keinen Aufrufer mehr (toter Code, vorbestehend).
- ITEMTILE-Zweig (inv_render_pc.c ~1152): einziger Nutzer der Seite `RE15_INV_PAGE_ITEMTILE` ist die Kisten-Liste
  (grep ueber engine/platform/include: nur re15_inv_screen.c:1587/1598). Die Abtastaenderung trifft also nur die
  Kisten-Zeilensymbole (breit UND schmal), keine anderen Schirme.
- inv_compose_cells: `re15_kiste2feld_zelle` liefert fuer Flag 1 Spalte 0, fuer Flag 2 Spalte 40; der Schwanz-Platz
  traegt dieselbe Id wie der Kopf (inventory_common.c:50-51, re15_itembox.c:259-261/303-305; FUN_8004dc4c
  @0x8004de78-98 schreibt s0 = Id nach 0x800b10ac UND 0x800b10b0) — die Zuordnung greift also im Spiel.

## 2. RE-Gate — Stichproben SELBST disassembliert

Werkzeug `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (Binaerdatei info/re2leon/PSX.EXE) bzw.
`re15_disasm.py` (info/Re1.5/PSX.EXE):

1. RE2 FUN_800710dc Zeilenschleife (`re2_disasm.py dis 0x800717ac 60`):
   `800717b4: addiu t0,zero,40` (u0), `8007182c: beq v0,zero,0x80071840`, `80071834: addiu a3,zero,48`,
   `80071838: j 0x80071848`, `8007183c: addiu t1,zero,120` (Delay-Slot, breit), `80071840: addiu a3,zero,24`,
   `80071844: addiu t1,zero,80` (schmal), `80071848: sb t1,-14(s0)` (s0 = Prim+0x22 -> Prim+0x14 = u1),
   `8007184c: sb t0,-22(s0)` (u0), `80071858: sb t0,-6(s0)` (u2), `80071860: sb t1,2(s0)` (u3),
   `800717c8: addiu s2,zero,270` / `800717d0: addiu s1,zero,240` (v2 - v0 = 30), `80071884: addiu v1,v1,147`
   (x1 = boxX+147+Breite). **Bestaetigt**: breit u 40..120 = 80 Texel auf 49 px, schmal 40..80 = 40 Texel auf 25 px.
2. RE2 FUN_80069c40 (`re2_disasm.py dis 0x80069c40 76`): `80069c74: addiu v0,zero,20` (RECT.w 20 hw),
   `80069cd8: addiu v0,zero,19` / `80069cdc: beq s2,v0,0x80069d68` (Id 0x13 ohne zweite Kachel),
   `80069ce0: addiu v0,s2,-14` / `80069ce4: sltiu v0,v0,0x6`, `80069cec: addiu v1,s0,1` (Zelle+1),
   `80069d4c: lui a1,0x1` / `80069d50: ori a1,a1,0x9320` / `80069d58: jal 0x80090664` / `80069d5c: addu a1,s1,a1`.
   **Bestaetigt**: zweite Kachel = src + 0x19320 (= 86 * 1200 = Kachel Id+0x56) in die Nachbarzelle.
   Anmerkung: RE2 nimmt Id 0x13 aus (in RE2 kein 2-Feld-Item); der Port folgt fuer die RE1.5-Ids 0x0e..0x13 dem
   RE1.5-Gate `addiu v0,s0,-14 / andi v0,v0,0xff / sltiu v0,v0,0x6` @0x8004dc70-78 (selbst gelesen) — 0x13 = MC51 ist
   in RE1.5 ein 2-Feld-Item. Folgerichtig.
3. RE1.5 (`re15_disasm.py dis 0x8001e0a8 12`): `8001e0b8: lui a2,0x800b` / `8001e0bc: lw a2,-14468(a2)` (0x800ac77c) /
   `8001e0c4: jal 0x8004dc4c` / `8001e0c8: addiu a2,a2,8608` (+0x21A0). **Bestaetigt.**
4. RE1.5 FUN_800492b8 (`dis 0x800492b8 18`, `dis 0x80049300 30`): Modus 1 (`800492d0: beq a1,v0,0x80049300`) ->
   `80049314: ori v0,zero,0x28` (RECT.w 40 hw = 80 Texel), `8004931c: ori v0,zero,0x1e`, `80049370: jal 0x80068c88`
   (LoadImage laut RE15_FUN_CATALOG). Modus 0 dagegen `800492fc: ori v0,zero,0x14`. **Bestaetigt.**
5. RE1.5 FUN_80049390 (`dis 0x800493a0 8`, `dis 0x80049454 4`): `800493ac: ori v0,zero,0x14`,
   `800493b4: ori v0,zero,0x1e`, `8004945c: jal 0x80068d50` (ghidra1_V2.txt Z. 162453: `jal MoveImage`). **Bestaetigt.**
6. RE1.5 FUN_8004dc4c (`dis 0x8004de40 32`): `8004de44: addu a0,zero,zero`, `8004de48: ori a1,zero,0x1`,
   `8004de64: ori v0,zero,0x1` -> `sb v0,4270(at)` (0x800b10ae = Flag Platz 0), `8004de70: ori v0,zero,0x2` ->
   `sb v0,4274(at)` (0x800b10b2 = Flag Platz 1), `8004deb4: j 0x8004df08` / `8004deb8: addu a2,s2,zero`,
   `8004df08: jal 0x800492b8`. **Bestaetigt** (Kopf/Schwanz-Flag 1/2, Breitbild ueber Zelle 0/1).
7. RE1.5 FUN_80049a5c (`dis 0x80049d6c 62`): `80049d7c: ori v0,zero,0x1` / `80049d80: bne v1,v0,...` (Flag == 1),
   `80049d8c: addiu at,at,25156` (0x80076244 uv-Tabelle), `80049df4: jal 0x8006b538`, `80049e08: addiu v0,v0,1`
   (Zelle eq+1), `80049e20: addiu s1,s1,40`, `80049e60: addiu v0,v0,40` (x + 40). **Bestaetigt**: Ausruest-Kasten =
   zwei 40x30-SPRTs aus den Speicherzellen eq und eq+1.

Konstanten im Diff ohne Disasm-Adresse: nur das Abtastmodell `(2048 + p*((n_tex<<12)/n_px)) >> 12` (GPU-Hardware,
kein Spielcode). Es ist im Kopf als Modell des PSX-GPU-Rasterizers gekennzeichnet und am RE2-Bild belegt; **selbst
nachgemessen** (§3.1) — es ist das einzige der geprueften Modelle, das das RE2-Bild vollstaendig erklaert.

## 3. Messungen

### 3.1 RE2-Referenzbild: Abtastmodell und zweite Kachel (eigene, CLUT-freie Pruefung)

Eigenes Werkzeug (scratchpad `tools/abn_re2ref.py`, NICHT das des Bau-Agenten): RE2 info/re2leon/COMMON/DATA/ITEMALL.PIX
(127200 Byte = 106 Kacheln) gegen analysis/itembox_re2/referenz/re2_itembox_original.png. Symbolfelder selbst aus dem
Bild bestimmt (Kachelblau 0x181840 links / 0x181838 rechts gegen Listengrund 0x181828): x 153..201 (49 px) bzw.
153..177 (25 px), Zeilen ab y 41/61/101/121 (19 px). Statt einer CLUT wird je Modell geprueft, ob "Texel-Index (je
Kachel) -> Bildfarbe" eine FUNKTION ist (gezaehlt: Pixel mit der Mehrheitsfarbe ihres Index; Unendlich-Zeichen
p <= 14, py >= 10 ausgenommen):

| Zeile | gerundet `(2048+p*step)>>12` | abgerundet `p*n/m` | Pixelmitte `floor((p+.5)n/m)` |
|---|---|---|---|
| Gatling (Kachel 0x12 + 0x12+0x56, 80 -> 49) | **796/796** | 649/796 | 682/796 |
| R. Launcher (0x11 + 0x11+0x56) | **796/796** | 672/796 | 674/796 |
| F. Aid Spray (0x23, 40 -> 25), Zeile 1 und Zeile 3 | **475/475** | 433/475 | 421/475 |

Gegenprobe (falsche rechte Kachel): Gatling mit rechts = 0x13+0x56 -> 578/796, rechts = 0x12 -> 569/796 — die
Pruefung unterscheidet. Ergebnis: RE2 zeichnet die ganze Waffe aus Kachel Id + Id+0x56, gerundet abgetastet — breit
und schmal. Das bestaetigt die Behauptung des Bau-Agenten (er hatte mit Masken 770/771 bzw. 752/771; mit der
CLUT-freien Pruefung sind es 100 %, die "Restabweichung" seines §6 ist ein Artefakt seiner Maske).

RE1.5-Quelle selbst geprueft: shared_assets ITEMALL.PIX (86400 = 72 Kacheln) und ITEM/ITPS.ITP sind bytegleich mit
info/Re1.5/PSX; ITPS-Breitbild Spalten 0..39 == ITEMALL-Kachel: 0x0e 766/1200, 0x0f-0x11 1195/1200, 0x12/0x13
1200/1200 (wie Dossier §1.2).

### 3.2 Kisten-Liste Leon ROOM1150 (echter Weg: Spielstand + CONTINUE, Viereck am Kisten-AOT)

Messkopie `re15_pc_r37k2f_abn0.exe`, Karte `probe_r37_kiste2feld_karte re15_card.mcr 1150 -25650 -12100 0 alle`,
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
RE15_PAD_AT=40:A,90:A,100:D,115:D,130:U,145:U,160:U RE15_FRAMEDUMP=95-175/1:f RE15_EXIT_AT=176#1150`.
debug.log: `[save] CONTINUE: resumed in room 1150 (hp=100)`, `[flow] EXIT_AT: Bild 176 in Raum 1150 erreicht -> exit`.

Eigene Pixelpruefung (scratchpad `tools/abn_k2f.py`, eigene Umsetzung; Erwartung "rund" = round(p*n/m) auf das
ITPS-Breitbild bzw. die 40er-Kachel, "alt" = 40er-Kachel 1:1 + Spalte 39 wiederholt):

| Bild | Zeile 0 | Zeile 1 | Zeile 2 | Zeile 3 | Zeile 4 |
|---|---|---|---|---|---|
| F99 | 0e 867/867 (alt 453) | 0f 867/867 (319) | 10 818/818 (288) | 11 818/818 (288) | 12 867/867 (401) |
| F110 | 0f 867/867 | 10 867/867 | 11 818/818 | 12 818/818 | 13 867/867 (alt 273) |
| F125 | 10 867/867 | 11 867/867 | 12 818/818 | 13 818/818 (alt 242) | 0b schmal 411/411 (alt 333) |
| F170 | 15 schmal 315/315 (alt 198) | 0e 867/867 | 0f 818/818 | 10 818/818 | 11 867/867 |

Spalten 40..48 gleich Spalte 39: 0/9 bei allen Ids ausser 0x12 (4/9 — gleichfarbiger Rohrlauf in der Quelle selbst;
die "rund"-Erwartung haelt dort 818/818 bzw. 867/867). Die rechte Symbolhaelfte (px 25..48) traegt 426..456
Nicht-Grund-Pixel. Sichtpruefung F99 (scratchpad `abn_R/kiste/F99_x3.png`, `F99_list_x4.png`): Flammenwerfer mit
Tank, Granatwerfer mit Schaft, Raketenwerfer ganz — keine Streifen mehr; Symbole buendig rechts wie Spur M.

### 3.3 Der Spielfall: MC51 ueber die Kisten-Bedienung in die Kiste legen (nicht im Riegel des Bau-Agenten)

Die einzige platzierte 2-Feld-Waffe ist die MC51; der Nutzer sieht den Fehler, wenn er sie in die Kiste legt. Der
Riegel integration_r37_kiste2feld setzt die Kiste per Karte (Belegung "alle"); den Tausch selbst faehrt er nicht.
Selbst gefahren: Karte `... 1150 -25650 -12100 0 mc51` (Inventar 0/1 = MC51 x100 Flag 1/2, 2 = Browning, 3 = 0x15;
Kiste 0 = 0x0B, 1 = 0x15, 2 = LEER, 3 = 0x1D, 4 = 0x44), `RE15_PAD_AT=40:A,90:A,100:A` (Viereck am Kisten-AOT,
Viereck = MC51 waehlen -> Box-Seite, Viereck = Tausch in die leere Auswahlzeile; re15_itembox.c Fall 2 setzt
Size 3 = RE15_BOX_KIND_WIDE), `RE15_FRAMEDUMP=85-140/1:f`, `RE15_EXIT_AT=141#1150`.
debug.log `[save] CONTINUE: resumed in room 1150 (hp=100)`, `[flow] EXIT_AT: Bild 141 in Raum 1150 erreicht -> exit`.

| exe | Bild | Zeile 2 = MC51 (0x13 x100, 2-Feld) | schmale Zeilen 0x0B / 0x15 / 0x1D / 0x44 |
|---|---|---|---|
| nachher (HEAD, Kopie re15_pc_r37k2f_abn0.exe) | F105, F120, F139 | rund **722/722**, alt 170/722, Spalten 40..48 = 39: 0/9 | rund 411/411, 315/315, 338/338, 475/475 |
| vorher (Kopie der Bau-Agenten-exe re15_pc_r37k2f_vorher.exe als re15_pc_r37k2f_abn0v.exe) | F139 | rund 170/722, **alt 722/722**, Spalten 40..48 = 39: **9/9** | alt 411/411, 315/315, 338/338, 475/475 |

Bild scratchpad `abn_R/mc51_vorher_nachher_x4.png`: vorher Lauf + grauer Streifen, nachher die ganze MC51 (Lauf,
Griff, Magazin, Schaft) — wie RE2 die Gatling Gun im Referenzbild. Die Vorher-exe zeigt genau den Befund des Auftrags
(Bestaetigung, dass die Gegenprobe den alten Stand trifft). Ganzer Schirm nachher: `abn_R/mc51/F139_x3.png`.

Elza ROOM1151, gleicher Weg (Karte `... 1151 ... mc51`, Figur 4): `[save] CONTINUE: resumed in room 1151`, F139
Zeile 2 MC51 **722/722** (alt 170), schmale Zeilen alle "rund" gleich (`abn_R/elza/F139_x3.png`).

### 3.4 Inventar-Gitter (Statusschirm) und Ausruest-Kasten

Eigenes Werkzeug (scratchpad `tools/abn_gitter.py`): Zellen-Lage selbst gelesen (`re15_disasm.py read 0x80076274 20
--w 2`: `[4,32, 44,32, 4,62, 44,62, 4,92, 44,92, 4,122, 44,122, 4,152, 44,152]`; uv-Tabelle `read 0x80076244 12`:
u = (k%3)*40, v = (k/3)*30), Listenbasis (215,26); verglichen die deckenden Texel (ST_00-CLUT != 0) der ITPS-Haelften,
Ziffernfeld der Kopfzelle ausgenommen. Karte "alle" bzw. "ausr" (MC51 ausgeruestet, Kopf Zelle 5), `RE15_PAD_AT=40:S`,
Bild F80; je Lauf `CONTINUE: resumed in room 1150` im debug.log.

| Kopfzelle (Speicher-Spalte) | Id | links (Zelle k) | rechts (Zelle k+1) vorher -> nachher |
|---|---|---|---|
| 0 (0) | 0x0F | 1135/1135 | 1183/1183 -> 1183/1183 |
| 2 (2) | 0x12 | 1136/1136 | **257/1200 -> 1200/1200** |
| 5 (2) | 0x13 | 1088/1088 | **0/1200 -> 1200/1200** |
| 8 (2) | 0x0E | 1040/1040 | **0/1200 -> 1200/1200** |
| Ausruest-Kasten (MC51, x 130 / 170, y 58) | 0x13 | 1088/1088 | **530/1200 -> 1200/1200** |

Gleich in den Laeufen "status" und "ausr". Bild `abn_R/ausr_vorher_nachher_x2.png`: vorher endet die MC51 im
Ausruest-Kasten nach der vorderen Haelfte, im Gitter zeigen die Schwanzzellen Spraydose bzw. leer; nachher alles
vollstaendig. Kopf in Speicher-Spalte 0 (0x0F) ist vorher wie nachher bitgleich — keine Nebenwirkung.

Erreichbarkeit (selbst nachvollzogen in re15_itembox.c): Kopf in Speicher-Spalte 2 entsteht nur mit einer zweiten
2-Feld-Waffe (Fall 3 = 2-Feld <-> 2-Feld an Ort und Stelle `cur`, oder Zusammenschieben hinter einer zweiten Waffe).
Mit nur der MC51 landet sie immer in Zelle 0/1 (Fall 1 `inv_front_shift2`). Der Gitter-Teil ist also Vorsorge "an
anderen Stellen derselben Kachel-Logik", wie der Auftrag es verlangt; der Kisten-Teil trifft das Spiel sofort (§3.3).

### 3.5 Nachtrag 9 (Lage der Symbole, Spur M) — keine Rueckwirkung

Rechtestes helles Textpixel je Zeile (eigene Auswertung der Bilder §3.2/§3.3) gegen den Symbolanfang:
F99 (fuenf 2-Feld-Zeilen): Text bis x 106/136/136/136/127, Symbol ab x 155. MC51-Bild F139: Text bis x
146/104/89/157/171, Symbol ab x 179 (schmal) bzw. 155 (MC51, "HXOK MC51" endet bei x 89). Kein Name reicht an ein
Symbol; Lage x 155..203 (49 px) bzw. 179..203 (25 px) unveraendert wie Spur M. Die Spur-M-Riegel unit_r37_kiste und
integration_r37_kiste (Rahmen ueber den Zeilen, Symbol buendig rechts) sind gruen (§4).

Item-Modal (Aufnahme): eigener Weg render_pc.c item_modal_build_tex (re15_itps_pixel, 112x72), vom Diff nicht
beruehrt (Code gelesen) — nicht betroffen.

## 4. Tests (gezielt, keine volle Suite)

1. Eigene: `ctest --test-dir re15_port/build -R "r37_kiste2feld" --timeout 600 --output-on-failure -V` ->
   `unit_r37_kiste2feld ... Passed 0.02 sec` (`=== OK ===`), `integration_r37_kiste2feld ... Passed 54.00 sec`
   (`r37_kiste2feld[kiste] ROOM1150: 26`, `[elza] ROOM1151: 16`, `[status] ROOM1150: 8`, `[ausruest] ROOM1150: 10
   Symbole/Haelften pixelgleich`) -> `100% tests passed, 0 tests failed out of 2`.
2. Module des Diffs (re15_inv_screen.c, inv_render_pc.c = Inventar-, Kisten-, Karten-, Dokumentenschirm, Symbol-Cache,
   ersetzte Kacheln 0x40/0x42/0x45) + Werfer-Unit-Riegel (werfer_r35.c fasst Flag 2 an):
   `ctest --test-dir re15_port/build -R "r37_kiste|itembox|inv_|inventar|map_|karte|dokument|dachkiste|kartenitems|item_icon|item_modal|r30_sicherung_bild|r36_schluessel|unit_r35_werfer" --timeout 900 -j 3 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 86`** (Total Test time (real) = 1332.01 sec; parallel liefen
   Fenster-Haken anderer Baeume, keine Wiederholung noetig). Darin u.a. unit/integration_r37_kiste (Spur M),
   unit_itembox_transfer, unit_itembox_screen, unit_room1150_itembox, unit/integration_r30_sicherung_bild,
   unit/integration_r36_schluessel*, integration_r36_inventar, integration_r35_inventar1050, unit_r35_inventar1050_*,
   integration_r34n_e_dokumente_bild, integration_r36_kartenitems_bild, integration_r36_dachkiste, integration_map_*.
3. Unterscheidungskraft der Riegel-Werkzeuge selbst geprueft: `probe_r37_kiste2feld_bild` auf MEINE Vorher-Bilder
   (Kopie der Vorher-exe): ausruest -> `530/1200 ... FALSCH`, `KISTE2FELD-BILD FEHLER 1`; gitter -> `FEHLER 3`;
   kiste (MC51-Tausch F139) -> Zeile 2 `Spalten 40..48 gleich Spalte 39: 9/9 -> FALSCH`, `FEHLER 5`. Der Riegel
   unterscheidet also alten und neuen Stand.

Messkopien `re15_pc_r37k2f_abn0.exe` / `re15_pc_r37k2f_abn0v.exe` nach den Laeufen geloescht; keine fremde exe beendet.

## 5. Vertrag (VERTRAG.md)

- Pfade (`git diff badd228f --name-only`): nur analysis/befunde_runde37/R_*, engine/src/kiste2feld_r37.c,
  include/re15_kiste2feld_r37.h, engine/src/re15_inv_screen.c, include/re15_inv_screen.h (nur Enum-Kommentar),
  platform/pc/src/inv_render_pc.c, tests/unit/probes/r37_kiste2feld.cmake + Testquellen,
  tests/integration/test_r37_kiste2feld.cmake. Keine release/, platform/android/, shared_assets/PSX/, keine
  Test-CMakeLists. **Haelt.**
- §1.4 Spur R: re15_inv_screen.c (box_cell_icon_sized) und inv_render_pc.c an Ort und Stelle, Kommentar
  "Runde 37 Spur R" (6 Stellen im Diff); neue Logik in kiste2feld_r37.c / re15_kiste2feld_r37.h. **Haelt.**
- §1.1-1.3: keine Bank-9-Bits, keine Nachrichten-Ids, keine Ereignisse/Slots belegt. **Haelt.**
- §1.5: Tests nur ueber probes/r37_kiste2feld.cmake; integration-Skript mit `cmake_minimum_required(VERSION 3.16)`;
  keine POSIX-Aufrufe (kein popen/system) in den neuen C-Quellen; eigener exe-Name im Riegel (Muster r34n_b).
  **Haelt.**
- §1.6: keine neuen Assets (rechte Haelfte aus dem vorhandenen ITEM/ITPS.ITP, bytegleich mit der Original-CD).
  **Haelt.**
- Commit-Messages tragen die @0x-Belege (ebc575f1, 8c44a2e3 gelesen).

## 6. Gates

- RE-Gate: jede Konstante des Code-Diffs traegt @0x (40/80/30 @0x800717b4/@0x80071844/@0x8007183c/@0x800717c8-d0,
  0x3000/0x21A0 @0x8001e0b8-c8, 80/30 @0x80049314/@0x8004931c, 40 @0x800493ac, Flag 1/2 @0x8004de64/@0x8004de70).
  Sieben Stellen selbst disassembliert (§2), alle enthalten das Behauptete. Abtastmodell (GPU-Hardware) selbst
  am RE2-Bild nachgemessen (§3.1). **Haelt.**
- Rate-Woerter: `git diff badd228f -- re15_port | grep "^+" | grep -i -E "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|ungefaehr|geschaetzt|vermutlich"` -> kein Treffer.
  Kein `getenv` im Produktionscode des Diffs (kein Env-Schalter als Abschluss). **Haelt.**
- Erklaert der Fix den Befund? Ja: vorher u = 0 -> Zeichner nahm ITEMALL-Kachel (= linke Haelfte) 1:1 und klemmte
  Spalte 39 (gemessen: alt-Modell 722/722, 9/9 Spalten = 39); nachher Spanne 80 -> ITPS-Breitbild gerundet auf 49x19
  (gemessen: 722/722 bzw. 867/867, 0/9). Gitter: vorher 80-Texel-Schreiben ab Kopfzelle in Speicher-Spalte 2
  (rechte Haelfte ausserhalb, Schwanz alt: 257/0/0), nachher je Zelle eigene Haelfte (1200/1200). **Haelt.**
- Build: local_build.sh configure + build OK, 0 Warnungen (nach touch der Diff-Quellen neu uebersetzt).

## 7. Urteil je Punkt

| # | Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|---|
| 1 | Nachtrag 9: "In der Kiste müssen die Icons offenbar so weit wie möglich rechts liegen, damit sie nicht vom Text überdeckt werden." (Spur M erledigt; Spur R schliesst den vorbestehenden Fehler derselben Symbole) | **erfuellt** | Lage unveraendert buendig rechts (x 155..203 / 179..203), kein Name erreicht ein Symbol (§3.5); Spur-M-Riegel gruen (§4); die 2-Feld-Symbole an dieser Stelle sind jetzt vollstaendig (§3.2/§3.3). |
| 2 | Auftrag R: 2-Feld-Symbole (49 px) in der Kisten-Liste vollstaendig wie RE2 (zwei Cache-Spalten u1 0x78, zweite Kachel FUN_80069c40); auch Inventar-Gitter / andere Stellen derselben Kachel-Logik pruefen | **erfuellt** | RE2-Mechanismus selbst disassembliert (§2.1/2.2) und am RE2-Bild CLUT-frei 796/796 bestaetigt (§3.1); exe: alle sechs 2-Feld-Ids in 4 Bildern + MC51 nach echtem Tausch bei Leon und Elza pixelgleich der gerundet abgetasteten 80x30-Breitkunst, 0/9 wiederholte Spalten (vorher 9/9) (§3.2/§3.3); Gitter-Schwanzzellen 1200/1200 (vorher 257/0/0), Ausruest-Kasten 1200/1200 (vorher 530) (§3.4); Item-Modal nicht betroffen. |

## 8. Maengel

Keine. Hinweise (nicht abnahmerelevant, fuer Dossier/Integration):

- H1: Der Riegel integration_r37_kiste2feld legt die Kiste per Karte an; den Spielweg "MC51 per Kisten-Bedienung in
  die Kiste" (Fall 2 setzt Size 3) faehrt er nicht. Der Zeichenweg ist derselbe; selbst gefahren (§3.3: Leon 1150 und
  Elza 1151, 722/722). Ein vierter Riegel-Lauf mit Belegung "mc51" und `RE15_PAD_AT=40:A,90:A,100:A` waere billig.
- H2: Die "Restabweichung" im Dossier §2.3/§6 (R. Launcher 752/771, Gatling 770/771) ist ein Artefakt der dortigen
  Waffen-/Grund-Maske: CLUT-frei erklaert das gerundete Modell das RE2-Bild zu 100 % (§3.1). Die Messskripte des
  Bau-Agenten (re2_maske.py u.a.) liegen nur in seinem scratchpad, nicht im Repo.
- H3: Dossier §2.4 nennt das Gate "@0x8004dc74-78"; die `addiu v0,s0,-14` steht bei @0x8004dc70 (andi @0x8004dc74,
  sltiu @0x8004dc78). Inhaltlich richtig.

**bestanden: ja** — beide Punkte erfuellt, alle Gates halten.

