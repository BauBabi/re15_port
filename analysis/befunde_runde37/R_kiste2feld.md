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
- [x] Messung vorher (Kiste, Inventar-Gitter, Statusschirm, Item-Modal)
- [x] RE2-Belege (zwei Cache-Spalten, zweite Kachel, Referenzbild, RE1.5-Gitter)
- [x] Umsetzung (ebc575f1; Fortsetzung 2026-10-13: Werkzeug 80082cf7 geprueft, Ausruest-Kasten nachgemessen)
- [x] Messung nachher (Kiste Leon 1150 / Elza 1151, Inventar-Gitter Kisten- und Statusschirm, Ausruest-Kasten)
- [x] Riegel probes/r37_kiste2feld.cmake (unit_r37_kiste2feld, integration_r37_kiste2feld) + gezielte Tests

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

### 1.4 Item-Modal (Aufnahme) — nicht betroffen

Echter Weg: Spielstand ROOM1110 Leon bei (-9950,-2970) Gier 1024 (Werkzeug probe_r37_kiste_karte der Spur M),
CONTINUE, Viereck F40 am MC51-AOT (main00 @0x00B5A, Rechteck x -10450..-9450 / z -2350..-1550) -> "Will you take th..."
mit dem Bild der MC51 (F79/F99; zwei andere Standorte trafen die Untersuchen-Zone "Weapons seized, ..."). Bild F99
(`R_belege/vorher_1110_modal_mc51_F99.png`): Bildfeld x 105..213 / y 73..141 (109x69); nur 2 Paare gleicher
Nachbarspalten, laengster Lauf 1 (kein Streifen), im rechten Drittel 720 von 2553 Pixeln Waffe (Schaft/Griff). Der Modal
tastet das 112x72-Bild des ITPS-Blocks ueber die volle Breite (render_pc.c item_modal_build_tex, u < 112) — kein 40er-Kachel-
Weg, keine Klemmung. **Kein Eingriff noetig.**

## 2. RE-Belege (selbst disassembliert: re2_disasm.py auf info/re2leon/PSX.EXE, re15_disasm.py auf info/Re1.5/PSX.EXE)

### 2.1 RE2: das Zeilen-Symbol ist EIN POLY_FT4 ueber zwei Cache-Kacheln (FUN_800710dc)

Schleife der 5 Zeilen ab @0x800717ac (s0 = Prim+0x22, also Feld f bei s0+(f-0x22)):

```
800717b4: addiu t0,zero,40         ; u0 = u2 = 40
800717bc: addiu s7,zero,146 / 800717c0: addiu s6,zero,127   ; y2-y0 = 146-127 = 19
800717c8: addiu s2,zero,270 / 800717d0: addiu s1,zero,240   ; v: je Zeile -30 (@0x800717f4 / @0x80071830)
800717d4: addiu t1,t1,146          ; x0 = x2 = boxX+0x92
80071824: lbu v0,-30590(at)        ; box[slot].size (0x800d4a6a + slot*4)
8007182c: beq v0,zero,0x80071840   ; size 0 -> schmal
80071834: addiu a3,zero,48         ; breit: Breite 48
80071838: j 0x80071848
8007183c: addiu t1,zero,120        ; (Delay-Slot) breit: u1 = u3 = 120
80071840: addiu a3,zero,24         ; schmal: Breite 24
80071844: addiu t1,zero,80         ; schmal: u1 = u3 = 80
80071848: sb t1,-14(s0)            ; Prim+0x14 = u1
8007184c: sb t0,-22(s0)            ; Prim+0x0c = u0
80071850: sb s1,-21(s0) / 80071854: sb s1,-13(s0)   ; v0 = v1
80071858: sb t0,-6(s0)             ; Prim+0x1c = u2
8007185c: sb s2,-5(s0) / 80071864: sb s2,3(s0)       ; v2 = v3 = v0 + 30
80071860: sb t1,2(s0)              ; Prim+0x24 = u3
80071880: addu v1,t1,a3 / 80071884: addiu v1,v1,147  ; x1 = x3 = boxX+0x93+Breite
```

