# B — An welcher Wand sitzt jede Tuer wirklich?

Stand 2026-08-31. Alles hier ist gemessen; jede Konstante traegt ihre `@0x…` bzw. ihren
Datei-Byte-Offset. Nichts committet, keine Produktivdatei geaendert.

Ergebnisdateien:

| Datei | Inhalt |
|---|---|
| `analysis/kartensymbole/tueren_wandlage.csv` / `.json` | 327 Tuer-Datensaetze, alle Rohfelder + Wandachse aus 3 Quellen + gesnappte Kartenposition |
| `analysis/kartensymbole/B_room1130_alt_neu.png` | ROOM1130-Kachel, links HEUTE, rechts NEU (Pixel-Gegenprobe) |
| `analysis/kartensymbole/B_weitere_kacheln_alt_neu.png` | 5 weitere Kacheln (S4r2, S4r6, S5r1, S5r0, S0r4) alt/neu |

Sonden (nicht committet, Scratchpad
`…/25fdb723-6925-4eae-ab46-1dd9a6a8a4c0/scratchpad/`): `collect.py` (RDT -> rooms.json),
`ray.py` (Yaw-Konventions-Messung), `analyse.py` (3 Quellen + heutige Marke),
`snap.py` (neue Projektion + Wand-Snap), `final.py` (Endtabelle), `draw.py`/`maptex.py` (Bilder).

---

## 0. Der Datensatz — byte-true nachgeprueft

`Door_aot_set` (SCD-Opcode `0x3B`) legt im VM-Handler **nur einen Zeiger** ab:
`@0x800405d8 sw v0,0(v1)` mit `v1 = 0x800ac9b0 + slot*4` und `v0 = pc+2`
(`@0x80040608 addiu v0,v0,2`). Der **Record beginnt also bei `pc+2`**.
PC-Vorschub konditional: `@0x80040618 lbu v0,3(v1)` / `@0x80040620 andi 0x80` →
`@0x80040630 +40` sonst `@0x80040634 +32`. (Mein Parser macht das mit; `gen_map_zones.py`
nimmt pauschal 32 — bei STAGE1 folgenlos, `pc[3]=0x31`, Bit klar.)

Feld-Layout, **nicht angenommen sondern aus den Konsumenten gelesen**:

* Trefftest `FUN_80042b64`: `@0x80042b68 lh v0,0(a1)` / `@0x80042b70 lhu a0,4(a1)` /
  `@0x80042b74 subu` / `@0x80042b78 sltu` — Treffer gdw. `(u32)(px-x0) <= w`, in z ebenso
  `@0x80042b84-98`. **ECKE + AUSDEHNUNG**, bestaetigt. Aufruf `@0x80042ecc jal 0x80042b64`
  mit `a1 = sp+44`, gefuellt aus `record+4/6/8/10` (`@0x80042e60-9c`)
  → SCD `pc+6/8/10/12` = `rect x / z / w / d`.
* Tuer-Handler `sce 2` = `@0x800430bc` (Tabelle `@0x800746a4`): legt `a0 = record+12`
  nach `DAT_800ac9a8` (`@0x800430c4`).
* Verbraucher `@0x8001d874`: `lh 0(a0) → playerX`, `lh 2(a0) → playerY`,
  `lh 4(a0) → playerZ`, **`lhu 6(a0) → DAT_800acabe = Spieler-Yaw`** (`@0x8001d8dc-e8`),
  `lbu 11(a0) → player+0x82 = BAND` (`@0x8001d8ec-f8`).
  → SCD `pc+14/16/18 = next x/y/z`, **`pc+20 = ncdir_y`**, `pc+25 = Ziel-Band`.

Gegenprobe an echten Bytes, `ROOM1130.RDT` Datei-Offset `0x08EE`:
`3b 03 02 31 00 00 | a4 06 46 37 d0 07 e8 03 | 17 ad 00 00 4a b6 | 00 04 | 00 17 | 08 00`
= slot 3, sce 2, rect (1700, 14150) 2000×1000, next (−21225, 0, −18870),
**ncdir 1024**, Stage 0 / Raum 0x17 (= ROOM1170), cut 8.

