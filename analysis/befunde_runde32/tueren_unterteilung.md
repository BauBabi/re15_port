# Runde 32 — Verzerrung in den RE2-Tuersequenzen (DivideGT3 / Objekt-Flag 0x20)

Nutzer-Befund woertlich: "Die Türen haben eine komische Verzerrung beim öffnen in den Türsequenzen. Das ist noch falsch."

Zweig `r32/tueren-unterteilung`, Arbeitsbaum `.claude/worktrees/r32_unterteilung`.

## Stand (laufend fortgeschrieben)

- [ ] 1. RE: 0x8008ebf4 = DivideGT3? DIVPOLYGON3-Felder, ndiv, pih/piv, Init-Stelle
- [ ] 2. Messen vorher (Framedumps DOOR13-Seite + Standardblatt, Knick-Mass)
- [ ] 3. Bauen (door_scene_pc.c, nur Objekte mit Bit 0x20)
- [ ] 4. Messen nachher + Kontaktbogen 184 Seiten, ohne-0x20 bitgleich
- [ ] 5. Riegel r32_unterteilung.cmake, RE15_MIN_TESTS anheben

## 1. RE: Flag 0x20 = PsyQ `DivideGT3` mit ndiv 3 (alles selbst disassembliert, `info/re2leon/PSX.EXE`)

Werkzeuge: `build/r32_unt/re/gte_dis.py` (Kopie aus `build/tor_1170/re_zeichnen/`, MIPS + GTE),
`build/r32_unt/re/elf_text.py` (.text eines PsyQ-Objekts), `build/r32_unt/re/cmp_sdk.py` (SDK-Code wortweise gegen die EXE).

### 1.1 Aufrufstelle in FUN_8001468c (je Dreieck)

```
80014814 4a280030 RTPT sf=1                ; die drei Modell-Vertices (lwc2 @0x800147f4..808)
80014818 96e20144 lhu v0,324(s7)           ; Objekt-Flags
80014820 30420020 andi v0,v0,0x20
80014824 1040001f beq v0,zero,0x800148a4
80014834..800148a0  lwl/lwr + swl/swr      ; Vertex 0/1/2 (je 8 B SVECTOR, s6/s3/s1) -> [0x800c3a80]+560/+568/+576
800148d0 4b400006 NCLIP ; 800148f8 bgez -> gezeichnet bei MAC0 >= 0
80014958 NCCT sf=1 lm=1 ; 80014980..88 swc2 RGB0/1/2,4/16/28(s0) (rgb0 traegt im Code-Byte 0x34)
80014994 AVSZ3 ; 800149a8/ac srl v0,a0,6 / beq -> verworfen bei otz < 64
800149bc..80014a24  OT-Platz s4 nach flags & 0xc0 (08_re_zeichnen.md 2.2)
80014a28 96e20144 lhu v0,324(s7)
80014a30 30420020 andi v0,v0,0x20
80014a34 10400022 beq v0,zero,0x80014ac0   ; ohne 0x20: addPrim des einen POLY_GT3 (@0x80014ac0..e4)
80014a38 26020018 addiu v0,s0,24  ; a4 uv1   (sw 16(sp))
80014a40 26020024 addiu v0,s0,36  ; a5 uv2   (sw 20(sp))
80014a48 26020004 addiu v0,s0,4   ; a6/a7/a8 = rgb0 DREIMAL (sw 24/28/32(sp) @0x80014a58/5c/60)
80014a54 2607000c addiu a3,s0,12  ; a3 uv0
80014a64 24c2000c addiu v0,a2,12  ; a11 divp = [0x800c3a80]+12  (sw 44(sp) @0x80014a7c)
80014a68 24c40230 addiu a0,a2,560 ; a0 v0
80014a6c 24c50238 addiu a1,a2,568 ; a1 v1
80014a70 8cc30008 lw v1,8(a2)     ; a9 Paketzeiger [work+8] (sw 36(sp) @0x80014a94)
80014a74 24c60240 addiu a2,a2,576 ; a2 v2
80014a78 afb40028 sw s4,40(sp)    ; a10 ot = derselbe OT-Platz wie das ungeteilte Dreieck
80014a90 0c023afd jal 0x8008ebf4
80014aa4 ac620008 sw v0,8(v1)     ; neuer Paketzeiger -> [work+8]; das eigene POLY_GT3 s0 wird NICHT eingehaengt
```

### 1.2 0x8008ebf4 = `DivideGT3` (PsyQ libgte)

