# GEGENPRUEFUNG B5 — Rolle: ALTERNATIV-ERKLAERER

Behauptung B5: *Das Original hat innerhalb eines Raums KEINEN Etagenbegriff; der Seiten-Setzer
@0x8004b568 liest nur die Raumnummer, DAT_800acad6 kommt im Kartencode nicht vor, die
Markerformel hat nur X, Z, Raum-Index. Jede rauminterne Etagen-Umschaltung im Port ist eine
PORT-ERGAENZUNG.*

**Ergebnis: NICHT widerlegt.** Fuenf konkurrierende Mechanismen gesucht und einzeln vermessen —
alle fallen. Der Mechanismus-Kern von B5 ist mit einem *breiteren* Mass als im Bericht bestaetigt.
Zwei **Neben**belege des Berichts sind dabei aber sachlich falsch (A6/A7 unten).

Methode (unabhaengig vom Bericht): eigener lui-verfolgender Absolut-XREF-Scanner ueber den
kompletten EXE-Text (`xref.py`, `allrefs.py`, `tabrefs.py`, `checkstores.py`) und ueber alle acht
Overlays (`xref_ovl.py`), plus Direkt-Disasm. Kein Text-Grep im Ghidra-Dump.
Scratchpad: `.../scratchpad/b5/`.

---

## A. Die geprueften Alternativ-Kandidaten

### A1 — "Ein SCD-/Overlay-Opcode schaltet die Kartenseite mitten im Raum um" -> WIDERLEGT
Scan aller acht Overlays (STAGE1..6, TITLE, DEBUG.BIN, geladen @0x80100000) auf Schreibzugriffe
nach `0x800b260d` (Skalenzeile) / `0x800b260e` (Seite) / `0x800b0fe2` (Raum):
**0 Treffer.** Die Overlays lesen aus dem Umfeld nur `0x800b0fe0` (Stage), rein lesend
(z.B. STAGE1 @0x80106dcc, @0x8010712c, ... 8 Stellen). Ein Skript kann die Karte also nicht
umschalten.

### A2 — "Das Band wird registerindirekt gelesen (der Bericht hat nur nach dem DAT_-Symbol gegreppt)" -> WIDERLEGT
Das waere die klassische Grep-Luecke: `lbu v0,0x82(s0)` traegt kein `DAT_800acad6`.
Gemessen im gesamten Kartenbereich 0x80046a1c..0x8004c3fc:
* **kein einziger** Load mit Immediate 0x82 (=130) auf irgendein Register;
* die einzigen beruehrten Spieler-Globals sind `0x800aca88` X (2x, @0x80046ff8 + @0x8004741c),
  `0x800aca90` Z (2x, @0x80047078 + @0x8004746c), `0x800acaec/ee` HP, `0x800aca34` Puffer-Index;
* **`0x800aca8c` (Spieler-Y) kommt nicht vor** — der Kartencode ueberspringt Y zwischen X und Z;
* es wird auch **kein Zeiger** auf die Spielerstruktur geladen, ueber den +0x82 erreichbar waere.

### A3 — "Der untersuchte Codebereich war zu klein" -> WIDERLEGT
EXE-weite Referenz-Zaehlung auf die Kartendaten selbst (statt auf einen Adressbereich zu vertrauen):

| Tabelle | Refs in der GANZEN EXE | wo |
|---|---|---|
| Rect-Paartabelle 0x80076840 | **3** | 0x80047058, 0x80047068, 0x800474c4 |
| Marker-Skalentabelle 0x800768b0 | **8** | 0x80047004 ... 0x80047510 |
| MAP0x.PIX-ID-Tabelle 0x80074c4c | **1** | 0x8004c348 |
| Skalenzeile 0x800b260d / Seite 0x800b260e | 114 / 10 | alle in 0x80046ff0..0x8004c334 |

Der gesamte Kartenapparat sind genau **drei** Funktionen: `FUN_80046fd8` (Rects+Marker in den
Prim-Puffer), `FUN_800473f8` (Marker), `FUN_8004c328` (PIX laden). Alles innerhalb des vom
Bericht untersuchten Bereichs. Nichts liegt ausserhalb.

