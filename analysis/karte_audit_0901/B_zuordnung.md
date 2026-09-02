# B — Welcher Raum gehoert auf welches Rechteck? (Seiten 2/3/4 = 1F/2F/3F)

Datum 2026-09-01. Alle Zahlen selbst gemessen; Sonden/Skripte im Scratchpad `.../scratchpad/mapB/`
(load.py, anchor.py, prop.py, fit.py, place.py, final.py, render.py). CSV: `B_zuordnung.csv`.

---

## 0. Der wichtigste Befund zuerst: RE1.5 hat KEINE Raum-Rechteck-Zuordnung

RE2 hat eine harte Tabelle (FUN_8006eae8). RE1.5 hat nichts dergleichen — und braucht auch nichts,
weil das Original die Rechtecke gar nicht einfaerbt.

Belegt, nicht vermutet:

1. Die Rechteck-Tabelle @`0x80076840` hat im ganzen Spiel genau ZWEI Leser
   (`grep 80076840/80076844 ghidra1_V2.txt`): `FUN_80046fd8` (Zeichnen) und `FUN_800473f8` (Marker).
   Keiner von beiden indiziert mit einer Raumnummer.
2. Die Zeichen-Schleife `FUN_80046fd8` @`0x800472dc`–`0x800473e0` laeuft `a3 = cnt` mal ueber die
   Tabelle (`addiu t3,t3,12` @0x800473bc / `addiu a0,a0,12` @0x800473c8) und schreibt je Rechteck
   zwei Sprite-Pakete. Die Farbe steht KONSTANT in den Registern (`ori t2,zero,0x4` @0x800472e8,
   `ori a1,zero,0x80` @0x800472f0) — kein einziges Byte Zustand wird je Rechteck gelesen.
   Im Original gibt es also weder "unbesucht" noch "besucht" noch "aktuelles Rechteck".
3. Raum→SEITE gibt es sehr wohl: Verteiler @`0x8004b568`, Sprungtabelle @`0x8001103c`
   (`sltiu v0,v1,0x26` @0x8004b574), Ergebnis `sb v0,DAT_800b260e` @`0x8004b88c`, dazu
   `sb v1,DAT_800b260d` @`0x8004b894` = der Raum-Index.
   STAGE1 byte-true: Raum 0..11 → Seite 2 (`ori v0,zero,0x2` @0x8004b684), 12..17 → 3
   (@0x8004b6f8), 18..22 → 4 (@0x8004b758), 23 → 5 (@0x8004b768), 24..29 → 0
   (`sb zero,DAT_800b260e` @0x8004b7dc), 30..37 → 1 (@0x8004b884).
   Das deckt sich EXAKT mit `page_of()` im Generator — diese Funktion ist byte-true bestaetigt.
4. Der Marker-Massstab wird mit dem RAUM-INDEX adressiert: `lbu a2,DAT_800b260d` @`0x80047424`,
   `sll a2,a2,3` @0x80047434, Basis `0x800768b0`.

Folge: jede Raum-Rechteck-Tabelle im Port ist eine Erfindung des Ports (aus RE2 uebernommen).
Sie kann nicht "byte-true falsch" sein — aber sie kann der ZEICHNUNG widersprechen, und genau
das tut sie heute auf Seite 2 flaechendeckend.

## 0b. Zweiter Strukturbefund: die Rechtecke sind KACHEL-FENSTER, nicht Raeume

Die Rechteck-Boxen ueberlappen einander stark und schneiden quer durch die gezeichneten Waende
(siehe seite2.png / seite3.png / seite4.png im Scratchpad).

Gemessen (Zaehlung ueber alle Schirm-Pixel je Seite):

| Seite | Pixel von >1 Rechteck ueberdeckt | davon UNTERSCHIEDLICHER Inhalt | davon mit Wand-/Tuerlinie (Index 4) | groesstes Konfliktpaar |
|---|---|---|---|---|
| 2 | 5440 | 5059 | 412 | (r3,r4) 1058 px |
| 3 | 5025 | 3125 | 857 | (r3,r4) 799 px |
| 4 | 2712 | 2284 | 361 | **(r4,r6) 641 px** |

