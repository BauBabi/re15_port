# T1 — Tuer-Zensus RE1.5 + Ausschnitte + RE2-Referenzblaetter

Runde 31, Teil T1 (Datengrundlage fuer den Vergleich RE1.5-Tueren <-> RE2-Tuerarchive).
Zweig `r31/tueren`, Arbeitsbaum `.claude/worktrees/r31_tueren`. Kein Spielcode.

Stand: angelegt, wird laufend gefuellt.

## 0. Auftrag / Abgrenzung
- Zensus aller Door_aot_set-Saetze der ausgelieferten RDTs, Engine-Marke, Leon/Elza-Zusammenfuehrung, Paarung zu physischen Tueren, Kategorie.
- Ausschnitte je Tuerseite aus den Hintergruenden (Projektion Blatt ~1950 x ~3549), entzerrtes Blatt 128 x 218.
- RE2-Referenzblaetter DOOR00..DOOR36 + Uebersicht.
- Automatisches Aehnlichkeitsmass (Zweitbeleg), validiert an RE2-Tueren mit bekanntem Archiv.
- Kontaktboegen je Raum.

## 1. Quellen und Werkzeuge (Vorarbeit)

Werkzeuge (neu, alle nur LESEND gegenueber Originaldaten, schreiben nach `build/r31_tueren/t1/`):

| Datei | Zweck |
|---|---|
| `re15_port/tools/tueren/zensus_lib.py` | Tuersatz lesen (beide Spiele, Rechteck- und Viereckform), Raum (Kamera, Hintergrund, RVD), Raumnamen |
| `re15_port/tools/tueren/zensus_tueren.py` | Zensus, Engine-Marke, Seiten, Paarung, Kategorie |
| `re15_port/tools/tueren/zensus_bild.py` | Tuerkante, Blatt, Sichtbarkeit, Bild-Verfeinerung, Ausschnitt, Entzerrung |
| `re15_port/tools/tueren/zensus_pruefung.py` | Pruefung des Blatt-Verfahrens an den 11 von Hand vermessenen Tueren (06_massstab §4.3) |

Wiederverwendet (unveraendert): `re15_port/tools/scd_walk_lib.py` (RE1.5-Walker, Laengentabelle =
`scd_vm.c`), `analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py` (RE2-Walker),
`re15_port/tools/tor/tor_kamera.py` (Kamera, Projektion), `re15_port/tools/tor/do2_format.py`
(DO2/TIM), `re15_port/tools/engine_tueren.txt` (230 Tuerseiten, die die Engine aufstellt).

Hintergruende: `extracted/PSX/STAGEn/ROOMsrr/ROOMsrrNN.bmp` (RE1.5, Java-Extraktor),
`info/re2leon/COMMON/BSS/ROOMsrr/ROOMsrrNN.bmp` (RE2). Raumnamen: DEBUG.BIN-Sprungliste
`@Datei 0x2642 + 0x4FA*Stage + 0x1A*Index` (Beleg `analysis/bug_save_room_name.md` M5:
"ROOM1070 @0x26f8: LOBBY OFFICE"; Anzeige `debug_menu_common.c` re15_debug_menu_room_name).

## 2. Zensus — Verfahren und Belege

### 2.1 Satzformat RE1.5 `Door_aot_set` (0x3B), selbst disassembliert

`python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x800405bc 40`:

```
800405c4: lbu  v0,1(v0)          ; pc[1] = Slot
800405c8: lui  v1,0x800b / 800405cc: addiu v1,v1,-13904   ; Tabelle 0x800ac9b0
80040600: lw   v0,28(a0) / 80040608: addiu v0,v0,2 / 8004060c: sw v0,0(v1)
                                  ; Tabelle[Slot] = pc+2  (der Handler kopiert NICHTS)
80040618: lbu  v0,3(v1)          ; pc[3]
80040620: andi v0,v0,0x80
80040624: beq  v0,zero,0x80040634
80040630: addiu v0,v1,40         ; Bit gesetzt -> 40 Byte
80040634: addiu v0,v1,32         ; sonst 32 Byte
```

Der Scan (`dis 0x80042c50 40` / `dis 0x80042ee0 80`) liest den Satz s0 = pc+2:

```
80042cac: lbu  v0,2(s0)          ; pc[4] Band; Bit 0x80 = jedes Band
80042cc0: lbu  v1,130(s1)        ; Spieler obj+0x82
80042ccc: bne  v1,v0,...         ; sonst muss das Band gleich sein
80042f04: andi v0,v1,0x80        ; pc[3] & 0x80 -> Viereck-Trefftest
80042f10: jal  0x80014368        ;   Viereck (4 Punkte)
80042f20: jal  0x80042b64        ;   sonst Rechteck
80042f90: addiu a0,s0,20         ; Viereck: Nutzlast = pc+22
80042fb8: addiu a0,s0,12         ; Rechteck: Nutzlast = pc+14
```