### A4 — "Eine Selbst-Tuer schaltet den Kartenkontext um (Tuer-getrieben statt Band-getrieben)" -> WIDERLEGT
Das war der aussichtsreichste Konkurrent: die Kartenseite haengt an `DAT_800b0fe2`, und
`DAT_800b0fe2` wird beim Tuer-Uebergang aus dem **Tuer-Record** gesetzt
(`lbu v0,9(a0)` -> `sh v0,DAT_800b0fe2` @0x8001d95c; daneben `10(a0)`->0x800b0fe4 Cut,
Vorgaengerraum nach 0x800b0fe6). Eine Selbst-Tuer koennte also theoretisch einen anderen
Kartenkontext setzen als den geladenen Raum.
Gemessen aus ROOM1170.RDT (Door_aot_set 0x3B, Layout wie `scripts/door_graph.py`):

```
off=0x1206 band=4 rect=(1500,14400,2100,1700)    dest_stage=0 dest_room=0x17 cut=11 -> ROOM1170
off=0x12cc band=0 rect=(0,0,0,0)                 dest_stage=0 dest_room=0x17 cut=0  -> ROOM1170
off=0x135a band=4 rect=(-11940,-28450,1750,1200) dest_stage=0 dest_room=0x17 cut=0  -> ROOM1170
```

Alle drei Selbst-Tueren tragen `dest_room = 0x17` = Raum 23 — identisch mit dem geladenen Raum.
Und es ist **dieselbe** Variable, die auch die Raumdaten waehlt (Loader @0x80021c90:
`room<<5 + stage*2` in die Raumtabelle). Ein Tuer-getriebener Kartenwechsel im selben Raum ist
damit strukturell unmoeglich, nicht nur hier ungenutzt.
(Nebenbefund: die Tuer-Records tragen sehr wohl ein **Band**-Byte pc[4] — 4 fuer die Hof-Tueren,
0 fuer die des 2. Bereichs. Das Original kennt den Etagenbegriff also — nur die Karte liest ihn nie.)

### A5 — "Der Zeichner filtert Rechtecke (besucht/Etage) — so aendert sich die Karte im Raum" -> WIDERLEGT
Die Rect-Schleife in `FUN_80046fd8` @0x800472fc-0x800473e0 laeuft `count`-mal
(count = `lhu` aus 0x80076840[Seite*8], @0x80047070) und zeichnet **jedes** Rechteck
bedingungslos: kein Flag-Test, kein Sichtbarkeits-Bit, keine Etagen-Abfrage; auch die zwei festen
Sprites davor (Titel, Kompass) sind unbedingt. `FUN_800473f8` liest die count nur, um
`count+2` Prims per `AddPrim` (0x8006b538) einzuhaengen (@0x800475e0-0x8004761c) — **kein**
Containment-Test, **keine** Klemmung des Markers in ein Rechteck. Das Original kennt also
ueberhaupt keine Zuordnung "Marker gehoert zu Rechteck k".

### A6 — "Die Skalenzeile ist gar nicht der Raum-Index" -> TEILWEISE RICHTIG, aber kein Etagen-Mechanismus
Hier ist der Bericht faktisch falsch. Er schreibt: "In **jedem** Fall wird zusaetzlich
`DAT_800b260d = DAT_800b0fe2` gesetzt". Gemessen ueber alle 112 `sb`-Stores auf 0x800b260d:
**107x** direkt der Raum, **5x** `Raum + Stage-Basis`:
`addiu v0,v0,38` @0x8004b9c0 - `+50` @0x8004bc88 - `+65` @0x8004bdc0 - `+77` @0x8004bf5c -
`+98` @0x8004c044.
Und es gibt **sechs** solcher Setter, nicht einen — einen pro Stage:

| Kopf | Schranke | Sprungtabelle | Zeilenbasis | Seite (Sammel-Tail) |
|---|---|---|---|---|
| 0x8004b568 | 0x26 | 0x8001103c | +0 | pro Fall 0..5 |
| 0x8004b8a0 | 0x0c | 0x800110d4 | +38 | 13 (@0x8004b9b4) |
| 0x8004b9d4 | 0x20 | 0x80011104 | +50 | 7 (@0x8004bc7c) |
| 0x8004bc9c | 0x0c | — | +65 | 8 |
| 0x8004bdd4 | 0x15 | 0x800111b4 | +77 | 9 |
| 0x8004bf70 | 0x08 | 0x8001120c | +98 | 12 |

