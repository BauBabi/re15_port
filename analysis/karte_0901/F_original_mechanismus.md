# F — Was das ORIGINAL auf dem Kartenschirm zeichnet, und was nur unser Port

**Datum:** 2026-09-01 · **Quelle:** `ghidra1_V2.txt` (RE1.5 `PSX.EXE`, `t_addr=0x80010000`,
`t_size=0xaf000`, Datei-Offset = `addr - 0x80010000 + 0x800`), `ghidra_re2_Leon.txt`,
`info/Re1.5/PSX.EXE`, `re15_port/shared_assets/PSX/DATA/MAP0*.PIX`.
**Bilder:** `F_alle_schirme.png` (alle 13 Original-Schirme), `F_feste_sprites.png`,
`F_abdeckung_seite2.png` / `F_abdeckung_seite7.png`, `F_schirm_seite{0,1,2,3,4}.png`,
`F_kachel_seite{2,6,12}.png`.

---

## 0. Kurzantwort

Das Original zeichnet auf dem Kartenschirm **genau 3 + count Primitive**:

| # | Was | Prim | Quelle |
|---|---|---|---|
| 1 | Positionsmarker (8×8) | `POLY_FT4` code `0x2e` | TEX-Seite (tpage `0x1b`), uv (224,128) |
| 2 | Gebiets-/Etagen-Beischrift 88×32 @(30,30) | `SPRT` code `0x66` | Karten-Kachel, uv (0,0) |
| 3 | N/S-Kompass 32×48 @(270,40) | `SPRT` code `0x66` | Karten-Kachel, uv (96,0) |
| 4…3+count | `count` Raum-Rechtecke | `SPRT` code `0x66` | Karten-Kachel, uv aus der 12-B-Tabelle |

Dazu — außerhalb von `FUN_800473f8`, aber am selben Riegel — ein `DR_MODE` (Karten-tpage)
und **4 Schrauben** (16×16, TEX-Seite) aus der allgemeinen UI-Gruppe 11.
**Gesamt über alle 13 Blätter: 157 Primitive** (Tabelle §6.1).

