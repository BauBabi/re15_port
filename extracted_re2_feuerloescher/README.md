# extracted_re2_feuerloescher

Ergebnis der Frage „gibt es in Resident Evil 2 ein Feuerloescher-3D-Modell?".

**Kurz: in RE2 nicht — weder Gegenstand noch Raum-Objekt noch ueberhaupt das Wort.
In RE1.5 gibt es den Feuerloescher als Gegenstand `0x31`, aber auch dort ohne
3D-Modell.** Die vollstaendige Messung mit allen Sollzahlen steht in
`analysis/befunde_2026-09-21/re2-feuerloescher-modell.md`.

Dieser Ordner enthaelt deshalb kein Feuerloescher-Modell (es existiert keines),
sondern das, was statt dessen brauchbar ist.

## `re15_feuerloescher/` — das Original-Bildmaterial aus RE1.5

| Datei | Herkunft | Anmerkung |
|---|---|---|
| `RE15_item31_fire_extinguisher_ITPS.tim` | `info/Re1.5/PSX/ITEM/ITPS.ITP` file@0x093000, 8 608 Byte | 112 × 72, 8 bpp mit eigener CLUT — das Gegenstands-Bild des Feuerloeschers |
| `…_ITPS.png` / `…_ITPS_6x.png` | daraus dekodiert | roter Loescher, Chromgriff, weisses Etikett |
| `RE15_item31_fire_extinguisher_ITEMALL_tile.raw` | `info/Re1.5/PSX/DATA/ITEMALL.PIX` file@0x00E5B0, 1 200 Byte | 40 × 30 Inventar-Kachel, **Palettenindizes** |
| `…_ITEMALL_tile_indices*.png` | daraus | ⚠ Graustufen der Indizes, **nicht** die Endfarben — die CLUT liegt nicht in der Datei (siehe Dossier §5) |

Schrittweite und Layout von `ITPS.ITP` stammen aus dem byte-treuen Decoder des
Ports, `re15_port/engine/src/itps_common.c` (72 TIMs, Id × 0x3000).

## `basis_kandidaten/` — die naechstliegenden vorhandenen Modelle

Kein Feuerloescher, sondern die vier formnaechsten Zylinder/Dosen, falls ein
Modell gebaut werden soll. Je als rohes `.md1`, rohe `.tim`, `.obj` + `.mtl` +
Textur-`.png` und vier gerenderte Ansichten (`front`, `q35`, `side`, `top`).

| Basisname | Quelle | md1 / tim | Masse | Flaechen |
|---|---|---|---|---|
| `RE2_ROOM60D0_m02_laborflasche` | `info/re2leon/PL0/RDT/ROOM60D0.RDT` | @0x1424 / @0x1F384 | 82 × 408 × 82 | 60 |
| `RE2_ROOM1010_m03_sprayflasche` | `info/re2leon/PL0/RDT/ROOM1010.RDT` | @0x3394 / @0x2A4A0 | 158 × 489 × 180 | 40 |
| `RE15_ROOM1010_m00_spraydose` | `info/Re1.5/PSX/STAGE1/ROOM1010.RDT` | @0xAF4 / @0x22D28 | 158 × 489 × 180 | 40 |
| `RE15_ROOM4050_m03_spraydose_rot` | `info/Re1.5/PSX/STAGE4/ROOM4050.RDT` | @0x3788 / @0x2DDA8 | 127 × 400 × 147 | 40 |

Die beiden RE1.5-Dosen sind die beste Basis: der Item-Zensus bindet
`ROOM1010`-Prop 0 an Gegenstand `0x22` **First Aid Spray** und `ROOM4050`-Props 3/4
an `0x23` **Antidote Spray** — also dieselbe Sorte Objekt (Druckdose mit Duese) im
richtigen Groessenbereich.

`.obj`-Konvention: PSX-Modellraum mit `Y' = −Y` (CLAUDE.md „Technical Notes"),
1 Einheit = 1 PSX-Einheit, UVs in das Pixelraster der jeweiligen TIM umgerechnet.

## Herkunft und Reproduktion

`info/re2leon/` und `info/Re1.5/` wurden ausschliesslich gelesen. Alle Werkzeuge
liegen in `analysis/befunde_2026-09-21/tools/`, die Belegbilder (Kontaktboegen
aller 392 RE2- und aller 192 RE1.5-Netze, Item-Icon-Boegen, Namenslisten) in
`analysis/befunde_2026-09-21/belege/`.
