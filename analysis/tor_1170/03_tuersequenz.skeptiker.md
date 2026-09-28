# Skeptiker-Pruefung zu Dossier 03 (Mechanik der Tuersequenz)

Stand 2026-09-28. Nur gelesen und gemessen. Nichts am Port geaendert, nichts gebaut, kein git.
Geprueft gegen `info/Re1.5/PSX.EXE` (t_size 0xaf000 = Auslieferungsstand, md5 b55fdaa5...) und
`info/re2leon/PSX.EXE`. Savestates: nur saubere (Stub `@0x80026e4c` = `08 00 e0 03`).

Eigene Werkzeuge (alle unter `build/tor_1170/skeptiker_tuersequenz/`, vom Werkzeug des Untersuchers
unabhaengig; `tuerskript_dump.py` wurde NICHT benutzt):

| Skript | Zweck |
|---|---|
| `sk_mips.py` | eigener MIPS-Dekoder (`<re15\|re2> dis\|bytes\|words <addr> <n>`) |
| `sk_xref.py` | Xrefs (`jal`, `mem`, `word`) ueber die ganze EXE |
| `sk_func.py` | Funktionsumfang, Aufrufliste |
| `sk_emu.py` + `sk_rotmatrix.py` | MIPS-Interpreter, fuehrt die ORIGINAL-RotMatrix aus |
| `sk_do2_re2.py`, `sk_dis.py`, `sk_md1.py` | eigener DO2-/Skript-/MD1-Leser; Opcode-Laengen aus `analysis/nutzer_batch_2026-08-27/tools/re2_scd_lens.py` (fremde, aeltere Quelle) |
| `sk_zensus.py`, `sk_zensus_re15.py`, `sk_zensus_re2_rdt.py` | Zaehlungen |
| `sk_projektion.py`, `sk_nclip.py` | Bildrechtecke und nclip, Gleitkomma UND GTE-ganzzahlig |
| `sk_do2_re15.py` | RE1.5-DO2 nach den Feldern aus Door_init |
| `sk_savestate.py`, `sk_vram.py` | dynamische Gegenprobe (RAM und VRAM sauberer Savestates) |
| `sk_ctc2_H.py` | alle Schreiber des GTE-Registers H |

## Ergebnis in einem Satz

Alle 13 Schwerpunkt-Aussagen halten. K31 ist zusaetzlich DYNAMISCH belegt (Tuertextur liegt im VRAM,
Door_init-Spuren im RAM). Zwei Nebenaussagen haben falsche Einzelwerte (K07, K28-Zahlwert), und die
Luecken-Liste K33 ist unvollstaendig: drei weitere Unterschiede RE1.5 gegen RE2 fehlen (unten N1..N3).

## Schwerpunkt

### K31 RE1.5 erreichbar - BESTAETIGT (statisch + dynamisch)

Statisch, selbst gelesen:
- `@0x8001d600 lui v0,0x800b` / `@0x8001d604 lw v0,-13912(v0)` = `[0x800ac9a8]`, `@0x8001d618 bne v0,zero,0x8001d82c`.
- `@0x8001d830 jal 0x80021634` (a0=2,a1=0), `@0x8001d838 jal 0x800171f4`, `@0x8001d840/44` a1=`0x80016188`,
  `@0x8001d848 jal 0x80029a98` mit `@0x8001d84c ori a0,zero,0x1` (Delay-Slot).
- `0x80029a98`: `sll a0,a0,7`, Eintrittsadresse nach `0x800b2928 + task*0x80`, Zustand 2 nach `0x800b2924 + ...` = Task starten.
- Einziger Aufrufer von `0x8001d600`: `@0x8001ca54` (Zustand 1 der Uebergangs-FSM, Sprungtabelle `0x8001069c[0]` = `0x8001c9c8`).
- Schreiber von `0x800ac9a8`: nur `@0x800430c4 sw a0` (Tuer-Handler `0x800430bc` = Tabelle `0x8007469c[2]`) und
  `@0x80014a50 sw zero` (Debug-Sprung).