⚠️ **`ncdir` gehoert zur ZIEL-Seite.** Es ist der Kurs, den der Spieler *nach* dem Durchgang
im ZIELRAUM hat. Fuer die Wand einer Tuer in Raum R braucht man also das `ncdir` des
**Partner-Datensatzes** (die Tuer im Nachbarraum, die zurueck nach R fuehrt). Paarung ueber
`|Partner.next − unsere Rect-Mitte|`: 305 von 327 Datensaetzen finden einen Partner,
Median-Abstand **900** Einheiten, p90 1701.

---

## 1. Die Yaw-Konvention — gemessen, nicht angenommen

`FUN_8004f008` (der Vorwaerts-Punkt des AOT-Scans) baut aus dem Yaw eine Y-Rotationsmatrix
(`@0x8004f01c sh a0,18(sp)` = Index 1 = Y; `@0x8004f038 jal 0x80068098` = RotMatrix;
`@0x8004f048 jal 0x800661c0` = ApplyMatrix) und dreht damit `(620,0,0)`
(`@0x80042bd0 ori v0,0x26c`, `@0x80042bf4 sh zero,20(sp)`), Ergebnis addiert auf
`entity+0x34`/`+0x3C` (`@0x80042c10`/`@0x80042c34`).

Statt der Matrix-Handedness zu vertrauen, **gemessen**: von jedem Ankunftspunkt
`(next_x,next_z)` im Zielraum in alle 4 Achsenrichtungen bis zur ersten soliden SCA-Zelle
strahlen (Schritt 50, max 9000) und schauen, wo relativ zum Ankunfts-Yaw die naechste Wand
liegt. 276 Ankuenfte mit achsenparallelem Yaw (Abweichung ≤ 64) und Wand ≤ 4000:

|  ncdir | Wand +X | Wand −Z | Wand −X | Wand +Z |
|---:|---:|---:|---:|---:|
| 0    | 10 | 4  | **69** | 4  |
| 1024 | 19 | 0  | 2  | **33** |
| 2048 | **77** | 2  | 1  | 1  |
| 3072 | 14 | **37** | 0  | 3  |

Die Wand liegt also **hinter** dem Spieler, wenn `vorwaerts = (cos(yaw), −sin(yaw))`:
216 / 276 Treffer auf dieser Diagonale. Die drei um 90°/180°/270° gedrehten Konventionen
kommen auf 25 / 14 / 21. **Damit ist belegt:**

```
yaw    0 → +X      yaw 1024 → −Z      yaw 2048 → −X      yaw 3072 → +Z
```

Und daraus: **Yaw 0/2048 ⇒ Spieler laeuft laengs X ⇒ die Wand VERLAEUFT laengs Z.**
Yaw 1024/3072 ⇒ Wand laengs X.

Kartenrichtung: `FUN_800473f8` rechnet
`mx = ox + ((playerX+32000)*sx*10 >> 20 + 5)/10` (`@0x8004741c-5c`, `@0x800474e8`) und
`my = oy − (((playerZ+32000)*sy*10 >> 20) + 5)/10` — die **Negation `@0x800474b0
subu v1,zero,v1`**. Also **+X = rechts, +Z = OBEN**.

---

## 2. Die drei Quellen und wer gewinnt

### (a) Das Tuer-Rechteck: die LANGE Achse laeuft LAENGS der Wand

Das war die eine Stelle, die die Aufgabe ausdruecklich belegt sehen wollte statt angenommen.
Beleg gegen die **Kollisionsgeometrie**, ohne Yaw:
Ankunftspunkt, 4 Strahlen; nur Faelle mit **eindeutiger** Wand (naechste ≤ 1500 und
mindestens 2× naeher als die zweitnaechste):

* **Rect-Langachse == SCA-Wandachse: 153 / 154 = 99,4 %**
  (Z/Z 101, X/X 52, ein einziger Ausreisser Z/X)
* Ankunfts-Yaw == SCA-Wandachse: **130 / 130 = 100,0 %**

Und ueber alle 237 Tueren, bei denen alle drei Quellen etwas sagen:

