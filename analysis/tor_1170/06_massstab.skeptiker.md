# Skeptiker-Pruefung zu Dossier 06 (Massstab Raumwelt - Tuerszene)

Stand 2026-09-28. Nur gelesen und gemessen. Nichts am Port geaendert, nichts gebaut, kein git.
Binaerdateien: `info/Re1.5/PSX.EXE` (t_size 0xaf000 = Auslieferungsstand), `info/re2leon/PSX.EXE`,
RDT aus `info/Re1.5/PSX/STAGE*/` und `info/re2leon/PL0/RDT/`.

Eigene Werkzeuge unter `build/tor_1170/skeptiker_massstab/` (vom Werkzeug des Untersuchers
unabhaengig; aus `massstab.json` werden nur Raum, Cut, Satz-Offset und die behaupteten Ecken gelesen):

| Skript | Zweck |
|---|---|
| `sk_messen.py` | eigene Kamera aus RDT-Bytes, Rueckprojektion ohne Ausgleich |
| `sk_kante.py`, `sk_merkmal.py` | eigene Kantenverfeinerung, Merkmals-Schwerpunkte |
| `sk_zweistrahl.py`, `sk_5050_lamellen.py`, `sk_mehrsicht.py` | Triangulation aus zwei/drei Cuts, OHNE Bodenanker |
| `sk_zweitsicht.py` | Regression H gegen 1/Kamerahoehe, Suche nach Zweitsichten |
| `sk_sca.py`, `sk_linien.py`, `sk_bodenlinie.py`, `sk_unten.py` | Kollisionszellen und Weltlinien im Bild, Unterkanten |
| `sk_robust.py` | Blattrichtung auf Achse, ganzzahlige Blickmatrix |
| `sk_pld_hoehe.py`, `sk_bodenzensus.py` | Spielermodell, Tuersatz-Zensus per Mustersuche |

## Ergebnis in einem Satz

Die Rechnung des Untersuchers ist aus seinen Ecken exakt nachvollziehbar, aber der **Bodenanker
traegt nicht**: bei mindestens 2 der 11 gezaehlten Tueren liegt die Blattunterkante 116 bis 160
ueber der Bodenhoehe, die Blattebene liegt dort 8 % bzw. 15 % naeher an der Kamera, und die beiden
kleinsten k-Werte (1,753 und 1,780) sind Artefakte. Berichtigt: **k aus der Hoehe = 1,90**
(RE1.5 n=5: 1,899 +- 0,023), nicht 1,860.

## 1. Was haelt

| Aussage | Befund |
|---|---|
| Rechnung | Aus den Ecken des Untersuchers liefert `sk_messen.py` dieselben H und k (groesste Abweichung 0,002 in k). Statistik 1,860 / 0,068 / 0,021 / Median 1,890 nachgerechnet |
| H-FOV | `@0x80021e5c lw v1,36(v1)`, `@0x80021e60 sll v0,v0,5`, `@0x80021e68 lhu a0,2(v0)`, `@0x80021e6c jal 0x80066c30`, `@0x80021e70 srl a0,a0,7` (Delay-Slot). Ziel `0x80066c30` = `48c4d000` = COP2 CTC2 rt=a0 rd=26 (H), dann `jr ra`. RE2 `@0x8002c12c..40` gleich, Ziel `0x8008de24` = `48c4d000`. Zweiter RE1.5-Aufrufer `@0x80046124` mit derselben Formel, davor `@0x800460f4 ori a0,zero,0xa0` / `@0x800460fc ori a1,zero,0x78` / `jal 0x80066d60` = Bildmitte (160,120) |
| SPIELERHOEHE | EXE Datei 0x64694: `00 00 06 fa 00 00 c2 01 fa 05 c2 01`. Eigener PLD-Leser: RE1.5 Wurzel `@0xC68` `00 00 f4 f8 00 00`, Grundhaltung y -1215..1807 = 3022; RE2 `@0x5A4` `00 00 ee f8 00 00`, 3037. "Halbe Hoehe" ist Deutung, im Code nicht nachgelesen |
| GEGENPROBE, BEZUGSMASSE | Rechnung stimmt. Knauf-Satz `@0x504C` `4d 01 00 e2 01 01 d0 00 10 00 82 00 68 f3 d4 f2` = (130,-3224,-3372); Knauf-Mesh y -107..107 |
| TUERSZENE-KAMERA | `@0x5036` `... d0 07 ce 0e 00 08` = (2000,3790,2048); Kamera `@0x80010830` = `10 27 00 00` + Nullen, `@0x80013e34 addiu a0,zero,290` |
| STANDARDBLATT | 36 von 55, davon 30 mit hoechstens 2 Meshes, Ausnahmen 01 05 17 19 29 2C. Satzzahl per Mustersuche 530 (davon 364 Standard) statt 552 / 365 - anderes Verfahren, gleicher Anteil |
| BODEN | RE2 Stage 1-7: 299 Saetze, 296 passend, dieselben drei Abweichler. RE1.5: 307 von 307. Siehe N6 |
| HOEHE-ROBUST | eigene Rechnung: Richtung auf Achse aendert H um hoechstens 0,22 %, W bis 5,2 %; ganzzahlige Matrix hoechstens 0,09 % |

