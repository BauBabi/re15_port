# 04 - Tuerkatalog: Gegenpruefung (Skeptiker)

Stand 2026-09-28. Geprueft wurde `analysis/tor_1170/04_tuerkatalog.md` gegen die Aussagenliste
`build/tor_1170/aussagen_tuerkatalog.json`. Das Werkzeug des Untersuchers (`tuerkatalog.py`, `do2_format.py`)
wurde NICHT benutzt. Alle Zahlen unten stammen aus eigenen Skripten unter
`build/tor_1170/skeptiker_tuerkatalog/` und aus eigener Disassembly (`re2_disasm.py` / `re15_disasm.py`,
Binaerdateien `info/re2leon/PSX.EXE` und `info/Re1.5/PSX.EXE`, beide t_addr 0x80010000).

## 0. Ergebnis

- 29 von 32 Aussagen bestaetigt, 2 widerlegt (K13 in einem von vier Werten, K31 in der Sache), 1 nicht pruefbar (K16).
- Alle Schwerpunkt-Aussagen zum Vorbild DOOR2E (K17..K27, K05, K30) halten: Modell, Aufbau-Saetze, Drehung
  570 Einheiten in Bild 130..209, Fahrt, Zeitgeruest 291 Bilder, Richtung, Flag-Unterschied RE2/RE1.5.
- Die Haendigkeit der Kamera (Bild-rechts = +z) ist jetzt ZUSAETZLICH ueber die Rueckseiten-Probe des Renderers
  belegt; der Untersucher hatte sie nur gerechnet.

## 1. Widerlegt oder eingeschraenkt

### K13 - Oeffnungswinkel DOOR26 / DOOR31

Behauptet: 887 Einheiten = 77,96 Grad in 115 Bildern. Gemessen: 887 ist der AUSSCHLAG, nicht die Endlage.
Der Fluegel schwingt drei Schritte zurueck.

| Groesse | Wert | Beleg |
|---|---|---|
| Endlage | 884 Einheiten = 77,70 Grad | `s08_alle.py`; DOOR26 Obj 0 Drehung y Bild 0 = 0, Ende = 64652 = -884 |
| Ausschlag | 887 Einheiten = 77,96 Grad | Summe vor den letzten drei Schritten |
| Schritte | 112 in Bild 110..224 (Spanne 115, Bilder 213, 217, 221 ohne Schritt) | eigene Simulation |
| letzte Schritte | ... -5 -5 -5 -3 -3 -3 -1 -1 -1 +1 +1 +1 | DOOR26 Skript 11 @Datei 0x5b44 |

Ursache im Skript: `2f 0a 02 00` (awy = +2) @Datei 0x5b64, danach `0d 00 0a 00 04 00` (For 4) @0x5b7a mit je
Add_aspeed + drei Add_speed; die Geschwindigkeit laeuft -5, -3, -1, +1. Die uebrigen drei Werte stimmen:
1030 / 141 Bilder (80 Instanzen in 27 Tueren), 588 / 120 Bilder (DOOR07), 570 / 80 Bilder (DOOR0A, DOOR2E).
Fuer das Vorbild DOOR2E aendert sich nichts (keine Rueckschwingung, Schritte durchweg positiv).

### K31 - "keine RE2-Textur traegt eine Warnschraffur"

Die Schwelle des Untersuchers (R >= 150, G >= 110, B <= 70) findet nur KRAEFTIGES Gelb. DOOR15 traegt im
Blattbereich ein schraeg schraffiertes Warnband mit gerahmter Tafel links daneben, nur in gedecktem Ocker:

| Groesse | Wert | Beleg |
|---|---|---|
| Lage des Bandes | v = 77..112, u = 34..114 (Rahmen der Tafel links davon bei u = 5 und u = 30) | Streuung der Helligkeit je Zeile (u 40..110) und je Spalte (v 80..110) an `tex_15_x4.png`; Band = Streuung ab 46 |
| helle Streifen | Median RGB (90, 74, 57), oberes Viertel der Helligkeit im Feld u 36..112 / v 78..112 | `tex_15_x4.png`, jedes vierte Pixel |
| dunkle Streifen | Median RGB (24, 24, 24), unteres Viertel desselben Felds | derselbe Lauf |
| hellstes Ocker im Band | (139, 106, 82) | `s12_textur.py 15 34 77 115 113` |
| Treffer der Untersucher-Schwelle | 0 von 28160 Pixeln im Blattbereich v 0..219 | `s12_textur.py 15 0 0 128 220` |

Folge: Es gibt ein RE2-Vorbild dafuer, ein Warnschild mit Tafel in die Tuertextur zu malen. Der Schluss
"das Schild muss aus dem Hintergrund von ROOM1170 kommen" bleibt fuer die FARBE richtig (kein RE2-Gelb), folgt
aber nicht aus einem Fehlen des Motivs.

### K16 - Rangliste (nicht pruefbar)

Die Gewichte sind eine Festlegung des Dossiers. Bestaetigt sind nur die Messwerte, die ich nachgemessen habe:
DOOR2E 17 Huell-Ecken und 8 Staebe, DOOR0A 8 Huell-Ecken und 12 Staebe. Die Deckungswerte 0,619 / 0,531 und
die Alleinstellung ("nur 2E und 0A") habe ich nicht nachgemessen.

## 2. Schwerpunkt: was gemessen wurde

### K05 - Flag-Wort RE2 gegen RE1.5 (bestaetigt)

| Was | RE2 | RE1.5 |
|---|---|---|
| Handler | 0x80014ba4, Tabelle @0x800a75fc = `a4 4b 01 80` | 0x80016f20, Tabelle @0x800745e4 = `20 6f 01 80` |
| Eltern-Bit | @0x80014c64 `andi v0,v1,0x10` (Bytes `10 00 62 30`) | @0x80016fe4 `andi a3,v0,0x8` (Bytes `08 00 47 30`) |
| Eltern-Nummer | @0x80014c6c `andi v0,v1,0xf` (Delay-Slot des beq) | @0x80016ff4 `andi v0,v0,0x7` (Delay-Slot des bne) |
| Elternmatrix | @0x80014c80 `addiu v0,v0,84` (Delay-Slot des j) | @0x80017018 `addiu v0,v0,72` |
| ohne Eltern | @0x80014c84/88 Zeiger 0x800dcba8 | @0x80017004 `sw zero,116(a1)` (Delay-Slot des j) |
| Ablage des Zeigers | @0x80014c8c `sw v0,128(a0)` | @0x8001701c `sw v0,116(a1)` |
| Satzlaenge | @0x80014cac `addiu v0,a1,22` | @0x80017020 `addiu v0,a2,22` |

0x800dcba8 ist die Blickmatrix: FUN_80076cb0 schreibt sie dorthin (Decompilat Zeile 1..8, MulMatrix-Ziel).
Zusatz: Der RE1.5-Renderer FUN_800166c4 nimmt bei Zeiger 0 die Matrix 0x800b5288 (@0x80016738 `lw v0,104(s1)`,
@0x80016740 `bne`, @0x80016748/4c `lui/addiu a2 = 0x800b5288`).

### K17..K20 - Modell DOOR2E (bestaetigt)

Eigener MD1-Leser `s03_md1.py 2E`: Datei 75772 B, Tabelleneintrag Klang 0x4ae8 / Modell 0xd7fc / Sektor 10,
Modellteil @0x5000, MD1 @0x5260, TIM @0xa5dc, 9 Skripte. Mesh-Eintraege @0x526c und @0x52a4.

| Mesh | Vertices | Dreiecke | Huelle | Komponenten |
|---|---|---|---|---|
| 0 | 272 | 448 | x -120..120, y -5977..-1, z -3044..2 | 9: 352 + 8 x 12 |
| 1 | 16 | 26 | x 0..240, y -43..47, z -30..480 | 1 |

