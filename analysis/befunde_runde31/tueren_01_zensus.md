# T1 — Tuer-Zensus RE1.5 + Ausschnitte + RE2-Referenzblaetter

Runde 31, Teil T1 (Datengrundlage fuer den Vergleich RE1.5-Tueren <-> RE2-Tuerarchive).
Zweig `r31/tueren`, Arbeitsbaum `.claude/worktrees/r31_tueren`. Kein Spielcode.

Stand: 2026-09-29, fertig (Werkzeug + Daten + Boegen). Ergebnisdatei `build/r31_tueren/t1/zensus.json`.

**Kurz:** 653 Door_aot_set in 240 RDT -> 324 Tuerseiten (299 begehbar, 298 davon in einem Cut
sichtbar) -> 163 Tueren, davon 144 physisch (normal 114, selbst 15, aufzug 9, leiter 2, sonstiges 4).
Kontaktboegen fuer 98 Raeume, RE2-Referenzblaetter fuer alle 55 Archive. Das automatische Mass ist
schwach (RE2-Validierung streng: Top-1 26 %, Top-3 37 %) und nur Zweitbeleg. Nebenbefund: die 4
"Scan-Artefakte" in ROOM4030/4031 sind Viereck-Tueren nach ROOM4040, die der Port falsch liest (§2.2).

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
| `re15_port/tools/tueren/zensus_tueren.py` | Zensus der Saetze, Engine-Marke, Tuerseiten |
| `re15_port/tools/tueren/zensus_paare.py` | Zwillingsraeume, Paarung, Kategorien (inkl. Sicht-Tabelle) |
| `re15_port/tools/tueren/zensus_bild.py` | Kandidatenkanten, Blatt, Sichtbarkeit, Bild-Verfeinerung, Ausschnitt, Entzerrung |
| `re15_port/tools/tueren/zensus_ausschnitte.py` | Kantenwahl + Ausschnitte je Seite, RE1.5 und RE2 |
| `re15_port/tools/tueren/zensus_pruefung.py` | Pruefung des Blatt-Verfahrens an den 11 von Hand vermessenen Tueren (06_massstab §4.3) |
| `re15_port/tools/tueren/zensus_mass.py` | Aehnlichkeitsmass + Validierung |
| `re15_port/tools/tueren/zensus_bogen.py` | RE2-Referenzblaetter, Uebersicht, Kontaktboegen |
| `re15_port/tools/tueren/zensus_alles.py` | Gesamtlauf, schreibt `zensus.json` |

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

`python re15_port/tools/tueren/zensus_paare.py`:

| Groesse | Wert |
|---|---|
| RDT-Dateien unter `shared_assets/PSX/STAGE*` | 240, davon 34 Stubs mit 4 Byte (u.a. ROOM1270/1, 20C0..20F1, 30F0/1, 4060/1, 40C0..40F1, 5150..5171, 6050..6071) |
| `Door_aot_set`-Saetze (je Datei-Offset einmal) | **653**, davon **4 in Viereckform** (40 B) |
| Tuerseiten (Leon/Elza zusammengefuehrt, siehe 2.3) | **324** |
| Engine-Zeilen `engine_tueren.txt` | 230, **alle 230** treffen genau einen Satz |

**Befund Viereck-Saetze (neu):** Die 4 Saetze mit `pc[26..27] = 86 24` bzw. `34 08` in ROOM4030/4031
`@0x47E`/`@0x4A6` sind KEINE Scan-Artefakte (so fuehren sie `05_port_anschluss.md` §1.4 und
`aot_common.c:604-609` "non-door scan artifacts"), sondern die beiden Tueren ROOM4030 -> ROOM4040 in Viereckform.
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

Werkzeug `re15_port/tools/tueren/zensus_paare.py` (`--liste` gibt jede Tuer mit Seiten, Kategorie und
Kriterium aus).

### 3.1 Zwillingsraeume

Grundraeume, deren Seiten in mindestens 2 Faellen dieselbe Flaeche + Band + Zielraum wie ein anderer
Grundraum haben, sind Zustandsvarianten desselben Orts (Hauptraum = der Raum, auf den die Tueren der
Nachbarn zeigen). Gemessen: 6 Paare