Breites Symbol: x0..x1 = boxX+146 .. boxX+195 = **49 px** (Kante exklusiv), u **40..120 = 80 Texel**, v 30 Texel auf
**19 px** — die GANZE Breitkunst wird gestaucht. Schmal: 25 px, u 40..80 = 40 Texel. Entscheidung breit/schmal ueber das
Size-Byte der Kiste (im Port `RE15_BOX_KIND_WIDE` = Size 3).

### 2.2 RE2: woher die zwei Kacheln kommen (FUN_80069c40, Aufrufer im Kisten-Code)

```
80069c74: addiu v0,zero,20 / 80069c78: sh v0,20(sp)     ; RECT.w = 20 hw = 40 Texel (8bpp)
80069c7c: addiu v0,zero,30 / 80069c84: sh v0,22(sp)     ; RECT.h = 30
80069c88..cb4: x = ((cell%3)*40 - 128)/2 + 512          ; Spalte cell%3
80069cb8..cc4: y = (cell/3)*30 + 256
80069cc8: jal 0x80090664 (LoadImage) / 80069cd0: jal 0x800903a0 (DrawSync)   ; linke Kachel = src
80069cd8: addiu v0,zero,19 / 80069cdc: beq s2,v0,0x80069d68                  ; Id 0x13 -> keine zweite
80069ce0: addiu v0,s2,-14 / 80069ce4: sltiu v0,v0,0x6 / 80069ce8: beq v0,zero ; nur Id 14..18
80069cec: addiu v1,s0,1                                                      ; Zelle+1 (Spalte daneben)
80069d3c: jal 0x80090664                                                     ; LoadImage(Zelle+1, src)
80069d4c: lui a1,0x1 / 80069d50: ori a1,a1,0x9320 / 80069d58: jal 0x80090664 / 80069d5c: addu a1,s1,a1
                                                         ; LoadImage(Zelle+1, src + 0x19320) = rechte Haelfte
```

Aufrufer: Zeilen-Kacheln `addiu s2,zero,10` @0x8007009c, `addiu s2,s2,3` @0x800700c4 (Zellen 10, 13, 16, 19, 22 = u 40,
v 90..210 — genau die u0/v0 des POLY_FT4), Quelle `a2 = 0x8019c000 + Id*0x4b0` (@0x80070098-b8, Id*5*15*16). Die rechte
Haelfte liegt 0x19320 = 86*1200 Byte dahinter = **Kachel Id+0x56**; FUN_800703b8 nutzt dieselbe Rechnung fuer die
Inventar-Seite (`(DAT_800d4a3c + 0x56) * 0x4b0`). Datenbeleg: info/re2leon/COMMON/DATA/ITEMALL.PIX = 127200 Byte = 106
Kacheln (86 + 20 rechte Haelften). Im VRAM: Zeile k links bei Zelle 10+3k = (468, 256+30*(3+k)) 20x30 hw, rechts bei
Zelle 11+3k = (488, ...) — direkt daneben, deshalb ueberstreicht u 40..120 beide.

### 2.3 Gegenprobe am RE2-Bild (analysis/itembox_re2/referenz/re2_itembox_original.png)

Werkzeug scratchpad re2_maske.py / re2_varianten.py / re2_schmal.py: RE2-ITEMALL.PIX Kachel[Id] + Kachel[Id+0x56] (80x30)
gegen die Symbol-Pixel des laufenden RE2 (Waffen-/Grund-Maske je Pixel, Grund = Kachelblau, Unendlich-Zeichen unten links
ausgenommen):

| Zeile (RE2-Id) | NEU 80 -> 49, gerundet | NEU 80 -> 49, abgerundet | ALT (40er 1:1, Spalte 39 wdh.) | nur linke Kachel gestaucht |
|---|---|---|---|---|
| 0 Gatling Gun (0x12) | **770/771** | 759/771 | 624/771 | 616/771 |
| 4 R. Launcher (0x11) | **752/771** | 746/771 | 439/771 | 395/771 |
| 1/3 F. Aid Spray (0x23, schmal 40 -> 25) | **475/475** | 469/475 | - | - |

