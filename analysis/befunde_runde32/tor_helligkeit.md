# Runde 32 — Helligkeit der Tuersequenz Gelaendertor ROOM1170

Nutzer-Befund (woertlich): "Das von uns erstellte Tor in ROOM 1170 sieht gut aus, aber ist zu dunkel."

Zweig `r32/tor-hell`, Arbeitsbaum `.claude/worktrees/r32_tor`. Messwerkzeug (neu):
`re15_port/tools/tor/tor_helligkeit.py` (`licht` | `bild <ppm> tor|2E <var> [bk]` | `gemalt`).

## 0. Stand / Protokoll (laufend)

- [x] 1. Messen: Tor in Sequenz vs. gemaltes Tor (gleiche Texelstellen), RE2 DOOR2E gezeigt/Texel
- [x] 2. Ursache belegt (Licht-Rechnung mit Adressen + 5-Bit-Rundung im Generator)
- [x] 3. Korrektur (RE2-Flag 0x1000 + Generator + Renderer > 0x80), nachher gemessen, RE2-Tueren vorher/nachher
- [x] 4. Riegel (probes/r32_tor.cmake: unit_r32_tor_hell, integration_r32_tor_hell), RE15_MIN_TESTS 411 -> 413

## 1. Messung vorher (Stand master eb10ceba + Auftrag 48d43b34)

### 1.1 Aufnahme

Echte exe des Arbeitsbaums (`re15_port/build/platform/pc/re15_pc.exe`, beschleunigter SDL-Renderer),
`RE15_TUER_TEST=0|1` (Tor V0/V1) bzw. `RE15_TUER_SEITE=S315` (RE2 DOOR2E V0) und `S017` (DOOR13),
`RE15_TUER_SERIE=<dir>` (Rueckleser `SDL_RenderReadPixels` VOR dem Present, 960x720),
`RE15_TUER_SCHNELL=1`, `RE15_NOAUDIO=1`. Kein AUTOSHOT, kein SOFTWARE_RENDER.
Gemessen wird Bild 100 der Door_move-Schleife (nach dem Einblenden, vor dem Schwenk ab Bild 130:
Lage und Licht stehen still, 09 §1).

### 1.2 Verfahren "gleiche Bildstellen"

`tor_helligkeit.py bild` rechnet das Modell im selben Bild nach (Door_model_set der Variante,
RotMatrix @0x8008e1f4, Kamera 08 1.2, H 290, Abtastphase 0,375) und rastert es im 3x-Raster der
Serie. Je Bildpunkt im Inneren eines Dreiecks (>= 1,5 Rasterpunkte von jeder Kante, keine
Ueberdeckung): F = gezeigt, T = Texel (5 Bit << 3 wie `re15_tim_rgb555_to_argb8888`),
P = der gemalte Cut-12-Bildpunkt, aus dem dieser Texel stammt (`tor_modell.textur_bauen` VOR der
5-Bit-Rundung). Also exakt dieselbe Stelle des gemalten Tors.

**Modellpruefung:** mittlere Abweichung |F - Modell| = **0,16** Helligkeitsstufen (Tor V0 und V1),
0,29 (DOOR2E). Der Port zeichnet genau, was die Rechnung sagt; die Zahlen unten sind also
Eigenschaften der Rechnung, keine Messfehler.

### 1.3 Ergebnis (Helligkeit Y = 0,299R + 0,587G + 0,114B, Summenverhaeltnisse)

| Tor Bild 100 | Punkte | F | T | P | F/T | T/P | **P/F = zu dunkel** |
|---|---|---|---|---|---|---|---|
| V0 Schild | 35873 | 20,87 | 36,65 | 40,09 | 0,5695 | 0,914 | **1,921** |
| V0 Rohr | 14954 | 34,66 | 62,87 | 66,25 | 0,5513 | 0,949 | 1,911 |
| V0 Pfosten | 1745 | 21,90 | 38,53 | 41,83 | 0,5684 | 0,921 | 1,910 |
| V0 Laschen/Fuesse | 690 | 18,70 | 32,70 | 36,09 | 0,5719 | 0,906 | 1,930 |
| **V0 gesamt** | 53262 | | | | 0,5622 | | **1,917** |
| **V1 gesamt** | 52854 | | | | 0,5636 | | **1,912** |