- Lader `0x800171f4`: `@0x8001720c lbu v0,12(v0)`, `@0x80017214 sll v0,v0,1`, Tabelle `0x80071d2c` = `25 00 00 00`,
  `@0x80017224 lhu a0,0(at)` -> Datei 37, Ziel `0x801a1000`, Name `@0x80010670` = "DOOR TEXTURE".
  Dateitabelle `0x8006f43c + 37*8` = `b8 de 00 00 07 05 00 dd`: Groesse 0xdeb8 = 57016 = Groesse von `DOOR00.DO2`
  (einzige Datei dieser Groesse im ganzen `info/Re1.5/PSX`-Baum).
- Door_main `0x80016188`, Tabelle `0x80071d30` = `e0 61 01 80 | c8 64 01 80 | 64 66 01 80 | 00 00 00 00`.
- Skriptblock `DOOR00.DO2` Datei 0x9a4 = `02 00 01 00`. Evt_end `0x8003f1f0`: `@0x8003f1fc bne v1,zero` (Ebene),
  `@0x8003f204 sb zero,1(a1)`, Rueckgabe 2. Schleife `@0x800164c8..0x80016504` liest `[0x800b39ad]` = Thread 10 + 1.
- Zeichnen: `@0x80016728 lw v0,0(s0)` / `@0x80016730 beq v0,zero,0x80016b14` - Objekt mit +0 == 0 wird uebersprungen.

Dynamisch (`python sk_vram.py ...`, `python sk_savestate.py ...`):

| Savestate | VRAM (320,256) 64x256 Worte == DOOR00-TIM | CLUT (0,511) | `0x800b23f4..` | `0x800b2210` / `0x800b221c` |
|---|---|---|---|---|
| `doorA_square`, `doorB_walkin`, `doorB2_walkin`, `room1140_entry` | 256/256 Zeilen | 256/256 | 801bd000 801bd090 801bd120 801bd1b0 | 30000 / 22000 |
| `orig_1170_gp`, `room1090_orig` | 0 (ueberschrieben) | 256/256 | wie oben | 30000 / 22000 |
| `boot_52`, `mzd_title` | 0 | 1/256 | 0 0 0 0 | 0 / 0 |

`0x800b2210`, `0x800b221c` und die Tabelle `0x800b2400` haben in der ganzen EXE nur Door_init als Schreiber
(`sk_xref.py re15 mem ...`). Die aeltere Notiz "kein DO2 geladen" ist damit widerlegt.

Einschraenkungen (nicht im Dossier):
- "bei jedem Tuer-AOT" gilt fuer den Normalpfad. Zustand 1 ruft `0x8001d600` NICHT, wenn
  `[0x800aca3c] & 0x8000` (`@0x8001c9f8/fc`) oder `[0x800aca38] & 0x40000` (`@0x8001ca00..14`) gesetzt ist.
- "1 Bild" gilt fuer die Schleife Door_move. Die Task selbst lebt laenger, siehe N3.

### K32 Kamera RE1.5 - BESTAETIGT

- `@0x80016458/5c` Basis `0x800b2210`, `@0x80016460 ori v0,zero,0x7530` + `@0x80016464 sw`, `@0x80016468 ori v0,zero,0x55f0`
  + `@0x80016480 sw v0,[0x800b221c]`, uebrige vier Worte `sw zero`. `@0x80016494 jal 0x80053ca4` mit a0 = `0x800b220c`.
- Feldzuordnung in `0x80053ca4` selbst gelesen: Auge = +4/+8/+12, Ziel = +16/+20/+24
  (`@0x80053d1c lw v0,16(s3)`, `@0x80053d20 lw v1,4(s3)`, Translation aus -(+4..+12) `@0x80053f60..90`).
- H: `@0x80016434 jal 0x80066c40`; darin `@0x80066c88 addiu t0,zero,1000`, `@0x80066c8c ctc2 t0,r26`.
  In der ganzen EXE gibt es genau zwei Schreiber von H: `@0x80066c30` und `@0x80066c8c` (`sk_ctc2_H.py`).
  Aufrufer von `0x80066c30`: `@0x80021e6c`, `@0x80046124`, keiner im Tuercode, kein Zeiger in den Daten.
