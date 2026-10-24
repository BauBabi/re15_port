# Spur M masken36 — Runde 41 (2026-10-24)

Auftrag (AUFTRAG.md, Wortlaut massgeblich):
1. "Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall Loecher aufweisen."
   -> "danach kannst du deine Masken schliessen in der naechsten Version."
2. "Ich habe diverse neue PRIs fuer STAGE2 erstellt. die sollen alle sauber eingebaut werden!" (53 Masken in master
   5924939d) -> Sichtpruefung im Spiel; Orchestrator-Hinweis 2026-10-24 03:21: ROOM2020 Cut 3 Reling ueber Leons
   Beinen an der Kurbel (Pos_set @0x0A8C (3650,-1800,-2925)), dieselbe Pruefung fuer alle 53 Masken.

Baum: .claude/worktrees/r41_masken36 (Zweig r41/masken36, Basis b28c7236 = master-HEAD 4c667c97 + AUFTRAG-Hinweise).

## Stand
- [ ] Schritt 0: Pflichtlektuere (AUFTRAG, VERTRAG, UEBERGABE R40, P_masken36 R37, Memory Masken)
- [ ] Punkt 1: Messung vorher (Loecher je Cut), Ansatz, Umsetzung, Messung nachher
- [ ] Punkt 2: Sichtpruefung 53 Masken (11A0 + STAGE2), ROOM2020 Cut 3
- [ ] Tests (probes/r41_masken36.cmake), gezielte Laeufe

## Punkt 1 — STAGE3-6-Masken "ueberall Loecher"

### 1.1 Messung vorher (2026-10-24 08:10-08:40)

**Befund A — die Loecher der Uebersichtsbilder sind ein Fehler des Werkzeugs, nicht der Masken.**
Die Uebersichtsbilder `pri/STAGE3-6/<raum>/<cc>_port.png` (a8be31a7, 2026-10-23 12:59; danach die Nutzer-Aussage
"ueberall Loecher") und die Messung der UEBERGABE R40 ("17,3 % der Rechteck-Texel = Index 0, 3050 C1: 51 %") lasen den
Vordergrund-Atlas ueber `re15_port/tools/maske/original.py atlas()` -> `sld_decompress()`. Dessen langer Rueckverweis
(3-Byte-Form) war falsch:
```
s = raw - 0x1000 if raw & 0x800 else raw        # schon negativ (12-Bit-Vorzeichen)
off = ((s << 2) | (b3 >> 6))                     # Python: korrekt negativ, z.B. -8192
off = off - 0x10000 if off & 0x8000 else off     # bit 15 einer NEGATIVEN Zahl ist gesetzt -> -73728
```
Das Original FUN_800c47e8 (BIN/DEBUG.BIN +0x47E8, Bytes aus der ausgelieferten Datei gelesen):
`+0x4890 80 18 0a 00 sll v1,t2,2` / `+0x4894 25 10 43 00 or v0,v0,v1` / `+0x4898 00 14 02 00 sll v0,v0,16` /
`+0x489C 2a 12 03 08 j 0x800c48a8` / `+0x48A0 03 54 02 00 sra t2,v0,16` = signext16 der UNTEREN 16 Bit (so auch
engine/src/sld_common.c re15_sld_decode, das das Spiel benutzt). Jeder lange Rueckverweis las 65536 Byte zu weit
vorn (negativer Python-Index = Pufferende = Nullen -> Palettenindex 0 = "durchsichtig").
Gemessen (Skript Scratchpad `m41/py/a3_dekoder.py`, `a7_dekoder_alle.py`): Python-Entpacker gegen eine exakte
Nachbildung von re15_sld_decode, ALLE Atlas-Cuts aller Stages: **vorher 0 von 359 gleich** (Stage 1: 0/122, 2: 0/15,
3: 0/77, 4: 0/33, 5: 0/86, 6: 0/26; 39 Cuts brachen sogar mit "Entpacken fehlgeschlagen" ab, z.B. ROOM3040 Cut 0 lieferte
ein Bild der Breite 0); **nachher 359 von 359 bytegleich** (Index UND CLUT). Bekannt war die Abweichung schon in
`tools/r34n_g/schrift1150_masken.py` ("NICHT tools/maske/original.atlas() benutzen ... weicht fuer ROOM1150 Cut 2"),
behoben war sie nie. Fix: `original.py` kuerzt auf 16 Bit vor dem Vorzeichen (Kommentar mit den Adressen).

**Befund B — im Spiel haben die Rechtecke keine Index-0-Loecher.** Mit dem Engine-Dekoder (`a4_loecher.py`, 108
Leon-Sektionen der 15 Stage-3..6-Raeume, Elza-Dateien bytegleich): **Index 0 in den Atlasflaechen aller 3216 Rechtecke
= 0,00 %** (0 von 1 125 178 Texeln). Der Generator gen_masken.py (b8db7b2b) legt nur VOLLE Rechtecke in die gewachsene
Region (`rects_from_mask` = groesstes Vollrechteck), deshalb ist jedes Rechteck vollstaendig deckend. Blit-Pruefung mit
dem Engine-Dekoder: 1 125 178 gemalte Punkte, Abweichung zum Hintergrund im Mittel 12,70 / 765 (wie R37 §2.1: 12,63).
An der exe (Szene h3050c2 aus R37, ROOM3050 Cut 2 (-22000,-20768), Leon hinter der Kiste vorne links, Framedump
RE15_WINDOW_SCALE=1 Bild 120, mit/ohne RE15_NO_PRI): Leon vollstaendig verdeckt, kein Loch sichtbar (1379 Punkte mit
Maskenwirkung).

**Was im Spiel wirklich fehlt (die echten "Loecher"), gemessen ueber alle 108 Sektionen (`a6_luecken.py`):**
| Klasse | Bildpunkte |
|---|---|
| gezeichnet (nach dem Ausschluss der Spur P R37) | 727 561 |
| nur in AUSGESCHLOSSENEN Rechtecken (masken36_r37.c, Figur VOR dem Objekt wuerde uebermalt) | 396 737 |
| Kuenstler-Atlas einer platzierten Komponente, von KEINEM Rechteck gedeckt (Treppenkanten, Saeume, Kleinkram < 16 px) | 156 376 |
| eingeschlossene Luecken zwischen gezeichneten Rechtecken (binary_fill_holes) | 4 584 (130 Komponenten) |
| Atlas-Komponenten ohne bekannte Lage (kein Rechteck beruehrt sie, keine NCC-Verschiebung) | 452 144 Texel (927 Komponenten) |
Das Spiel zeichnet also 57 % der platzierten Kuenstler-Silhouetten (727 561 von 1 280 674); die Objektkanten sind
Treppen aus Vollrechtecken, und wo ein Teil der Rechtecke eines Objekts ausgeschlossen ist, bleibt ein loechriges Objekt.