RE2 DOOR2E (S315 = V0) in derselben Maschine, Blatt: F/T = **0,5666** (c-Mittel 72,9) - RE2 zeigt
seine Tuertextur ebenfalls mit 57 % des Texels. Die DOOR2E-Textur ist dafuer HELL GEMALT:
Texel-Mittel im Blatt 69,1 gegen 36,7 im Torschild; TIM-Mittel 43,0 (p99 135, max 214) gegen
34,9 (p99 116, max 148). Die Tortextur dagegen ist ein Abzug des schon beleuchteten Hintergrunds.

**Das Tor ist um den Faktor 1,92 zu dunkel** gegen dieselben Stellen des gemalten Tors (Cut 12).

### 1.4 Das gemalte Schild in den drei Cuts (Kontext)

`tor_helligkeit.py gemalt` (Modell-Schild ueber die Raumkameras in den Cut gerastert, Deckung >= 0,99):

| | Punkte | Y Schild |
|---|---|---|
| Cut 0 (Landeplatz, Tor klein, von der Lampe beleuchtet) | 235 | 125,6 |
| Cut 11 (Laufsteg von oben) | 476 | 50,9 |
| Cut 12 (Laufsteg-Seite, Quelle der Textur) | 2983 | 33,7 |
| Textur Schild gemalt / 5 Bit | 128x78 | 33,4 / 30,0 |

Die Cuts sind untereinander bis 3,7fach verschieden hell (Raumlicht ist gemalt, je Kamera anders).
Die Textur stammt aus Cut 12; "gleiche Bildstellen" ist deshalb Cut 12 (1.3). Cut 0/11 siehe Offen.

## 2. Ursache (Licht-Rechnung, jede Konstante selbst disassembliert)

### 2.1 RE2-Tuerlicht (info/re2leon/PSX.EXE)

```
800142ac: 96220130  lhu v0,304(s1)        ; s1 = obj+20 -> obj+324 = Objekt-Flags
800142b4: 30421000  andi v0,v0,0x1000
800142b8: 1040000b  beq v0,zero,0x800142e8
800142bc: 24070088  addiu a3,zero,136     ; Flag 0x1000: BK 136 (..c0/c4 t0,t1)
800142c8: 00076100  sll t4,a3,4           ; <<4
800142d4: 48cc6800  ctc2 t4,RBK           ; ..d8 GBK, ..dc BBK
800142e8: 24070044  addiu a3,zero,68      ; sonst BK 68 (..ec/f0)
80014300: 48cc6800  ctc2 t4,RBK           ; ..04 GBK, ..08 BBK
8009a470: 90 01 20 03 0c fe f8 f8 18 fc 74 f5 ac 0d 2c 1a b0 04   ; L (400,800,-500)(-1800,-1000,-2700)(3500,6700,1200)
8009a490: 40 06 40 06 40 06 ...                                     ; LCM neunmal 1600
80014b4c: 3c020080  lui v0,0x80 / 80014b58: 34428080 ori v0,v0,0x8080 / 80014b5c: ac82007c sw v0,124(a0)  ; RGBC 0x808080
```

### 2.2 Eckfarbe am Schild

Schild-Normale im Blickraum (V0 Drehung 0: (4096,0,0); V1 Drehung 2048: Rueckseite (-4096,0,0)
gedreht -> dieselbe). LLM = L * C * R: IR = lm1(500, 2700, -1200) = (500, 2700, 0).

- BK 68: IR_c = 68*16 + 1600*3200/4096 = 1088 + 1250 = 2338 -> c = 128*2338/4096 = **73**
- BK 136: IR_c = 2176 + 1250 = 3426 -> c = **107**

Gezeigt = Texel * c / 128 (psx-spx GPU:1438-1446; Port render_pc.c `psx_prim_to_sdl_vert`).
`tor_helligkeit.py licht` ueber alle gezeichneten Dreiecke (Bild 100):

| | BK | c Flaechenmittel | c min..max | Ecken > 128 |
|---|---|---|---|---|
| Tor Fluegel V0/V1 (flags 0x0a80) | 68 | 73,0 | 40..118 | **0 / 246** |
| | 136 | 107,0 | 74..152 | 48 / 246 |
| Tor Pfosten (0x0280) | 68 / 136 | 71,3 / 105,3 | 46..73 / 80..107 | 0 |
| DOOR2E Blatt (0x0a80) | 68 | 72,7 | 34..137 | 26 / 849 |
| | 136 | 106,7 | 68..171 | 122 / 849 |