Signatur (`LibRef47.pdf`, `LIBGTE.H` DIVPOLYGON3 Zeile 186-194):
`POLY_GT3 *DivideGT3(SVECTOR *v0,*v1,*v2, u_long *uv0,*uv1,*uv2, CVECTOR *rgb0,*rgb1,*rgb2, POLY_GT3 *s, u_long *ot, DIVPOLYGON3 *divp)`
- 12 Argumente, genau die Belegung von 1.1.
- Beleg per Objektvergleich: `psyq-4.7-converted-full/lib/libgte/dvgt3_04.o` exportiert `DivideGT3` (0x1e0 B, importiert
  `RotAverageNclip3`, `ReadSZfifo3`, `RCpolyGT3A`). Die RE2-Funktion ist 0x1e0 B lang (0x8008ebf4..0x8008edc8) mit denselben
  Speicherzugriffen in anderer Registerwahl (aelterer Compilerlauf): divp->cr[0].r0/r1/r2 = divp+24/48/72
  (@0x8008ec38/3c/40 `sw ..,168/172/176(s0)`), v0/v1/v2 -> divp+24/48/72 (@0x8008ec58..cb4),
  `jal 0x8008edec` = RotAverageNclip3 (@0x8008ecf0; RTPT @0x8008ee08, NCLIP @0x8008ee1c, **`bgtz v0` @0x8008ee34 = nur MAC0 > 0**,
  SXY -> r0/r1/r2.sxy, AVSZ3), `blez v0,0x8008ed94` (@0x8008ecf8, Rueckseite -> nichts), `jal 0x8008edcc` = ReadSZfifo3
  (SZ1/2/3 -> r0/r1/r2.sz), ot -> divp+20 (@0x8008ed0c), rgbc <- *rgb0 (@0x8008ed10..20), clut <- uv0[1] hi (@0x8008ed24/2c),
  tpage <- uv1 hi (@0x8008ed30/38), r0/r1/r2.uv <- *uv0/1/2 (@0x8008ed44/50/5c), r0/r1/r2.c <- *rgb0/1/2 (@0x8008ed68/74/8c),
  `jal 0x8008ee84` = RCpolyGT3A(s, divp, 0, divp+96) (@0x8008ed88).
- Der rekursive Kern **RCpolyGT3/RCpolyGT3A @0x8008ee7c: 296 von 296 Worten gleich `divgt3a.o`** (jal-Ziele maskiert,
  `cmp_sdk.py divgt3a.bin 0x8008ee7c`).

### 1.3 DIVPOLYGON3 in RE2: ndiv = 3, pih = 320, piv = 240 (Door_init)

```
80013db0 8c843a80 lw a0,14976(a0)     ; a0 = [0x800c3a80] = Arbeitsbereich
80013dc0 24020003 addiu v0,zero,3
80013dcc ac82000c sw v0,12(a0)        ; divp->ndiv = 3
80013dd0 24020140 addiu v0,zero,320
80013dd4 ac820010 sw v0,16(a0)        ; divp->pih  = 320
80013dd8 240200f0 addiu v0,zero,240
80013ddc ac820014 sw v0,20(a0)        ; divp->piv  = 240
```
Weitere Schreiber auf [0x800c3a80]+12..20: keine (alle 21 lui-Zugriffe auf 0x800c3a80 durchsucht, `find_divp.py`);
clut/tpage/rgbc/ot schreibt DivideGT3 selbst je Aufruf. ndiv 3 = drei Stufen = **64 Teildreiecke** je Dreieck.

### 1.4 RCpolyGT3A (a0 = Paket, a1 = divp, a2 = Stufe, a3 = CRVECTOR3; RVECTOR 24 B: v@0 uv@8 c@12 sxy@16 sz@20)