RE2 zeigt also die ganze Waffe aus beiden Kacheln. Die Abtastung ist die des PSX-GPU-Rasterizers in DuckStation (Fixkomma
12 Bit, Startwert +halb, Schritt (Texel<<12)/Pixel): `t = (2048 + p * ((n_tex << 12) / n_px)) >> 12` = auf die naechste
Ganzzahl gerundet — fuer 80 -> 49 identisch mit (p*80+24)/49, fuer 30 -> 19 mit (p*30+9)/19, fuer 40 -> 25 mit
(p*40+12)/25 (alle p nachgerechnet). Der Port nutzt dasselbe Modell schon fuer die Gouraud-Kaesten (inv_render_pc.c
"ATTRIB_SHIFT=12 ... +half init").

### 2.4 RE1.5: die Breitkunst und das Inventar-Gitter

- RE1.5 hat keine Kiste und kein ITEMALL-Gegenstueck zu RE2s Kachel Id+0x56: ITEMALL.PIX hat 72 Kacheln (86400 Byte), die
  2-Feld-Kunst liegt als EIN 80x30-Bild im ITPS-Block (Aufnahme: `a2 = *(0x800ac77c)+0x21A0` @0x8001e0b8-c8 an
  FUN_8004dc4c). FUN_800492b8 Modus 1: `ori v0,zero,0x28` @0x80049314 (40 hw = 80 Texel), `ori v0,zero,0x1e` @0x8004931c,
  x = ((Zelle%3)*40-128)/2+704 @0x8004933c-54, y = (Zelle/3)*30+256 @0x80049360-6c, LoadImage `jal 0x80068c88` @0x80049370.
  Das ITPS-Breitbild ist damit das RE1.5-Gegenstueck zu RE2s zwei Kacheln (Spalten 0..39 = Kachel[Id], 40..79 = Id+0x56).
- Einfuegen einer 2-Feld-Waffe (FUN_8004dc4c, Gate `addiu v0,v0,-14 / sltiu v0,v0,0x6` @0x8004dc74-78): erst 8x
  FUN_80049390 (Zelle i -> i+2, @0x8004dc98 .. @0x8004de3c), dann `addu a0,zero,zero` / `ori a1,zero,0x1` @0x8004de44-48,
  `j 0x8004df08` / `addu a2,s2,zero` @0x8004deb4-b8 -> FUN_800492b8(0, 1, Breitbild): das 80x30-Bild landet IMMER ueber
  Zelle 0 und 1.
- FUN_80049390 (Verschieben): RECT w `ori v0,zero,0x14` @0x800493ac (40 Texel), h `ori v0,zero,0x1e` @0x800493b4, Quelle
  aus a0, Ziel aus a1, MoveImage `jal 0x80068d50` @0x8004945c — JE 40x30-ZELLE. Zusammenschieben ruft es je Platz
  (`jal 0x80049390` @0x8004db30, a0 = s0, a1 = s0-1). Damit traegt im Original die Zelle k immer die linke, Zelle k+1 die
  rechte Haelfte — auch wenn k in Speicher-Spalte 2 liegt (Zelle 2 -> u 80, Zelle 3 -> u 0 der naechsten Zeile).
- Port-Fehler (inv_compose_cells): schreibt das Breitbild 80 Texel breit ab der Kopfzelle; fuer Kopfzellen 2/5/8 liegt die
  rechte Haelfte dann bei u 120..159 statt in Zelle k+1 (gemessen §1.3).

### 2.5 Folgerung fuer den Port

1. Kisten-Liste: das 2-Feld-Symbol tastet die BREITKUNST (ITPS-Block Id*0x3000+0x21A0, 80x30) auf 49x19 ab, mit RE2s
   Spanne u 40..120 (Op-Feld u = 80 = u1-u0, @0x8007183c/@0x800717b4) und der GPU-Abtastung aus §2.3. Lage/Groesse bleiben
   wie Spur M (x 155..203, 49x19). Dieselbe Abtastregel gilt fuer das schmale Symbol (u 40..80, Op-Feld u = 40 = 0x50-0x28,
   @0x80071844/@0x800717b4) — RE2 hat EINEN Prim fuer beide Breiten.
