# RE2-Sicherung — Extrakt für den Port

**Frage:** Hat Resident Evil 2 eine Sicherung als Gegenstand?
**Antwort: JA, zwei.** Beide sind hier vollständig ausgeschnitten.

| Ordner | Item-Id | Name (Original, englisch) | Weltmodell |
|---|---|---|---|
| `item_076_main_fuse/` | **76 = 0x4C** | `Main Fuse` | ROOM60D0.RDT Prop-Slot 3 |
| `item_077_fuse_case/` | **77 = 0x4D** | `Fuse Case` | ROOM60D0.RDT Prop-Slot 1 |

Beide sind für **Leon** erreichbar — sie stecken in `PL0/RDT/ROOM60D0.RDT`, und `PL0` ist
die Leon-Seite der Disc. ROOM60D0 ist der einzige Raum des ganzen Spiels, der sie führt.

Der vollständige Beleg mit allen Adressen steht in
`analysis/befunde_2026-09-21/re2-sicherung-item.md`.

## Was in jedem Item-Ordner liegt

| Datei | Inhalt |
|---|---|
| `beleg.json` | Alle Adressen, Rohbytes und Messwerte in einer Datei |
| `symbol_<id>.png` | Inventarsymbol, 40×30, native Größe |
| `symbol_<id>_x8.png` | dasselbe achtfach vergrößert, zum Anschauen |
| `symbol_<id>_roh_40x30_8bpp.bin` | die 1200 Rohbytes aus `COMMON/DATA/ITEMALL.PIX` |
| `symbol_clut_256_bgr555.bin` | die 512-Byte-Palette (256 × u16 BGR555, VRAM y=496) |
| `weltmodell.md1` / `.tim` | das Bodenobjekt, roh aus `ROOM60D0.RDT` geschnitten |
| `weltmodell.obj` | dasselbe Mesh als OBJ (Dreiecke + Vierecke, ohne UV) |
| `weltmodell_ansicht.png` | texturiertes Vorschaubild |

## Daneben

* `tabellen/re2_item_names_140.tsv` — **alle 140** Namenseinträge mit Adresse und Rohbytes,
  englisch und japanisch. Das ist die Sollzahl: mehr Item-Namen hat RE2 nicht.
* `tabellen/re2_item_properties.json` — **alle 101** Eigenschafts-Records
  (Stapelgrenze, Varianten-Nibble, Klassenbyte, alle 80 Kombinations-Rezepte).
* `tabellen/alle_106_symbole/` + `re2_alle_106_symbole.png` — **alle 106** Inventarsymbole
  einzeln und als Kontaktbogen. Dort sieht man auch, dass keines ein Feuerlöscher ist.
* `tabellen/ITEMALL_clut_256_bgr555.bin` — die Palette für alle Symbole.
* `room60D0_skript/` — die drei Skriptblöcke, die die beiden Gegenstände steuern,
  roh (`.scd`) und opcode-weise aufgeschlüsselt (`.txt`).
* `re15_vergleich/re15_item_names.tsv` — **alle 100** RE1.5-Itemnamen mit Id.
  RE1.5 hat selbst eine **`Fuse` (Id 0x40)** — aber kein Raum stellt sie auf.

## Feuerlöscher

**RE2 hat keinen** — weder als Gegenstand noch als 3D-Modell. Geprüft wurden alle
140 Namen, alle 106 Symbole, alle 252 verschiedenen Raum-Requisiten-Modelle und
alle 21602 Dateien der Disc auf die Zeichenfolge. Alles null.

**RE1.5 hat den Gegenstand** (`Fire Extinguisher`, Id 0x31, in ROOM1000), aber
**kein 3D-Modell dafür** — der Aufhebe-Auslöser trägt Prop-Slot 255 = „kein Modell".
Auch unter den 122 RE1.5-Requisiten-Modellen ist kein Feuerlöscher.

## Erzeugen

```
python tools/re2_sicherung/export.py extracted_re2_sicherung
```

Quellen sind ausschließlich `info/re2leon/` und `info/Re1.5/` (nur gelesen).