Beispiel Seite 4: Schirmpunkt (161,147) — die gemalte Doppeltuer von ROOM1140 — liegt in ZWEI
Fenstern: aus r4 (uv(0,32), Schirm (144,80)) kaeme dort Kachelpixel uv(17,99), aus r6 (uv(32,32),
Schirm (160,110)) Kachelpixel uv(33,69). Welche Tuer man sieht, entscheidet allein die
ZEICHENREIHENFOLGE. Konkreter Kandidat fuer "in ROOM1140 ist die Eingangstuer 2 mal vorhanden,
die Tuer Richtung 3F-Ausgang fehlt" — passt zu [[reai-v2-zeichenreihenfolge-invers]].
NICHT entscheidbar ohne weiteren RE: ob das Original vorwaerts (Index 0..cnt-1, so laeuft die
Schleife) oder rueckwaerts rastert. Naechster Weg: `jal 0x8006b3d8` @0x80046fe4 (liefert `t4`,
den Tag jedes Pakets) und die OT-Verkettung im Aufrufer lesen.

---

## 1. Die Anker: das Original eicht 6 Raeume selbst (Frage 1a)

Tabelle @`0x800768b0`, 8 Byte je Raum-Index `{s16 ox, s16 oy, s16 sx, s16 sy}`.
13 von 38 Zeilen sind gefuellt, der Rest ist der Stub `{0,0,1,1}`. Auf den Seiten 2/3/4:

| Raum | Zeile | Adresse | ox,oy,sx,sy | Seite |
|---|---|---|---|---|
| ROOM1020 | 2 | `0x800768c0` | 100,136,2287,2287 | 2 |
| ROOM1030 | 3 | `0x800768c8` | 108,194,2428,2229 | 2 |
| ROOM1040 | 4 | `0x800768d0` | 94,193,2080,2320 | 2 |
| ROOM1070 | 7 | `0x800768e8` | 79,205,2229,2088 | 2 |
| ROOM1150 | 21 | `0x80076958` | 111,130,2296,2312 | 4 |
| (ROOM1170 | 23 | `0x80076968` | 100,206,2280,2268 | 5) |
| **Seite 3 (2F)** | — | — | **KEINE einzige geeichte Zeile** | 3 |

MASSSTAB: Median der 13 gefuellten Zeilen sx=2287, sy=2268 → 2^20/2287 = **458,5** bzw. 462,3
Welteinheiten je Kartenpixel (RE2 rechnet fest mit 450, FUN_8006e1f0). Ich habe durchgehend
S=2287 benutzt — das ist zugleich Zeile 2 (ROOM1020).

Gegenprobe der Anker untereinander (dieselbe Tuer von beiden Seiten mit der jeweils eigenen
Original-Zeile projiziert): 1020↔1040 2,2/2,0 px · 1030↔1040 4,5/4,2 px · 1030↔1070 5,4/8,9 px.
Die Original-Eichung ist also selbst nur auf ~2–9 px genau; mehr Genauigkeit darf man von keiner
Rekonstruktion verlangen.

Systematischer Versatz Kollisions-Bbox → Zeichnung, an den 5 byte-truen Ankern gemessen:
+5,7/+4,5 (1020), +3,0/+10,9 (1030), +2,4/+5,0 (1070), +5,2/0,0 (1150) px, also
**Bbox ≈ Zeichnung + rund 5 px** (Wandstaerke). Ausreisser ROOM1040 (+20,5/+17,1), dessen
Kollisions-Bbox weit ueber die gezeichnete Flurbreite hinausgeht.

## 2. Methode fuer den Rest: Tuer-Kohaerenz (Frage 1b)

Fuer jede Tuer gilt: der ORT der Tuer in Raum A und die ANKUNFT derselben Tuer in Raum B sind
derselbe Punkt auf der Karte. Bei festem Massstab bleibt je Raum nur `(ox,oy)` — plus die Frage,
ob der Raum gespiegelt gezeichnet ist.

Restfehler dieser Kette:

* **Seite 4:** alle 6 seiten-internen Tuerpaare ≤ 2,0 px (Median 1,0). Relativlage steht fest;
  ROOM1150 pinnt sie byte-true absolut.