| Schritt | Befehle |
|---|---|
| Ecken t0/t1/t2 = cr->r0/r1/r2 | @0x8008ee84..8c `lw t0..t2,72/76/80(a3)` |
| Nah-Verwurf: alle drei `sz < H/2` (unsigned) -> nichts | @0x8008ee90 `cfc2 t9,H`, @0x8008eea0 `sra t8,t9,1`, @0x8008eea4..b8 `sltu` x3 |
| Bild-Verwurf: alle drei `sx > OFX+pih/2`, oder alle `< OFX-pih/2`, dasselbe fuer y mit OFY/piv | @0x8008eecc `cfc2 OFX`, @0x8008eed8 `sra 16`, @0x8008eedc/e0 `srl v0/v1,1`, @0x8008eef4..08 `slt t8,t4..t6`, @0x8008ef20..34, @0x8008ef48..a4 |
| Kantenmitten im OBJEKTRAUM: r01 = (r0+r1)>>1, r12 = (r1+r2)>>1, r20 = (r2+r0)>>1 je x/y/z (`lh`, `add`, **`sra`**) | @0x8008efb4..8008f040, Ablage `sh` a3+0/+24/+48 |
| RTPT der drei Mitten (sf=1, aktuelle RT/TR = Objektmatrix) | @0x8008f044..58 `lwc2`, @0x8008f08c |
| u, v, r, g, b: (a+b)>>1 (`lbu`, `addu`, **`srl`**), cd nicht | @0x8008f05c..88 (u), @0x8008f090..bc (v), @0x8008f0c0..14c (r,g,b) |
| Stufe+1 == ndiv? | @0x8008f150 `lw t4,0(a1)`, @0x8008f154 `addiu a2,a2,1`, @0x8008f158 `bne t4,a2,0x8008f1d0` |
| **Blatt:** SXY der Mitten ablegen, 4 Dreiecke ausgeben in der Folge (r1,r12,r01) (r01,r12,r20) (r0,r01,r20) (r2,r20,r12) | @0x8008f160..68, @0x8008f170..1bc (`jal 0x8008f288` x4) |
| **Tiefer:** SZ+SXY der Mitten ablegen, cr+88, dann rekursiv (r0,r01,r20) (r1,r12,r01) (r2,r20,r12) (r01,r12,r20) | @0x8008f1d0..e4, @0x8008f1e8 `addiu a3,a3,88`, @0x8008f1f0..268 (`jal 0x8008ee84` x4) |
| Ausgabe 0x8008f288: uv0\|clut<<16 -> +12, uv1\|tpage<<16 -> +24, uv2 -> +36; **Code-Byte = divp->rgbc.cd** in t0.c.cd; sxy -> +8/+20/+32; c -> +4/+16/+28; **addPrim auf divp->ot** (`*ot = p & 0xffffff`, tag = alt \| 0x09000000); p += 40 | @0x8008f288..8008f314 (`lb t4,19(a1)` @0x8008f2ac, `sb t4,15(t0)` @0x8008f2bc, `lw t4,20(a1)` @0x8008f2d8, `sw t9,0(t4)` @0x8008f2e8, `lui t6,0x900` @0x8008f2ec) |

**Folgen fuer den Port (Flag 0x20):**
1. Das Dreieck wird in 3D (Objektraum, vor der Projektion) dreimal halbiert, jeder Teilpunkt einzeln projiziert (RTPT);
   UV linear (Mittel der Bytes, abgeschnitten). 64 affine Teildreiecke statt eines -> der Knick an der Diagonale verschwindet
   bis auf Reste der Groesse eines Teildreiecks.
2. **Farbe:** alle drei Farbzeiger = rgb0 -> jedes Teildreieck hat an allen Ecken die NCCT-Farbe der Ecke 0 (flach je Ursprungsdreieck).
3. **OT:** alle Teildreiecke auf DENSELBEN Platz wie das ungeteilte Dreieck (s4), addPrim vorn -> innerhalb des Platzes wird das
   zuletzt ausgegebene Teildreieck zuerst gezeichnet (wie bisher je Dreieck).
4. NCLIP: DivideGT3 zeichnet nur bei MAC0 > 0 (der Aufrufer liess MAC0 >= 0 durch); die Teildreiecke selbst werden NICHT geclippt
   (kein NCLIP, kein otz-Test je Teildreieck), nur die Verwurf-Tests je Stufe (Nah/Bild) auf die Ecken des jeweiligen Dreiecks.
5. Code-Byte 0x34/0x36 (Halbdurchsichtigkeit) und tpage-Bits bleiben (rgbc.cd, tpage aus uv1).

### 1.5 Welche benutzten Archive tragen 0x20 (Door_model_set, alle Skripte)

`06 09 13 15 1A 1B 1C 1D 23 24 25 29` (Blatt obj 0 flags 0x0aa0; 06/15/1B/1D auch obj 1 0x02a0; Spender 04 obj 0/3, 07 obj 0 -
die Griff-Objekte selbst ohne 0x20). **Ohne:** `0A 16 19 1E 26 27 2A 2D 2E 31` und das Tor ROOM1170 (Skripte nach DOOR2E, 0x0a80)
-> muessen bitgleich bleiben.

## 2. Messung VORHER (echte exe, beschleunigter Renderer, Rueckleser vor dem Present)

