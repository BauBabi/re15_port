# Runde 37 — Spur M "kiste": Unabhaengige Abnahme 0

Baum `.claude/worktrees/r37_kiste`, Zweig `r37/kiste`, gepruefter Stand **13885c53** (Basis 70030492).
Pruefer: unabhaengige Abnahme (keine Code-Aenderung). Datum 2026-10-12.

Wortlaut des Nutzers (AUFTRAG.md Nachtrag 9):
> In der Kiste müssen die Icons offenbar so weit wie möglich rechts liegen, damit sie nicht vom Text überdeckt werden.
> Außerdem ist mir aufgefallen, das beim scrollen/bewegen in der Kiste währendessen die Items oberhalb des Kastens
> liegen. Das darf nicht sein, der Kasten muss layertechnisch über den Items liegen.

## 0. Stand der Abnahme (fortlaufend)

- [x] AUFTRAG/VERTRAG/Dossier gelesen, Code-Diff `git diff master...HEAD -- re15_port` gelesen
- [x] Bau: `local_build.sh configure` + `build` -> LOCAL-BUILD-OK, "ninja: no work to do" (exe 15:16 juenger als letzte
      Code-Aenderung 14:52; nach 5cde2afa nur Test-Skripte geaendert)
- [x] RE-Gate (Stichproben, §3)
- [ ] gezielte Tests
- [x] Messung M1 an der exe (§1)
- [x] Messung M2 an der exe (§2)
- [ ] Vertrag / Nebenwirkungen


## 3. RE-Gate (selbst disassembliert, info/re2leon/PSX.EXE == SLUS_007.48 per cmp, t_addr 0x80010000)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis|bytes` (liest die BINAERDATEI, nicht den Ghidra-Text),
gegengelesen in ghidra_re2_Leon.txt / RE2_Quellcode_V2.