`dis 0x80014368`: `lh t5,4(a1)`, `lh t0,6(a1)`, `lh v1,8(a1)`, `lh v1,10(a1)`, ..., `lh v0,16(a1)`,
`lh v1,18(a1)` = vier Punkte (x,z) ab s0+4 = **pc+6..pc+21**.

| Feld | Rechteckform (32 B) | Viereckform (40 B) |
|---|---|---|
| Slot / sce / sat / Band | pc+1 / +2 / +3 / +4 | gleich |
| Flaeche | pc+6 s16 x, +8 s16 z, +10 u16 w, +12 u16 d | pc+6..+21 vier Punkte (x,z) |
| Ziel x/y/z/Richtung | pc+14/16/18/20 | pc+22/24/26/28 |
| Stage / Raum / Cut / Ziel-Band | pc+22/23/24/25 | pc+30/31/32/33 |
| Archiv / Variante | pc+26/27 (Lader `@0x8001720c lbu v0,12(v0)`, `@0x800164a8 lbu v0,13(v0)`) | pc+34/35 |

### 2.2 Zahlen

`python re15_port/tools/tueren/zensus_tueren.py`:

| Groesse | Wert |
|---|---|
| RDT-Dateien unter `shared_assets/PSX/STAGE*` | 240, davon 34 Stubs mit 4 Byte (u.a. ROOM1270/1, 20C0..20F1, 30F0/1, 4060/1, 40C0..40F1, 5150..5171, 6050..6071) |
| `Door_aot_set`-Saetze (je Datei-Offset einmal) | **653**, davon **4 in Viereckform** (40 B) |
| Tuerseiten (Leon/Elza zusammengefuehrt, siehe 2.3) | **324** |
| Engine-Zeilen `engine_tueren.txt` | 230, **alle 230** treffen genau einen Satz |

**Befund Viereck-Saetze (neu):** Die 4 Saetze mit `pc[26..27] = 86 24` bzw. `34 08` in ROOM4030/4031
`@0x47E`/`@0x4A6` sind KEINE Scan-Artefakte (so fuehren sie `05_port_anschluss.md` §1.4 und
`aot_common.c:561-562`), sondern die beiden Tueren ROOM4030 -> ROOM4040 in Viereckform.
`@0x47E` = `3b 01 02 b1 00 00 | 82 a1 6e 9c ae 9d 40 98 fa 97 7c 9d 5e 9d 86 a2 | be 0a 00 00 86 24 00 06 | 03 04 0c 00 | 00 00 00 00 00 00`:
sat 0xB1 (Bit 0x80), Punkte (-24190,-25490) (-25170,-26560) (-26630,-25220) (-25250,-23930),
Ziel (2750, 0, 9350), Richtung 1536, Stage 3 -> STAGE4, Raum 04, Cut 12, Band 0, Archiv 0,
Variante 0. Der Port liest diese Saetze mit dem 32-Byte-Schema (`scd_vm.c op_door_aot_set`
liest Ziel-Stage/Raum aus pc[22]/pc[23] = `be 0a`) und stellt sie mit den Zielen
"BF0A0" / "C5F00" auf (so stehen sie in `engine_tueren.txt`). **Folge im Port: die beiden
Tueren ROOM4030 -> ROOM4040 fuehren ins Leere.** Nicht T1-Auftrag; siehe §9.

**Engine-Marke:** `engine_tueren.txt` traegt in der Band-Spalte in allen 230 Zeilen 0 (auch fuer
Saetze mit pc[4] = 1..8, z.B. ROOM1060 -> ROOM1120, Band 8) - die Datei stammt aus dem Stand vor
der Band-Korrektur (`test_map_uebergang.c:195-201`). Verglichen wird deshalb nur
(Raum, Ziel, Mittelpunkt), und zwar mit genau den Groessen, die der Port liest (auch bei
Viereck-Saetzen falsch gelesen), `cx = x + w/2` mit C-Division. Die Datei deckt nur die
Leon-Raeume xxx0 ab (Schleife `test_map_uebergang.c:163`, `rid = st<<12 | r`); fuer Seiten, die
es nur in xxx1 gibt, ist die Marke `null`.

### 2.3 Tuerseite

Tuerseite = gleicher Grundraum (ROOMsrr, ohne Variante) + gleiches Band pc[4] + gleiche Flaeche
(Rechteck bzw. vier Punkte) + gleicher Zielraum. Mehrere Saetze einer Seite (Leon/Elza, main/sub,
verschiedene Ziel-Cuts) stehen in `saetze` der Seite. Eine Flaeche mit Breite = Tiefe = 0 ist ein
Skript-Uebergang (Trefftest `FUN_80042b64` trifft nur einen Punkt; ausgeloest per `Aot_on`,
vgl. `test_map_uebergang.c:171-179`), keine begehbare Tuer.

## 3. Paarung und Kategorien
(offen)

## 4. Ausschnitte — Verfahren, Pruefung an den 5 vermessenen Tueren

### 4.1 Verfahren (`zensus_bild.py`)