- Der Raum-Kamerasetzer (`0x80021bbc`, ruft `@0x80021e6c`) laeuft erst NACH der Tuer-Task: `0x8001d600` wartet
  `@0x8001dab8..d4` auf Bit 0x2000000 und setzt erst danach `@0x8001daec [0x800b5457] = 1`. H bleibt also 1000.
- Bildmitte: `@0x8001643c ori a0,zero,0xa0`, `@0x80016444 ori a1,zero,0x78`, `@0x80016440 jal 0x80066d60`.
- Rahmung nachgerechnet (`sk_projektion.py`): RE1.5 x 104,3..233,5 / y 19,1..256,1; RE2 x 102,8..235,6 / y 16,3..260,0.

### K04 Kamera RE2 - BESTAETIGT

`@0x80013e30 jal 0x8008de24` + `@0x80013e34 addiu a0,zero,290`; `0x8008de24` = `ctc2 a0,r26`.
Daten `@0x80010830` = `10 27 00 00` + 8 Null, `@0x8001083c` 12 Null; `@0x80013e38 addiu a0,sp,20`, `@0x80013e40 addiu a1,sp,32`.
Anmerkung: `SetGeomOffset` hat DREI Aufrufer, nicht zwei: `@0x80049cd4`, `@0x80068e84` (beide 160,120) und
`@0x800764e8` (a0 = 114, a1 = Wert + 70, Statusbild-Code `0x80076498`). Die Bildmitte der Tuer ist geerbter Zustand.

### K05 Achsen - BESTAETIGT

Aus beiden Look-at-Routinen selbst hergeleitet (nicht nachgerechnet mit fremdem Werkzeug):
RE2 `0x80076cb0`: `@0x80076f14 sh (-dx/dxz),20(sp)` = m[0][2], `@0x80076f1c sh (dx/dxz),28(sp)` = m[2][0],
`@0x80076f08/0c` m[0][0] = m[2][2] = dz/dxz. Mit dx = -10000: Zeilen (0,0,1),(0,1,0),(-1,0,0), t = R * (-Auge) = (0,0,10000).
RE1.5 `0x80053ca4`: dieselben Ziele `@0x80053f4c/50/54/5c`.

### K10 Elternmatrix - BESTAETIGT

RE2 `@0x80014c64 andi v0,v1,0x10`, `@0x80014c6c andi v0,v1,0xf`, `@0x80014c80 addiu v0,v0,84`, sonst `0x800dcba8`, Ziel `@0x80014c8c sw v0,128(a0)`.
RE1.5 `@0x80016fe4 andi a3,v0,0x8`, `@0x80016ff4 andi v0,v0,0x7`, `@0x80017018 addiu v0,v0,72`, Ziel `@0x8001701c sw v0,116(a1)`.
Zaehlung (`sk_zensus.py`): 378 Saetze, 213 mit Flag 0x10.

### K11 Opcode 0x4F RE1.5 - BESTAETIGT

`0x800744a8[0x4f]` = `0x80016f20`. Alle Ziele einzeln gelesen (`@0x80016f54` +8, `@0x80016f60` +9, `@0x80016f6c` +0,
`@0x80016f78` +0x8e, `@0x80016f84` +0x8c, `@0x80016f90` +0xc, `@0x80016f9c/a8/b4` +0x34/38/3c,
`@0x80016fc0/cc/dc` +0x68/6a/6c), `@0x80017020 addiu v0,a2,22`. Objekte: `@0x800161e4 ori a3,zero,0x3`, `@0x8001622c addiu a1,a1,-144`.
Kein Grenzen-Test auf Objekt- oder Elternnummer.

### K21 Ursprung = Drehachse - BESTAETIGT