| Vergleich | Uebereinstimmung |
|---|---|
| Rect == Yaw | 233 / 237 = 98,3 % |
| SCA  == Yaw | 214 / 237 = 90,3 % |
| Rect == SCA | 214 / 237 = 90,3 % |
| alle drei   | 212 / 237 = 89,5 % |

**Regel:** `|rd| > |rw|` ⇒ Wand laengs **Z** (senkrecht auf der Karte);
`|rw| > |rd|` ⇒ Wand laengs **X** (waagerecht). Bei `rw == rd` (18 Datensaetze) sagt das
Rechteck nichts.

### (b) SCA-Kollisionszellen

Die SCA-Zellen sind **solide Hindernisse**, nicht Boden: `push_rect`/`push_circle` schieben
den Spieler heraus (`re15_collision.c:722-732`, Original `LAB_8003d00c` u.a.). Eine Tuer
sitzt daher in der Luecke einer soliden Zellenkette; die Zelle **neben** der Tuer ist die
Wand. Der 4-Strahlen-Test am Ankunftspunkt ist die direkte Messung dafuer.

**Warum (b) trotzdem nur die Nummer 2 ist:** in 19 Faellen sagt (b) `Z`, wo (a) und (c)
`X` sagen — der Strahl trifft dort ein Moebel/eine Saeule seitlich statt der Wand. Mit dem
strengen Eindeutigkeits-Filter verschwindet der Fehler (99,4 % / 100 %), aber der Filter
laesst auch 40 % der Tueren uebrig.

### (c) Ankunfts-Yaw des Partners

Von 327 `ncdir`-Feldern liegen **263 exakt** auf einem 90°-Vielfachen und **276 innerhalb
von ±63**, 51 nicht (Abweichung 64…512). Wo es exakt ist, ist es die praeziseste Quelle — es ist die einzige,
die zusaetzlich die **Seite** liefert (auswaerts = `−vorwaerts`), und die brauche ich zum
Ansnappen.

### Rangfolge bei Widerspruch

**(c) Yaw  >  (a) Rechteck  >  (b) SCA.**
Begruendung mit Zahlen: (c) ist gegen (b) im sauberen Test 130/130 fehlerfrei und ist die
*Absicht des Designers* (der Kurs, den er dem Spieler nach dem Durchgang gibt, steht
senkrecht auf der Tuer). (a) folgt (c) zu 98,3 % und deckt auch die 90 Tueren ohne Partner
ab. (b) ist die verrauschteste Quelle (90,3 %), taugt aber als letzter Rueckfall und als
unabhaengiger Schiedsrichter — genau dafuer wurde sie oben benutzt.

---

## 3. Die Tabelle

`analysis/kartensymbole/tueren_wandlage.csv` — 327 Zeilen, eine je Tuer-Datensatz.
Spalten: `room, slot, rdt_off, sce, dest, partner, rect_x/z/w/d, welt_x, welt_z,
ausgang_ncdir, ausgang_next, ankunft_ncdir, ankunft_spawn,
achse_a_rect, achse_b_sca, achse_c_yaw, achse, sca_wand_richtung, sca_wand_abstand,
yaw_abw, auswaerts_karte, auswaerts_quelle, seite, rect,
karte_heute_x/y, kind_heute, kind_soll, karte_neu_x/y, karte_snap_x/y, snap_px, lage`.

Abdeckung: 327 Datensaetze, davon **181 mit Karten-Zone UND belegter Achse**, 145 ohne
Karten-Zone (Raum hat auf seiner Seite kein zugeordnetes Rechteck), 1 mit Zone ohne Achse.

---

## 4. ROOM1130 — Tuer fuer Tuer

Zone: Bbox x [−8650…6000], z [−18400…19750] → Seite 4, Rect 4 `(144,80) 32×80`, uv `(0,32)`
in `DATA/MAP05.PIX`. Der Nutzer nennt drei Fehler — **alle drei bestaetigt.**

Die 11 (je 5× partitioniert gespeicherten) SCA-Zellen ergeben einen U-foermigen Flur:
Nord-Sued-Gang `x(−3050…−300)`, West-Gang `x(−6650…−3950)`, Sued-Gang `z(14700…17750)`.