| Zwilling | Hauptraum | Namen (DEBUG.BIN) |
|---|---|---|
| ROOM1160 | ROOM11D0 | KENNEL / KENNEL LIGHT |
| ROOM1230 | ROOM1180 | B1 CORR. LIGHT / B1 CORRIDOR |
| ROOM20B0 | ROOM2000 | L TUNNEL FLOOD / L TUNNEL |
| ROOM30D0 | ROOM30C0 | LOB.PAR-MARVIN / LOBBY |
| ROOM5120 | ROOM5040 | F B-2 CORRIDOR / B-2 CORRIDOR |
| ROOM5070 | ROOM5130 | D-2 CORRIDOR / F D-2 CORRIDOR |

Seiten mit gleicher Flaeche + Band in (zwillings-)gleichem Raum mit (zwillings-)gleichem Ziel sind EINE
Tuerseite (`zwilling_von`); 28 solche Zwillingsseiten, z.B. ROOM10A0 Band 4 -> ROOM1180 und -> ROOM1230
(dasselbe Rechteck, Ziel je nach Lichtzustand).

### 3.2 Paarregeln

1. **Regelpaar** A->B mit B->A: Ankunft von B->A (Ziel x/z, liegt in A) nahe am Rechteck von A->B und
   umgekehrt; Guete = Summe beider Abstaende (0 = Ankunft im Rechteck), Grenze 2500.
   Gemessene Verteilung der 125 Regelpaare: Quantile 0 / 25 / 50 / 75 / 90 / 100 % =
   0 / 450 / 683 / 898 / 1100 / 2000. Kein Regelpaar liegt zwischen 2000 und 2500; die Grenze
   trennt also nichts Knappes ab.
2. **Skript-Gegenseite** (6): B->A ist ein Skript-Uebergang (Flaeche 0); es zaehlt nur der Abstand
   seiner Ankunft zum Rechteck von A->B. Das sind die Fahrstuhltueren (Kabine ROOM1080 / ROOM4020
   faehrt per Skript an).
3. **Einziges Paar** (2): zwischen zwei Raeumen je Richtung genau eine freie Seite -> gepaart ohne
   Grenze: ROOM11A0 <-> ROOM3000 (8414; Seite ROOM3000 hat ein 10 x 10-Rechteck am Kanaldeckel) und
   ROOM50B0 <-> ROOM6000 (3623; Hubbuehne).

Ergebnis: 133 Paare, **163 Tueren**, davon **144 physisch** (mindestens eine begehbare Seite, Flaeche > 0).
19 Tueren bestehen nur aus Skript-Uebergaengen (Flaeche 0).

Einseitige begehbare Seiten ohne Gegenseite (Auswahl, alle im JSON): ROOM1090 Band 6 -> ROOM1100
(S031), ROOM10F0 Band 1 -> ROOM1090 (S048, Lueftung), ROOM2070 -> ROOM2030 / -> ROOM2000 (S157/S158, die
Ankunftsorte decken sich mit S155/S156), ROOM20B0 -> ROOM2030 (S171), ROOM3070 -> ROOM3080 (S195,
Lastenaufzug), ROOM4000 -> ROOM3070 (S218), ROOM1250 -> ROOM1000 (3 inerte Saetze).

### 3.3 Kategorien - Kriterien

Reihenfolge der Datenkriterien (`zensus_paare.kategorie`), danach die Sicht-Tabelle `SICHT`:

| Kategorie | Kriterium | physisch |
|---|---|---|
| aufzug | einer der beiden Raeume traegt eine Fahrt-Signatur aus `engine/src/gen/re15_elev_se.inc` (SIG1 ROOM1080/4020, SIG2 ROOM3080; `gen_re15_elev_anchors.py`: 0 Fehltreffer) - oder SICHT | 9 |
| skript (Sonstiges) | alle Seiten Flaeche 0 (nur per `Aot_on`) | 0 |
| selbst | Zielraum = eigener Raum | 15 |
| etage | Seiten auf verschiedenen Kartenblaettern (`re15_map_zones.h` s_map_floors / s_map_zones) -> Kandidat, per Sicht entschieden | 0 (alle 10 per Sicht aufgeloest) |
| leiter | nur per Sicht | 2 |
| sonstiges | nur per Sicht | 4 |
| normal | sonst | 114 |
| Rolltor/Schott | in keinem angesehenen Bogen gefunden | 0 |

