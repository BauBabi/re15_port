# Runde 37 — Spur R "kiste2feld" (2-Feld-Symbole in der Kisten-Liste wie RE2 aus zwei Kacheln)

Baum `.claude/worktrees/r37_kiste2feld`, Zweig `r37/kiste2feld`, Basis badd228f (= abgenommener Stand der Spur M aef88681
+ AUFTRAG/VERTRAG vom master-Stand). Wortlaut: AUFTRAG.md Nachtrag 9 + Orchestrator-Auftrag R.

## Wortlaut

> In der Kiste müssen die Icons offenbar so weit wie möglich rechts liegen, damit sie nicht vom Text überdeckt werden.
(Nachtrag 9 — Lage/Ebene von Spur M erledigt und abgenommen, M_abnahme_0.md; Spur R schliesst den dabei gefundenen
vorbestehenden Fehler derselben Symbole.)

Orchestrator-Auftrag R: 2-Feld-Symbole (49 px, z.B. Waffen) in der Kisten-Liste werden vollstaendig wie in RE2 gezeichnet —
heute liest der Port nur die schmale 40-Texel-Kachel, die Spalten 40..48 wiederholen Spalte 39 (verschmiert). RE2 zeichnet
die breite Zeile aus zwei Cache-Spalten (u1 0x78 @0x8007183c/@0x80071848, zweite Kachel FUN_80069c40 @0x80069cd8). Auch im
Inventar-Gitter und an anderen Stellen derselben Kachel-Logik pruefen.

## 0. Stand (fortlaufend)

- [x] Code gelesen (Block 7b, Kachel-Cache, inv_render_pc.c)
- [x] Messung vorher (Kiste, Inventar-Gitter, Statusschirm) — Item-Modal folgt
- [ ] RE2-Belege (zwei Cache-Spalten, zweite Kachel)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Riegel probes/r37_kiste2feld.cmake + gezielte Tests

## 1. Messung vorher (exe dieses Baums, Stand badd228f, Kopie re15_pc_r37k2f_vorher.exe)

### 1.1 Code (wo die Symbole herkommen)

- **Kisten-Liste** (re15_inv_screen.c Block 7b, box_cell_icon_sized): Op `RE15_INV_PAGE_ITEMTILE`, v = Item-Id, u = 0,
  Breite 25 (schmal) bzw. 49 (2-Feld, `RE15_BOX_KIND_WIDE` = Size 3), Hoehe 19. Gerastert in inv_render_pc.c
  (ITEMTILE-Zweig): `w == 80` -> ITPS-Breitbild, sonst ITEMALL-Kachel[v] (40x30) mit
  `sxp = (o->w >= 40) ? pxx : (pxx * 40) / o->w; if (sxp > 39) sxp = 39;` und `syp = (py * 30) / o->h`.
  Fuer w = 49 heisst das: Spalten 0..39 = Kachel 1:1 (NICHT gestaucht), Spalten 40..48 = Spalte 39 (Klemmung).
- **Inventar-Gitter** (Statusschirm Abschnitt 5 + Kisten-Schirm Abschnitt 7) und **Ausruest-Kasten** (Abschnitt 7 des
  Statusschirms, zwei 40er-Haelften aus Zelle eq/eq+1): SPRT 40x30 aus dem Symbol-Speicher s_icon8, Zelle k bei
  uv (k%3*40, k/3*30) (RE1.5 Tabelle @0x80076244). Belegt von inv_compose_cells: fuer 2-Feld-Waffen (Kind 1/2,
  re15_inv_screen_cache_tile = -2) schreibt der KOPF das ITPS-Breitbild 80 Texel breit ab uv der Kopfzelle, der
  SCHWANZ (Kind 2) wird uebersprungen. Liegt die Kopfzelle in Speicher-Spalte 2 (k = 2, 5, 8: u 80), landet die rechte
  Haelfte bei u 120..159 — ausserhalb des Speichers —, und die Schwanzzelle k+1 (u 0 der naechsten Speicherzeile) zeigt
  ALTEN Inhalt.
- **Item-Modal** (Aufnahme "Will you take ..."): render_pc.c item_modal_build_tex tastet das 112x72-Bild des ITPS-Blocks
  (re15_itps_pixel, u 0..111) — eigener Datenweg ohne 40er-Kachel (Messung §1.4).

### 1.2 Quellgrafik (Werkzeug scratchpad quellbild.py, Bild `R_belege/quelle_2feld_itemall_itps_alt_neu.png`)

ITEMALL.PIX (72 Kacheln 40x30) traegt fuer die 2-Feld-Waffen 0x0e..0x13 nur die LINKE HAELFTE des Breitbilds
(ITPS-Block id*0x3000+0x21A0, 80x30): Kachel == ITPS-Spalten 0..39 in 1195/1200 (0x0f..0x11), 1200/1200 (0x12, 0x13),
766/1200 (0x0e: aeltere Fassung des Flammenwerfers ohne Tank). Die rechte Haelfte (Schaft/Griff/Rohrende) gibt es NUR im
Breitbild. Spalten im Bild: ITEMALL 40x30 | ITPS 80x30 | Kiste ALT (49x19) | Kiste NEU (49x19, 80 -> 49 gestaucht).