| Slot | Ziel | Rect w×d | Welt (x,z) | Ankunfts-`ncdir` (Partner) | a / b / c | Achse | **HEUTE** | **RICHTIG** |
|---|---|---|---|---|---|---|---|---|
| 0 | 1140 | 1000×**4000** | (−300, −13900) | 2048 (`1140#0`) | Z / Z / Z | **Z** | (162,150) **kind 0** — *ausserhalb der Zeichnung* | (160,148) **kind 2**, auf der Wandlinie |
| 1 | 1120 | 1000×**2000** | (−3050, −2150) | 0 (`1120#2`) | Z / Z / Z | **Z** | (156,125) kind 2 — *mitten in der Innenflaeche* | (152,123) kind 2, auf der Wandlinie |
| 2 | 1150 | 1000×**2000** | (−6650, 16350) | 0 (`1150#0`) | Z / Z / Z | **Z** | (148,87) kind 2 — 4 px neben der Wand | (144,83) kind 2, auf der Wandlinie |
| 3 | 1170 | **2000**×1000 | (2700, 14650) | 3072 (`1170#4`) | X / X / X | **X** | (168,90) **kind 2** — *ausserhalb der Zeichnung* | (163,89) **kind 0**, auf der Wandlinie |

Zuordnung zur Nutzer-Beschreibung:
* „**der Balken rechts ist falsch gedreht**" = Slot 3 (groesstes `mx`=168). Wand laengs X
  (Suedwand des oberen Gangs, Weltkante z = 14700 = Sued-Flaeche der SCA-Zelle
  `x[−3950…6000] z[4200…14700]`), also **waagerecht** — heute senkrecht. ✔
* „**der in der Mitte ist mitten im Raum statt an der Wand**" = Slot 1. Achse ist richtig
  (senkrecht), aber die Tuer sitzt in einer **INNENWAND** (Weltkante x = −3050 = Ost-Flaeche
  der Zelle `x[−7600…−3050] z[−18400…1050]`), nicht am Aussenrand des Rechtecks. ✔
* „**die Tuer unten ist auch 90 Grad falsch gedreht**" = Slot 0 (groesstes `my`=150). Wand
  laengs Z (Weltkante x = −300 = West-Flaeche der Zelle `x[−300…1700] z[−18400…6200]`),
  also **senkrecht** — heute waagerecht. ✔

**Warum die heutige Regel das nicht treffen kann:** sie fragt „welche Kante des
32×80-Rechtecks ist naeher". Fuer Slot 0 ist `d_senk = min(162−144, 175−162) = 13`,
`d_waag = min(150−80, 159−150) = 9` → 13 ≥ 9 → kind 0. Fuer Slot 3 `d_senk = 7`,
`d_waag = 10` → kind 2. Beides genau falsch herum. Die Kante des **Rechtecks** hat mit der
Wand, in der die Tuer steckt, nichts zu tun — bei einem U-foermigen Flur schon gar nicht.

**Zweitens** stimmt die Skalierung nicht: das gezeichnete Bild in der Kachel ist nur
**26 × 79** Pixel gross, das Rechteck aber 32 × 80. Alles wird um Faktor 32/26 = 1,23 nach
rechts gestreckt — deshalb landen Slot 0 und Slot 3 **ausserhalb** der Zeichnung, im
schwarzen Rand. Gemessen ueber alle Tuermarken: **76 von 181 (42 %) liegen heute
ausserhalb des gezeichneten Raums**, 86 frei in der Innenflaeche, **nur 19 (10,5 %) auf
einer Wandlinie**.

Bildbeweis: `analysis/kartensymbole/B_room1130_alt_neu.png` (links heute, rechts neu).

---

## 5. Empfehlung fuer `gen_map_zones.py`

Drei Aenderungen. Alle Konstanten unten sind entweder Adressen aus dem Original oder
gemessene Schwellen mit Zahl.

### 5.1 Projektion: FREIFLAECHE → GEZEICHNETE Bbox (statt SCA-Bbox → ganzes Rechteck)