**Sicht-Entscheidungen** (Boegen angesehen, Tabelle `SICHT` in `zensus_paare.py`, Schluessel Raum/Band/Mitte):

| Tuer | Seiten | vorher | nachher | Beleg |
|---|---|---|---|---|
| T022 | ROOM10A0 b4 <-> ROOM1180 | etage | normal | Tuer am Treppenabsatz B1 (ROOM10A0 c2, Aufschrift "B1"); Kartenblatt fuer Band 4 fehlt in s_map_floors |
| T024 | ROOM10B0 b5 <-> ROOM1170 b4 | etage | **leiter** | Wandleiter rechts neben der Flaeche (ROOM10B0 c0, `t1_bogen_ROOM10B0.jpg`); oben Gelaenderoeffnung (ROOM1170 c3) |
| T047 | ROOM11A0 b1 <-> ROOM3000 | etage | **leiter** | Leiter zur Deckenluke (ROOM11A0 c0); Kanaldeckel im Boden (ROOM3000 c0) |
| T112 | ROOM4000 -> ROOM3070 | etage | **aufzug** | Lastenaufzug: Flaeche 4300 x 3800 am Schacht (ROOM4000 c1, "Level-1"); Hinweg ROOM3070 -> ROOM3080 (SIG2) -> Skript -> ROOM4000 |
| T143 | ROOM5080 <-> ROOM6010 | etage | normal | Metalltuer (ROOM5080 c0, ROOM6010 c7) |
| T147 | ROOM5090 <-> ROOM6030 | etage | normal | Zugtuer mit Fenster (ROOM6030 c10) |
| T148 | ROOM50B0 <-> ROOM6000 b2 | etage | **aufzug** | Hubbuehne mit Warnrand + Treppe (ROOM6000 c0), Bedienpult (ROOM50B0 c3); kein Tuerblatt |
| T029 | ROOM10F0 b1 -> ROOM1090 | normal | **sonstiges** | Lueftungsgitter unter der Decke (ROOM10F0 c7), Band 1, Ankunft y -9000 |
| T064/T065/T066 | ROOM1250 -> ROOM1000 | etage | **sonstiges** | inert (sce 0 in allen Saetzen), Empfangstresen ohne Tuerblatt (ROOM1250 c2) bzw. unsichtbar |

Die Selbst-Tueren sind ueberwiegend ECHTE Tueren zwischen Teilbereichen eines RDT (Bogen ROOM1110
angesehen: vier Stahltueren der Asservatenkammer, `t1_bogen_ROOM1110.jpg`; ROOM4050 PRIVATE ROOMS
6 Paare; ROOM5090 Zugwagen-Doppelschiebetueren); dazu das Tor ROOM1170 (T041, eigene Sequenz).

`breit`-Hinweis: 68 begehbare Seiten haben eine Tuerkante >= 3000 (Standard 2000, 152 Seiten); dort
kann eine Doppeltuer oder ein Tor stehen (z.B. ROOM1090 S030: Doppeltuer im Bild, Kante 4700). Nur
Hinweis im Bogen ("BREIT"), keine Kategorie.

## 4. Ausschnitte — Verfahren, Pruefung an den 5 vermessenen Tueren

### 4.1 Verfahren (`zensus_bild.py`, `zensus_ausschnitte.py`)

1. **Kandidatenkanten** = die langen Kanten des Rechtecks bzw. Vierecks (>= 0,8 x laengste).
2. **Blatt** je Kandidat: senkrechtes Rechteck, Breite 1950 (RE1.5) bzw. 1640 (RE2) - beides
   06_massstab §1 -, Hoehe 3549, Unterkante y = -Band*1800, mittig auf der Kante.
3. **Cuts** je Kandidat: Projektion mit der Gleitkomma-Blickmatrix (`tor_kamera.Kamera(exakt=True)`,
   H = fov>>7 nach `@0x80021e70 srl a0,a0,7`, Mitte 160/120). Ein Cut ist "ok", wenn alle Ecken
   >= 300 vor der Kamera liegen, die Raumseite des Blatts zur Kamera zeigt, beide Seitenkanten
   weniger als 45 Grad gegen die Bildsenkrechte stehen, >= 60 % der Blattflaeche im Bild liegen und
   das Blatt >= 24 px hoch ist.
