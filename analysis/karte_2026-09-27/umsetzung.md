# RE2-Kartensystem im Port — Umsetzung (Runde 27)

Abzuege in diesem Ordner, alle im echten Renderpfad (RE15_INV_FB_SHOT,
Statusmenue -> Reiter MAP, Blatt 9 "LABORATORY B2", Spieler in ROOM5030):

| Datei | Zustand |
|---|---|
| `vorher_mit_plan.png`  | ALT, Flag(3,115) gesetzt — der Plan bewirkt NICHTS, nur ROOM5030 in Gruen |
| `nachher_mit_plan.png` | NEU, Plan im Besitz — das ganze Blatt erscheint: unbesuchte Raeume schwarz mit heller Wandlinie, der eigene Raum dunkelrot |
| `nachher_ohne_plan.png`| NEU, ohne Plan — wie RE2s Zweig `beq v0,zero,0x8006e768` @0x8006E744: nur der eigene Raum |
| `vorher_besucht.png`   | ALT, ganzes Blatt besucht — Raumkoerper GRUEN (Modulation 40,144,40) |
| `nachher_besucht.png`  | NEU, ganzes Blatt besucht — Raumkoerper BLAU (RE2 1040b0) |

## Was gebaut ist

1. **Besitz** — `re15_map_owned.c/.h`. Port-Gegenstueck zu RE2s Bank 33
   (`0x800D4924`, Bank-Zeigertabelle `0x800A78C8` Index 33; Bit = Karten-Id aus
   `0x800AAA3D + id*8`, gelesen `lbu a1,-21955(at)` @0x8006E660, Bank-Basis
   `addiu a0,a0,18724` @0x8006E668, Bit-Test `jal 0x80077360` @0x8006E66C).
2. **Zeichner** — `re15_inv_screen.c`. Vier Zustaende statt drei:
   ohne Plan wird eine unbesuchte Kachel uebersprungen (@0x8006E744), mit Plan in
   der Zeile CLUT-Y 498 gezeichnet (@0x8006E71C).
3. **Farben** — RE2 moduliert nicht, es waehlt eine CLUT-ZEILE
   (`GetClut(256,s5)` @0x8006E750; s5 = 501 @0x8006E614, +1 fuer den aktuellen
   Raum @0x8006E648, 498 @0x8006E71C). Der Port macht das jetzt genauso.
4. **Fundstelle** — ROOM5030/5031, `Set(3,115,1)` @Datei 0x10C8 bzw. 0x1126.

## Die gemessenen Palettenwerte

`info/re2leon/COMMON/DATA/ST0.TIM`, zweites TIM @Datei 0x10820, CLUT-Block
x=256 y=480 w=16 h=21, Zeilenindex k = CLUT-Y − 490 (Slot-Wort
`addiu v0,zero,2587` = 0x0A1B @0x80068588; `addiu v0,v0,480` @0x80076B08):

| Datei-Offset | CLUT-Y | Eintrag 1 | bedeutet |
|---|---|---|---|
| 0x10936 | 498 | `0x0000` | durchsichtig — Karte da, Raum unbesucht |
| 0x10996 | 501 | `0xD902` = 1040b0, STP | besucht |
| 0x109B6 | 502 | `0x842D` = 680808, STP | aktueller Raum |

Die drei Zeilen unterscheiden sich in **genau** den Eintraegen {1,12,13,14};
die zwoelf uebrigen sind bitgleich, Eintrag 4 (Wandlinie) in allen `888888`.

⛔ Meine erste, von Hand gerechnete Umrechnung RGB888 → RGB555 war bei **beiden**
Farben falsch (0xD802 statt 0xD902, 0x8061 statt 0x842D). Der Riegel
`unit_karte_besitz` liest die Datei deshalb selbst nach.

## ⛔ Richtigstellung zum Dossier: das Gruen ist KEINE Port-Erfindung

`kartensystem-mechanik.md` Befund I schreibt, das Gruen sei eine Erfindung des
Ports. Das ist falsch. RE1.5 liefert seine Kartenpalette in **derselben**
VRAM-Zeile 501 (TEX.TIM, CLUT x=256 y=480 w=32, Zeile 21 = clut-Id 0x7d50 =
`GetClut(0x100,0x1f5)` @0x80046fdc-fe8), und ihr Eintrag 1 ist
**`0x206800` = GRUEN mit gesetztem STP**, die Wandlinie `0xb0b0b0`.
Das Gruen, das der Nutzer beschreibt, ist RE1.5-Original; die Modulation
(40,144,40) war nur eine unsaubere Naeherung davon. Blau ist RE2s Wert.

**Preis, gemessen an den Abzuegen oben** (Panel `(0,16,88)`):
Raumkoerper alt `(16,56,40)`, neu `(8,40,128)`. Helligkeitsabstand zum Panel
22,8 → 21,0 (praktisch gleich), **Farbabstand 64,5 → 47,3** — die blauen Raeume
heben sich messbar schwaecher vom blauen RE1.5-Panel ab. In RE2 faellt das nicht
auf, weil dort der Kartengrund schwarz ist; der schwarze Kasten wurde hier
2026-09-02 auf Nutzerwunsch entfernt ("alle Karten Hintergruende ein schwarzes
square. Das ist auch kaese.").
Zurueck auf RE1.5s Gruen sind es drei Defines in `re15_inv_screen.h`
(`RE15_KARTE_BESUCHT` = `0x81A4`, aus `shared_assets/PSX/DATA/TEX.TIM`
@Datei-Offset 0x556 gelesen — Zeile 21 Eintrag 1, `206800` mit STP).

## Was NICHT gebaut ist

* **Gegenstandsmarken (RE2 FUN_8006dcc0).** Der Port hat kein Gegenstueck dazu.
  Seine Markenliste (`s_map_marks`, `re15_map_zones.h:381`) fuehrt **Tueren und
  Treppen** — aus RE1.5s eigenen Tuer-Datensaetzen und den Aot_set-Zonen Typ
  12/13 —, nicht Gegenstaende. Sie sind erstens etwas anderes als RE2s 14
  Item-Marken (Tabelle `0x800A9B10`, Bank 31 `0x800D4A34`) und zweitens
  ausdruecklich auf der Nicht-anfassen-Liste. Ein Besitz-Gatter waere hier also
  falsch. Ihr vorhandenes Gatter (`zid_besucht` zuerst) ist von der Aenderung
  nicht betroffen: unbesuchte Raeume zeigen weiterhin keine Marken, auch mit Plan.
  Wer RE2s Item-Marken will, braucht zuerst eine RE1.5-Quelle fuer die 14
  Positionen — die gibt es im Port heute nicht.