```python
# Die Kachel zeichnet nur den BEGEHBAREN Raum + 1 px Wandlinie. Die SCA-Bbox enthaelt
# dagegen die WANDDICKE (SCA-Zellen sind SOLIDE: push_rect/push_circle schieben heraus,
# re15_collision.c:722-732). Und das Bild fuellt das Rechteck nicht aus
# (ROOM1130: 26x79 gezeichnet in 32x80).
free_bbox(zone)  = Bbox der Rasterpunkte (Schritt 100), fuer die solid(sca, x, z) FALSCH ist
drawn_bbox(page, rect) = Bbox der Texel != 0 im uv-Fenster der Kachel
        (Rect-Eintrag +8 = u, +10 = v; Kachel = DATA/MAP<page+1>.PIX,
         256x256 4bpp, 128 B/Zeile, unteres Nibble = linkes Pixel)

mx = R.x + dx0 + (clamp(wx) - fx0) * (dx1-dx0) / (fx1-fx0)
my = R.y + dy1 - (clamp(wz) - fz0) * (dy1-dy0) / (fz1-fz0)     # z gespiegelt: @0x800474b0
```

### 5.2 Ausrichtung: aus der Wandachse, NICHT aus der Rechteck-Kante

```python
def wandachse(door):
    p = partner(door)                       # Tuer im Nachbarraum, die hierher zurueckfuehrt
    if p:
        yaw = p.ncdir & 0xFFF
        q   = round(yaw / 1024) % 4
        if min(abs(yaw - q*1024), abs(yaw - (q*1024 + 4096))) <= 128:   # 276/327 innerhalb +-63
            return 'Z' if q in (0, 2) else 'X'                          # vorwaerts=(cos,-sin)
    if abs(door.rd) != abs(door.rw):        # 233/237 = 98,3 % deckungsgleich mit dem Yaw
        return 'Z' if abs(door.rd) > abs(door.rw) else 'X'
    return sca_wandachse(door)              # 4 Strahlen, Schritt 50, max 9000; nur wenn
                                            # naechste <= 1500 UND >= 2x naeher als die zweite
kind = 2 if wandachse == 'Z' else 0         # 2 = senkrechter Balken, 0 = waagerechter
```

### 5.3 Ansnappen: in AUSWAERTS-Richtung bis zur gezeichneten Wandlinie

```python
# auswaerts = weg vom Raum = -vorwaerts des Ankunfts-Yaw:
#   q=0 -> Karte (-1, 0)   q=1 -> (0,-1)   q=2 -> (+1, 0)   q=3 -> (0,+1)
# (Karte: +X rechts, +Z OBEN; my-Negation @0x800474b0)
# Ohne exakten Yaw: Richtung der naechsten SCA-Wand (>= 2x naeher als die zweitnaechste).
K = 6                                       # Messreihe unten
for s, sgn in [(s, +1) for s in 0..K] + [(s, -1) for s in 1..K]:
    q = start + auswaerts * s * sgn
    if q ausserhalb des uv-Fensters: continue
    if texel(page, q) == 4:                 # CLUT-Index 4 = 0x5ad6 rgb(176,176,176) = Wandlinie
        marke = q; break
else:
    marke = start                           # KEINE Wand gefunden -> stehen lassen, nicht raten
```

`K`-Messreihe (Marken, die danach auf einer Wandlinie liegen, von 181):

| K | 2 | 3 | 4 | **6** | 8 | 10 | 12 |
|---|---|---|---|---|---|---|---|
| auf Wandlinie | 84 | 98 | 111 | **133** | 140 | 147 | 156 |
| noch innen | 70 | 56 | 43 | **29** | 23 | 17 | 12 |
| noch ausserhalb | 27 | 27 | 27 | **19** | 18 | 17 | 13 |

**K = 6** ist der Knick; darueber wird jeder gewonnene Treffer mit Verschiebungen von
8…12 px erkauft, und die Kacheln sind nur 16…80 px gross — da schnappt eine Tuer auf die
falsche Wand. Wenn mehr gewollt ist: K = 8 (140 statt 133, max 8 px).