Stand 48d43b34 (= master v0.8.17 + Auftrag), Arbeitsbaum-Bau `local_build.sh build`.
- `RE15_TUER_SEITE=ALLE RE15_TUER_BOGEN=build/r32_unt/vorher/bogen RE15_TUER_SCHNELL=1 re15_pc.exe` -> 368 Bilder
  (184 Seiten x Anfang/Mitte). ⚠ Der erste Lauf endete nach 70 Seiten mit exit 1 ohne Meldung (S149 fertig, S150 nicht begonnen);
  der Rest (114 Seiten, `RE15_TUER_SEITE=<Liste>`) lief sauber durch - vermutlich ein fremdes `taskkill re15_pc.exe` einer
  parallelen Sitzung (bekannte Falle), S150 laeuft im Rest-Lauf normal.
- Tor: `RE15_TUER_TEST=0/1 RE15_TUER_SERIE=build/r32_unt/vorher/tor0|tor1` -> je 324 Bilder.

Knick-Mass `re15_port/tools/tueren/unterteilung_knick.py` (je Spalte Zeile des staerksten Helligkeitssprungs im Fenster,
Ausgleichsgerade, Abweichung in Pixeln des 960x720-Abzugs = 3 x PSX-Pixel):

| Seite | Archiv | Kante (Fenster x0 x1 y0 y1, Richtung) | max | RMS |
|---|---|---|---|---|
| S017 Mitte | DOOR13 V0 | Unterkante Fenster (475 700 270 370, +1) | **26,88 px** | **12,57 px** |
| S043 Mitte | DOOR1B V3, linker Fluegel | Unterkante Fenster (240 340 335 390, -1) | **8,91 px** | **3,58 px** |
| S043 Mitte | DOOR1B V3, linker Fluegel | Oberkante Griffleiste (125 365 425 475, +1) | 15,04 px (Ausreisser Leistenende) | 1,66 px |

Angesehen: S017 Mitte zeigt die V-Form an Fenster-Unterkante UND Feld-Oberkante (Spitze bei x ~ 605), S043 den fuenfeckigen
Fensterumriss des linken Fluegels (Knick oben rechts und unten).

## 3. Bau

- **`engine/src/door_seq_zeichnen.c` (neu, plattformfrei):**
  - `re15_door_rtpt` = die RTPT-Rechnung, die bisher als `projizieren` in `door_scene_pc.c` stand (unveraendert verschoben;
    OFX/OFY 160/120 jetzt mit RE2-Beleg @0x80068e80/88 -> SetGeomOffset 0x8008de04).
  - `re15_door_divide_gt3` = DivideGT3 0x8008ebf4 + RotAverageNclip3 + ReadSZfifo3 + RCpolyGT3A, Schritt fuer Schritt nach 1.4
    (Nah-/Bild-Verwurf streng, Mitten `sra` bzw. `srl`, Blatt- und Rekursionsfolge getrennt, cr[5]).
  - `re15_door_mesh_zeichnen` = die Dreiecksschleife FUN_80014234 e/f + FUN_8001468c, bisher `mesh_zeichnen` in
    `door_scene_pc.c`, unveraendert verschoben; neu nur der Zweig `flags & 0x20` nach der OT-Platz-Wahl: DivideGT3 mit
    `c[0]` (NCCT-Farbe Ecke 0) fuer alle Ecken, ndiv/pih/piv = `RE15_DOOR_NDIV/PIH/PIV` (3/320/240, @0x80013dcc/d4/dc),
    Teildreiecke auf den OT-Platz des Ursprungsdreiecks, laufende Nummer zaehlt je Teildreieck weiter (addPrim vorn).
    Das Ursprungsdreieck selbst wird dann nicht abgegeben (RE2 @0x80014ab8 `j 0x80014ae8`).
- `include/re15_door_seq.h`: Konstanten mit @-Adresse, `re15_door_ecke_t`, `re15_div_rvec_t` (RVECTOR), `re15_door_dreieck_t`.
- `platform/pc/src/door_scene_pc.c`: nur noch die Abgabe je Dreieck an `re15_render_textured_tri_lit` (Atlas-Versatz
  `(page & 0xF) * 128` wie bisher).
- Nicht angefasst: Tor-Daten/Tor-Generatoren, Maschine, Griff-Tausch (zeichnet ueber denselben Pfad; Griff-Objekte tragen
  kein 0x20), PSX-Plattform (keine Tuerszene).
- Schluessel-Umfang: laufende Nummer `& 0xfff` - groesstes Bild: 2 Blaetter x 12 Dreiecke x 64 + Griffe < 4096 (die Blatt-Meshes
  aller 0x20-Archive haben 12 Dreiecke, gezaehlt aus den MD1), Warteschlange 8192.