4. **Tuerbereich**: Umschaltgraph der RVD-Zonen (RDT+0x28, 20 B je Zone; erste Zone je "von" =
   Bildbereich, `rdt_common.c` re15_rdt_get_region_quad). Zusammenhangskomponenten = Raumteile
   (ROOM1000: {0,1,2}, {3,4,5}, {6,7,8} - Ostraum und zwei Toiletten). Nur Cuts derselben
   Komponente wie der "aktive" Cut (Bildbereich enthaelt die Rechteckmitte) zaehlen; Cuts ohne jede
   Umschaltzone (nur per Skript) kommen zuletzt. Ohne diesen Filter projizierte ROOM1000 S001 in die
   andere Toilette (Bild `S001_ROOM1000_c06_voll.png` angesehen: Umriss hinter der Wand).
5. **Kantenwahl**: nur Kandidaten mit mindestens einem ok-Cut. Einer -> dieser (202 Seiten). Mehrere
   und Ankunft der Gegenseite AUSSERHALB des Rechtecks -> die von ihr abgewandte Kante (72). Mehrere,
   Ankunft im Rechteck -> das in den aktiven Cuts frontalste Blatt (23; Breite/Hoehe im Bild gegen
   1950/3549), Gleichstand -> groessere Flaeche; ohne aktiven ok-Cut -> staerkster Bildbeleg (1).
   Kein Kandidat mit ok-Cut -> Ankunftsregel ohne Bild (1: ROOM1250 S125, inert).
   Die SCA-Kollision wird NICHT benutzt (06_massstab §6: Zellen 12..1256 vor der Wand).
6. **Verfeinerung am Bild** je gewaehltem Cut: Gitter dn -900..900 (50), dt -600..600 (50),
   W = Blattbreite +-300 (100); Wert = Median des Helligkeitssprungs quer zu linker/rechter/oberer
   (+ halb unterer) Kante minus Strafe `Median der Suche x groesste Eckverschiebung / 10 px`.
7. **Ausschnitt** = Huelle + 35 % Rand (mind. 10 px), 3-fach NEAREST, gelb = Datenlage,
   rot = verfeinert. **Entzerrtes Blatt** = Homographie des roten Vierecks auf 128 x 218, bilinear.
   Bis zu 3 Cuts je Seite; dazu das Vollbild mit Umriss.

Alle Schwellen sind Messwerkzeug (PORT-WAHL, keine Original-Adresse), nicht Spielverhalten.

### 4.2 Pruefung an den von Hand vermessenen Tueren

`python re15_port/tools/tueren/zensus_pruefung.py` -> `build/r31_tueren/t1/pruefung.json`, Bilder
`pruefung_<spiel>_<raum>_cNN.png` (gruen = Messung 06 §4.3, gelb = Datenlage, rot = verfeinert).
Mittlere Eckabweichung in Pixeln (Blatt im Bild 100..140 px hoch):

| Tuer | Datenlage | verfeinert | dn / dt / W |
|---|---|---|---|
| RE1.5 ROOM1000 c3 | 5,5 | 3,3 | -800 / -50 / 1750 |
| RE1.5 ROOM1000 c6 | 3,6 | 3,9 | -450 / -50 / 2050 |
| RE1.5 ROOM1010 c0 | 3,4 | 4,1 | +150 / -50 / 2050 |
| RE1.5 ROOM1010 c4 | 7,0 | 5,0 | -150 / 0 / 2050 |
| RE1.5 ROOM2060 c0 | 8,8 | 6,7 | -550 / -150 / 1850 |
| **RE1.5 Mittel (die 5 Tueren aus 06 §4.3)** | **5,6** | **4,6** | |
| RE2 ROOM60B0 c4 | 4,0 | 5,5 | +500 / -150 / 1740 |
| RE2 ROOM3060 c0 | 18,9 | 14,1 | +300 / +450 / 1340 |
| RE2 ROOM5050 c3 | 5,9 | 1,7 | -500 / +50 / 1340 |
| RE2 ROOM20E0 c3 | 4,9 | 1,0 | -900 / 0 / 1540 |
| RE2 ROOM2040 c0 | 11,3 | 8,7 | -250 / -50 / 1740 |
| RE2 ROOM40A0 c8 | 8,5 | 12,5 | +350 / -400 / 1740 |
| **RE2 Mittel (6 Tueren)** | **8,9** | **7,3** | |