### 5.4 Wirkung, game-weit gezaehlt

Von 181 kartierten Tuer-Datensaetzen:

* **55 (30,4 %) aendern ihre DREHUNG.** Verteilung heute 102× senkrecht / 79× waagerecht,
  neu 111 / 70.
* **177 (97,8 %) aendern ihre POSITION** (Median-Verschiebung 5 px Manhattan, p90 10 px,
  max 37 px). Das ist so viel, weil auch die Projektion selbst korrigiert wird (5.1) — nur
  4 Marken bleiben exakt liegen.
* Lage im Kachelbild: **Wandlinie 19 → 133**, Innenflaeche 86 → 29, **ausserhalb 76 → 19**.
* Im ausgelieferten Header stehen (nach Dedup) **156 Tuermarken**; meine Neurechnung
  ergibt 157, davon sind **3 identisch** mit heute.

### 5.5 Wenn eine Marke gar nicht an eine Wand gehoert

48 der 181 finden in ±6 px keine Wandlinie. Dafuer gilt: **nicht verschieben, sondern
liegen lassen** (die 5.1-Projektion allein bringt sie schon von 33 auf 19 „ausserhalb").
Die Restfaelle haeufen sich auf wenigen Zonen — ROOM1230/S1r3 (6), ROOM5090/S9r10 (4),
ROOM11D0/S0r5 (3) — dort ist der Verdacht **die Rechteck-Zuordnung der Zone selbst**, nicht
die Tuer. Das gehoert in die Zonen-Zuordnung (Hill-Climbing in `gen_map_zones.py`), nicht in
die Tuerlogik. Solange eine Marke nicht auf einer Wandlinie sitzt, sollte sie besser
**gar nicht gezeichnet** werden als falsch — sonst wiederholt sich der Effekt aus
[[reai-v2-zeichenreihenfolge-invers]] („ein Rect OHNE Zone wird GRAU gezeichnet statt
weggelassen").

### 5.6 Zwei Stolperfallen fuer die Umsetzung

1. **Die Selbst-Tuer-Zusammenfassung** (`merged` in `gen_map_zones.py`) mittelt heute zwei
   Kartenpunkte. Mit dem Wand-Snap muss zuerst gemittelt und **dann** gesnappt werden —
   sonst schnappen die beiden Haelften auf zwei verschiedene Waende und die Mitte liegt
   wieder im Nichts.
2. **Doppelte Slots.** Mein Parser findet z.B. `ROOM1160#0` zweimal (Ziel 1180 und 1230) —
   derselbe Slot wird im Skript mit zwei Zielen installiert. Der `seen`-Dedup faengt das
   heute nur, wenn Position UND kind gleich sind; mit dem Snap fallen sie zusammen.

---

## 6. Was offen bleibt

* Die Kachel-Zeichnungen sind **handgezeichnet, nicht massstabsgetreu**. Beispiel ROOM1130:
  aus der Geometrie ergibt sich fuer die Suedwand des oberen Gangs Zeile 7,0
  (`(17750−14700)/437,8`), gezeichnet ist sie in Zeile 9. Ein rein rechnerischer Ansatz
  kommt deshalb prinzipiell nur auf ±2 px heran — genau darum der Snap.
* 51 der 327 Ankunfts-Yaws sind **nicht** achsenparallel (Abweichung 64…512). Fuer die
  Achse faellt dort (a) ein; die **Seite** (auswaerts) muss dann aus (b) kommen. Ob es
  schraege Tueren mit schraeger Wandzeichnung gibt, habe ich nicht geprueft.
* 145 der 327 Tueren haben gar keine Karten-Zone. Das ist die Zonen-Zuordnung, nicht diese
  Aufgabe.
* Ich habe **nicht** geprueft, ob das Original selbst irgendwo Tuersymbole in die Kacheln
  zeichnet. Gegen-Indiz: Seite 4 / Rect 4 enthaelt ausschliesslich die CLUT-Indizes 0, 1
  und 4 (1599 / 728 / 233 Pixel) — kein eigener Symbol-Index. Die Tuerbalken sind also
  eine Zutat des Ports.