| # | Behauptung (Diff/Dossier) | selbst gelesen | Ergebnis |
|---|---|---|---|
| R1 | Menue-OT 8 Eintraege: `li a1,0x8` @0x8002af34, `jal ClearOTagR` @0x8002af90 | `8002af34: addiu a1,zero,8`; `8002af90: jal 0x800908a8` (Ghidra: `jal ClearOTagR`, Funktion referenziert den String "ClearOTagR(%08x,%d)" @0x800908d4) | stimmt |
| R2 | DrawOTag ab Eintrag 7: `lw a0,0x40(s2) / jal DrawOTag / addiu a0,a0,0x1c` @0x8002bd54-5c | `8002bd54: lw a0,64(s2)`, `8002bd58: jal 0x800909a0`, `8002bd5c: addiu a0,a0,28` (davor @0x8002bd4c die Spiel-OT +4092) | stimmt: Reihenfolge 7..0, Eintrag 2 wird NACH 6 und 3 gezeichnet = oben |
| R3 | FUN_800710dc: OT-Basis 0x800cc1e8 @0x80071118-1c, Eintrag 2 `addiu v0,t1,8` @0x8007112c, Eintrag 6 `addiu v0,t1,0x18` @0x80071154 | `80071118: lui t1,0x800d`, `8007111c: addiu t1,t1,-15896` (=0x800cc1e8), `8007112c: addiu v0,t1,8`, `80071148: sll v1,a0,5`, `80071154: addiu v0,t1,24`; Dekompilat: 16 Rahmen-Sprites (Tabelle 0x800a9c38..77) `AddPrim(ot=0x800cc1f0+buf*0x20)`, Zeilen-Symbole an `0x800cc200+buf*0x20` | stimmt |
| R4 | Fensterrand rechts `addiu s6,t1,0xc5` @0x80071238 / Auswahllinie `addiu v0,t1,0xc4` @0x80071adc (t1 = boxX) | `80071238: addiu s6,t1,197`, `80071adc: addiu v0,t1,196`, t1 = `lw t1,40(sp)` = DAT_800d5c40 | stimmt (204 exklusiv / 203 inklusiv) |
| R5 | Symbol x0 `addiu t1,t1,0x92` @0x800717d4, Breite `li a3,0x30` @0x80071834 / `0x18` @0x80071840, `addiu v1,v1,0x93` @0x80071884, `li t1,0x78` im Delay-Slot @0x8007183c | `800717d4: addiu t1,t1,146`, `80071834: addiu a3,zero,48`, `8007183c: addiu t1,zero,120` (Delay-Slot von `j` @0x80071838), `80071840: addiu a3,zero,24`, `80071884: addiu v1,v1,147` | stimmt (RE2: 153..177 / 153..201) |
| R6 | Rahmen-Tabelle @0x800a9c38: Kopfband (6,6)+(134,6), Fussband (6,127)+(134,127) | Bytes: Eintraege 7..10 = (6,6) (6,127) (134,6) (134,127); Marken-Tabelle @0x800a9c78 = 5x (200,12) | stimmt |
| R7 | Ziffern-Tabelle @0x800aae2c `bd ff ee ff / bd ff 02 00 / ...`, Ursprung `li v0,0xdc` @0x80074b88 / `li v0,0x46` @0x80074b90 | Bytes `bd ff ee ff bd ff 02 00 bd ff 16 00 bd ff 2a 00 bd ff 3e 00` = (-67,-18+20k); `80074b88: addiu v0,zero,220`, `80074b8c: sh v0,52(s1)`, `80074b90: addiu v0,zero,70`, `80074b94: sh v0,54(s1)` | stimmt (x 153 = linke Symbolkante, y 52+20k = Zeilenoberkante 41+20k + 11) |
| R8 | einlaufende Ziffern `li s7,0x52` @0x80072e48 (Platz scroll+5) / `li s7,-0x26` @0x80072e74 | `80072e48: addiu s7,zero,82` mit `addiu v0,v0,5` @0x80072e54; `80072e74: addiu s7,zero,-38` | stimmt |
| R9 | Ziffern in OT-Eintrag 3 (FUN_8007300c AddPrim 0x800cc1f4) | Dekompilat FUN_8007300c: beide `AddPrim(&UNK_800cc1f4 + buf*0x20, ...)` (= 0x800cc1e8 + 0xc) | stimmt |
| R10 | Namen: Textkopf 2i/2i+1 -> OT-Eintrag i (FUN_80031bf0 @0x80031c38-58) | `80031c00: addiu s2,zero,8`, a1 = 0x800ce5e0-9208 = 0x800cc1e8, Schleife `jal 0x8008f918` (AddPrim) @0x80031c3c/@0x80031c48, `addiu s1,s1,24`, `addiu s0,s0,4` | stimmt (Kopf 12 -> Eintrag 6) |
| R11 | Einlauf-Zeile Symbol: Zustand 2 local_50 -0x14 / local_48 -1, Zustand 3 100 / 5 | Dekompilat FUN_800710dc: `if (uVar11 == 2) {local_50 = -0x14; local_48 = -1;} if (uVar11 == 3) {local_50 = 100; local_48 = 5;}`, y = Versatz + boxY + local_50 + 0x1b | stimmt |

Konstanten im Code-Diff (re15_kiste_r37.h, re15_inv_screen.c Block 7b, inv_render_pc.c):
- `RE15_KISTE_SYMBOL_RECHTS 204` @0x80071238 (R4) + als NUTZER-VORGABE gekennzeichnet (rechtsbuendig statt RE2s gemeinsamer
  linker Kante 153) — zulaessig.
- `RE15_KISTE_FENSTER_BX0/BX1/BY0/BY1` 7/197/27/127: boxX+6, boxX+0xc4 @0x80071adc, Band-Tabelle @0x800a9c38 + Messung
  BOXPANEL.BIN — belegt. `RE15_KISTE_PANEL_X/Y/W/H` 6/25/214/156 = bestehende Port-Lage des PANEL-Ops (Port-Wahl aus
  2026-08-31), per `#error` gegen RE15_BOX_PANEL_Y gekoppelt — kein neues Verhalten.