2. Inventar-Gitter: die zwei Haelften des Breitbilds gehoeren je in die eigene Zelle (Kopf k: Spalten 0..39 bei uv(k),
   Schwanz k+1: Spalten 40..79 bei uv(k+1)) — der Zustand, den das Original durch LoadImage ueber Zelle 0/1 und
   zellenweises MoveImage herstellt. Fuer Kopfzellen in Speicher-Spalte 0/1 ist das bitgleich zum bisherigen
   80-breiten Schreiben.

### 2.6 RE1.5: Ausruest-Kasten = dieselben Speicherzellen (FUN_80049a5c, selbst disassembliert)

Der Ausruest-Kasten des Statusschirms zeichnet eine 2-Feld-Waffe als ZWEI 40x30-SPRTs aus den Speicherzellen eq und
eq+1 — genau den Zellen, die inv_compose_cells belegt:

```
80049d7c: ori v0,zero,0x1 / 80049d80: bne v1,v0,0x80049e8c   ; Flag (0x800b10ae + eq*4) == 1 (Kopf)
80049d8c: addiu at,at,25156 (0x80076244) ... 80049d9c: sb v0,12(s1)    ; u = uv-Tabelle[eq]
80049db4: addiu at,at,25158 (0x80076246) ... 80049dc4: sb v0,13(s1)    ; v
80049dcc: lhu v0,9692(v0) (0x800b25dc) / 80049dd4: sh v0,8(s1)         ; x = 25dc
80049df4: jal 0x8006b538 (AddPrim)
80049e08: addiu v0,v0,1 / 80049e14: addiu at,at,25156 / 80049e24: sb v0,12(s1)   ; u = uv-Tabelle[eq+1]
80049e60: addiu v0,v0,40 / 80049e64: sh v0,8(s1)                        ; x = 25dc + 0x28
```

(Decompilat RE_15_Quellcode_V2/FUN_80049a5c.c Z. 65-78 sagt dasselbe.) Der Port bildet das in re15_inv_screen.c
Abschnitt 7 schon 1:1 ab; er war nur so falsch wie die Zellen-Belegung: mit der Kopfzelle in Speicher-Spalte 2 zeigte
die rechte Haelfte alten Speicherinhalt (Messung §4.3).

## 3. Umsetzung (ebc575f1, unveraendert uebernommen)

Neue Dateien (VERTRAG §1.4: neue Logik in neuen Dateien):
- `re15_port/include/re15_kiste2feld_r37.h` — Konstanten mit Beleg: `RE15_KISTE2FELD_SPANNE_SCHMAL 40` (RE2 u1
  `addiu t1,zero,80` @0x80071844 minus u0 `addiu t0,zero,40` @0x800717b4), `RE15_KISTE2FELD_SPANNE_BREIT 80`
  (`addiu t1,zero,120` @0x8007183c, gespeichert @0x80071848/@0x80071860), `RE15_KISTE2FELD_SPANNE_HOCH 30`
  (@0x800717c8/@0x800717d0), `RE15_KISTE2FELD_ITPS_BLOCK 0x3000` + `RE15_KISTE2FELD_ITPS_BREITBILD 0x21A0`
  (@0x8001e0b8-c8), `RE15_KISTE2FELD_BREIT_W 80`/`_H 30` (`ori v0,zero,0x28` @0x80049314, `ori v0,zero,0x1e`
  @0x8004931c), `RE15_KISTE2FELD_HAELFTE_W 40` (FUN_80049390 `ori v0,zero,0x14` @0x800493ac);
  `re15_kiste2feld_texel()` = GPU-Abtastung (§2.3, am RE2-Bild gemessen).
- `re15_port/engine/src/kiste2feld_r37.c` — `re15_kiste2feld_zelle()`: Zelle -> (Id, erste Breitbild-Spalte) fuer
  Kopf (Flag 1, `ori v0,zero,0x1` @0x8004de64) und Schwanz (Flag 2, `ori v0,zero,0x2` @0x8004de70).