- Komponenten sind ueber Vertex-Indizes UND ueber Vertex-Positionen gleich (9 / 9), es gibt keine doppelten
  Positionen (272 von 272 verschieden).
- Stab (Indizes 48..59): 12 Vertices, 12 Dreiecke, 0 Dreiecke mit konstantem y (kein Deckel), 12 von 24 Kanten
  offen, 6 Normalenrichtungen zu je 2 Dreiecken. Querschnitt 60 in x, 52 in z. `s04_stab_huelle.py 2E 48 59`.
- DOOR0A Stab (Indizes 48..63): 16 / 16, 8 Richtungen, 139 x 136. `s04_stab_huelle.py 0A 48 63`.
- Huelle der Frontansicht: 17 Ecken; vier Ecken zu je 3 Punkten, Eckmasse z/y 182/187, 179/182, 186/212, 210/208.
- Ursprung = Drehachse: der Renderer ruft RotMatrix mit obj+116 nach obj+36 (@0x800142a0/a4/a8), multipliziert
  die Spalten von obj+36 mit der Elternmatrix (@0x800144f4..0x800145b8) und rechnet die Lage obj+56 mit
  Eltern-Translation (@0x800145bc..0x80014608). Welt = Eltern x (Drehung, Lage).

EINSCHRAENKUNG zu K20: "Fluegel erstreckt sich nach -z" gilt fuer DOOR2E, DOOR0A und die Standardplatte, NICHT
allgemein. DOOR26 und DOOR31 liegen bei z 0..2990, DOOR30 bei z 0..1216. Gemeinsam ist allen nur: Angel bei z = 0.

### K21 / K22 - Aufbau-Saetze (bestaetigt)

Rohe Bytes selbst gelesen, alle fuenf Saetze stimmen byteweise mit dem Dossier ueberein:
`@0x5036 4d 00 00 00 01 00 80 0a 10 00 c4 09 b8 0d b2 06 00 00 00 00 00 00`,
`@0x504c 4d 01 00 00 01 01 d0 00 10 00 82 00 fc f4 4c f5 00 00 00 00 00 00`,
`@0x5096 ... a2 fa 00 00 00 08 00 00`, `@0x50ac ... 7e ff fc f4 4c f5 00 10 00 00 00 08`,
`@0x50c2 4d 02 00 00 01 01 50 00 10 00 82 00 2e f5 4c f5 38 07 00 08 00 08`.

### K23..K26 - Bewegung und Zeit (bestaetigt)

Eigener Simulator `skept_sim.py`, Lauf `s07_door2e.py 2E`. Semantik je Opcode aus eigener Disassembly:

| Punkt | Beleg |
|---|---|
| Plaetze 10..13 der Reihe nach je Bild | FUN_80014058 @0x80014068 `addiu s2,zero,10`, @0x80014150 `sltiu v0,s2,0xe`, Schritt @0x80014158 `addiu s1,s1,372` |
| Rueckgabe 1 weiter, 2 Bild zu Ende | @0x800140e8 `beq v1,v0` (v0 = 1), @0x800140f0 (v0 = 2) |
| Sleep n kostet n Bilder | Sleeping @0x80053a24: BEIDE Wege enden in @0x80053a84/88 `jr ra` / `addiu v0,zero,2` |
| For / Next | @0x80053b1c (Zaehler u16@4, Block s16@2), @0x80053cbc |
| Speed_set | @0x80055aa0 `sll v1,v1,1`, @0x80055aac `sh a1,344(v1)` |
| Add_speed | Lage @0x80055abc..e0 (+56/60/64), Drehung @0x80055aec..b10 (+116/118/120) |
| Add_aspeed | @0x80055b2c..b90 (344+2i += 356+2i, i = 0..5) |
| Work_set Typ 5 | Tabelle @0x800111f0 Eintrag 4 = 0x800559b0, dort `lw v0,19928(at)` = 0x800d4dd8 + 4 x Nummer |
| Schleifenende | FUN_80013eb4 @0x80014028..34: solange Byte 0x800d86e9 (Platz 10 aktiv) ungleich 0 |
| Variable 0x0D loeschen | @0x80013f68 `sh zero,19502(s0)` = 0x800d4806 |