- `RE15_KISTE_ZIFFER_DX/DY` 0/11 (R7), `RE15_KISTE_EINLAUF_HOCH/RUNTER` -1/5 (R8, R11) — belegt.
- `-3` / `-20` im box_digits-Aufruf gleichen die bestehende Glyphen-Geometrie von box_digits aus (FUN_80048f28 x+3, y+20,
  im Funktionskopf belegt) — keine neue Konstante.
- Suche im Diff nach "deferred|tunable|interim|for now|faithful|plausibel|TODO|approx": siehe §3a.

## 1. Punkt M1 — "Icons so weit wie moeglich rechts, nicht vom Text ueberdeckt"

### Messweg (eigene Werkzeuge, nicht die des Bau-Agenten)
- Eigenes Kartenwerkzeug (Scratchpad `M_abn0/src/abn_karte.c`, gegen `libre15_engine.a` dieses Baums gelinkt; schreibt
  Spielstand + frei waehlbare Kistenbelegung). Kiste mit den LAENGSTEN Namen der Namensbank belegt:
  63 = 0x5B "K. Sullivan - R. Aiken", 0 = 0x44 "Minidisc Player w/ Disc", 1 = 0x59, 2 = 0x5A, 3 = 0x1D x100,
  4 = 0x10 x6 (2-Feld), 5 = 0x43, 6 = 0x0B x3, 7 = 0x15 x100, 62 = 0x47.
- Echter Weg: Spielstand ROOM1150 Leon vor der Kiste (-25650,-12100, Gier 0; Kisten-AOT main00 `2C 05 03 31 ... F4 9D C8 CE
  40 06 E8 03` = x -25100..-23500 / z -12600..-11600 nachgerechnet), CONTINUE, VIERECK oeffnet die Kiste ueber das AOT.
- exe = Kopie der gebauten `re15_pc.exe` unter eigenem Namen (`re15_pc_abn0_<n>.exe`, danach geloescht; nie taskkill):
  `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
  RE15_PAD_AT=40:A,90:A,100:D,120:D,140:U,160:U,180:U,200:U RE15_FRAMEDUMP=95-215/1:f RE15_EXIT_AT=216#1150`
  -> debug.log `[save] CONTINUE: resumed in room 1150`, `[flow] EXIT_AT: Bild 216 in Raum 1150 erreicht`, 121 Bilder.
- Auswertung `M_abn0/py/zeilen.py` (eigene Spalten-Laeufe der Nicht-Listengrund-Pixel je Zeile im Fenster x 13..203).

### Ergebnis nachher (F99 = Ruhebild, Box-Seite aktiv)
| Zeile | Name | Name endet x | Symbol x | Luecke |
|---|---|---|---|---|
| 0 | 0x44 Minidisc Player w/ Disc | 173 | 179..203 (schmal) | 5 px (174..178 reiner Listengrund) |
| 1 | 0x59 E. Dewey - K. Dooley | 164 | (Kachel ohne Pixel) | - |
| 2 | 0x5A J. Frost - F. Speyer | 159 | (Kachel ohne Pixel) | - |
| 3 | 0x1D Empty Grenade Shells x100 | 158 | 179..203, Ziffern "100" unten links im Symbol | 20 px |
| 4 | 0x10 2-Feld x6 | 137 | 155..203 (breit) | 17 px |