Haken an Ort und Stelle (VERTRAG §1.4 Spur R, Kommentar "Runde 37 Spur R"):
- `re15_inv_screen.c` box_cell_icon_sized: Op-Feld u = Quell-Spanne (2-Feld 80, schmal 40) statt 0.
- `re15_inv_screen.h`: Enum-Notiz RE15_INV_PAGE_ITEMTILE (u = Spanne).
- `inv_render_pc.c` ITEMTILE-Zweig: Spanne 80 (oder w == 80) -> 80x30-Breitbild des ITPS-Blocks, sonst die 40x30-Kachel
  ITEMALL[v]; beide mit `re15_kiste2feld_texel` auf w x h (vorher: w >= 40 1:1 + Klemmung auf Spalte 39, w < 40
  abgerundet). Die SCHMALEN Symbole laufen damit ueber dieselbe RE2-Abtastung (RE2 hat EINEN POLY_FT4 fuer beide
  Breiten, §2.1; RE2-Bild F. Aid Spray 475/475 gerundet gegen 469/475 abgerundet, §2.3) — an der exe gemessen §4.1
  (0x0B, 0x15). Damit ist die Frage "brauchen auch die schmalen Symbole die RE2-Abtastung?" mit JA beantwortet und
  schon in ebc575f1 gebaut.
- `inv_render_pc.c` inv_compose_cells: jede 2-Feld-Zelle schreibt ihre EIGENE 40er-Haelfte an ihr eigenes uv (Kopf
  Spalten 0..39, Schwanz 40..79), statt dass der Kopf 80 Texel ab seinem uv schreibt (§2.4). Der Ausruest-Kasten
  (§2.6) liest dieselben Zellen und ist damit mit repariert.

Lage/Groesse der Spur M unveraendert (x = 204 - Breite, 49x19 / 25x19, Ziffern im Symbol).

Pruefung des Fortsetzungsstands 80082cf7 (Bildwerkzeug, vom Orchestrator ungeprueft gesichert): baut warnungsfrei;
Erwartung NEU = eigene GPU-Abtastung (nicht aus dem Port-Kopf), ALT = alter Zeichner; schmale Zeilen mit derselben
Abtastung (u 40..80 @0x80071844), Ziffernfeld nur fuer Ids < 0x22 (`sltiu id,0x22` @0x80049124). Gegen die Vorher-exe
meldet es jede Zeile FALSCH (§5), gegen die Nachher-exe jede ok — es unterscheidet beide Staende. Ergaenzt um den Modus
`ausruest` (§2.6) und das Kartenwerkzeug um die Belegung `ausr` (MC51 ausgeruestet, Kopf in Zelle 5 = Speicher-Spalte 2).

## 4. Messung nachher (exe dieses Baums, Code-Stand ebc575f1, gleiche Wege wie §1.3)

### 4.1 Kisten-Liste Leon ROOM1150 (CONTINUE, Viereck F40 am Kisten-AOT, F90 Box-Seite, RUNTER F100/F115, HOCH F130/F145/F160)

`probe_r37_kiste2feld_bild kiste` — Pixel gleich der Erwartung NEU (GPU-Abtastung der breiten Kunst bzw. der 40er-Kachel);
in Klammern ALT = alter Zeichner, wdh = Spalten 40..48 gleich Spalte 39:

| Bild | Zeile 0 | Zeile 1 | Zeile 2 | Zeile 3 | Zeile 4 |
|---|---|---|---|---|---|
| F99 | 0x0e 867/867 (ALT 453, wdh 0/9) | 0x0f 867/867 (319, 0/9) | 0x10 818/818 (288, 0/9) | 0x11 818/818 (288, 0/9) | 0x12 867/867 (401, 4/9) |
| F110 | 0x0f 867/867 | 0x10 867/867 | 0x11 818/818 | 0x12 818/818 (wdh 4/9) | **0x13** 867/867 (273, 0/9) |
| F125 | 0x10 867/867 | 0x11 867/867 | 0x12 818/818 | 0x13 818/818 | 0x0B schmal 411/411 (ALT 333) |
| F170 | 0x15 schmal 315/315 (ALT 198) | 0x0e 867/867 | 0x0f 818/818 | 0x10 818/818 | 0x11 867/867 |