`@0x800142a0 addiu a0,s0,116`, `@0x800142a4 jal 0x8008e1f4`, `@0x800142a8 addiu a1,s0,36`. Zusammensetzung:
Eltern-R in die GTE (`@0x800144c4..f0`), drei Spalten der lokalen Matrix mit `4a49e012`, Position 16 Bit
(`@0x800145d8 lhu t5,4(v0)`, `@0x800145dc lhu t4,0(v0)`), `4a480012` mit Eltern-t.
Vorbilder im Skripttext: `DOOR00` Skript 5 `2f 04 06 00` (Index 4), Skript 3 `2f 03 32 00` (Index 3), `DOOR1E` Skript 4 `2f 0b ff ff` (Index 11).
Handgerechnet Skript 5: 80 + 30 + 644 + 240 + 36 = 1030.

### K22 RotMatrix - BESTAETIGT (am Original ausgefuehrt)

`python sk_rotmatrix.py`: Routine `0x8008e1f4` (RE2) und `0x80068098` (RE1.5) im eigenen Interpreter.
rot (300,700,1100) -> `[[-227,-1934,3600],[3456,-2015,-865],[2180,2989,1743]]`; Rx*Ry*Rz Fehler 6,8, naechstbeste Ordnung 1588.
rot (0,1024,0) -> `[[0,0,4096],[0,4096,0],[-4096,0,0]]`. Sinustabelle `@0x800aeeac` = `00 10 00 00`.
57 Saetze mit mehr als einer Achse: gezaehlt 57.

### K23 Bildgroesse - BESTAETIGT

`sk_projektion.py`: x 102,8..235,6 / y 16,7..260,3 = 132,8 x 243,7 (Dossier 243,6 = Rundung). GTE-ganzzahlig x 102..235, y 16..260.
Wurzel-Saetze 165, x = 2000 in 122, y = 3800 in 108. Das Rechteck ist die Vorderflaeche bei Tiefe 7857, nicht 8000.
Hinweis: die Zeitachse 3.7 nennt fuer `DOOR00` y = 3790 (Bytes `ce 0e`), K23 rechnet mit 3800.

### K24 Grenzen - BESTAETIGT (mit Vorbehalt)

6620,7 x 8827,6 / 3310,3 x 4413,8 / 2813,8 x 3751,7. Die Formel gilt nur fuer ein um die Bildmitte zentriertes Teil.
Ein Teil, das auf der Standard-Bodenlinie y = 3800 steht, steht bei Tiefe 8000 nie ganz im Bild (Bodenlinie = Zeile 257,8).

### K28 Umlaufsinn - BESTAETIGT, Zahlwert korrigiert

`@0x800148d0 4b400006`, `@0x800148f8 bgez v0,0x80014938`, `@0x80014958 4b18043f`. Eckenfolge: `@0x8001475c lh 2(fp)`, `@0x80014760 lh 6(fp)`, `@0x80014770 lh 10(fp)`.
`sk_nclip.py`: gezeichnet genau Dreieck 4 und 5 (Flaeche x = +143), MAC0 **+32319** mit GTE-ganzzahligen Bildkoordinaten.
Der Wert +32370 des Dossiers ist das Gleitkomma-Produkt 132,84 * 243,68, nicht der GTE-Wert.

### K33 RE1.5-Luecken - BESTAETIGT, aber unvollstaendig

Alle genannten Punkte gelesen: `@0x80016370 addu a2,zero,zero`, `@0x8001639c addu a3,zero,zero`, `@0x80016398 jal 0x80022150`
(dort `@0x800221c0 sll a2,s4,16`, `@0x800221f0 sll t4,s5,22`); Exit `0x80016664..c0` ohne Ton und ohne Blende;
`@0x800164a8 lbu v0,13(v0)`, `@0x800164b4 sh v0,[0x800b0fde]`, Basis `0x800b0fd0` `@0x8003fad0`.
`andi` im Tuercode RE1.5 (`0x80016188..0x800171f4`): nur 0x10, 0x200, 0xc0, 0x20, 0xffff, 0x8, 0x7, 0xff.
Es fehlen N1, N2, N3.

### K35 nicht vertraeglich - BESTAETIGT

