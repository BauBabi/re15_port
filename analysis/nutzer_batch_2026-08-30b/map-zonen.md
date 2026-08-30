# Karten-Reparatur: das ZONEN-Modell (Türen, Treppen, Marker, Etagen)

Nutzer-Report 2026-08-30 (im Spiel getestet), sinngemäß:
1. In ROOM1170 läuft man oben durch eine Tür — **die ist auf der Karte nicht eingezeichnet**.
2. Nach dem Durchlaufen steht der **Marker unten im großen Rechteck** statt oben im kleinen.
3. Das **kleine Rechteck oben ist nicht hervorgehoben**, sondern das große, aus dem man kam.
4. **Links im kleinen Rechteck müsste die Treppe eingezeichnet sein** — fehlt das Symbol in
   RE1.5, aus RE2 übernehmen.
5. Treppe hinunter = **andere Ebene**; bei der Tür nach 1150 stimmt die Ebene, bei der
   direkten Tür 1170 → 1130 nicht, im Treppenhaus ist alles falsch.
6. „Im Original ist hier noch viel kaputt, also orientiere dich da nur grob dran."

## Die Ursache (gemessen, nicht vermutet)

**Ein RDT-„Raum" ist nicht immer EIN Ort.** ROOM1170 hat im Tür-Datensatz zwei
**Selbst-Türen** (`dest == 0x1170`), die den Spieler von z = +15250 nach z = −26500
versetzen. Die Kollisionsgeometrie zerfällt entsprechend in **zwei zusammenhängende
Bereiche**:

| Bereich | Welt-Bbox | Größe |
|---|---|---|
| 0 (groß) | x[−13430, 15658] z[−10910, 17746] | 29088 × 28656 |
| 1 (klein) | x[−28900, −8784] z[−28841, −17070] | 20116 × 11771 |

Seite 5 der Karte hat **genau zwei Rechtecke**: r0 (140,80) 48×24 — klein, oben — und
r1 (148,101) 48×56 — groß, unten. Das alte Modell „ein Raum = ein Rechteck" klebte
beides auf r1. Deshalb blieben Marker und Hervorhebung unten, egal wo man stand.

**26 der 103 Basis-Räume** sind so gebaut (1000, 1020, 1070, 1080, 10D0, 1100, 1110,
1140, 1170, 11E0, 11F0, 1260, 2040–2080, 3050, 30C0, 30E0, 4020, 4090, 5030, 5050,
5110, 6020).

## Die Reparatur

### Zonen statt Räume
Jeder zusammenhängende Bereich der Kollisionsgeometrie ist eine **Zone** mit eigenem
Karten-Rechteck und eigener Marker-Abbildung. Erzeugt von `tools/gen_map_zones.py`:
Zellen verschmelzen (Toleranz 1500), Splitter < 3 % verwerfen, Zuordnung je Seite über
Seitenverhältnis + Flächen-Rang + **Tür-Graph** (Türposition und Eintrittsposition
derselben Tür müssen auf denselben Kartenpunkt fallen), Hill-Climbing mit Neustarts.
Ergebnis: **100 Zonen** zugeordnet.

ROOM1170 jetzt: Zone 0 → Seite 5 **Rect 1** (groß, unten), Zone 1 → **Rect 0** (klein, oben).

### Die aktive Zone folgt dem Spieler — jeden Frame
`re15_map_zone_update` läuft im Spiel-Schritt (nicht nur beim Raumwechsel), denn die
Selbst-Türen wechseln den Bereich **ohne** Raumwechsel. Damit stimmen Hervorhebung und
Marker sofort nach dem Durchgang. **Kleinste passende Zone gewinnt**: die Bbox eines
großen Bereichs umschließt oft die eines kleinen (ROOM1140), sonst bliebe der Wechsel
unsichtbar.

### Marker
Lineare Abbildung der Zonen-Bbox auf ihr Rechteck — der Marker liegt damit **immer in
dem Rechteck, das auch hervorgehoben ist**. Die Original-Formel (FUN_800473f8) trägt für
65 der 103 Räume nur eine Platzhalter-Zeile `{0,0,1,1}` und projiziert dort alles in die
Bildecke; sie bleibt unter `RE15_MAP_STOCK=1` erreichbar.

### Etagen
Weil Zonen auf verschiedenen Seiten liegen dürfen, ist die Etage eine Eigenschaft der
Zone — der Ebenenwechsel über eine Treppe wechselt damit automatisch die Kartenseite.

### Türen und Treppen einzeichnen
RE1.5 malt auf seinen Karten-Seiten **weder Türen noch Treppen**; RE2 setzt gelbe
Türpunkte in die Grafik und eigene 8×8-Marker für Übergänge (Tabelle @0x800a9b1c,
Icons ab v=0xf0 der Kartenseite). Da RE1.5 diese Icons nicht mitbringt, zeichnet der
Port sie aus Linien:

- **Tür** = kurzer Strich in RE2s Türgelb (Palette 0x12dc → RGB 224/176/32)
- **Treppe** = Leiter mit drei Sprossen, hell

Positionen kommen aus den RDT-Daten und sind zur Erzeugungszeit in Karten-Pixel
umgerechnet: Türen aus den Tür-Datensätzen, **Treppen aus den SCD-Zonen `Aot_set`
Typ 12/13** (die Band-Wechsel-Zonen). **181 Marken** insgesamt, davon **22 Treppen**.
Marken erscheinen nur für Zonen, die der Spieler schon gesehen hat.

Für ROOM1170 heißt das konkret: Seite 5 trägt **7 Marken — 3 Türen und 4 Treppen-Marken
im kleinen Rechteck** (x 147…159 = links darin, genau wie im Report gefordert).

## Grenzen (ehrlich)

- Die Zuordnung Zone → Rechteck ist eine **Optimierung**, keine Ableitung aus
  Original-Daten (die es nicht gibt). Sie ist gegen Seitenverhältnis, Flächen-Rang und
  Tür-Positionen geprüft, kann aber im Einzelfall danebenliegen. Nicht zuordenbare Zonen
  bleiben leer und färben sich nie falsch.
- Räume ohne Kartenseite (13) und die defekte Seite 13 (CD-Datei-Id-Überlauf) bleiben
  außen vor.
- Die Marken sind Symbole an der richtigen **Stelle**, keine Nachbildung der
  Original-Grafik.