### 2.3 Zerlegung des Faktors 1,92

| Anteil | Faktor | Herkunft |
|---|---|---|
| Tuerlicht | 128/73 = **1,753** | BK 68 (@0x800142e8), L (@0x8009a470), LCM 1600 (@0x8009a490), RGBC 0x808080 (@0x80014b58) auf eine Textur, die das Raumlicht SCHON traegt (Abzug der Cut-12-Pixel, 07 §1) |
| 5-Bit-Abschneiden im Generator | 1/0,914 = **1,094** (Schild), 1,054 (Rohr) | `do2_format.tim_aus_bild`: `r5 = a >> 3` (abschneiden), Port liest `<< 3` (`include/re15_tim.h:114`): im Mittel 3,5 Stufen weg, bei Y ~ 35 sind das 9 % |
| Produkt | 1,753 * 1,094 = **1,918** | gemessen 1,921 (Schild), 1,917 (gesamt) |

**Die Kappung ueber 0x80 im PC-Renderer ist NICHT die Ursache:** bei BK 68 hat das Tor keine
einzige Ecke ueber 128 (0/246). Sie wird es erst, wenn das Tor mit BK 136 gezeichnet wird
(48/246 Ecken bis 152) - dann gehoert sie mit behoben (3.x).

Leiter-Vermutung "Textur traegt Raumlicht, Tuerlicht dunkelt ein zweites Mal ab": **bestaetigt**
(Anteil 1,753), dazu ein zweiter, nicht vermuteter Anteil (5-Bit-Abschneiden, 1,094).

RE2-Mechanismus fuer hellere Modelle: nur Flag 0x1000 (BK 136). Belegt in genau einem der
55 RE2-Archive: **DOOR2B.DO2 @Datei 0x5022** `4d 00 00 00 01 00 80 1a ...` (Blatt, Skript 1,
flags 0x1a80). Weitere Wege gibt es nicht: der Member_set-Setter (0x80055cb0, Sprungtabelle
@0x80011228, 44 Felder) schreibt obj+124 (RGBC) mit keinem Feld (selbst disassembliert:
Felder 0..43 = +0,+2,+4..+8,+270,+10,+11,+16,+56,+60,+64,+116..+120,+262,+340,+450..+474,
+324,+326,+328,+334,+148..+158,+478,+280,+282,+536,+538,+467); Flag 0x100 pulsiert RGBC 0..255.

## 3. Korrektur (Commit 52ea9ba1)

Ziel: das Tor an DENSELBEN Stellen so hell wie das gemalte Tor (Cut 12), RE2-Tueren unveraendert
oder begruendet naeher an RE2.

### 3.1 RE2-Mechanismus: Flag 0x1000 (BK 136)

`tor_sequenz_bauen.py` (d): Fluegel `0x0a80 -> 0x1a80`, Pfosten `0x0280 -> 0x1280` in beiden
Aufbauskripten (4 Bytes im Archiv: Skript 1/2 je +7 und +0x1d). Das Torblatt traegt damit
byte-gleich das Flagwort des DOOR2B-Blatts (@Datei 0x5022). Der Generator prueft die DOOR2B-Bytes
als Anker. Schild-Eckfarbe 73 -> **107** (2.2). Allein: gezeigt/gemalt 0,52 -> 0,76 (noch 1,31fach
zu dunkel), also reicht der RE2-Mechanismus nicht.

### 3.2 PORT-WAHL im Generator: Textur fuers Tuerlicht

`tor_sequenz_bauen.textur_fuers_tuerlicht`: je Kanal (1) 5 Bit GERUNDET `v5 = (p+4)>>3` statt
abgeschnitten, (2) `v5' = round(v5 * 128 / c_schild)`, c_schild = 107 aus den RE2-Konstanten im
Generator nachgerechnet (`ncct_eckfarbe`, bricht ab, wenn nicht 107/73). Herleitung: F = T * c/128
(psx-spx GPU:1438-1446), Ziel F = P -> T = P * 128/c. Schritt (2) auf der 5-Bit-Farbe haelt die
Palette bei 228 Farben (direkte 8-Bit-Skalierung gaebe 297 > 255 CLUT-Plaetze -> Median-Cut).
0 Texel gekappt (hoechster Kanal vorher 152). Gemessen Schild gezeigt/gemalt: zweistufig 1,008,
direkt+Median-Cut 0,997.