Ergebnis beider Varianten: 291 Bilder; Fluegel 80 Schritte Bild 130..209, Summe 570
(30 x 7, dann 8 8 9 9 10 10 11 11 12 12, dann je viermal 11..2); Riegel 29 Schritte Bild 71..99, Summe -365;
Fahrt Bild 201..289, x +7140, z +1600 / -1600; Endlage (9640,3512,3314) / (9640,3512,-2974).
Stuetzwerte Bild 159 / 169 / 199 / 200 / 209 = 210 / 310 / 542 / 546 / 570 stimmen.
Skript 6 wird von keinem Gosub/Evt_exec genannt (eigenes lineares Zerlegen).

Gegenprobe zur Annahme "Klang ist in Bild 100 geladen": mit 20 Bildern Wartezeit (`s07_door2e.py 2E 120`)
laeuft die Sequenz 311 Bilder, Fluegel 150..229, Fahrt 221..309. Der Riegel (71..99) bleibt. Die Annahme ist im
Dossier offen ausgewiesen; sie bleibt OFFEN.

### K11 / K27 - Kamera und Richtung (bestaetigt, mit neuem Beleg)

| Punkt | Beleg |
|---|---|
| Auge (10000,0,0), Ziel 0 | @0x8001082c: `00 00 2d 64 10 27 00 00 ...`; Kopie ab @0x80013c28, Uebergabe @0x80013e38 `addiu a0,sp,20`, @0x80013e40 `addiu a1,sp,32` |
| H = 290 | @0x80013e34 `addiu a0,zero,290`, Ziel 0x8008de24 = `ctc2 a0,r26` (Wort 0x48c4d000) |
| Blickmatrix | FUN_80076cb0 @0x80076f08..1c: `sh v0,16(sp)`, `sh v0,32(sp)`, `subu v0,zero,v1`, `sh v0,20(sp)`, `sh v1,28(sp)`; mit dx = -10000: m[0][2] = +4096, m[2][0] = -4096 |
| Grundmatrix | @0x8009db44: `00 10 00 00 00 00 00 00 00 10 00 00 00 00 00 00 00 10` = Einheitsmatrix |
| RotMatrix | 0x8008e1f4, @0x8008e2b0..b4 t6 = untere Haelfte des Tabellenworts, @0x8008e2c8 `sh t6,4(a1)`; Tabelle @0x800adeac: Winkel 1024 -> (4096, 0), also untere Haelfte = Sinus |
| Sinus 570 | 3142 (Tabelle), Kosinus 2623 (genau waere 2627,5; Tabelle ist gerundet, groesster Fehler 6,8) |

Rechnung: freie Kante z = -3044, x = 2500 + (3142 x -3044) / 4096 = 2500 - 2335 = 165 (Variante 0),
x = 2500 + 2335 = 4835 (Variante 1, Winkel 2618, Sinus -3142).

NEUER Beleg fuer die Haendigkeit (`s15_nclip.py`): Der Renderer laedt V0/V1/V2 aus den Indizes @+2/+6/+10
(@0x8001475c..80), rechnet NCLIP (@0x800148d0, Wort 0x4b400006) und zeichnet bei MAC0 >= 0
(@0x800148f8 `bgez v0,0x80014938`). Mit Bild-rechts = +z, Bild-unten = +y, Tiefe = 10000 - x:

| Abbildung | kamerazugewandte Flaechen gezeichnet | abgewandte gezeichnet |
|---|---|---|
| Bild-rechts = +z, Variante 0 | 71 von 71 | 0 von 71 |
| Bild-rechts = -z (gespiegelt) | 0 von 71 | 71 von 71 |
| Bild-rechts = +z, Variante 1 (y = 2048) | 71 von 71 | 0 von 71 |