`0x800744a8[0x4d]` = `0x800408a8` (`@0x80040900 addiu v1,v1,10`), `[0x4f]` = `0x80016f20`, `[0x53]` = `0x80040e18` (`@0x80040e44 addiu v0,v0,3`).
Eintraege 0x5f.. sind keine Zeiger (`00110010`, `01130012`, ...). Zusaetzlich unvertraeglich: N1 und Flag 0x200.

## Neue Befunde

- **N1 Ordnungstabelle.** RE1.5 waehlt anders als RE2: 0x80 -> `0x800aa6d8`, Platz otz>>4 (`@0x80016df0..0c`);
  0x40 -> `0x800ac6d8`, otz>>10 (`@0x80016e10..28`); 0xc0 -> `0x800aa698`, otz>>11, aber nur wenn Blende Kanal 0 fertig
  (`@0x80016dc0 jal 0x8002178c`), sonst wie 0x80; 0x00 -> s2 bleibt unbelegt (`@0x80016dac j 0x80016e34`).
  Kein Verwerfen bei otz < 64.
- **N2 Hauptschleife zeichnet waehrend der Tuer nur eine Tabelle.** Solange `[0x800aca38] & 0x10000`
  (`@0x80020dec..f8`), ruft die Hauptschleife nicht `0x8002137c`, sondern zeichnet allein `0x800aa6b4 + buf*32`
  (`@0x80020efc jal 0x80068fc4`). Das Bit setzt Zustand 1 `@0x8001ca20..2c`. RE2 wartet in Door_move auf dieses Bit
  (`@0x80013ed0..f8`), RE1.5 nicht.
- **N3 Dauer der Task.** Door_exit wartet auf Blende Kanal 0 (`@0x8001666c`). Dasselbe Wort `0x800b5458` ist in der
  Hauptschleife der Zaehler des Ladebilds: 20 setzen `@0x80020e3c/4c`, je Bild -1 `@0x80020f04..10`, bei < 0 Bit 0x10000
  loeschen `@0x80020f1c..30`. Statisch gelesen lebt die Task also etwa 21 Bilder, Door_move davon 1. Nicht im Emulator gemessen.
- **N4 Flag 0x200.** In RE1.5 Pulsieren (`@0x80016a78..ac`), in RE2 0x100 (`@0x80014468`). 148 RE2-Saetze tragen 0x200.

## Nebenaussagen

| id | Urteil | Anmerkung |
|---|---|---|
| K01, K02, K03, K06, K09, K12, K14, K15, K16, K17, K18, K20, K25, K26, K27, K29, K30, K36, K37 | bestaetigt | an Instruktionen bzw. Bytes nachgelesen, Zaehlungen mit eigenem Leser |
| K07 | zwei Einzelwerte falsch | 250 `ROOM*.RDT` (495 sind Verzeichniseintraege); Payload+12 hat 54 verschiedene Werte, 0x21 fehlt. 572 Saetze und die Verteilung stimmen |
| K08 | bestaetigt | Skripttext + RotMatrix am Original |
| K13 | bestaetigt | mein Leser zaehlt 980 Nop / 11101: die 54 Mehrbytes sind Fuellbytes hinter dem letzten Evt_end (27 Archive x 2) |
| K19 | nur `DOOR00` Typ 0 | 70+50+60+50+40+30 = 300 -> 301 Bilder, x = 2000 + 2450 + 4000 + 3000 = 11450. 165/301/451 ueber 141 Sequenzen nicht nachgemessen |
| K34 | bestaetigt | mein Leser findet 653 Saetze (4 davon 4P), alle +12..+17 = 0; ROOM1170 Datei 0x1206 und 0x135a gelesen |
| K38 | bestaetigt | kein `andi 0x200` in `0x80013bc4..0x80014cd0` |

## Offen

1. H waehrend einer laengeren RE1.5-Sequenz ist nur statisch hergeleitet.
2. N3 ist nicht im Emulator gemessen.
3. K19, K30: die Verteilungen ueber alle 141 Sequenzen brauchen einen vollstaendigen Scheduler-Nachbau.