Wahl der Strafe (an denselben 11 Tueren, also optimistisch; damals RE2 noch mit Breite 1950):
ohne Strafe RE1.5 5,2 / RE2 6,6 px, aber ROOM1010 c0 sprang auf eine innere Linie des Blatts (10,4 px;
Bild angesehen: rote Kante auf dem Schattenstreifen innen); Strafe 0,5/1/2/4 ergab RE1.5
4,5/4,6/4,9/4,8 und RE2 6,7/6,5/8,4/8,4. Genommen: 1. Mit RE2-Breite 1640 statt 1950 wird die
RE2-Datenlage besser (9,9 -> 8,9), die Verfeinerung schlechter (6,5 -> 7,3); 06_massstab misst 1640,
deshalb bleibt es dabei.

Angesehen: `t1_pruefung_re15_1000_c03.png` (gelb etwas innen, rot auf der Zargen-Innenkante, gruen
dazwischen), `t1_pruefung_re15_1010_c00.png` (gelb/rot/gruen innerhalb ~2 px),
`t1_pruefung_re2_3060_c00.png` (schlechtester Fall: gelb 1 Blattbreite zu weit rechts, rot dazwischen,
Unterkante beider ~15 px zu hoch - die RE2-Tuer steht dort in Band 3, dieselbe Bodenannahme wie 06).

### 4.3 Ergebnis Ausschnitte

| | RE1.5 | RE2 (Leon-Raeume ROOM1..7xx0 mit Hintergrund) |
|---|---|---|
| begehbare Seiten | 299 | 263 |
| in >= 1 Cut sichtbar | **298** | 251 |
| nicht sichtbar | 1: S125 ROOM1250 -> ROOM1000 (inert) | 12 |

Physische RE1.5-Tueren ohne sichtbaren Cut auf irgendeiner Seite: **1** (T066, inert, ROOM1250).

## 5. RE2-Referenzblaetter

`build/r31_tueren/t1/re2/DOOR00.png .. DOOR36.png` (55 Stueck) und `re2/uebersicht.png`
(verkleinert `tueren_belege/t1_re2_uebersicht.jpg`, angesehen).

Je Blatt: Textur 2-fach mit Blattbereich (gruen, u 0..126 / v 0..217) und Beschlagstreifen v 219..255
4-fach, Textbeschreibung und Bewegung aus `analysis/tor_1170/04_tuerkatalog.md` §2, dazu bis zu 3
Ausschnitte derselben Tuer aus RE2-Hintergruenden (Door_aot_set mit pc+26 == Archiv; verschiedene Raeume
zuerst, Rang = Pixelhoehe x Frontalitaet) je mit entzerrtem Blatt.

**Blattbereich belegt** (MD1 von RE2 DOOR00/03/0B gelesen, mesh0 = 12 Dreiecke): beide
Grossflaechen x = -145 und x = +143 tragen uv (126,217) (0,0) (0,217) / (126,217) (126,0) (0,0); von der
Gegenseite erscheint die Textur also gespiegelt. Die Schmalseiten nutzen die Streifen u 123..127.

Archive ohne sichtbaren RE2-Ausschnitt: 20, 21, 32, 34, 36 (alle "Textur wie DOOR00", kein Objekt im
Skript, 04 §2) und 33 ("wie DOOR1E"). 27 Archive haben 3 Ausschnitte, 19 zwei, 3 einen.
Identische Blattbereiche (Pixel gleich): 20/21/32/34/36 = 00, 33/35 = 1E.

Angesehen: `DOOR08.png` (`t1_re2_DOOR08.jpg`): ROOM30B0 c5, ROOM2030 c0, ROOM2090 c0 zeigen dieselbe
genietete Stahltuer; ROOM2030 teils von Kisten verdeckt.

## 6. Automatisches Mass + Validierung

Werkzeug `zensus_mass.py` (NUR Zweitbeleg; PORT-WAHL, keine Original-Adresse).

- Vorbereitung: mittlere 80 % Breite / 90 % Hoehe des entzerrten Blatts, Helligkeit auf 2..98-Perzentil.
- Merkmale: Farbe (Chromatizitaet Mittel+Streuung, 4 x 6 Felder), Struktur (Helligkeit 16 x 28,
  normiert, Pearson), Gradient (8 Richtungen, 4 x 7 Felder, Kosinus); Wert = Mittel der drei (0..1).