## 2. Was faellt

### 2.1 Triangulation ohne Bodenanker

Der Massstab kommt hier aus dem Abstand der Kameras (RDT-Bytes), nicht aus der Bodenhoehe.

| Tuer | Kamerahoehe | H Untersucher | H trianguliert | Blattebene Untersucher / trianguliert / SCA-Kante | Unterkante ueber Boden |
|---|---|---|---|---|---|
| RE2 ROOM5050 c3 | 1098 | 3710 | **3163** | x -10617 / -11559 / -11585 | 160 / 164 |
| RE2 ROOM5050 c0 | 2790 | 3348 | **3175** | x -11167 / -11559 / -11585 | 144 / 150 |
| RE1.5 ROOM2060 c0 | 1461 | 3767 | **3467** | x 8999 / 8500 / 8400 | 102 / 130 |
| RE2 ROOM3060 c0 | 3312 | 3492 | 3502 .. 3519 | z -26971 / -26963 / -26991 | -26 .. -9 |
| RE1.5 ROOM3090 c0 | 5058 | 3607 | 3595 | z 1750 / 1736 / 1618 | 17 |

- ROOM5050: Merkmal = Schwerpunkt der Lueftungslamellen, Cut 3 (106,41 143,86), Cut 0 (117,35 133,00),
  Cut 12 (110,65 155,80). Drei Paare: (-11674,-755,-25163) Fehlabstand 6, (-11482,-728,-25187) 40,
  (-11520,-709,-25190) 2. Die Ecken aus Cut 3 und Cut 0 ergeben auf dieser Ebene 3163 und 3175
  (0,4 % Unterschied). Die Tuer hat eine sichtbare Schwelle (Schott im Seilbahnwagen).
- ROOM2060: vier Merkmale (Blattecken oben, Schild, Warndreieck), Strahlwinkel 47 bis 57 Grad,
  Empfindlichkeit 0,3 bis 0,6 % je Pixel: x 8491, 8435, 8528, 8549. Abstand Kamera - Ebene 5979 statt
  6495 (Verhaeltnis 0,921). Im Bild liegt unter dem Blatt ein 4 bis 5 Pixel hoher Schwellenstreifen.
  Mit der triangulierten Ebene liegt das Blatt mittig auf dem Tuer-Rechteck (3994 gegen 3950).

### 2.2 Regression

H gegen 1/Kamerahoehe, 18 Tueren (ohne ROOM1130): Achsenabschnitt 3426 +- 65, Steigung
233385 +- 65563, r = 0,66. Je tiefer die Kamera, desto groesser das gemessene Blatt. Das ist das
Muster einer Unterkante, die im Mittel rund 68 ueber der Bodenhoehe liegt. k bei unendlich hoher
Kamera = 6602 / 3426 = 1,927.

### 2.3 Berichtigte Werte

| Groesse | Dossier | berichtigt |
|---|---|---|
| k Hoehe, 11 Tueren | 1,860 +- 0,068 | 1,902 +- 0,079, Median 1,904 |
| k Hoehe, RE1.5 (n=5) | 1,869 +- 0,069 | **1,899 +- 0,023** (1,864 .. 1,927) |
| k Hoehe, RE2 (n=6) | 1,853 +- 0,073 | 1,904 +- 0,110; ohne ROOM5050 1,868 +- 0,071 |
| Blatthoehe im Raum | 3553 +- 133 (3412 .. 3767) | 3476 +- 141 (3165 .. 3750); RE1.5 3476 +- 41 |
| Empfehlung | 1,86 +- 0,07 | **1,90 (+0,05 / -0,03)** |
| 500 / 1000 / 1500 / 2000 | 930 / 1860 / 2791 / 3721 | 950 / 1900 / 2850 / 3800 |
| Knauf, Scheitel, Hueftgelenk, Dicke | 1733 / 5621 / 3355 / 155 | 1697 / 5742 / 3428 / 152 |

Die Spanne des Dossiers (1,792 .. 1,928) enthaelt 1,90. Der "Fehler des Mittels 0,021" gilt aber
nicht, weil der Fehler gerichtet ist und nicht zufaellig.

## 3. Neue Befunde

- **N1 WIEDERHOLPROBE geklaert.** Beide Kameras passen zum Bild. Falsch ist der Bodenanker. Der Fehler
  einer einzelnen bodenverankerten Tuer erreicht 17 %, nicht 5 %.
- **N2 WANDEBENE umgekehrt.** In allen vier pruefbaren Tueren liegt die triangulierte Blattebene
  26 bis 120 hinter der naechsten SCA-Kante. Wo Untersucher und SCA um 599 und 968 auseinanderliegen
  (ROOM2060, ROOM5050), ist seine Ebene falsch. Die AOT-Kanten dort (+111, -49) stuetzen nichts: die
  wahre Ebene liegt 940 und 500 davon entfernt. Ausnahme Nische: ROOM60B0, SCA-Kante x -5950 liegt
  an der Vorderkante einer sichtbaren Stufe.