Alle sechs 2-Feld-Ids in jeder Zeilenlage (auch Auswahlzeile 2 und unter dem Band) = ganze Breitkunst; keine Zeile hat
mehr 9/9 wiederholte Spalten (0x12: 4 der Spalten 40..48 gleichen Spalte 39 auch in der Quelle — gleichfarbiger Lauf
des Rohrs, nicht geklemmt). Bild `R_belege/nachher_1150_kiste_F125_symbole_alt_neu.png` (links vorher, rechts nachher,
4x): vorher Lauf + Streifen, nachher die ganze Waffe mit Schaft/Griff; dazu `nachher_1150_kiste_F99_symbole_alt_neu.png`
und `nachher_1150_kiste_F99_liste_gitter.png` (ganzer Schirm).

Elza ROOM1151 (gleicher Weg, integration_r37_kiste2feld Lauf `elza`): F99 + F110 je 5/5 Zeilen ok, Gitter 6/6.

### 4.2 Inventar-Gitter (Kisten-Schirm F99, Statusschirm F80)

| Kopfzelle (Speicher-Spalte) | Id | links (Zelle k) | rechts (Zelle k+1) vorher -> nachher |
|---|---|---|---|
| 0 (0) | 0x0F | 1135/1135 | 1183/1183 -> 1183/1183 |
| 2 (2) | 0x12 | 1136/1136 | 257/1200 -> **1200/1200** |
| 5 (2) | 0x13 | 1088/1088 | 0/1200 -> **1200/1200** |
| 8 (2) | 0x0E | 1040/1040 | 0/1200 -> **1200/1200** |

Kisten-Schirm F99 gleich (Zelle 0/1 unter dem Cursor ausgenommen).

### 4.3 Ausruest-Kasten (Statusschirm, MC51 ausgeruestet, Kopf Zelle 5 = Speicher-Spalte 2; Kasten bei x 0x82 / y 0x3a)

| | linke Haelfte (x 130) | rechte Haelfte (x 170) |
|---|---|---|
| vorher (re15_pc_r37k2f_vorher.exe) | 1088/1088 | **530/1200 FALSCH** |
| nachher | 1088/1088 | **1200/1200** |

Bild `R_belege/nachher_1150_status_ausr_F80_alt_neu.png` (links vorher, rechts nachher, 2x): vorher endet die MC51 im
Kasten nach der vorderen Haelfte und die Schwanzzellen im Gitter zeigen Spraydose/leer; nachher vollstaendig.

### 4.4 Item-Modal

Nicht betroffen (§1.4, eigener 112x72-Weg) — nicht angefasst, nicht neu gemessen.

## 5. Tests (probes/r37_kiste2feld.cmake, nur dort registriert)

- **unit_r37_kiste2feld** (test_r37_kiste2feld.c, echte Kisten-FSM + Op-Liste, 0,01 s): (A) alle Ids 1..255 auf der
  Auswahlzeile: 6 2-Feld-Ids = ITEMTILE 49x19 mit Spanne u 80 (= RE2 120 - 40), 249 schmale = 25x19 mit Spanne 40;
  (B) Gitter-Belegung Kopf 0/2/5/8: 4 Kopf- (Spalten 0..39) + 4 Schwanzzellen (40..79), jede Zelle mit
  re15_inv_screen_cache_tile == -2 hat eine Zuordnung, schmale/leere keine; Masse 0x28/0x1e/0x14 hw; (C) GPU-Abtastung
  80->49 = (p*80+24)/49, 30->19 = (p*30+9)/19, 40->25 = (p*40+12)/25, streng steigend, n->n Identitaet.
  **Gegenprobe** (alter Op-Vertrag u = 0 in box_cell_icon_sized, nur das Testziel neu gebaut, danach zurueck und neu
  gebaut): exit 1, `=== 255 FEHLER ===` (jede Id: "Spanne u 0").
- **integration_r37_kiste2feld** (echte exe, 45 s): vier Laeufe auf dem echten Weg (Spielstand, CONTINUE) —
  `kiste` Leon 1150 (26 ok: 4 Bilder x 5 Zeilen + Gitter 6), `elza` 1151 (16 ok), `status` (8 ok), `ausruest` (10 ok);
  Soll-Zahl der ok-Zeilen je Lauf fest, sonst FATAL. **Gegenprobe** mit der Vorher-exe
  (`cmake -DRE15_PC_EXE=.../re15_pc_r37k2f_vorher.exe ... -P test_r37_kiste2feld.cmake`): exit 1 im ersten Lauf,
  `KISTE2FELD-BILD FEHLER 5` (F99, alle fuenf 2-Feld-Zeilen); einzeln gegen die Vorher-exe: F110/F125/F170 je
  FEHLER 5 (auch die schmalen 0x0B/0x15: ALT-Abtastung), Statusschirm-Gitter FEHLER 3, Ausruest-Kasten FEHLER 1.