Es gibt im Original **keine** Türmarken als Daten, **keine** Treppensymbole als Daten,
**keinen** Besucht-/Hervorhebungs-Zustand und **keinen** Etagen-Begriff innerhalb des
Kartenschirms. Türen sind **in die Kachel gemalt**; „Etage" ist die **ins Titelbild der
Kachel gemalte Beischrift** („POLICE STATION / 1F"), und welche Kachel gilt, entscheidet
**allein die Raumnummer**.

---

## 1. `FUN_800473f8` — der Per-Frame-Zeichner, vollständig

Aufrufer: **genau einer** — `FUN_80049a5c:80049bcc`, hinter dem Riegel

```
80049ba8 lui   a0,0xff
80049bac ori   a0,a0,0xffff          ; a0 = 0x00ffffff
80049bb0 lui   v1,0x1
80049bb4 lui   v0,0x800b
80049bb8 lw    v0,0x25c0(v0)         ; das gepackte Menue-Zustandswort
80049bbc ori   v1,v1,0x100           ; v1 = 0x00010100
80049bc0 and   v0,v0,a0
80049bc4 bne   v0,v1,LAB_80049bfc    ; != -> Karte NICHT zeichnen
80049bcc jal   FUN_800473f8
```

Also: die Karte wird nur gezeichnet, wenn `word(DAT_800b25c0) & 0x00ffffff == 0x00010100`.

### 1.1 Marker-Position (@0x8004741c–0x80047528)

```
8004741c lw    v0,0x0(s4)            ; s4=0x800aca88 -> Spieler-X
80047424 lbu   a2,0x260d(a2)         ; DAT_800b260d = ZEILENINDEX der Massstabstabelle
80047428 addiu v0,v0,0x7d00          ; +32000
8004742c sll   v1,v0,0x2
80047430 addu  v1,v1,v0              ; *5
80047434 sll   a2,a2,0x3             ; Zeile*8
80047444 lhu   v0,0x0(at)            ; sx @0x800768b4 + Zeile*8      (UNSIGNED)
80047448 sll   t0,v1,0x1             ; *10
8004744c mult  t0,v0
80047450 lui   a3,0x6666
80047454 ori   a3,a3,0x6667          ; Magic fuer /10
80047458 mflo  t0                    ; nur LO -> 32-Bit-Truncation
8004745c sra   t0,t0,0x14            ; >>20
80047460 addiu t0,t0,0x5             ; +5
80047464 mult  t0,a3
8004746c lw    v1,-0x3570(v1)        ; DAT_800aca90 = Spieler-Z
8004747c lhu   a0,0x0(at)            ; sy @0x800768b6 + Zeile*8
80047480 addiu v1,v1,0x7d00          ; +32000
800474a0 lbu   a0,0x260e(a0)         ; DAT_800b260e = SEITE
800474ac addiu v1,v1,0x5
800474b0 subu  v1,zero,v1            ; z wird NEGIERT (Karte: y = -z)
800474cc lhu   s3,0x0(at)            ; count @0x80076840 + Seite*8
800474d8 subu  t0,a1,v0              ; = ((...+5)/10)
800474e8 lh    v0,0x0(at)            ; ox @0x800768b0 + Zeile*8      (SIGNED lh!)
800474f4 addu  t0,t0,v0              ; mx = ... + ox     (bleibt im Register)
80047518 lhu   v1,0x0(at)            ; oy @0x800768b2 + Zeile*8      (UNSIGNED lhu)
80047520 addu  v0,v0,v1
80047528 sh    v0,0x2606(at)         ; DAT_800b2606 = my
```

Formel (bestätigt identisch zu `sonde_lib.proj_cal`):

```
mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
```

Nachgemessen an der ausgelieferten Tabelle (Zeilen 0..101): `max sx = 3168`, `max sy = 3984`
⇒ die 32-Bit-Truncation bei `mflo` (@0x80047458) greift erst ab `wx > 35786`, also nie.
Kein einziges `ox` hat das Vorzeichenbit gesetzt (`lh` == `lhu` für alle 39 geeichten Zeilen),
kein `oy ≥ 0x8000` — die `lh`/`lhu`-Asymmetrie zwischen `FUN_800473f8` (@0x800474e8 `lh`)
und `FUN_80046fd8` (@0x80047088 `lhu`) ist in der ausgelieferten Datenlage folgenlos.

### 1.2 Marker-Quad (@0x8004752c–0x800475dc)

```
8004752c bne   a0,zero,LAB_8004753c  ; a0 = DAT_800aca34 = OT-Paritaet
80047530 _ori  a3,a3,0xa800          ; Paritaet != 0 -> Prim 0x8019a800
80047538 ori   a3,a3,0xa828          ; Paritaet == 0 -> Prim 0x8019a828
80047540 lbu   v0,0x2602(v0)         ; DAT_800b2602 = Puls-Helligkeit
80047548 sb    v0,0x4(a3)            ; r0
80047558 sb    v0,0x5(a3)            ; g0
8004756c sb    v0,0x6(a3)            ; b0
80047554 addiu a0,t0,-0x4            ; mx-4
80047564 addiu a1,t0,0x4             ; mx+4
80047568 sh    a0,0x8(a3)            ; x0
80047578 ori   v1,zero,0xfffc        ; -4
80047584 sh    v0,0xa(a3)            ; y0 = my-4
8004757c sh    a1,0x10(a3)           ; x1
80047598 sh    v0,0x12(a3)           ; y1 = my-4
80047590 sh    a0,0x18(a3)           ; x2
800475b0 sh    v0,0x1a(a3)           ; y2 = my+4
800475a8 sh    a1,0x20(a3)           ; x3
800475c4 sh    v0,0x22(a1)           ; y3 = my+4
800475d0 addiu s2,s4,-0x23b0         ; OT-Basis 0x800a86d8
800475d4 sll   a0,a0,0xc             ; + Paritaet*0x1000
800475d8 jal   AddPrim               ; <<< AddPrim #1: MARKER
```

Die Feld-Offsets (`+4/5/6` rgb, `+8`, `+0x10`, `+0x18`, `+0x20` xy) sind exakt das
`POLY_FT4`-Layout; der Rest (len `9`, code `0x2e`, uv, clut, tpage) wird einmalig im
Builder gesetzt (§2.1).

**Puls-Mechanik** (`FUN_80048a44`, @0x80048ad0–0x80048b34):

```
80048ad0 lbu   v1,0x0(v0)=>DAT_800b2603      ; Phasenzaehler
80048ad8 addiu a0,v1,0x1
80048ae4 slti  v1,v1,0x20
80048ae8 beq   v1,zero,LAB_80048b00
80048afc _addiu v0,v0,0x4                    ; Phase <0x20: 2602 += 4
80048b0c addiu v0,v0,-0x4                    ; sonst    : 2602 -= 4
80048b14 sb    v0,0x2602(at)
80048b28 slti  v0,v0,0x40
80048b34 sb    zero,0x0(v1)                  ; Phase >= 0x40 -> 0
```

64-Frame-Dreieck, Schrittweite 4 ⇒ Marker-RGB läuft 0…128…0 (0x80 = neutral,
Startwert 0 gesetzt @0x8004644c). **Der Marker pulsiert, blinkt aber nicht an/aus.**

### 1.3 Die statische Schleife (@0x800475e0–0x8004761c) — count+2, OHNE JEDE BEDINGUNG

```
800475e0 addiu v0,s3,0x2             ; s3 = count  ->  v0 = count+2
800475e4 beq   v0,zero,LAB_80047620
800475e8 _move a1,s1                 ; s1 = 0x8019a900 (gesetzt @0x80047498/0x800474b8)
800475f0 move  s3,s2                 ; OT-Basis
800475f4 move  s2,v0                 ; Schleifengrenze
LAB_800475f8:
800475f8 addiu s1,s1,0x28            ; Schrittweite 0x28
800475fc lbu   a0,0x0(s4)            ; Paritaet
80047600 addiu s0,s0,0x1
80047604 sll   a0,a0,0xc
80047608 jal   AddPrim               ; <<< AddPrim #2..#(count+3)
8004760c _addu a0,a0,s3
80047610 andi  v0,s0,0xffff
80047614 slt   v0,v0,s2
80047618 bne   v0,zero,LAB_800475f8
8004761c _move a1,s1
```

Der Schleifenkörper hat **9 Instruktionen und keinen einzigen Vergleich außer dem
Schleifenzähler.** Verlinkt werden `0x8019a900, 0x8019a928, 0x8019a950, …` (Schrittweite
`0x28`), also **jedes** Rechteck der Seite, in **jedem** Frame, ab dem **ersten**
Kartenaufruf.

> ⛔ **Damit ist die im Port stehende Behauptung widerlegt.**
> `re15_port/engine/src/re15_inv_screen.c:1655` schreibt:
> *„das Original zeichnet ein Rechteck nur, wenn dessen Besucht-Bit steht (FUN_800473f8,
> Schleife @0x800475f8-61c …)"* — die zitierte Schleife enthält kein Bit und keinen Test.
> Der Besucht-Filter ist eine **Port-Ergänzung nach RE2-Vorbild** (§5), keine Nachbildung.

**Zweite, unabhängige Gegenprobe (Datenseite):** Ein Zustands-Kanal bräuchte pro Rechteck
eine veränderliche Größe. Die Rechteck-`SPRT`s tragen: `x,y,w,h,u,v` wörtlich aus der ROM-
Tabelle, `clut` = **ein und derselbe** Wert `GetClut(0x100,0x1f5)` (@0x80046fe4, in `t4`,
geschrieben @0x8004735c/0x800473cc), `rgb` = konstant `0x80,0x80,0x80`, `code` = konstant
`0x66`. **Kein Feld eines Rechteck-Prims ist je verschieden.** Beide Wege sagen dasselbe.

### 1.4 Zeichenreihenfolge

`AddPrim` hängt vorn ein ⇒ **zuletzt eingehängt = zuerst gerastert = unten**.
Add-Reihenfolge in `FUN_800473f8`: Marker, Sprite1, Sprite2, Rect0 … Rect(n-1).
Zeichenreihenfolge: Rect(n-1) … Rect0, Sprite2, Sprite1, **Marker zuletzt = ganz oben**.
Der Port hält das ein (Kommentar `re15_inv_screen.c:1447 ff.`).

---

## 2. `FUN_80046fd8` — der Builder (einmal je Kartenseite)

Aufrufer: `LAB_80049524:80049540` (Menü-Init) und `FUN_8004c058:8004c1bc` — letzterer
**direkt nach** `LoadImage` (@0x8004c1ac) + `DrawSync` (@0x8004c1b4), also nach dem
Hochladen einer neuen Karten-Kachel. Rückgabe `v0 = t0` = Ende des belegten Prim-Bereichs.

### 2.1 Marker-Prim (2 Kopien, `0x8019a800` / `0x8019a828`, @0x80047128–0x80047200)

```
8004712c sb  0x09, +0x03      ; len 9  -> POLY_FT4 (0x28 Byte)
80047134 sb  0x2e, +0x07      ; code 0x2c (POLY_FT4) | 0x02 (ABE)
80047138 sb  0x80, +0x04/5/6  ; rgb neutral
8004714c sh  0x1b, +0x16      ; TPAGE 0x1b = VRAM (704,256), 4bpp, abr 0
80047150 sh  DAT_800b261c, +0x0e ; CLUT (aus GetClut(0x100,0x1ee) @0x800461c4)
800471cc/d0 u0,v0 = 0xe0,0x80    ; uv (224,128)
800471d4/d8 u1,v1 = 0xe8,0x80    ;    (232,128)
800471dc/e0 u2,v2 = 0xe0,0x88    ;    (224,136)
800471e4/e8 u3,v3 = 0xe8,0x88    ;    (232,136)
```

Der Marker ist also ein **texturiertes 8×8-Icon von der TEX-Seite**, halbtransparent,
mit eigener tpage — nicht ein gefärbtes Quadrat.

### 2.2 Feste Sprites (@0x80047204–0x800472d8)

```
Sprite 1 (0x8019a900 / 0x8019a914):
  80047224 sb 0x04, +0x03      ; len 4  -> SPRT (0x14 Byte)
  80047228 sb 0x66, +0x07      ; code 0x64 (SPRT) | 0x02 (ABE)
  8004722c sb 0x80, +0x04/5/6
  80047238 sh 0x1e, +0x08 ; x = 30
  8004723c sh 0x1e, +0x0a ; y = 30
  80047240 sh 0x58, +0x10 ; w = 88
  80047244 sh 0x20, +0x12 ; h = 32
  80047248 sb 0x00, +0x0c ; u = 0
  8004724c sb 0x00, +0x0d ; v = 0
  80047250 sh t4,   +0x0e ; clut = GetClut(0x100,0x1f5)

Sprite 2 (0x8019a928 / 0x8019a93c):
  800472a8 sh 0x10e,+0x08 ; x = 270
  800472ac sh 0x28, +0x0a ; y = 40
  800472b0 sh 0x20, +0x10 ; w = 32
  800472b4 sh 0x30, +0x12 ; h = 48
  800472b8 sb 0x60, +0x0c ; u = 96
  800472bc sb 0x00, +0x0d ; v = 0
```

**Gemessen, was diese beiden Sprites zeigen** (aus den 13 `MAP0*.PIX`, `F_feste_sprites.png`):

| Seite | Datei | Sprite 1 (uv 0,0 · 88×32) | Sprite 2 (uv 96,0 · 32×48) |
|---|---|---|---|
| 0 | MAP01 | POLICE STATION **B1** | N/S-Kompass |
| 1 | MAP02 | POLICE STATION **B2** | N/S-Kompass |
| 2 | MAP03 | POLICE STATION **1F** | N/S-Kompass |
| 3 | MAP04 | POLICE STATION **2F** | N/S-Kompass |
| 4 | MAP05 | POLICE STATION **3F** | N/S-Kompass |
| 5 | MAP06 | POLICE STATION *(ohne Etage)* | N/S-Kompass |
| 6 | MAP07 | DRAINS **B2** | N/S-Kompass |
| 7 | MAP08 | FACTORY | N/S-Kompass |
| 8 | MAP09 | LABORATORY **B1** | N/S-Kompass |
| 9 | MAP0A | LABORATORY **B2** | N/S-Kompass |
| 10 | MAP0B | LABORATORY **B3** | N/S-Kompass |
| 11 | MAP0C | LABORATORY **B4** | N/S-Kompass |
| 12 | MAP0D | SUBWAY | N/S-Kompass |
| 13 | — | **keine Datei** (id 0) | — |

Sprite 2 ist auf allen 13 Seiten **gleich belegt (337 gesetzte Texel)**, Sprite 1
unterscheidet sich je Seite (344…741 Texel) — die Beischrift ist der Etagen-Träger.

### 2.3 Raum-Rechtecke (@0x800472dc–0x800473e0)

```
800472dc andi  a3,t8,0xffff   ; count
800472e0 beq   a3,zero,LAB_800473e4
800472e8 ori   t2,zero,0x4    ; len 4
800472ec ori   t1,zero,0x64   ; code SPRT, danach |= 2 (@0x80047314)
800472f4 addiu a0,t3,0x2      ; a0 = Datensatz + 2
...
8004731c lhu   v0,0x2(a0)     ; = *(u16*)(rec+4)   -> +0x10 (w)
80047328 lhu   v0,0x4(a0)     ; = *(u16*)(rec+6)   -> +0x12 (h)
80047334 lbu   v0,0x6(a0)     ; = *(u8 *)(rec+8)   -> +0x0c (u)
80047340 lbu   v0,0x8(a0)     ; = *(u8 *)(rec+10)  -> +0x0d (v)
8004734c lhu   v0,0x0(t3)     ; = *(u16*)(rec+0)   -> +0x08 (x)
80047358 lhu   v0,0x0(a0)     ; = *(u16*)(rec+2)   -> +0x0a (y)
8004735c sh    t4,0x0(v1)     ;                    -> +0x0e (clut)
800473bc addiu t3,t3,0xc      ; Datensatz = 12 Byte
```

**Der 12-Byte-Datensatz ist damit belegt: `{u16 x; u16 y; u16 w; u16 h; u8 u; u8 _; u8 v; u8 _}`.**
Der Schleifenkörper ist ×2 abgerollt (Kopie A bei `+0`, Kopie B bei `+0x14`, Slot `0x28`).

### 2.4 Ein toter Zweig: Kopie B wird nie gezeichnet

Builder legt je Slot zwei identische `SPRT`s (A@+0, B@+0x14) an; der Zeichner läuft mit
Schrittweite `0x28` ab `0x8019a900` und trifft damit **immer nur Kopie A**. Der Marker
dagegen ist korrekt paritätsgepaart (`0x8019a800` / `0x8019a828`, @0x8004752c–38).
Ghidra bestätigt es negativ: `DAT_8019a900` hat 2 Xrefs (beide `FUN_800473f8`),
`0x8019a914` hat **keine**. Folge: beide OTs verketten dieselben statischen Prims.
Das ist eine Original-Eigenheit ohne Bildwirkung (der Port hat keine OT) — nur zur Akte.

### 2.5 Eine zweite, ältere Marker-Formel im Builder

`FUN_80046fd8` rechnet die Startposition **anders** als der Per-Frame-Zeichner:

```
80047010 addiu a0,a0,0x61a8   ; +25000 (nicht 32000)
80047014 div   a0,v1          ; DIVISION durch sx (nicht Multiplikation)
80047088 lhu   a1,...         ; ox als UNSIGNED (Zeichner nutzt lh)
80047094 sh    a0,0x2604(at)  ; DAT_800b2604 = mx   <- nur hier geschrieben
80047124 sh    v1,0x2606(at)  ; DAT_800b2606 = my
```

`(wx+25000)/sx` gegen `((wx+32000)*10*sx>>20 +5)/10`. Das ist ein **Überbleibsel einer
früheren Eichungs-Konvention** (dieselbe Bauart wie RE2s Division, §5). Ohne Bildwirkung:
`DAT_800b2604` wird von niemandem gelesen, und `DAT_800b2606` überschreibt der Zeichner im
nächsten Frame. Es belegt aber, dass die Karten-Eichung im Prototyp **umgestellt wurde** und
die Tabelle @0x800768b0 nachgezogen werden musste — was zur Beobachtung passt, dass nur
39 von 106 Zeilen überhaupt gefüllt sind.

---

## 3. Die Datenquellen — welche der Zeichner wirklich liest

| Adresse | Inhalt | Wer liest | Wer schreibt |
|---|---|---|---|
| `0x80076840` | **Seiten-Paar**, 8 B: `{u16 count; u16 pad; u32 listptr}` | `FUN_80046fd8@0x80047070`, `FUN_800473f8@0x800474cc` | niemand (ROM) |
| `0x800762a0…0x8007683f` | 119 Rechteck-Datensätze à 12 B (siehe §3.1) | nur über `listptr` | ROM |
| `0x800768b0` | **Maßstabszeile**, 8 B: `{s16 ox; u16 oy; u16 sx; u16 sy}` je Raum-Index | `FUN_80046fd8@0x8004700c/0x80047088/0x800470a8/0x80047110`, `FUN_800473f8@0x80047444/0x8004747c/0x800474e8/0x80047518` | ROM |
| `0x80074c4c` | **Kachel-ID je Seite** (u16) | **nur** `LAB_8004c328@0x8004c350` | ROM |
| `0x8004b568 ff.` | Raum→`{Zeile, Seite}`-Schalter je Stage | Dispatch `0x8004998c` über `PTR@0x80074c0c` | — |
| `DAT_800b260d` | Zeilenindex | **genau 2 Leser**: `0x80046ff0`, `0x80047424` | die Schalter |
| `DAT_800b260e` | Seite | **genau 3 Leser**: `0x80047048`, `0x800474a0`, `0x8004c334` | die Schalter |

Die vier Tabellen sind **disjunkt in ihrer Rolle**: `0x80076840` liefert Anzahl+Liste der
Rechtecke (Zeichner), `0x800768b0` liefert die Marker-Eichung (Zeichner), `0x80074c4c`
liefert die **Datei-ID der Kachel** und wird **ausschließlich vom Lader** gelesen —
der Zeichner sieht sie nie.

### 3.1 Die Seiten-Tabelle, vollständig (aus `PSX.EXE`)

| Seite | @ | count | listptr | Kachel-ID (`0x80074c4c`) | Datei |
|---|---|---|---|---|---|
| 0 | 0x80076840 | 7 | 0x800762a0 | 12 | MAP01.PIX |
| 1 | 0x80076848 | 10 | 0x800762f4 | 13 | MAP02.PIX |
| 2 | 0x80076850 | 11 | 0x8007636c | 14 | MAP03.PIX |
| 3 | 0x80076858 | 10 | 0x800763f0 | 15 | MAP04.PIX |
| 4 | 0x80076860 | 7 | 0x80076468 | 16 | MAP05.PIX |
| 5 | 0x80076868 | 2 | 0x800764bc | 17 | MAP06.PIX |
| 6 | 0x80076870 | 11 | 0x800764d4 | 18 | MAP07.PIX |
| 7 | 0x80076878 | 14 | 0x80076558 | 19 | MAP08.PIX |
| 8 | 0x80076880 | 15 | 0x8007660c | 20 | MAP09.PIX |
| 9 | 0x80076888 | 15 | 0x800766c0 | 21 | MAP0A.PIX |
| 10 | 0x80076890 | 4 | 0x80076774 | 22 | MAP0B.PIX |
| 11 | 0x80076898 | 4 | 0x800767a4 | 23 | MAP0C.PIX |
| 12 | 0x800768a0 | 8 | 0x800767d4 | 24 | MAP0D.PIX |
| 13 | 0x800768a8 | 1 | 0x80076834 | **0** | **keine** |

Die Listen liegen lückenlos hintereinander (`ptr[k] + 12*count[k] == ptr[k+1]`) — mit
**einer** Ausnahme: `0x80076558 + 14*12 = 0x80076600`, aber Seite 8 beginnt erst bei
`0x8007660c`. Der Datensatz **@0x80076600 `{x=168,y=128,w=56,h=40,u=184,v=16}`** gehört zu
keiner Seite und wird nie gezeichnet — ein Kopier-Rest, fast identisch mit Seite 7 Rect 12
(`x=167`).

Seite 13 wird von STAGE2-Räumen 0x0a/0x0b angesteuert (`LAB_8004b8a0`), ihre Kachel-ID im
Tabellen-Eintrag `0x80074c66` ist aber **`0`** — keine der 13 MAP-Dateien (IDs 12…24).
`LAB_8004c328` lädt also für diese Seite eine **fremde** DATA-Datei nach VRAM (448,256);
das eine Rechteck der Seite 13 (`{120,60,64,24,u128,v16}`) zeigt entsprechend keinen
Grundriss. Auch das steht so im Auslieferungsstand.

### 3.2 Der Raum→Seiten-Schalter `@0x8004b568` (STAGE1)

```
8004b568 lh    v1,0x0fe2(v1)         ; DAT_800b0fe2 = Raum-Index
8004b574 sltiu v0,v1,0x26            ; >= 0x26 -> default, KEINE Zuweisung
8004b58c lw    v0,0x0(at)            ; Sprungtabelle @0x8001103c
8004b5a0/ac   DAT_800b260d = (u8)Raum     ; jeder Fall, dann FALL-THROUGH
8004b680 j LAB_8004b888 ; v0=2       ; Fall 0x0b  -> Seite 2
8004b6f4 j LAB_8004b888 ; v0=3       ; Fall 0x11  -> Seite 3
8004b754 j LAB_8004b888 ; v0=4       ; Fall 0x16  -> Seite 4
8004b764 j LAB_8004b888 ; v0=5       ; Fall 0x17  -> Seite 5
8004b7dc sb zero,0x260e(at)          ; Fall 0x1d  -> Seite 0
8004b884 ori v0,zero,1               ; Fall 0x25  -> Seite 1
8004b88c sb v0,0x260e(at)
8004b894 sb v1,0x260d(at)
```

Wegen der Fall-Through-Kette gilt: `0x00–0x0b → 2`, `0x0c–0x11 → 3`, `0x12–0x16 → 4`,
`0x17 → 5`, `0x18–0x1d → 0`, `0x1e–0x25 → 1`. **Zeilenindex = Raum-Index + 0.**

STAGE2 (`LAB_8004b8a0`): `sltiu v0,v1,0xc`; Fälle 0–9 → Seite 6, Fälle 0x0a–0x0b → Seite 13;
Zeile = Raum + `0x26` (@0x8004b9c0 `addiu v0,v0,0x26`). Das deckt sich mit den im Port
verwendeten Basen `[0, 38, 50, 65, 77, 98]` und mit `gen_map_zones.page_of()`.

**Eingang des Schalters ist ausschließlich `DAT_800b0fe2` (Raum-Index) — sonst nichts.**

---

## 4. Türen, Treppen, Etagen: was gemalt ist und was Daten sind

### 4.1 Türen und Treppen = **gemalt**, nicht Daten (Antwort auf Auftrag 3)

**Beweis 1 (Vollständigkeit der Zeichenwege).** `DAT_80076840` hat im gesamten Ghidra-Dump
**genau 2 Xrefs** (`0x80047070`, `0x800474cc`); `DAT_80074c4c` hat **genau 1** (`0x8004c350`).
Die Rechteck-Daten `0x800762a0…0x8007683f` sind nur über `listptr` erreichbar.
Es existiert also **eine einzige** Karten-Zeichenkette, und die besteht aus
`FUN_80046fd8` (Bau) + `FUN_800473f8` (Verkettung).

**Beweis 2 (was diese Kette zeichnen kann).** Sie zeichnet ausschließlich (a) den 8×8-Marker
und (b) Rechtecke, deren komplette Geometrie und Textur-uv aus einer ROM-Tabelle stammen.
Türen als Prims bräuchten pro Tür einen Datensatz und einen Zeichenschritt — beides
existiert nicht. Der Kartenschirm hat auch keinerlei Zugriff auf die RDTs fremder Räume.

**Beweis 3 (die Kachel selbst).** `F_kachel_seite2.png` zeigt in den Grundrissen die
Tür-Nischen mit angewinkeltem Türblatt als **gemalte Pixel** (Palettenindex 4). Der Befund
deckt sich mit dem unabhängigen Symbolkatalog `analysis/kartensymbole/symbolkatalog.csv`
(90 Türblätter über 119 Rechtecke).

**Treppensymbole:** in keiner Kachel und in keinem Prim. RE1.5 liefert dafür schlicht nichts.

### 4.2 Es gibt keinen Etagen-Begriff IM Kartenschirm (Antwort auf Auftrag 4)

`DAT_800acad6` (Spieler-Band, +0x82) hat **27 Xrefs**, alle in
`0x80014280`, `0x8001d7a4…0x8001d900`, `0x8002bf38`, `0x8002d364…0x8002d760`,
`0x80031d04`, `0x80035b08`, `0x800366d4…0x800368e4`, `0x800381c8…0x80038edc`.
**Keine einzige liegt im Kartencode** (`0x80046f68`–`0x8004b9d0`).

`DAT_800b260d` (Zeilenindex) hat 2 Leser (beide Karte) und wird nur von den Raum-Schaltern
geschrieben, deren einziger Eingang `DAT_800b0fe2` ist. Dasselbe für `DAT_800b260e`.

**Fazit:** Der einzige Etagen-Ausdruck des Originals ist die **ins Titelbild der Kachel
gemalte Beischrift** („1F", „B2", …). Sie folgt der **Raumnummer**, nicht dem Band. Ein
Raum, der über zwei Etagen reicht, bleibt im Original auf **einem** Blatt.
Es gibt **keine** Blätterfunktion, keinen Etagen-Wechsel und keinen Etagen-Zustand.

### 4.3 Wie viel der Kachel überhaupt auf den Schirm kommt

Deckungsmessung (jedes gesetzte Texel gegen die Vereinigung aller `count` Rechteck-uv
plus der beiden festen Sprites):

| Seite | gesetzte Texel | davon geblittet | ungenutzt | ungenutzt in Zeilen 0–15 |
|---|---|---|---|---|
| 0 | 30223 | 15637 (51,7 %) | 14586 | 258 |
| 1 | 14703 | 7402 (50,3 %) | 7301 | 258 |
| 2 | 23584 | 11670 (49,5 %) | 11914 | 258 |
| 3 | 24127 | 9680 (**40,1 %**) | 14447 | 258 |
| 4 | 13218 | 7105 (53,8 %) | 6113 | 258 |
| 5 | 7182 | 3906 (54,4 %) | 3276 | 258 |
| 6 | 30468 | 15259 (50,1 %) | 15209 | 258 |
| 7 | 40452 | 24315 (60,1 %) | 16137 | 258 |
| 8 | 14505 | 7754 (53,5 %) | 6751 | 244 |
| 9 | 23487 | 11995 (51,1 %) | 11492 | 244 |
| 10 | 10310 | 5602 (54,3 %) | 4708 | 244 |
| 11 | 12457 | 6070 (48,7 %) | 6387 | 244 |
| 12 | 22239 | 10565 (47,5 %) | 11674 | 258 |

Zwei belegte Konsequenzen:

1. **Der Maßstabsbalken ist tote Kunst.** Jede Kachel trägt in den Zeilen `v = 0…15` bei
   `u ≥ 128` einen Lineal-Streifen „0 5 10 15 [m] 1:610" (242–258 gesetzte Texel).
   Das **kleinste `v` über alle 119 Rechtecke ist 16** — der Streifen wird nie geblittet.
2. **Rund die Hälfte jeder Kachel ist gemalter, aber unreferenzierter Grundriss.**
   `F_abdeckung_seite2.png` zeigt es: der komplette zusammengesetzte Plan im unteren
   rechten Kachelviertel hat **keinen** Rechteck-Datensatz. Die Kartendaten des Prototyps
   sind unfertig — die Kunst ist da, die 12-Byte-Platzierung fehlt.

### 4.4 ⛔ Seite 3 („POLICE STATION 2F") ist im Original selbst kaputt

Die Rechteck-Liste der Seite 3 (`0x800763f0`, 10 Sätze = 120 B) ist ein
**byte-identisches Duplikat** der ersten 10 Sätze von Seite 2 (`0x8007636c`):

```
A @0x8007636c: b4 00 45 00 20 00 60 00 00 00 20 00  6d 00 4d 00 38 00 38 00 20 00 20 00  …
B @0x800763f0: b4 00 45 00 20 00 60 00 00 00 20 00  6d 00 4d 00 38 00 38 00 20 00 20 00  …
   erste 120 Byte identisch: True
```

Seite 2 liest damit MAP03.PIX (1F), Seite 3 dieselben uv aus **MAP04.PIX** (2F) — einem
anders aufgebauten Bild. Das Ergebnis (`F_schirm_seite3.png` gegen `F_schirm_seite2.png`)
ist ein zerrissener Grundriss: abgeschnittene Räume, eine Wandlinie, die ins Leere läuft,
Teile, die nicht aneinander stoßen. Seite 3 ist auch die Seite mit der **schlechtesten
Kachel-Ausnutzung (40,1 %)**.

**Das ist die Ursache des Nutzer-Reports „ab Police Station 2F ist die Karte vollkommen im
Eimer" — und sie steht im Auslieferungsstand, nicht im Port.** Kein Port-Fix an unseren
Ergänzungen kann das heilen; die 2F-Rechtecke müssten neu gesetzt werden, und das wäre
ausdrücklich eine **Reparatur über das Original hinaus**, keine Nachbildung.
(Gegenprobe: von allen 13 Seiten ist dieses Paar das einzige mit ≥3 identischen
Datensätzen in gleicher Reihenfolge; alle übrigen Blätter setzen sich zu geschlossenen
Grundrissen zusammen — `F_alle_schirme.png`.)

### 4.5 Die restlichen karten-eigenen Prims außerhalb `FUN_800473f8`

* **`DR_MODE`-Paket** `0x800b2650 + Paritaet*12`, `AddPrim` @0x80049bf4 hinter demselben
  `0x00010100`-Riegel. Gebaut per `SetDrawMode(p,0,0,tpage,NULL)` @0x8004632c mit
  tpage `0x27`/`0x17` = VRAM **(448,256)**, 4 bpp — die **Karten-Kachel**.
  Das Gegenstück `0x800b2638 + Paritaet*12` (tpage `0x2b`/`0x1b` = (704,256), TEX-Seite,
  gebaut @0x800462a4) wird **vor** der Karte eingehängt, wird also **nach** ihr gerastert
  und stellt die TEX-Seite wieder her (`AddPrim` @0x80049ba0).
* **4 Karten-Schrauben** (UI-Gruppe 11, Meistertabelle `@0x80074b10` `{clut 4, count 4,
  rects 0x80075630}`), gezeichnet nur bei `word(25c0)&0xffffff == 0x00010100`
  (@0x80048020–0x8004802c):
  `(16,120) uv(112,56)`, `(280,120) uv(112,72)`, `(155,24) uv(112,40)`, `(155,200) uv(112,88)`,
  je 16×16 von der TEX-Seite.

---

## 5. RE2-Vergleich (Auftrag 5)

### 5.1 Raum → Rechteck: `FUN_8006eae8` @0x8006eae8

```
8006eae8 sltiu v0,a0,0x14      ; 20 Stages/Blaetter
8006eafc lw    v0,0x1cd0(at)   ; Sprungtabelle @0x80011cd0
8006eb04 jr    v0              ; je Stage ein eigener switch(room)
8006eb0c sltiu v0,a1,0x18      ; Stage 0: room<0x18 -> v0=room, sonst room-0x14
8006eb20 li    v1,0x16 ; bne a1,v1 -> …; sonst v0=0x11   (Stage 2)
8006eb34 li    v0,0x1b ; bne a1    -> …; sonst v0=0x0e   (Stage 3)
```

RE2 löst „welches Rechteck gehört zu diesem Raum" mit einem **handgeschriebenen
Umsetzer im Code**: pro Blatt eine Kette aus Sonderfällen und Verschiebungen.

### 5.2 Marker-Projektion: `FUN_8006e120`, fester Maßstab 450

```
8006e1dc lui   v0,0x91a2
8006e1e4 lw    a0,-0x3d0(a0)=>DAT_800cfc30   ; Spieler-X
8006e1e8 ori   v0,v0,0xb3c5                  ; Magic 0x91a2b3c5
8006e1ec addiu a0,a0,0x6d60                  ; +28000
8006e1f0 mult  a0,v0
8006e200 addiu a1,a1,0x6d60                  ; Spieler-Z +28000
8006e204 mfhi  v1
8006e208 addu  v1,v1,a0
8006e20c sra   v1,v1,0x8
8006e218 subu  v1,v1,a0(>>31)                ; = (wx+28000) / 450
8006e268 subu  v0,zero,v0                    ; z negiert (wie RE1.5)
```

Numerisch verifiziert: `q(n) == n // 450` für alle `n` im Raster `0…200000` Schritt 997
(und **kein anderer** Divisor 440…460 passt). RE2 hat also **einen globalen Maßstab**.

Der Ort kommt aus zwei Summanden statt aus einer Raum-Zeile:

```
8006e228 lhu a0,0x5c48(a0)=>DAT_800d5c48 ; Blatt-Ursprung X
8006e240 addu v1,v1,a0
8006e264 lhu a1,0x5c4a(a1)=>DAT_800d5c4a ; Blatt-Ursprung Y
8006e274 addu v0,v0,a1
8006e2cc lhu a1,0x8(v1)                  ; + x des ZUGEHOERIGEN Rechtecks
8006e2e4 lhu v1,0xa(v1)                  ; + y des ZUGEHOERIGEN Rechtecks
```

Der Marker wird also **relativ zum Rechteck des Raums** gesetzt (`Rechteck-Index * 16`,
@0x8006e2b8 `sll v1,a3,0x4` — RE2-Datensatz = **16 Byte**, RE1.5 = 12).

### 5.3 Richtungs-Marker

```
8006e220 lh    v0,-0x392(v0)=>DAT_800cfc6e  ; Blickwinkel
8006e234 addiu v0,v0,0x100
8006e238 sra   v0,v0,0x9
8006e23c andi  s2,v0,0x7                    ; 8 Sektoren
8006e280 sll   v0,s2,0x1 ; addu ; sll 2     ; u = Sektor*12
8006e28c sb    v0,0xc(s4)                   ; -> 8 Pfeil-Kacheln
8006e2a0 addiu s2,v0,0x28                   ; rgb = Puls + 0x28
```

RE2 zeigt einen **gerichteten Pfeil** (8 Kacheln à 12 px). RE1.5 hat einen **festen Punkt**
(uv 224,128 konstant, §2.1) — die Richtung existiert dort schlicht nicht.

### 5.4 Zustands-Rechtecke (das RE2-Feature, das RE1.5 fehlt)

```
8006e45c lbu   s2,-0x55c4(at)=>DAT_800aaa3c  ; count je Blatt (Paar-Stride 8)
8006e460/68   ptr @0x800aaa38 + Blatt*8, Datensatz 16 B
8006e46c … Schleife ueber die Rechtecke:
8006e66c jal   FUN_80077360                 ; Flag-Test (Bank, Bit) …
8006e688 jal   FUN_80077360                 ; … Bit aus Datensatz +0xc
8006e718 jal   FUN_80077360                 ; … Bit aus Datensatz +0xd
8006e6b0/bc lbu a0, +0xf / +0xe             ; Zustandsbytes des Datensatzes
8006e6e0 beq   …,LAB_8006e768               ; -> NICHT zeichnen
8006e704 bne   …,LAB_8006e768               ; -> NICHT zeichnen
8006e744 beq   …,LAB_8006e768               ; -> NICHT zeichnen
8006e648 addiu s5,s5,0x1                    ; aktueller Raum: CLUT-Zeile +1
8006e750 jal   GetClut(0x100, s5)           ; s5 aus {0x1f2,0x1f7,0x1fa,0x1fe}
8006e760 jal   AddPrim                      ; erst hier
```

RE2 gatet **jedes** Rechteck auf Spiel-Flags und wählt pro Zustand eine **andere
CLUT-Zeile** (Farbe). RE1.5s Schleife (§1.3) hat weder das eine noch das andere, und seine
Prims haben nicht einmal ein Feld dafür (§1.3 Gegenprobe).

### 5.5 Übertragbar / nicht übertragbar

| RE2-Mechanismus | auf RE1.5 übertragbar? |
|---|---|
| Zustands-Gate je Rechteck (Flag → zeichnen/nicht) | **Mechanismus ja, Daten nein.** RE1.5s 12-B-Datensatz hat keine Flag-Bytes (RE2: +0xc…+0xf). Die Zuordnung Rechteck→Raum müsste erfunden werden — genau das tut unsere Zonen-Tabelle. Bleibt eine **Ergänzung**. |
| Zustands-CLUT (Farbe je Zustand) | **Nein als Byte-Wert.** RE2s CLUT-Zeilen 0x1f2/0x1f7/0x1fa/0x1fe liegen in RE2-CD-Datei 170; RE1.5 hat für die Karte **eine einzige** CLUT (`GetClut(0x100,0x1f5)`). Unsere Grün/Rot-Werte sind Port-Wahl. |
| 8-Richtungs-Marker | **Nein.** RE1.5s TEX-Seite hat an uv(224,128) genau **eine** 8×8-Kachel, keine 8er-Reihe. |
| Globaler Maßstab 450 + Rechteck-relativer Ursprung | **Nein — RE1.5 ist hier weiter.** RE1.5 hat pro Raum eine eigene Zeile `{ox,oy,sx,sy}`. Ein globaler Maßstab wäre ein Rückschritt. Interessant nur als Erklärung für den Rest im Builder (§2.5): `(wx+25000)/sx` ist genau RE2s Bauart. |
| Hartkodierter Raum→Rechteck-Umsetzer | **Nicht nötig.** RE1.5 löst dasselbe über `DAT_800b260e`/`DAT_800b260d` + die Maßstabszeile (die Zeile bestimmt implizit das Rechteck, siehe `massstabstabelle.md` §3). |

---

## 6. Fazit: Original gegen Port

### 6.1 Mengengerüst

| Seite | Original-Prims (1 Marker + 2 fest + count) | Port-Marken (Tür/Treppe) | Port-Zusatz-Ops (1×1-FILL) |
|---|---|---|---|
| 0 | 10 (7 Rects) | 9 (9/0) | 117 |
| 1 | 13 (10) | 6 (6/0) | 78 |
| 2 | 14 (11) | 13 (8/5) | 119 |
| 3 | 13 (10) | 10 (8/2) | 110 |
| 4 | 10 (7) | 11 (8/3) | 113 |
| 5 | 5 (2) | 2 (1/1) | 16 |
| 6 | 14 (11) | 16 (16/0) | 208 |
| 7 | 17 (14) | 13 (9/4) | 129 |
| 8 | 18 (15) | 13 (12/1) | 159 |
| 9 | 18 (15) | 14 (11/3) | 152 |
| 10 | 7 (4) | 3 (3/0) | 39 |
| 11 | 7 (4) | 5 (3/2) | 45 |
| 12 | 11 (8) | 13 (8/5) | 119 |
| **Σ** | **157** | **128** | **1404** |

Der Port zeichnet über alle Blätter **128 zusätzliche Symbole = 1404 zusätzliche
Zeichenoperationen** — knapp das Neunfache dessen, was das Original überhaupt zeichnet.

### 6.2 „Das macht das Original" gegen „Das macht nur unser Port"

**Original (belegt):**

1. Marker-Quad, 8×8, TEX-uv (224,128), Puls 0…128 über 64 Frames — `FUN_800473f8@0x800475d8`.
2. Gebiets-/Etagen-Beischrift 88×32 @(30,30) aus der Kachel — `@0x80047204–268`.
3. N/S-Kompass 32×48 @(270,40) aus der Kachel — `@0x8004726c–2c0`.
4. `count` Raum-Rechtecke, **immer alle, immer gleich, immer neutral** — `@0x800475f8–61c`.
5. `DR_MODE` Karten-tpage + Rückstell-`DR_MODE` — `@0x80049bf4` / `@0x80049ba0`.
6. 4 Schrauben, TEX-Seite — UI-Gruppe 11, Riegel `@0x8004802c`.
7. Seite und Maßstabszeile **allein aus der Raumnummer** — `@0x8004b568 ff.`.

**Nur Port:**

| Port-Ergänzung | Ort | Nötig, weil das Original hier unfertig ist? |
|---|---|---|
| **Türmarken** (kind 0–3, 13 FILL je Marke) | `re15_inv_screen.c:1485–1544` | **Ersetzt nichts, ergänzt** — aber teilweise **redundant**: das Original malt die Tür schon in die Kachel. Der Port unterdrückt das per `kachel_zeigt_tuer()`; wo die Prüfung danebengreift, steht die Tür doppelt (genau der Nutzer-Report „die Tür ist 2× da"). **Der sauberste Zustand wäre: gar keine eigenen Türmarken** — die Original-Kachel liefert sie bereits. |
| **Treppensymbole** (kind 4–5, 3 FILL je Marke) | `re15_inv_screen.c:1545–1580` | **Echte Lücke.** RE1.5 hat weder Prims noch gemalte Treppen. Reine Zutat auf Nutzerwunsch. |
| **Besucht-/Aktuell-Einfärbung, unbesucht = nicht zeichnen** | `re15_inv_screen.c:1649–1668` | **Ersetzt etwas, das es im Original NICHT gibt** — die Schleife `@0x800475f8` zeichnet bedingungslos. Der Port zeigt beim ersten Kartenaufruf **weniger** als das Original. Die Kommentar-Begründung in `:1655` zitiert eine Adresse, die das Gegenteil enthält. Der Mechanismus stammt aus RE2 (`FUN_8006e120@0x8006e6e0–760`), die Farben sind Port-Wahl. |
| **Etagen-Umschaltung nach Spieler-Band** (`re15_inv_map_page_shown`, `s_map_floors`, 16 Zeilen) | `re15_inv_screen.c:231–245` | **Ersetzt die Original-Regel.** Im Original entscheidet **nur** die Raumnummer; `DAT_800acad6` wird vom Kartencode nie gelesen. Ergänzung — sinnvoll für Treppenhäuser, aber **nicht** durch `RE15_MAP_STOCK=1` abschaltbar (§6.3). |
| **Zonen-Modell** (180 Einträge, Raum→mehrere Rechtecke) | `re15_map_zones.h` | Ergänzung; das Original kennt nur Raum→Zeile→Rechteck. |
| **„ROOF"-Beischrift auf Seite 5** | `re15_inv_screen.c:1626` | Ergänzung; Seite 5 trägt als einzige **keine** Etage im Titelbild (gemessen, §2.2). Korrekt als Ergänzung markiert. |
| **Bbox-Streckung als Marker-Rückfall** | `gen_map_zones.py` / `proj_bbox` | Ergänzung; 67 von 106 Maßstabszeilen sind der Stub `{0,0,1,1}`, der alles auf (0,0) abbildet. |

### 6.3 Zwei konkrete Reparaturen, die aus diesem Befund folgen

1. **`re15_inv_screen.c:1655` ist eine falsche Byte-true-Behauptung.** Der Satz muss
   ersetzt werden durch: *„Das Original zeichnet ALLE count Rechtecke bedingungslos
   (`FUN_800473f8` @0x800475f8–61c, Schleifenkörper ohne Vergleich; alle Rechteck-Prims
   tragen dieselbe CLUT `GetClut(0x100,0x1f5)` @0x80046fe4 und rgb 0x80). Der Besucht-Filter
   ist eine Ergänzung nach RE2 (`FUN_8006e120` @0x8006e6e0/704/744)."*
2. **`RE15_MAP_STOCK=1` liefert nicht den Original-Stand.** `re15_inv_map_page_shown()`
   (`:231`) ist nicht durch `re15_map_stock_mode()` abgeriegelt und liefert für die 16
   `s_map_floors`-Zeilen weiter eine band-abhängige Seite. Wer den Original-Stand messen
   will, misst derzeit einen Mod. Der Stock-Pfad muss auf den reinen Raum-Schalter aus
   `re15_inv_map_stage_init()` zurückfallen.

### 6.4 Was der Nutzer als „kaputt" sieht, sortiert nach Verursacher

| Symptom | Verursacher |
|---|---|
| „ab Police Station 2F ist die Karte vollkommen im Eimer" | **Original** — Seite-3-Liste ist ein 120-B-Duplikat der Seite-2-Liste (§4.4). |
| „die Tür ist auf beiden Seiten / 2× da" | **Port** — eigene Türmarke zusätzlich zur gemalten Tür der Kachel. |
| „Rechteck steht schon da, obwohl ich noch nicht dort war" | **Port** — Besucht-Modell; das Original zeigt ohnehin von Anfang an alles. |
| „Karte bleibt nach dem Treppenlauf auf der alten Ebene" | **Original** — Seite folgt nur der Raumnummer. Der Port-Fix ist eine bewusste Ergänzung. |
| viele Räume fehlen ganz auf der Karte | **Original** — ~50 % jeder Kachel ist gemalter Grundriss ohne Rechteck-Datensatz (§4.3). |