- **N3 Falsche Unterkanten-Richtung.** ROOM1000 c6: Unterkante im Dossier waagerecht (Steigung
  -0,005), die achsparallele Bodenlinie x = 1650 hat im Bild Steigung 0,093, der Helligkeitsanstieg
  des Bodens folgt ihr ((164,155) (176,156) (189,157) (201,158)). ROOM1010 c4: 0,063 gegen 0,199.
  ROOM1010 c0: -0,018 gegen -0,120. Die
  angepassten Richtungen 12 bis 15 Grad neben der Achse sind Artefakte. Gleiches bei RE2 ROOM2040:
  "schraeg 21 Grad" ist falsch, trianguliert liegen Blattecken und Knauf bei x 5258, 5224, 5273.
- **N4 Breiten.** ROOM2040 W 1458 .. 1483 statt 1697. ROOM2060 W 1598 statt 1716 (H/W 2,17, nicht
  Standardblatt). ROOM1010 c4: linke Kante 3,7 Pixel ausserhalb des Blatts (Fuge bei x 168..169,
  Dossier 166,1 .. 164,6).
- **N5 Tueren sind verschieden gross.** ROOM5050 3165 hoch und 1210 .. 1300 breit, uebrige rund 3450
  hoch. k ist je Tuer eine
  Vereinbarung (jede Tuer wird als Standardblatt gezeigt), kein einheitlicher Weltmassstab.
- **N6 Vier-Punkt-Tuersaetze.** Die zwei RE1.5-"Abweichler" im Bodenzensus sind ROOM4030 `@0x47E` und
  `@0x4A6` mit SAT 0xB1: nach dem 6-Byte-Kopf folgen vier Punkte (16 B). -26460 und -25220 sind
  z-Koordinaten, keine Zielhoehen.
- **N7 Griffhoehe.** RE1.5, Ebene = SCA-Kante + 50: Griff 1485 (ROOM1000 c6), 1471 (ROOM1010 c4),
  1516 (ROOM1010 c0) ueber dem Boden, Anteil an der Blatthoehe rund 0,43. Tuerszene 3224 / 6602 =
  0,488. k aus dem Griff waere 2,13 .. 2,19. Die Tuerszene ist keine gleichmaessige Vergroesserung;
  die gerechneten 1733 (Dossier) sind im Raum nicht gemessen.
- **N8 Oberkante ist robust.** Steht die Kamera auf Hoehe der Oberkante, haengt deren Hoehe nicht an
  der Ebene: ROOM1010 c4 (Kamera 3419) 3427, ROOM1000 c6 3485 .. 3558.
- **N9 RE2 zweiter H-Pfad.** `@0x80034840 srl v0,v1,7`, `@0x80034844 sltiu v0,v0,209`,
  `@0x8003484c addiu v0,v1,-128`, `@0x80034850 sh v0,2(s3)`: ab H 209 wird fov um 128 gesenkt. Nicht
  im Cut-Setzer `0x8002c13c`. Wirkung hoechstens 1 in H.
- **N10 Schwelle 1000 zu niedrig.** Mit einer Unterkante 100 bis 160 ueber dem Boden macht eine
  Kamera bei 1461 noch 8 % Fehler. ROOM3090 (ausgeschlossen wegen Restfehler) ist dagegen richtig.

## 4. Offen

- ROOM40A0 (k 1,760, Guete B): keine Zweitsicht, Wert ungeprueft.
- ROOM60B0, ROOM20E0, ROOM1000 c3, ROOM1010 c0: keine Zweitsicht; Hoehe der Unterkante unbekannt.
- ROOM1130 (Zarge): Triangulation uneinheitlich (Ebene -6910 .. -6370), kein Ergebnis.
- Das Tor in ROOM1170 ist weiterhin nicht vermessen. Cut 0 und Cut 11 zeigen es beide: dort
  triangulieren statt am Boden verankern.

## 5. Nachmessen

```
S=build/tor_1170/skeptiker_massstab
python $S/sk_messen.py ecken
python $S/sk_zweitsicht.py regress
python $S/sk_5050_lamellen.py
python $S/sk_zweistrahl.py re15 ROOM2060 0 0 1 "TL:142.06,66.51:265.9,58.0" "TR:187.87,58.98:275.9,53.8" \
   "Schild:165.0,87.0:270.0,76.0" "Dreieck:164.5,102.5:269.0,90.0" \
   "E:142.06,66.51;187.87,58.98;189.29,176.95;142.02,172.80"
python $S/sk_zweistrahl.py re2 ROOM3060 3 0 1 "TL:121.05,42.32:166.15,77.25" "TR:165.86,41.38:195.53,76.97" \
   "BR:164.77,146.08:193.64,146.82" "BL:126.74,140.64:166.10,143.66" \
   "E:121.05,42.32;165.86,41.38;164.77,146.08;126.74,140.64"
python $S/sk_robust.py ; python $S/sk_bodenzensus.py ; python $S/sk_pld_hoehe.py info/Re1.5/PSX/PLD/PL00.PLD
```