- Werkzeuge: probe_r37_kiste2feld_karte (Belegungen alle/mc51/ausr), probe_r37_kiste2feld_bild (kiste/gitter/ausruest).

**Gezielte Laeufe** (keine volle Suite):

- eigene: `ctest --test-dir re15_port/build -R "r37_kiste2feld" --timeout 900 --output-on-failure` ->
  `100% tests passed, 0 tests failed out of 2`
- Module des Diffs (re15_inv_screen.c, inv_render_pc.c: Inventar-, Kisten-, Karten-, Dokumenten-Schirm, Symbol-Cache,
  Item-Modal, ersetzte Kacheln 0x40/0x42/0x45) inkl. Spur-M-Riegel unit_r37_kiste / integration_r37_kiste: §5.1,
  `100% tests passed, 0 tests failed out of 84`.

### 5.1 Lauf Module (Bau nach dem Zuruecksetzen der unit-Gegenprobe, Endstand des Codes)

`ctest --test-dir re15_port/build -R "r37_kiste|itembox|inv_|inventar|map_|karte|dokument|dachkiste|kartenitems|item_icon|item_modal|r30_sicherung_bild|r36_schluessel" --timeout 900 -j 4 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 84`** (Total Test time (real) = 1493.86 sec). Darin: unit_r37_kiste2feld,
integration_r37_kiste2feld, Spur-M-Riegel **unit_r37_kiste** und **integration_r37_kiste** (4 Laeufe inkl. Taste gehalten,
Rahmen bitgleich, Symbol buendig rechts mit freier Spalte davor — auch mit der 2-Feld-Zeile 49 px), unit_itembox_*,
unit_room1150_itembox, unit_inv_fsm, unit_item_icon, unit_item_modal, unit/integration_r30_sicherung_bild (ersetzte
Kachel 0x40 im neuen Abtastweg), unit/integration_r36_schluessel* (Kacheln 0x42/0x45), unit_map_*, Karten-Riegel
r3x_karte*, integration_r34n_e_dokumente_bild, integration_r36_kartenitems_bild, integration_r36_dachkiste,
integration_map_raum_live.

## 6. OFFEN

- Keine offenen Punkte des Auftrags R. Restabweichung der Abtastung am RE2-Bild (§2.3): R. Launcher 752/771, Gatling
  770/771 Masken-Pixel — die gerundete Abtastung ist das beste der gemessenen Modelle (abgerundet 746/759, ALT 439/624);
  die Restpixel wurden nicht weiter aufgeschluesselt (Kandidat: Waffen-/Grund-Maske des Referenzbilds an Farbgrenzen).
- Erreichbarkeit: die Gitter-/Ausruest-Haelfte betrifft nur Kopfzellen in Speicher-Spalte 2 — im Spiel erst mit zwei
  2-Feld-Waffen (einzige platzierte ist die MC51, §1.3). Die Kisten-Liste trifft die MC51 sofort.

## 7. Fuer den Nutzer

- Neue Assets: **keine** (Paket-Gate unveraendert; die rechte Haelfte kommt aus dem vorhandenen ITEM/ITPS.ITP).
  Sprachdateien: keine.
- Sichtbar neu: In der Kiste zeigen Waffen (2-Feld-Symbole) jetzt die ganze Waffe — wie RE2 auf 49x19 gestaucht —
  statt der vorderen Haelfte mit einem Streifen. Die schmalen Symbole sind wie in RE2 gerundet abgetastet (kleine
  Pixel-Unterschiede, keine Lage-Aenderung). Im Inventar und im Ausruest-Kasten ist eine Waffe auch dann vollstaendig,
  wenn sie an dritter Stelle des Symbol-Speichers liegt (z.B. eine zweite Waffe neben der MC51).
- Bedienung unveraendert. Keine Entscheidungsfrage.