Jedes Symbol endet bei x 203 = letzte Spalte des Listenfensters (BOXPANEL.BIN-Listengrund x 13..203, selbst aus dem Asset
bestimmt); rechts davon liegt der Rahmen (x 204..207) bzw. die Schiene. Weiter rechts ginge nur UNTER den Rahmen, der jetzt
oben liegt — "so weit wie moeglich rechts" ist erreicht. Kein Name beruehrt ein Symbol.
- Elza ROOM1151 (`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, gleicher Weg): Zeilen-Laeufe bitgleich zu Leon
  (173 / 179..203, 155..203).
- Gegenprobe VORHER (Kopie der Hauptbaum-exe vom 2026-10-12 09:02, Block 7b dort unveraendert: `git diff 70030492 master`
  auf re15_inv_screen.c/inv_render_pc.c/re15_inv_screen.h/re15_itembox.c leer), gleicher Spielstand/Weg: Zeile 0 ein
  einziger Block x 146..182 (Name laeuft ins Symbol ab 158), Zeile 4 Symbol x 158..203 + Rahmenspalten 204..206 belegt.
  Der Messweg erkennt den Befund also.
- unit_r37_kiste (A) selbst gefahren: "101 Namen geprueft; laengster Name Id 0x44 bis x 173, Symbol x 179..203, kleinste
  Luecke 5 px" in allen drei Zustaenden (Anfang / alle Flags Leon / alle Flags Elza).

**Urteil M1: erfuellt.** Beleg: Bild `M_abn0` (Scratchpad) bzw. die Laeufe oben; im Repo das Dossier-Bild
`M_belege/nachher_1150_zeilen_zoom.png` stimmt mit meiner Messung ueberein (173 / 179).
Nebenbefund (kein Mangel der Spur, Hinweis fuer den Nutzer): die Mengen-Ziffern stehen jetzt IM Symbol unten links
(RE2-Lage, R7 belegt) statt rechts daneben — sichtbare Aenderung, im Dossier §7 dem Nutzer angekuendigt.

## 2. Punkt M2 — "Kasten liegt beim Scrollen/Bewegen layertechnisch ueber den Items"

### Messweg
Gleicher Spielstand/Weg wie §1. Eigene Auswertung `M_abn0/py/ana.py` + `regionen.py`: (a) jedes Bild gegen BOXPANEL.BIN
fuer alle Panel-Pixel AUSSERHALB des Listenfensters (Fenster aus dem Asset bestimmt: x 13..203 / y 52..152);
(b) jedes Bild gegen das Ruhebild F99 auf dem GANZEN Schirm, aufgeteilt in Fenster / Griffspalte x 208..213 / Textbox
y >= 181 / "SONST".

| Lauf | Bilder | Panel ausserhalb Fenster != BOXPANEL | Aenderung "SONST" (weder Fenster noch Griff noch Textbox) |
|---|---|---|---|
| Leon 1150, Einzeltasten RUNTER x2, HOCH x4 (ueber die Ring-Naht 0 -> 63 -> 62) | F95..F215 = 121 | in JEDEM Bild 1805 px, konstant (Spalten 6..12 Menue-Chrome/Band-Ende, 208..213 Griff, 215..219 Inventar-Gitter — liegen schon im Ruhebild so) | 0 in allen Bildern |
| Leon 1150, Taste GEHALTEN (`RE15_INPUT_SCRIPT=A0.034,W1.6,A0.034,W0.3,D2.4,W0.5,U3.0`, BASIS=spiel, START=40), 24 Plaetze belegt | F95..F330 = 236 (168 Bildwechsel, Fensterinhalt aendert sich F101..F176 in jedem Bild) | 236/236 Bilder 1805 px, konstant | 0 |
| Elza 1151, RUNTER, HOCH, HOCH | F95..F160 = 66 | 66/66 Bilder 1805 px | 0 |
| Gegenprobe VORHER (Hauptbaum-exe 09:02), gleicher Lauf wie Zeile 1 | F99..F130 | Ruhe 1875, im Gleiten bis 3292 (F107) | F104 558 px, F107 1535 px (x 16..206, y bis 35 = ueber dem Kopfband "ITEM BOX") |

Sichtpruefung (Ausschnitte 2-fach, Scratchpad `M_abn0/leon1150_panel_F99_104_107_125.png`, `halten1150_sheet.png`):
beim Gleiten verschwindet die obere Zeile unter dem Kopfband "ITEM BOX", die untere kommt unter dem Fussband hervor;
Schriftzug, Pfeile und Rahmen bleiben unversehrt; der gelbe Griff wandert (bei 62 am unteren Ende).
Ruhebild vorher gegen nachher: ausserhalb des Listen-Panels 0 Pixel anders (F99/F110/F130), im Panel ausserhalb des
Fensters nur x 204..206 (dort lag vorher das 2-Feld-Symbol auf dem Rahmen) — keine Nebenwirkung auf Textbox, Gitter,
Cursor, Chrome.

**Urteil M2: erfuellt.**