Warum nicht nur die Textur (BK 68, Faktor 1,75)? Dann waere jeder helle Texel ueber 145 gekappt
(Tafel), und die Abweichung vom RE2-Rechenweg groesser; mit BK 136 bleibt der Port-Anteil 1,196.

### 3.3 Renderer: Eckfarben ueber 0x80 (door_scene_pc.c `tri_psx`)

Mit BK 136 liegen 48 von 246 Ecken des Fluegels ueber 128 (bis 152). Die GPU hellt dort auf
(Texel * c / 128, Saettigung 1Fh; psx-spx GPU:349-354, 1438-1446; Befehl 0x34 = Modulation,
@0x80014744 `ori v0,v0,0x34`); die SDL-Modulation reicht nur bis 1,0
(`render_pc.c psx_prim_to_sdl_vert` kappt bei 0xFF). `tri_psx` zeichnet dasselbe Dreieck, NUR
wenn eine Ecke ueber 0x80 liegt, ein zweites Mal mit max(c-128,0) im ADD-Modus (Mischart 1) und
demselben z: interp(min(c,128)) + interp(max(c-128,0)) = interp(c), Saettigung durch die
ADD-Mischung bei 255. Stabile Sortierung (end_frame, strikt <) haelt die Zusatzlage direkt hinter
ihrem Dreieck. Aenderung in `door_scene_pc.c`: ein Block (Helfer `ueber_80`/`tri_psx`) + EIN
Aufruf in `mesh_zeichnen` (Hinweis fuer r32_unterteilung: unterteilte Dreiecke ebenfalls ueber
`tri_psx` abgeben, sonst fehlt ihnen die Aufhellung).

## 4. Messung nachher

### 4.1 Tor (gleiche Stellen wie 1.3)

| | vorher P/F | nur Archiv (3.1+3.2) | **nachher** P/F | F/P |
|---|---|---|---|---|
| V0 Bild 100 gesamt | 1,917 | 1,002 | **1,000** | 0,9997 |
| V1 Bild 100 gesamt | 1,912 | 1,001 | **0,999** | 1,0013 |
| V0 Bild 100 Schild | 1,921 | 0,992 | 0,992 | 1,0085 |
| V0 Bild 100 Rohr | 1,911 | 1,018 | 1,014 | 0,9865 |
| V0 Bild 100 Pfosten | 1,910 | 1,002 | 1,002 | 0,998 |
| V0 Bild 170 (Schwenk) gesamt | - | 0,978 | 0,977 | 1,0233 |
| V1 Bild 170 (Schwenk) gesamt | - | 0,979 | 0,977 | 1,0234 |

Bildpunkte mit Eckfarbe > 0x80 (Tor, Bild 100): V0 488, V1 480. Nur Archiv: F/T 1,000 (gekappt),
Abweichung zum PSX-Modell 8,63 / 11,38 Stufen; nachher F/T 1,0943 / 1,1200 gegen PSX-Modell 1,0936 /
1,1197, Abweichung **0,18 / 0,20**. Modell gegen Bild ueberall 0,19..0,26 Stufen.

Waehrend des Schwenks (Bild 170) ist das Schild um 3,5 % heller als gemalt: die Schild-Normale
dreht zum Licht (c 110 statt 107) - das ist das RE2-Licht, nicht die Textur.

### 4.2 RE2-Tueren vorher/nachher (gleiche exe-Pruefhaken, ganze Serie)

| | Bilder mit Unterschied | Bildpunkte | Richtung | max |
|---|---|---|---|---|
| S315 DOOR2E V0 (324 Bilder) | 193 | 45 773 | nur heller | +7 Stufen |
| S017 DOOR13 V0 (319 Bilder) | 98 | 4 460 | nur heller | +4 Stufen |

Geaendert sind genau die Ecken ueber 0x80 (BK 68: DOOR2E bis 137). An diesen Punkten folgte das
Bild vorher der PC-Kappung (DOOR2E Bild 131/139: Abweichung zum PC-Modell 0,02/0,01, zum
PSX-Modell 0,46/0,43), nachher dem PSX-Modell (0,08 / 0,08). Die RE2-Tueren kommen also **naeher
an RE2**, sonst bleibt jeder Bildpunkt gleich. Bild: `tor_belege/re2_door2e_bild139_vorher_nachher.jpg`.