Dasselbe an der Standardplatte DOOR00 (2 von 2 / 0 von 2). Zusaetzlich waechst u mit z auf beiden Blattseiten
(`s14_uv_haendigkeit.py`: du/dz > 0 bei 99,8..100 Prozent der Flaeche), die Textur erscheint in Variante 0 so, wie
sie gemalt ist. Eine Emulator-Aufnahme fehlt weiterhin; die Spiegel-Frage ist damit aber aus dem Code entschieden.

### K30 - Objektbaum (bestaetigt)

`s08_alle.py` und `s10_baum.py`: 146 Varianten, kein y-drehendes Objekt mit Mesh-Hoehe ab 3000 hat ein
Elternobjekt. Grosse Kinder gibt es (46 Instanzen: DOOR10 Glieder, DOOR2C, DOOR2D), sie drehen aber nicht um y.
DOOR15 Variante 2: Skript 3 @Datei 0x5930, Obj 1 Flags 0x02a0 ohne Eltern, Fahrt getrennt ueber
Evt_exec Platz 12 Skript 6 (Work_set Typ 5 Nummer 1) neben Gosub 5 (Nummer 0).

## 3. Uebrige Aussagen

| Aussage | Urteil | Wie geprueft |
|---|---|---|
| K01 | bestaetigt | `s01_container.py`: 55 von 55 Archive, Beginn und Laenge stimmen; Eintrag 0x37 ist keine Tuer mehr (`1c 3c 01 80 ...`). Instruktionen @0x80014d40..50, @0x80014d94, @0x80014da0 selbst gelesen |
| K02 | bestaetigt | 8560 Dreiecke, 0 Vierecke (`s11_modelle.py`); Indizes @+2/+6/+10 und `sll v0,v0,3` gelesen |
| K03 | bestaetigt | 55 von 55 TIM: Flags 9, CLUT (0,480) 256 x 1, Bild (0,0) 64 Worte x 256 |
| K04 | bestaetigt | Handler 0x80014ba4 vollstaendig gelesen |
| K06 | bestaetigt | alle genannten `andi` an den genannten Adressen; Ordnungstabelle @0x800149bc..0x80014a24 |
| K07 | bestaetigt | siehe Tabelle oben; 0x74 @0x80058004 `addiu s0,s0,4`; Work_copy @0x800542ec `sh` / @0x80054304 `sb` |
| K08 | bestaetigt | siehe Tabelle oben |
| K09 | bestaetigt | @0x80013e5c..98 gelesen; 0x800d47ec + 2 x 0x0C = 0x800d4804 |
| K10 | bestaetigt | DOOR0001.c Zeile 3 `40970`, Zeile 7 `Sleep(17920)`; Bytes `a0 0a` und `09 0a 46 00` |
| K12 | bestaetigt | 29 + 3 Dreher, 7 Heber, 3 Anbauteile, 5 mit an = 0 identisch; DOOR10 und DOOR2D haengen an der Regel des Dossiers |
| K14 | bestaetigt | Dreher 5639..6602, Schieber 6002..7163 |
| K15 | bestaetigt | 36 Tueren mit identischer Vertex- und Indexliste |
| K24 | bestaetigt | siehe K23..K26 |
| K28 | bestaetigt | 600 B, gleiche Offsets, 11 Bytes: 0x50 0x52 0x55 0xb0 0xb2 0xb5 0xc6..0xc9 0xcb |
| K29 | bestaetigt | 301 Bilder, Abstand 650 -> 155 in Bild 121..219, Fahrt 5830; Member 13 = Tabelle 0x80011228 Eintrag 13 = 0x80055d3c, Schreiben im Delay-Slot @0x80055d40 `sw a2,64(a0)` |
| K32 | bestaetigt | Dateien vorhanden, zwei Blaetter angesehen |