* **Seite 3:** alle 11 Tuerpaare ≤ 2,2 px (7 davon exakt 0,0). Relativlage exakt, Absolutlage offen.
* **Seite 2:** 17 von 25 Paaren ≤ 6,3 px, aber 1000↔1050 (15,8/16,8/31,3/32,3 px) und
  1010↔1020 (23,1/24,0/24,1/24,2 px) brechen aus. Ursache messbar: zwischen 1010 und 1020 haben
  die beiden gemeinsamen Tueren in z den gleichen ABSTAND (10850 gegen 11200 Welteinheiten =
  23,7 gegen 24,4 px), aber das UMGEKEHRTE VORZEICHEN — ROOM1010 ist gegen ROOM1020 gespiegelt
  gezeichnet. Deshalb rechne ich je Raum vier Spiegel-Varianten durch. Bemerkenswert: die
  Original-Formel kann eine Spiegelung nur ueber ein negatives sx/sy ausdruecken — und GENAU die
  Raeume, bei denen ich eine Spiegelung messe, tragen im Original den Stub `{0,0,1,1}`.

Kreuz-Validierung der Methode: der Versatz zwischen Seite 2 und Seite 4 laesst sich zweimal
unabhaengig messen und stimmt auf 2 px:

* ueber Rechteck-Positionen desselben Kachelfensters: uv(168,40) S2 r9 (109,134) → S4 r0 (127,137)
  = (+18,+3); uv(168,16) S2 r10 (119,134) → S4 r1 (136,137) = (+17,+3);
* ueber die projizierte Schacht-Tuer: Aufzug (115,145) → (131,146) = (+16,+2);
  Treppe (119,151) → (136,153) = (+17,+2).

## 3. Ergebnis Seite 4 (3F) — geschlossen

| Rect | Raum | IoU gg. Zeichnung | Vertrauen |
|---|---|---|---|
| r2 (120,80) 32x40 | **ROOM1150** | 0,85 | **belegt** (Zeile 21 @0x80076958) |
| r4 (144,80) 32x80 | **ROOM1130** | 0,94 | stark — 4 von 4 Tueren auf gemalten Symbolen (2,1/3,8/3,8/5,7 px) |
| r6 (160,110) 56x56 | **ROOM1140** | 0,78 | stark |
| r5 (120,119) 40x40 | **ROOM1120**, gespiegelt (−1,−1) | 0,87 | stark |
| r1 (136,137) 24x24 | **ROOM1060** Treppenhaus @3F | — | stark |
| r0 (127,137) 16x16 | **ROOM1080** Aufzug @3F | — | stark |
| r3 (152,89) 48x24 | **ROOM1170 Zone 1** @3F | — | stark |
| — | ROOM1160 | — | offen: vermutlich keine Zeichnung auf 3F |