- Ein Archiv wird vertreten durch seine Textur UND alle entzerrten RE2-Hintergrund-Ausschnitte mit
  diesem Archiv, je auch gespiegelt; Wert = Maximum ueber die Vertreter.

**Validierung** an RE2-Tueren mit bekanntem Archiv (`build/r31_tueren/t1/mass_validierung.json`);
Treffer, wenn die Klasse (identische Blattbereiche) stimmt; Vertreter aus DEMSELBEN Raum sind
ausgeschlossen, bei "streng" zusaetzlich aus dem Zielraum (= die andere Seite derselben Tuer):

| Satz | n | Top-1 | Top-3 | Top-5 | Grundrate "immer die 3 haeufigsten" |
|---|---|---|---|---|---|
| alle RE2-Seiten, Textur + Ausschnitte | 251 | 72 (28,7 %) | 99 (39,4 %) | 110 (43,8 %) | 19,1 % |
| Blatt-Archive, Textur + Ausschnitte | 212 | 68 (32,1 %) | 91 (42,9 %) | 100 (47,2 %) | 22,6 % |
| **Blatt-Archive, streng** | **212** | **56 (26,4 %)** | **79 (37,3 %)** | **90 (42,5 %)** | 22,6 % |
| alle, nur Textur | 251 | 49 (19,5 %) | 70 (27,9 %) | 79 (31,5 %) | 19,1 % |
| Blatt-Archive, nur Textur | 212 | 46 (21,7 %) | 65 (30,7 %) | 74 (34,9 %) | 22,6 % |

"Blatt-Archive" = ohne 0E 0F 12 (Treppenlaeufe), 16 (Leiter), 1E 33 35 (Schott), 1F, 28 (Bodenluke),
2B, 2D (Hubbuehne) - dort gibt es kein senkrechtes Blatt zum Entzerren.

Lesart: das Mass ist SCHWACH. Top-3 streng 37 % gegen 23 % Grundrate. Fuer RE1.5 ist es noch
unsicherer (anderes Rendering, keine Ansicht derselben Tuer in der Referenz). Gebraucht wird es nur,
um im Bogen 5 Kandidaten vorzuschlagen. Varianten, die ich an demselben Satz verglichen habe (daher
optimistisch): nur Textur ohne Vorbereitung Top-3 24 %; mit Zuschnitt+Spreizung 30 %; Vertreter aus
RE2-Ausschnitten 42..44 %.

Je RE1.5-Seite stehen die Top-5 in `zensus.json` (`seiten[].kandidaten`: Archiv, Wert, Einzelwerte,
bester Vertreter, gespiegelt, Cut). Haeufigste Top-1 ueber 298 Seiten: 1A (27), 25 (22), 09 (19),
28 (18), 26 (18), 08 (17), 00 (14) - 25/28/26 sind unplausibel haeufig (Aufzugtuer, Bodenluke,
Schott) und zeigen, dass das Mass dunkle, strukturarme Blaetter schlecht trennt.

## 7. Kontaktboegen — Sichtpruefung

`build/r31_tueren/t1/boegen/ROOMxxx0.png`, 98 Boegen (ein Bogen je Grundraum mit begehbarer Seite).
Je Seite eine Zeile: Kennung (Seite, Tuer, Ziel, Band, Form, Kantenlaenge, Mitte, Kategorie, Engine,
Gegenseite, Zwilling), Ausschnitt 3-fach (gelb Datenlage, rot verfeinert), Vollbild mit Umriss,
entzerrtes Blatt, bester Ausschnitt der GEGENSEITE (anderer Raum), Top-5 RE2-Texturen mit Wert
("sp" = gespiegelt). Skript-Uebergaenge (Flaeche 0) stehen als Textzeilen darunter.

Angesehen (Read auf die PNG), mit Befund:

| Bogen | Tuer getroffen? | Blatt erkennbar? | Bemerkung |
|---|---|---|---|
| ROOM1000 | 3/3 | ja | Toilettentuer und Spindraum-Tuer; in c0 liegt gelb 1 Blatt daneben, rot trifft |
| ROOM1030 | 3/3 | ja | Glastuer oben (S008/S009), blaues Licht faerbt das Entzerrte |
| ROOM10A0 | 4/4 | ja | Tueren an den Treppenabsaetzen 1F/B1/B2; S033 gelb daneben, rot trifft |
| ROOM10B0 | S037 nein (Leiter daneben), S038 Luke im Boden | nein | fuehrte zu T024 = Leiter |
| ROOM1170 | 4/5 | Dachtueren ja, Tor ja (Blatt zu hoch fuers Tor) | S069 zeigt die obere Leiterkante, kein Blatt |
| ROOM11A0 | 3 Tueren + 1 Leiter | ja ausser S090 (31 px, dunkel, unklar) | S088 zeigt die Leiter |
| ROOM3000 / ROOM3010 / ROOM3060 | alle | ja | Fabriktueren mit Warnstreifen; ROOM3060 S192 Doppeltuer |
| ROOM4000 | 4/4 | ja | S218 = Lastenaufzug, S215 Aufzugtuer |
| ROOM4030 | 2/3 | S224/S225 ja | S225 = Viereck-Tuer auf der Schraegwand getroffen; **S226 (Viereck -> ROOM4080) daneben**: Umriss als Streifen am rechten Bildrand von c0; die Gegenseite ROOM4080 c7 zeigt die Tuer |
| ROOM3070 | 2/2 | ja | Lastenaufzug-Gitter |
| ROOM5080, ROOM5090, ROOM50B0, ROOM6000 | ja | ja; ROOM5090 S280/S282/S283 steil und dunkel; 50B0/6000 Hubbuehne ohne Blatt | Zugtueren = Doppelschiebetueren mit Fenstern |
| ROOM1090 | S030 neben der Doppeltuer | teilweise | Kante 4700: Doppeltuer, Blatt 1950 deckt nur einen Fluegel |
| ROOM10F0 | S047 ja; S048 Lueftung | - | fuehrte zu T029 = sonstiges |
| ROOM1110 | 5/5 | ja, sehr gut | Selbst-Tueren = Stahltueren der Kammern |
| ROOM1250 | - | - | inert, kein Tuerblatt |

Taugen sie? Ja fuer die Zuordnung per Bild: in 20 angesehenen Boegen trifft der rote Umriss die Tuer
in allen Faellen mit echtem Tuerblatt ausser ROOM4030 S226 (Viereck-Tuer) und ROOM1090 S030
(Doppeltuer, Umriss neben dem Fluegel); wo er sonst daneben liegt (Leiter, Luke, Hubbuehne, Lueftung), zeigt
das Vollbild den tatsaechlichen Uebergang. Grenzen: Doppeltueren (Blatt 1950 deckt einen Fluegel),
steile Ansichten (Entzerrtes unscharf), Tueren im Dunkeln. Die Top-5 sind nur ein Hinweis (§6).

## 8. Ergebnis / Zahlen

`build/r31_tueren/t1/zensus.json` (Schluessel `zahlen`, `raeume`, `tueren`, `seiten`, `saetze`, `paare`,
`re2_seiten`, `re2_blaetter`, `validierung`, `pruefung`; alle Bildpfade relativ zur Repo-Wurzel).

| Groesse | Wert |
|---|---|
| RDT / davon Stubs | 240 / 34 |
| Door_aot_set-Saetze / davon Viereck | 653 / 4 |
| Tuerseiten / Flaeche 0 / begehbar / sichtbar | 324 / 25 / 299 / 298 |
| Zwillingsseiten | 28 |
| Engine-Zeilen / ohne Satz | 230 / 0 |
| Paare (Regel / Skript / einzig) | 133 (125 / 6 / 2) |
| Tueren / physisch / davon inert | 163 / 144 / 3 |
| Kategorien physisch | normal 114, selbst 15, aufzug 9, leiter 2, sonstiges 4 |
| Kategorien alle 163 | zusaetzlich skript 18, aufzug +1 (Kabinen-Ankunft ROOM3080 -> ROOM4000) |
| physisch ohne sichtbaren Cut | 1 (T066, inert) |
| Raeume mit begehbarer Seite | 98 |
| RE2: Seiten / sichtbar | 263 / 251 |