Anmerkung zu K07: In den 498 Skripten kommen 39 verschiedene Opcodes vor, nicht 41. 0x03 (Evt_chain) und
0x05 (Evt_kill) stehen in der Tabelle des Dossiers, werden aber von keinem Tuerskript benutzt.

## 4. Neue Befunde

1. DOOR26 / DOOR31 schwingen zurueck: Ausschlag 887, Endlage 884 (Abschnitt 1).
2. DOOR15 traegt ein Warnband mit Tafel in der Textur, v = 77..112 (Abschnitt 1).
3. Blattrichtung ist nicht einheitlich: DOOR26, DOOR31, DOOR30 laufen nach +z.
4. Haendigkeit ueber NCLIP belegt (Abschnitt 2).
5. RE1.5 hat dieselbe Drehkonvention: RotMatrix 0x80068098, @0x8006816c `sh t6,4(a1)`, m[0][2] = +sin y.
6. Die Beleuchtung rechnet mit der Weltmatrix des VORIGEN Bildes: obj+84 wird @0x8001436c..0x80014434 gelesen,
   aber erst @0x800144f4..0x800145b8 neu geschrieben.
7. Lagen muessen in 16 Bit passen: gelesen mit `lhu` (@0x800145d8/dc) und `lwc2` (@0x800145ec), abgelegt aus
   IR1..IR3 (@0x80014600..08, Worte 0xe8490000 / 0xe84a0004 / 0xe84b0008), die bei +-32767 saettigen.
8. Flag 0x0400 ist kein reiner Umschalter: bei (Flags & 0xc0) == 0xc0 wird 0x80 geloescht, sonst gesetzt
   (@0x800146f0..0x80014704).
9. Flag 0x2000 staucht mit Skript-Variable 0x04 (Adresse 0x800ce5e0 + 25108 = 0x800d47f4) und schreibt das
   Ergebnis in die Vertexdaten zurueck (@0x800147b8 `sh v0,4(s3)`).
10. Der Tabelleneintrag hat ein drittes Feld +8 (DOOR00: 0x0000429a), das das Dossier nicht benennt.

## 5. OFFEN

1. Wartezeit auf den Klang (Variable 0x0D) und Dauer eines Bildes: unveraendert offen.
2. Emulator-Aufnahme einer RE2-Tuersequenz fehlt; Bildmitte (160,120) ist nicht nachgelesen.
3. Befund 9: ob das Zurueckschreiben sich von Bild zu Bild aufschaukelt oder die Vertexdaten vorher
   wiederhergestellt werden, habe ich nicht verfolgt. Betrifft nur Objekte mit Flag 0x2000 (DOOR10), nicht DOOR2E.
4. Feld +8 des Tabelleneintrags.

## 6. Nachmessen

```
python build/tor_1170/skeptiker_tuerkatalog/s01_container.py          # K01, K03
python build/tor_1170/skeptiker_tuerkatalog/s03_md1.py 2E x           # K17, K20 (schreibt mesh_2E.pkl)
python build/tor_1170/skeptiker_tuerkatalog/s04_stab_huelle.py 2E 48 59   # K18, K19
python build/tor_1170/skeptiker_tuerkatalog/s07_door2e.py 2E          # K21..K26
python build/tor_1170/skeptiker_tuerkatalog/s07_door2e.py 2E 120      # Gegenprobe Klang-Wartezeit
python build/tor_1170/skeptiker_tuerkatalog/s08_alle.py               # K13, K30, Opcode-Zensus
python build/tor_1170/skeptiker_tuerkatalog/s10_baum.py               # K30
python build/tor_1170/skeptiker_tuerkatalog/s11_modelle.py            # K02, K15, K28
python build/tor_1170/skeptiker_tuerkatalog/s12_textur.py 15 0 90 128 150   # K31
python build/tor_1170/skeptiker_tuerkatalog/s14_uv_haendigkeit.py 2E  # K11, K27
python build/tor_1170/skeptiker_tuerkatalog/s15_nclip.py 2E           # K11, K27
```