1. **Tuerkante.** Die langen Kanten des Rechtecks (>= 0,8 x laengste) sind Kandidaten. Gewaehlt
   wird die Kante, die vom Ankunftsort der Gegenseite (Ziel x/z der Rueckrichtung = der Punkt
   in DIESEM Raum, an dem man durch diese Tuer hereinkommt) am weitesten nach aussen liegt.
   Ohne Gegenseite: die Kante, 900 Einheiten hinter der kein RVD-Kamerabereich des Raums liegt.
   Die SCA-Kollision wird NICHT benutzt: 06_massstab §6 hat gemessen, dass die Zellen 12..1256
   Einheiten vor der sichtbaren Wand liegen.
2. **Blatt.** 1950 x 3549 (06_massstab §1: RE1.5-Breite 1953, Hoehe 3549), Unterkante
   y = -Band*1800, mittig auf der Kante.
3. **Cut-Auswahl.** Projektion mit der Gleitkomma-Blickmatrix (`tor_kamera.Kamera(exakt=True)`,
   H = fov>>7, Mitte 160/120). Ein Cut zaehlt, wenn alle Ecken mindestens 300 vor der Kamera
   liegen, die Raumseite des Blatts zur Kamera zeigt, mindestens 60 % der Blattflaeche im Bild
   liegen und das Blatt mindestens 24 Pixel hoch ist. Rangfolge: RVD-Bereich des Cuts enthaelt
   die Rechteckmitte (= der Cut, der beim Oeffnen aktiv ist), dann Pixelhoehe x Bildanteil.
4. **Verfeinerung am Bild.** Gitter dn -900..900 (50), dt -600..600 (50), W 1650..2250 (100);
   Wert = Median des Helligkeitssprungs quer zu linker/rechter/oberer (+ halb unterer) Kante,
   minus Strafe `Median der Suche x groesste Eckverschiebung / 10 px`.
5. **Ausschnitt** = Huelle + 35 % Rand (mind. 10 px), 3-fach NEAREST, Umriss rot.
   **Entzerrtes Blatt** = Homographie auf 128 x 218 (RE2-Blattbereich v 0..218), bilinear.

### 4.2 Pruefung an den von Hand vermessenen Tueren

`python re15_port/tools/tueren/zensus_pruefung.py` -> `build/r31_tueren/t1/pruefung.json`,
Bilder `pruefung_<spiel>_<raum>_cNN.png` (gruen = Messung 06 §4.3, gelb = Datenlage, rot = verfeinert).
Mittlere Eckabweichung in Pixeln (Blatt im Bild 100..140 px hoch):

| Tuer | Datenlage | verfeinert | dn / dt / W der Verfeinerung |
|---|---|---|---|
| RE1.5 ROOM1000 c3 | 5,5 | 3,3 | -800 / -50 / 1750 |
| RE1.5 ROOM1000 c6 | 3,6 | 3,9 | -450 / -50 / 2050 |
| RE1.5 ROOM1010 c0 | 3,4 | 4,1 | +150 / -50 / 2050 |
| RE1.5 ROOM1010 c4 | 7,0 | 5,0 | -150 / 0 / 2050 |
| RE1.5 ROOM2060 c0 | 8,8 | 6,7 | -550 / -150 / 1850 |
| **RE1.5 Mittel (5 Tueren)** | **5,6** | **4,6** | |
| RE2 ROOM60B0 c4 | 7,2 | 5,7 | |
| RE2 ROOM3060 c0 | 19,1 | 8,3 | |
| RE2 ROOM5050 c3 | 10,5 | 6,3 | |
| RE2 ROOM20E0 c3 | 5,3 | 2,7 | |
| RE2 ROOM2040 c0 | 11,2 | 10,7 | |
| RE2 ROOM40A0 c8 | 6,1 | 5,3 | |
| **RE2 Mittel (6 Tueren)** | **9,9** | **6,5** | |

Wahl der Strafe (an denselben 11 Tueren, also optimistisch): ohne Strafe RE1.5 5,2 / RE2 6,6 px,
aber ROOM1010 c0 springt auf eine innere Linie des Blatts (10,4 px, Bild
`pruefung_re15_1010_c00.png` angesehen: rote Kante liegt auf dem Schattenstreifen innen);
Strafe 0,5/1/2/4: RE1.5 4,5/4,6/4,9/4,8, RE2 6,7/6,5/8,4/8,4. Genommen: 1.

Angesehen: `pruefung_re15_1000_c03.png` (Datenlage gelb etwas innen, verfeinert rot auf der
Zargen-Innenkante, Messung gruen dazwischen), `pruefung_re15_1010_c00.png` (Datenlage gelb deckt
sich mit gruen bis auf ~1 px links).

## 5. RE2-Referenzblaetter
(offen)

## 6. Automatisches Mass + Validierung
(offen)

## 7. Kontaktboegen — Sichtpruefung
(offen)

## 8. Ergebnis / Zahlen
(offen)

## 9. Offene Punkte
(offen)