Zu ROOM1120 (einziger Streitfall der Seite): ungespiegelt landet der Raumkoerper auf
px(147..189, 89..135) — trifft r3 nur in der oberen Haelfte, ist 25 px zu tief, und die
Treppentuer liegt dann 8,5 px, die Aufzugstuer 12 px von jedem gemalten Symbol entfernt.
Gespiegelt liegt der Koerper auf (116..158, 116..162) = IoU 0,87 auf r5, die Treppentuer 1,6 px
vom Symbol (137,152,W) und die Aufzugstuer mitten in r0. Groessenprobe massstabsfrei:
41,9x45,8 gegen r5 34x40 (Faktor 1,10/1,15) gegen r3 42x22 (1,00/**2,08**).

## 4. Ergebnis Seite 2 (1F)

| Rect | Raum | Vertrauen |
|---|---|---|
| r1 | **ROOM1020** | **belegt** (Zeile 2, IoU 0,93) |
| r4 | **ROOM1030** | **belegt** (Zeile 3, IoU 0,90) |
| r2 | **ROOM1040** | **belegt** (Zeile 4) |
| r3 | **ROOM1070** | **belegt** (Zeile 7, IoU 0,94) |
| r0 | **ROOM1050** | stark (29,2x87,6 gegen gezeichnet 27x87; einziges hohes schmales Rechteck) |
| r10 | **ROOM1060** Treppenhaus | stark (byte-true Treppentuer 2,0 px vom gemalten Symbol (120,149,W)) |
| r6 | **ROOM10A0** Treppenhaus | stark (IoU 0,71; 10A0 und 1060 sind masslich identisch, r6 und r10 auch) |
| r9 | **ROOM1080** Aufzug | stark (byte-true Aufzugstuer faellt IN r9) |
| r5 / r7 / r8 | ROOM1000 / ROOM1010 / ROOM1090 | **schwach — nicht entschieden** |
| — | ROOM10B0 (98,0x67,1 px) | offen: groesser als jedes Rechteck der Seite (groesstes gezeichnet 68x57) |

ROOM1010 ist auffaellig: seine Bbox px(158..182, 74..113) deckt r8 (164..179, 77..93) UND
r7 (164..179, 93..109) als Stapel — ein Raum ueber zwei Kachelfenster, passend zu §0b.

## 5. Ergebnis Seite 3 (2F) — NICHT ENTSCHEIDBAR (ausser r9)

Seite 3 hat keine geeichte Zeile. Ich habe versucht, sie ueber den Schacht-Anker zu schliessen:
Seite 2 und Seite 3 haben BUCHSTAEBLICH DIESELBE Rechteck-Tabelle — Index 0..9 identisch in
x/y/w/h UND uv, an zwei verschiedenen Zeigern (`0x8007636c` / `0x800763f0`); Seite 2 hat nur r10
zusaetzlich. Ergebnis: die Kette schliesst in sich (Streuung 0–1 px), aber KEIN IoU ueber 0,50
und KEINE projizierte Tuer naeher als 16 px an einem gemalten Symbol. Das ist zu schwach.
Ich sage das ausdruecklich, statt Zahlen zu erfinden.

Naechster Weg fuer Seite 3: (a) den Symbolkatalog nachziehen — er kennt fuer Seite 3 nur VIER
Symbole (Seite 2: 9, Seite 4: 6), waehrend die Kachel deutlich mehr Tuernischen zeigt; mit
vollstaendigem Katalog ist die Verschiebung ueberbestimmt. (b) Savestates der 2F-Raeume ziehen —
mit der Einschraenkung, dass die Stub-Zeile `{0,0,1,1}` im Original (0,0) liefert, das Original
auf 2F also gar keinen brauchbaren Marker zeigt.

## 6. Frage 3: Seite 3 Rect 9 (16x16, uv(168,40)) — BEIDE bisherigen Antworten sind falsch

Der Port sagt seit heute ROOM1060 (Treppenhaus), vorher ROOM1110 Zone 1. Gemessen:

* Die Kachel uv(168,40) ist auf Seite 2 (r9), Seite 3 (r9) und Seite 4 (r0) pixelgleich
  (0 von 256 Pixeln Unterschied) und zeichnet ein schlichtes 10x10-Kaestchen OHNE Tuersymbol.
* Die zweite wiederkehrende Kachel uv(168,16) (Seite 2 r10, Seite 4 r1) zeichnet 18x22 und HAT
  ein Tuersymbol.
* Groesse bei festem Massstab: ROOM1060 = 24,2x27,8 px (abzueglich Wandzuschlag 19x23) → passt
  auf 18x22, NICHT auf 10x10. ROOM1080 Zone 1 = 13,5x13,3 px (→ 8,5x8,3) → passt auf 10x10.
* Positionsprobe mit dem byte-truen ROOM1040: dessen Aufzugstuer projiziert auf px(115,145) —
  INNERHALB von r9 (109..125, 134..150). Dessen Treppentuer projiziert auf px(120,151) und liegt
  2,0 px vom GEMALTEN Symbol (120,149,W) auf r10.
* Etagen-Argument: ROOM1080 wird von ROOM1040 (1F), ROOM10C0 (2F) und ROOM1120 (3F) betreten,
  ALLE DREI am identischen Ankunftspunkt (−13650,−900) — ein Schacht ueber genau 1F/2F/3F. Und
  genau dort kommt uv(168,40) vor: Seiten 2, 3, 4. ROOM1060 spannt zwar ebenfalls 1F/2F/3F, aber
  seine Kachel uv(168,16) existiert nur auf den Seiten 2 und 4 — die Seite-3-Tabelle ist eine
  KOPIE der Seite-2-Tabelle, bei der genau der letzte Eintrag (r10) fehlt.

**→ Seite 3 Rect 9 = ROOM1080 (Aufzug), 2F. Vertrauen: stark.**

Zu ROOM1110 Zone 1: sie ist ein SOLIDER BLOCK INNERHALB von ROOM1110 Zone 0 (Bbox
400..9650 x −950..1050 liegt vollstaendig in −15850..14950 x −7650..7600), also dieselbe Klasse
wie ROOM1140 Zone 1. Das Weglassen war richtig — aber die Begruendung im Generator
("das Rechteck gehoert dem Treppenhaus") stimmt nicht.

## 7. Frage 4: Raeume ohne Rechteck, obwohl eines frei ist

**Seite 3 — ja, eindeutig.** Frei sind heute r0 (gezeichnet 32x86, das GROESSTE der Seite),
r2 (20x42), r7 (10x19), r9 (10x10). Ohne Rechteck sind:

* **ROOM10D0 Zone 0** — mit 70,1x90,2 px die GROESSTE Flaeche der ganzen Etage;
* **ROOM1110 Zone 0** — 67,2x33,3 px;
* das Treppenhaus ROOM1060 @2F und der Aufzug ROOM1080 @2F.

Gleichzeitig sind zwei der sechs vergebenen Seite-3-Rechtecke an SCHABLONEN-Zonen vergeben (§8).

**Seite 4 — ja.** Frei sind r0 und r3 (r3 nur indirekt ueber `s_map_floors {0x1170,1,0,4,3}`).
Ohne Rechteck: ROOM1060 @3F, ROOM1080 @3F, ROOM1170 Zone 1 als eigenstaendiger Ort.

**Seite 2 — anders gelagert:** dort sind formal alle 11 Rechtecke belegt, aber r9 traegt DREI
Zonen gleichzeitig (ROOM1060.z0, ROOM1080.z1, ROOM10A0.z0) und r7 beide ROOM1000-Zonen. Ohne
Rechteck sind ROOM1010, **ROOM1030**, **ROOM1040** und ROOM1050 — darunter zwei Raeume, die das
Spiel SELBST eicht (Zeilen 3 und 4).

## 8. Zusaetzlicher Defekt: SCHABLONEN-Zonen belegen echte Rechtecke

Die Bbox `(−31700..−7750, 14150..31150)` kommt in VIER verschiedenen STAGE1-Raeumen
buchstabengleich vor: ROOM1080.z0, ROOM1100.z0, ROOM11E0.z1, ROOM11F0.z0. Ein Ort ist das nicht,
das ist ein Vorlage-Kollisionsblock. ROOM10D0.z1 `(−31700..−7750, −31650..−14650)` hat denselben
x-Bereich und dieselben Masse — dieselbe Vorlage, nur in z verschoben.

Im Port belegen genau diese Zonen echte Rechtecke:
`ROOM1080.z0 → Seite 2 r8`, `ROOM1100.z0 → Seite 3 r5`, `ROOM10D0.z1 → Seite 3 r3`
(ausserhalb meines Auftrags zusaetzlich `ROOM11E0.z1 → Seite 1 r2`, `ROOM11F0.z0 → Seite 1 r9`).
Diese Rechtecke leuchten auf, sobald der Spieler den jeweiligen Raum betritt — an voellig
falscher Stelle.

## 9. Frage 2: Abweichung gegen `re15_map_zones.h` (Stand heute)

### Seite 2 — ALLE 11 Rechtecke abweichend, 4 davon gegen die eigene Eichung des Spiels

| Rect | Port heute | Gemessen | Begruendung |
|---|---|---|---|
| r0 | ROOM1070.z1 | **ROOM1050** | 1070 ist byte-true r3 (Zeile 7); z1 ist ein Block IN z0 |
| r1 | ROOM1090.z0 | **ROOM1020** | Zeile 2 @0x800768c0, IoU 0,93 |
| r2 | ROOM1070.z2 | **ROOM1040** | Zeile 4 @0x800768d0 |
| r3 | ROOM1020.z0 | **ROOM1070** | Zeile 7 @0x800768e8, IoU 0,94 — 1020 gehoert auf r1 |
| r4 | ROOM10B0.z0 | **ROOM1030** | Zeile 3 @0x800768c8, IoU 0,90; 10B0 ist mit 98x67 px zu gross fuer jedes Rechteck der Seite |
| r5 | ROOM1020.z2 | offen (1090/1000) | 1020.z2 ist ein Block IN 1020.z0 |
| r6 | ROOM1070.z0 | **ROOM10A0** | 1070 ist byte-true r3 |
| r7 | ROOM1000.z0 + z1 | **ROOM1010** (untere Haelfte) | zwei Zonen auf einem Rechteck |
| r8 | ROOM1080.z0 | **ROOM1010** (obere Haelfte) | 1080.z0 ist eine SCHABLONE (§8) |
| r9 | 1060.z0 + 1080.z1 + 10A0.z0 | **ROOM1080 allein** | drei Orte auf einem 16x16-Rechteck |
| r10 | ROOM1020.z1 | **ROOM1060** | 1020.z1 ist ein Block IN 1020.z0 |

Gemeinsamer Nenner: die Kostenheuristik hat ROOM1070s Moebelbloecke (z1, z2) und ROOM1020s
Moebelbloecke (z1, z2) wie eigenstaendige Orte behandelt und damit vier echte Rechtecke
verbraucht — waehrend die vier Raeume, die das Spiel selbst eicht, leer ausgehen.

### Seite 3 — 2 von 6 Zuordnungen sind Schablonen, r9 ist neu falsch

| Rect | Port heute | Gemessen |
|---|---|---|
| r3 | ROOM10D0.z1 | **SCHABLONE, streichen** |
| r5 | ROOM1100.z0 | **SCHABLONE, streichen** |
| r9 | heute frei (Generator hat 1110.z1 entfernt, Kommentar nennt ROOM1060) | **ROOM1080 Aufzug** |
| r1, r4, r6, r8 | 10F0.z0, 10E0.z0, 1100.z1, 10C0.z0 | **nicht entscheidbar** (§5) — weder bestaetigt noch widerlegt |
| r0, r2, r7 | frei | offen; ROOM10D0.z0 (70x90) und ROOM1110.z0 (67x33) haben gar kein Rechteck |

### Seite 4 — 4 von 7 stimmen

| Rect | Port heute | Gemessen |
|---|---|---|
| r2 | ROOM1150 | **ROOM1150** ✔ byte-true bestaetigt |
| r4 | ROOM1130 | **ROOM1130** ✔ — die Hand-Korrektur `ZONE_FIX {(0x1130,0):(4,4)}` ist unabhaengig bestaetigt |
| r5 | ROOM1120 | **ROOM1120** ✔ — aber die Projektion muss GESPIEGELT sein, sonst liegt der Marker 25 px zu tief |
| r6 | ROOM1140 | **ROOM1140** ✔ |
| r1 | ROOM1160 | **ROOM1060** (Treppenhaus) ✘ |
| r0 | keine Zone → wird als "unbekannt" GRAU gemalt | **ROOM1080** (Aufzug) ✘ |
| r3 | keine Zone, nur `s_map_floors` | **ROOM1170.z1** — die Generator-Begruendung ist richtig, die Umsetzung nur ueber die Etagen-Tabelle aber unvollstaendig |

Direkt zum Nutzer-Report "in ROOM1130 gibt es unten links schon ein Rechteck nach dem Flur":
r0 (127,137) 16x16 liegt genau unten links neben dem Flur-Rechteck r4 und hat KEINE Zone —
nach [[reai-v2-zeichenreihenfolge-invers]] wird ein Rechteck ohne Zone GRAU gezeichnet statt
weggelassen, ist also von der ersten Sekunde an sichtbar. Es gehoert dem Aufzug ROOM1080, den
man von 3F nur ueber ROOM1120 erreicht.

Zum Report "in ROOM1140 ist die Eingangstuer 2 mal vorhanden": zwei unabhaengige Kandidaten,
beide gemessen, keiner bisher entschieden —
(a) die 641 konfliktbehafteten Ueberlappungspixel von r4 und r6 (§0b), darunter die Doppeltuer
bei (161,147); und
(b) ROOM1140s RDT enthaelt den Tuer-Datensatz nach ROOM1170 ZWEIMAL identisch (beide
`ort(−7250,−250) → ank(−21937,−25713)`), waehrend die Tuer nach ROOM1130 nur einmal steht.

---

## 10. Was ich NICHT entscheiden konnte

1. **Seite 3 komplett** (ausser r9). Grund: keine geeichte Zeile, unvollstaendiger Symbolkatalog.
   Naechster Weg: Symbolkatalog fuer Seite 3 nachziehen (nur 4 Symbole erfasst), dann ist die
   Verschiebung ueberbestimmt.
2. **Seite 2 r5/r7/r8** (ROOM1000/1010/1090). ROOM1000 hat zwei Zonen mit widerspruechlichen
   Tuerlagen, ROOM1010 deckt zwei Rechtecke.
3. **ROOM10B0** (98x67 px) und **ROOM1160** (36,5x28,6 px) — beide ohne seiten-interne Tuer und
   ohne passendes freies Rechteck. Vermutung (NICHT belegt): auf dem Etagenblatt gar nicht
   gezeichnet.
4. **Die Zeichenreihenfolge der Kachelfenster** (§0b). Naechster Weg: `jal 0x8006b3d8` @0x80046fe4
   und die OT-Verkettung im Aufrufer disassemblieren.