### 1.3 Kiste und Gitter an der exe (echter Weg)

Werkzeug `probe_r37_kiste2feld_karte` (neu): Spielstand ROOM1150 Leon vor der Kiste (-25650,-12100, Gier 0, Lage wie
Spur M), Kiste Ring 0..5 = 0x0e..0x13 (Size 3, Mengen 1..6), 6 = 0x0B x3, 63 = 0x15 x100; Inventar 0/1 = 0x0F x6,
2/3 = 0x12 x4, 4 = 0x03 x15 (ausgeruestet), 5/6 = 0x13 x30, 7 = 0x15 x50, 8/9 = 0x0E x100 (Kopfzellen 0/2/5/8 = jede
Speicher-Spalte). exe: `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_WINDOW_SCALE=1 RE15_PAD_AT=40:A,90:A,100:D
RE15_FRAMEDUMP=95-115/1:f` (Viereck am Kisten-AOT F40, Box-Seite F90, RUNTER F100). debug.log
`[save] CONTINUE: resumed in room 1150`, `EXIT_AT: Bild 116`.

Werkzeug `probe_r37_kiste2feld_bild` (neu) vergleicht jedes Symbol PIXELGENAU (Kanal = 5 Bit << 3, ST_00-CLUT Zeile 0,
CLUT-Wert 0 = Listengrund (24,24,40); ausgenommen Auswahl-Band-Linien y 92/93/112/113 und das Ziffernfeld unten links)
gegen NEU (80x30-Breitbild, u = px*80/49, v = py*30/19) und ALT (ITEMALL-Kachel, Spalte 39 wiederholt):

| Bild | Zeile | Id | NEU gleich | ALT gleich | Spalten 40..48 = Spalte 39 |
|---|---|---|---|---|---|
| F99 | 0 | 0x0e | 451/867 | **867/867** | 9/9 |
| F99 | 1 | 0x0f | 337/867 | **867/867** | 9/9 |
| F99 | 2 | 0x10 | 305/818 | **818/818** | 9/9 |
| F99 | 3 | 0x11 | 305/818 | **818/818** | 9/9 |
| F99 | 4 | 0x12 | 542/867 | **867/867** | 9/9 |
| F110 (nach RUNTER) | 4 | 0x13 | 300/867 | **867/867** | 9/9 |

(Zeile 2/3 in F99 bzw. 2/3 in F110: 818 = 867 - 49, Bandlinie y 93 bzw. 112 liegt ueber der Zeile.) Befund bestaetigt fuer
ALLE sechs 2-Feld-Ids: das Symbol ist zu 100 % die 40er-Kachel 1:1 + 9 wiederholte Spalten — es zeigt nur die vordere
Haelfte der Waffe (Lauf), der Rest ist ein Streifen (Bild `R_belege/vorher_1150_kiste_F99_liste_gitter.png`, links).

Inventar-Gitter im selben Bild (Kisten-Schirm, Gitter bei list 215/26) und im **Statusschirm** (`RE15_PAD_AT=40:S`, F80,
Bild `R_belege/vorher_1150_status_F80_gitter.png`), deckende Texel je Zelle gegen die ITPS-Haelfte:

| Kopfzelle (Speicher-Spalte) | Id | linke Haelfte (Zelle k) | rechte Haelfte (Zelle k+1) |
|---|---|---|---|
| 0 (Spalte 0) | 0x0F | 1135/1135 | 1183/1183 (Statusschirm; im Kisten-Schirm liegt der Cursor darueber) |
| 2 (Spalte 2) | 0x12 | 1136/1136 | **257/1200** — zeigt eine Spraydose (alter Speicherinhalt) |
| 5 (Spalte 2) | 0x13 | 1088/1088 | **0/1200** — leer |
| 8 (Spalte 2) | 0x0E | 1040/1040 | **0/1200** — leer |

Gleiche Fehlerklasse "2-Feld-Symbol nur halb": im Gitter fehlt die rechte Haelfte, sobald die Kopfzelle in Speicher-Spalte 2
liegt. Erreichbarkeit im Spiel: die einzige platzierte 2-Feld-Waffe ist die MC51 (0x13, ROOM1110/1111 main00 @0x00B5A
`50 0a 09 31 00 00 2e d7 d2 f6 e8 03 20 03 13 00 64 00 62 00 03 00`, Zensus aller 164 Item_aot_set aller RDTs,
scratchpad breit_zensus.py); sie landet beim Aufnehmen immer in Zelle 0/1 (Front-Shift). Der Gitter-Fehler tritt also erst
mit ZWEI 2-Feld-Waffen auf (Debug-Gabe oder kuenftige Platzierung) — die Kisten-Liste dagegen trifft schon die MC51,
sobald Leon/Elza sie in die Kiste legt.

## 2. RE-Belege

(folgt)

## 3. Umsetzung

(folgt)

## 4. Messung nachher

(folgt)

## 5. Tests

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