### 4.3 Belegbilder (`analysis/befunde_runde32/tor_belege/`, angesehen)

- `v1_bild100_vorher_nachher.jpg`: Cut 12 gemalt (4x) | Sequenz V1 Bild 100 vorher | nachher -
  nachher Tafel, Streifen und Rohre in der Helligkeit des gemalten Tors, keine Kappung sichtbar.
- `v0_bild100_vorher_nachher.jpg`, `v1_bild170_vorher_nachher.jpg` (Schwenk).
- `re2_door2e_bild139_vorher_nachher.jpg`: DOOR2E vorher | nachher | Differenz x30 (schmale
  Oberkante des Rahmens).

## 5. Riegel

`re15_port/tests/unit/probes/r32_tor.cmake`, Quelle `tests/unit/probe_r32_tor.c`,
`tests/integration/test_r32_tor_hell.cmake`:

- `unit_r32_tor_hell` (Engine): A Maschine setzt Fluegel/Pfosten mit 0x1a80/0x1280 auf (V0/V1,
  Bild 100); B DOOR2B @0x5022 = `4d 00 00 00 01 00 80 1a`, Torblatt-Flags gleich; C Schild-Eckfarbe
  107 mit dem Port-Lichtcode (Gegenprobe BK 68 -> 73); D Schild-Texel x 107/128 / gemalt 33,41 =
  1,0085 (Band 0,97..1,03).
- `integration_r32_tor_hell` (echte exe, beschleunigter Renderer, Massstab 1, RE15_TUER_TEST=1,
  RE15_TUER_BOGEN -> Bild 20 = Bild 100): R1 Schild-Rechteck gezeigt/gemalt 1,0084 (Band 3 %);
  R2 Rohr mit c 148: F/Texel 1,1549 (Band 1,10..1,20; PSX 1,156).
- **Gegenproben (gemessen):** ohne `tri_psx`-Zusatzlage -> R2 0,9998 ROT (R1 gruen); altes Archiv
  -> unit ROT in A/B/C/D (Eckfarbe 73, Texel 0,751), integration R1 0,5032 ROT, R2 0,714 ROT.
- `unit_door_seq`, `unit_tor_1170_anfrage` gruen (Bild-fuer-Bild-Referenz unveraendert: die
  Maschine rechnet mit Lage/Drehung, Flags und TIM gehen nicht ein; `tuerseq_referenz.py` erzeugt
  dieselbe Datei).

## 6. Offen

1. **Cut 0 / Cut 11 malen das Schild heller** (Y 125,6 / 50,9 gegen 33,7 in Cut 12). Die Textur
   stammt aus Cut 12 (07 §1, NACHBAU 4: beide Seiten tragen den Laufsteg-Druck), die Sequenz ist
   jetzt so hell wie DIESES gemalte Tor. Wer von der Landeplatz-Seite (V0) kommt, sah das Tor zuletzt
   im Lampenlicht von Cut 0/11 - eine eigene, hellere Landeplatz-Seite waere eine Modell-Aenderung
   (zweiter Texturbereich aus Cut 0/11, dort nur 235/476 Bildpunkte Aufloesung). Nicht gemacht.
2. **Kappung ueber 0x80 in den uebrigen PC-Zeichenpfaden** (Figuren, Raum-Props ueber
   `re15_render_textured_tri_lit`) besteht weiter: behoben ist nur die Tuerszene. Fuer Figuren mit
   Raumlicht ist nicht gemessen, wie oft Eckfarben ueber 0x80 vorkommen.
3. 5-Bit-Rundung der GPU-Modulation und Dithering (08 Offen 3) nicht nachgebildet.

## 7. Suite

`bash re15_port/tools/local_build.sh all` auf 8b946a36: **`=== LOCAL-BUILD-OK (all) — Tests 413/413`**
(655 s). Drei vorherige Laeufe hatten je 1..5 GUI-Integrationstests rot (elza_vollstart,
r30_granate_laden, r30_irons_tisch_bild/licht, r30_sicherung_bild), alle mit exit=1 MITTEN im Lauf
ohne Absturzeintrag, waehrend zwei parallele Agenten (r32_hebetisch, r32_unterteilung) ihre Suiten
fuhren; jeder davon einzeln gruen (elza 100 s, granate 87 s). Keiner dieser Tests beruehrt die
Tuerszene.