Raeume mit begehbaren Tuerseiten (ROOMxxx0:Anzahl, fuer die Stapel der naechsten Welle; Bogen je
Raum `build/r31_tueren/t1/boegen/ROOMxxx0.png`):

1000:3, 1010:2, 1020:3, 1030:3, 1040:4, 1050:6, 1060:3, 1070:1, 1090:2, 10A0:4, 10B0:2, 10C0:3, 10D0:4, 10E0:1, 10F0:2, 1100:2, 1110:5, 1120:3, 1130:4, 1140:2, 1150:1, 1160:2, 1170:5, 1180:6, 1190:4, 11A0:5, 11B0:4, 11C0:1, 11D0:3, 11E0:4, 11F0:1, 1200:1, 1210:6, 1220:5, 1230:6, 1250:3, 1260:5, 2000:5, 2010:1, 2020:2, 2030:4, 2040:5, 2050:3, 2060:2, 2070:6, 2080:2, 2090:2, 20A0:3, 20B0:5, 3000:2, 3010:5, 3020:1, 3030:3, 3040:2, 3050:2, 3060:3, 3070:2, 3090:5, 30A0:1, 30B0:1, 30C0:2, 30D0:3, 30E0:4, 4000:4, 4010:1, 4030:3, 4040:4, 4050:14, 4070:3, 4080:4, 4090:1, 40A0:1, 40B0:1, 5000:5, 5010:1, 5020:1, 5030:4, 5040:3, 5050:2, 5060:2, 5070:3, 5080:1, 5090:5, 50A0:2, 50B0:2, 50C0:2, 50D0:4, 50E0:1, 50F0:1, 5100:2, 5110:4, 5120:3, 5130:3, 5140:2, 6000:4, 6010:3, 6020:2, 6030:4

Nachmessen:

```
python re15_port/tools/tueren/zensus_alles.py            # alles (ca. 6 min)
python re15_port/tools/tueren/zensus_paare.py --liste    # Tueren mit Kategorie und Kriterium
python re15_port/tools/tueren/zensus_pruefung.py         # Pruefung an 11 vermessenen Tueren
python re15_port/tools/tueren/zensus_mass.py validieren  # Mass-Validierung
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x800405bc 40
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80042ee0 80
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80014368 40
```

## 9. Offene Punkte

1. **Port-Befund Viereck-Tueren (nicht T1):** ROOM4030/4031 `@0x47E`/`@0x4A6` sind 40-B-Tuersaetze
   (sat 0xB1) nach ROOM4040. Der Port liest sie mit dem 32-B-Schema und stellt Tueren mit Ziel
   "BF0A0"/"C5F00" auf (`engine_tueren.txt`, `scd_vm.c op_door_aot_set`); der Kommentar
   `aot_common.c:604-609` nennt sie "non-door scan artifacts". Richtig: Punkte pc+6..21, Nutzlast
   pc+22 (`@0x80042f90 addiu a0,s0,20`), Trefftest `FUN_80014368`.
2. `engine_tueren.txt` ist veraltet (Band-Spalte immer 0) und deckt die Elza-Raeume xxx1 nicht ab;
   74 begehbare Seiten mit sce != 0 stellt die Engine beim Betreten NICHT auf (Skript-Tueren oder nur
   in xxx1) - im JSON als `engine: false/null`.
3. **Kategorien per Sicht** nur fuer die 10 "etage"-Kandidaten und 1 Lueftung gesetzt; die 114
   "normal" sind nicht alle angesehen (20 Boegen gesehen). Rolltor/Schott wurde nicht gefunden, kann
   aber unter "normal" stecken (z.B. breite Kanten >= 3000: 68 Seiten).
4. **Doppeltueren**: das Blattmodell ist einfluegelig; bei breiten Kanten deckt der Umriss nur einen
   Fluegel (ROOM1090 S030).
5. **Mass schwach** (Top-3 streng 37 % bei 23 % Grundrate); die Zuordnung muss am Bild entschieden werden.
6. Die RE2-Pruefung ROOM3060 c0 weicht 14 px ab (Unterkante zu hoch); die Bodenannahme y = -Band*1800
   ist dort nicht gegengeprueft.
7. Einseitige Seiten ROOM2070 S157/S158 und ROOM20B0 S171 (Ankunft deckt sich mit anderen Seiten) -
   ob sie im Spiel erreichbar sind, ist nicht untersucht.