Die Skalenzeile ist also `f(Stage, Raum)`, nicht `f(Raum)` — **aber weiterhin ohne Band, ohne Y,
ohne Position**. B5s Schluss bleibt unberuehrt; nur die Begruendung "Zeilenindex = Raum-Index"
gilt exakt nur fuer STAGE1.

### A7 — Folgefehler im Bericht: Zeile 38 ist NICHT "jenseits des Switch"
`analysis/karte_2026-08-31/B_kartenkacheln.md` §5 listet idx 38 als "(jenseits des Switch)". Mit der
Stage-Basis +38 ist Zeile 38 = **STAGE2, Raum 0**. Gegenprobe aus der Tabelle @0x800768b0:

```
idx 36/37 = Stub 0,0,1,1      idx 38 = 80,196,2340,2308   (STAGE2 Raum 0, kalibriert)
idx 39..49 = Stub             idx 50 = 71,148,2256,2257   (STAGE3 Raum 0)
                              idx 51 = 113,224,2208,2737  (STAGE3 Raum 1)
```

Die Kalibrier-Luecken sitzen also genau an den Stage-Grenzen — das passt zum Prototyp-Zustand und
entwertet die Lesart "38 Zeilen, davon 25 Stubs" (die Tabelle ist game-weit, nicht STAGE1-weit).

---

## B. Unabhaengige Bestaetigung der drei B5-Belege

1. **Seiten-Setzer liest nur die Raumnummer.** `lh v1,DAT_800b0fe2` @0x8004b56c -> `sltiu 0x26` ->
   `jr v0` ueber 0x8001103c. Fall 23 = 0x8004b75c: `lbu v1,DAT_800b0fe2; j 0x8004b888;
   ori v0,zero,0x5` -> Tail `sb v0,DAT_800b260e` @0x8004b88c, `sb v1,DAT_800b260d` @0x8004b894.
   **Raum 23 -> Seite 5, Zeile 23 — bestaetigt.** (Default >=0x26 = `jr ra` @0x8004b898: schreibt
   nichts, Seite bleibt stehen. STAGE1 hat 40 Basisraeume (80 RDTs), die Schranke deckt nur 38.)
2. **Kein Band im Kartencode.** Eigener Scanner: siehe A2 — 0 Vorkommen von 0x800acad6, 0 Loads
   mit Offset 0x82, kein Spieler-Y.
3. **Markerformel nur X, Z, Zeilenindex.** `FUN_800473f8` liest ausschliesslich 0x800aca88
   (@0x8004741c), 0x800aca90 (@0x8004746c), 0x800b260d (@0x80047424), 0x800b260e (@0x800474a0),
   0x800aca34 (Doppelpuffer, @0x800474f0). Die zweite, im Bericht **nicht** erwaehnte
   Marker-/Rect-Funktion `FUN_80046fd8` liest exakt dieselbe Menge (X @0x80046ff8, Z @0x80047078,
   Zeile @0x80046ff0, Seite @0x80047048) — sie stuetzt B5 zusaetzlich, statt ihm zu widersprechen.

## C. Praezisierung statt Gegenthese

B5 ist eher **zu schwach** formuliert. Der Port hat in `re15_map_zones.c/.h` gar keinen
Etagen-Term (grep `band|floor|0x82` -> 0 Treffer); die aktive Zone folgt der **Position**
(x0,z0,x1,z1), ROOM1170 hat zwei Zonen (Idx 26 Hof -> Seite 5 Rect 1, Idx 27 -> Seite 5 Rect 0).
Port-Ergaenzung ist also nicht "die Etagen-Umschaltung", sondern **das Zonenkonzept als solches**:
pro Zone ein eigenes Rechteck und eine eigene Marker-Abbildung. Das Original kennt weder Zonen
noch eine Rect-Zuordnung noch eine Klemmung — es zeichnet alle Rechtecke der Seite und rechnet
**eine** lineare Abbildung je (Stage, Raum).
