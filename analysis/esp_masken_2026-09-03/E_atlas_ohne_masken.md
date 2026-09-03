# AUFGABE E — Anomalie 1: 130 Cuts mit SLD-Vordergrundatlas, aber ohne Maskenrecords

Datum: 2026-09-03 · Quellen: `re15_port/shared_assets/PSX` (= `info/Re1.5/PSX`, Auslieferungsstand),
`ghidra1_V2.txt`, `RE_15_Quellcode_V2/`.
Werkzeuge (neu, in diesem Verzeichnis): `e_probe.py`, `e_sweep.py`, `e_sizes.py`, `e_rooms.py`,
`e_sig.py`, `e_scan2.py`, `e_map.py`, `e_scd45.py`. Roh-Abzug: `E_pri_offsets_dump.txt`.

---

## 0. ERGEBNIS IN EINEM SATZ

**Es gibt keine Maskengeometrie für diese 130 Cuts — weder eine zweite Tabelle noch versteckte
Bytes.** Der `pri_offset` jedes betroffenen Cuts zeigt auf eine gültige, korrekt gestaffelte
RDT-Stelle innerhalb des `sprite.pri`-Blocks, und dort stehen exakt vier Bytes `FF FF FF FF` =
die NULL-Sektion. Die Original-Engine setzt daraufhin den Maskenzähler auf 0
(`sb zero,0x0(a0)` @**0x80039338**) und der Zeichner steigt sofort aus
(`beq s4,zero,LAB_80039678` @**0x800395cc**). Der SLD-Atlas wird trotzdem in den VRAM geladen —
sein Upload hängt an einem **anderen** Zweig (`beq v0,zero,LAB_80021df8` @**0x80021d74**), der die
Maskentabelle nicht kennt. Ergebnis: **Der Atlas liegt im VRAM auf (320,256) und wird von nichts
gesampelt.** Totes Prototyp-Material. Der Port darf dort nichts zeichnen — und tut es bereits nicht.

---

## 1. BYTE-GENAUE PRÜFUNG DER `pri_offset` (Teilaufgabe 1)

### 1.1 Wie der `pri_offset` überhaupt zum Zeiger wird

Der RDT-Loader **FUN_800396fc** fixiert erst alle Header-Dwords und dann **unbedingt** jedes
`cut[i]+0x1C` — es gibt dort **keinen** 0-/0xFFFFFFFF-Test:

```
; Header-Directory 0x08..0x5C (nur !=0 wird fixiert)
80039800 00 00 64 8c     lw    a0,0x0(v1)          ; v1 = rdt_base+8 .. rdt_base+0x5C
80039808 05 00 80 10     beq   a0,zero,LAB_80039820
80039818 21 10 82 00     addu  v0,a0,v0            ; v0 = rdt_base
8003981c 00 00 62 ac     sw    v0,0x0(v1)
80039828 60 00 42 24     addiu v0,v0,0x60          ; Schranke = rdt_base+0x60
8003982c 2b 10 62 00     sltu  v0,v1,v0
80039830 f3 ff 40 14     bne   v0,zero,LAB_80039800

; Kamera-Cut-Records: cut[i].pri_offset += rdt_base   (UNBEDINGT, i = 0..hdr[1]-1)
80039844 01 00 62 90     lbu   v0,0x1(v1)          ; nCut  = RDT-Header Byte[1]
8003985c 40 21 06 00     sll   a0,a2,0x5           ; i*32
80039860 24 00 62 8c     lw    v0,0x24(v1)         ; rid-Basis (Kamera-Cut-Tabelle)
80039868 21 20 82 00     addu  a0,a0,v0
8003986c 1c 00 82 8c     lw    v0,0x1c(a0)         ; <-- pri_offset (RDT-relativ)
80039874 21 10 43 00     addu  v0,v0,v1            ; += rdt_base
80039878 1c 00 82 ac     sw    v0,0x1c(a0)         ; -> absoluter Zeiger
80039894 f2 ff 40 14     bne   v0,zero,LAB_80039860
```

⇒ **Ein Cut-Record kann gar kein `FFFFFFFF` im Feld +0x1C tragen** — das würde beim
Dereferenzieren in FUN_800392d4 sofort in den Kernel-Bereich zeigen. Der `FFFFFFFF`-Wert steht
IMMER im *Ziel* der Adresse, nie in der Adresse.

### 1.2 Wo der `sprite.pri`-Block liegt (kein Pointer im Header)

Über **alle 206 echten RDTs** gemessen (`e_sweep.py`), 0 Ausnahmen:

| Invariante | Ergebnis |
|---|---|
| `min(pri_offset) == lit_addr + nCut*40` | **206/206** |
| Blockende == nächster Header-Pointer (immer `sca`, +0x20) | **206/206** |
| Blockinhalt lückenlos (Sektionen aneinander, **0 Byte Schlupf**) | **206/206** (`e_sizes.py`) |

Der Block hat einen bisher nicht dokumentierten **Abschluss**:
`[nCut Sektionen][nCut Dwords Rückwärts-Offsettabelle][1 Dword == RDT-Header-Byte[7]]`.

Beispiel ROOM1170 (hat echte Masken, `hdr7=0x38=56`), Rohbytes:
```
000A48  D8 03 00 00 D4 03 00 00 D0 03 00 00 CC 03 00 00   -> 0x3D8..0x3CC (Cut 12,11,10,9)
000A58  C8 03 00 00 C4 03 00 00 C0 03 00 00 3C 01 00 00   -> Cut 8,7,6,5
000A68  58 00 00 00 54 00 00 00 2C 00 00 00 04 00 00 00   -> Cut 4,3,2,1
000A78  00 00 00 00 38 00 00 00                            -> Cut 0 | 0x38 = hdr7 = 56
000A80  <sca beginnt>
```
`0x66C + 0x3D8 = 0xA44` = `pri_offset[cut12]`; `0x66C + 0x00 = 0x66C` = `pri_offset[cut0]`.
Die Tabelle ist die exakte Umkehrung der Cut-Reihenfolge, das letzte Dword ist immer `hdr[7]`
(206/206). Sie wird von **keinem** Engine-Zweig gelesen (siehe §4) — Build-Metadaten des
RDT-Werkzeugs, keine zweite Geometriequelle.

### 1.3 Die 130 Cuts, byte-genau

`e_rooms.py` (voller Abzug: `E_pri_offsets_dump.txt`) prüft jeden der 130 Cuts:
Adresse des Feldes, gespeicherter Offset, Staffelung, und die 4 Bytes am Ziel.
**Alle 130 tragen `FF FF FF FF`.** Beispiele:

```
ROOM1220 nCut=10 hdr7=0  lit=0x34C  pri_block=[0x4DC,0x530)  laenge=84 = 10*4 + 10*4 + 4
  cut  0  rec+0x1C @0x007C = 0x000004DC (= lit+10*40+0)  -> RDT[0x04DC..0x04DF] = FF FF FF FF
  cut  1  rec+0x1C @0x009C = 0x000004E0 (= lit+10*40+4)  -> RDT[0x04E0..0x04E3] = FF FF FF FF
  ...
  cut  9  rec+0x1C @0x019C = 0x00000500 (= lit+10*40+36) -> RDT[0x0500..0x0503] = FF FF FF FF

ROOM3040 nCut=14 hdr7=0  lit=0x554  pri_block=[0x784,0x7F8)  laenge=116 = 14*4 + 14*4 + 4
  cut  0  rec+0x1C @0x007C = 0x00000784 -> RDT[0x0784..0x0787] = FF FF FF FF
  ... (alle 14 identisch, Schrittweite 4) ...

ROOM40A0 nCut=8 hdr7=35  lit=0x2B8  pri_block=[0x3F8,0xCD0)  laenge=2264
  cut  0  @0x007C = 0x000003F8 -> FF FF FF FF          (NULL, aber kein SLD -> keine Anomalie)
  cut  1  @0x009C = 0x000003FC -> 04 00 1F 00          (4 Gruppen, 31 Masken deklariert)
  ...
  cut  7  @0x015C = 0x00000CA8 -> FF FF FF FF          <-- ANOMALIE-Cut
```

Rohbytes des kompletten `sprite.pri`-Blocks von ROOM1220 (0x4DC..0x52F):
```
0004D0  .. .. .. .. .. .. .. .. .. .. .. .. FF FF FF FF   <- ab 0x4DC: Cut 0
0004E0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   <- Cuts 1..4
0004F0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   <- Cuts 5..8
000500  FF FF FF FF 24 00 00 00 20 00 00 00 1C 00 00 00   <- Cut 9 | Tabelle 0x24,0x20,0x1C
000510  18 00 00 00 14 00 00 00 10 00 00 00 0C 00 00 00
000520  08 00 00 00 04 00 00 00 00 00 00 00 00 00 00 00   <- ... 0x00 | letztes Dword = 0 = hdr7
000530  <sca beginnt: 50 DA 0E CB ...>
```

Der Bereich **zwischen LIT-Ende und SCA-Anfang ist restlos belegt**: bei ROOM1220
`0x530-0x4DC = 84 = 10*4 (NULL-Sektionen) + 10*4 (Rückwärtstabelle) + 4 (hdr7-Wort)`.
Es bleibt **kein einziges freies Byte** für versteckte Geometrie. Gleiches Muster bei
ROOM3040 (116 B), ROOM3050 (132 B), ROOM30A0 (60 B), ROOM30E0 (84 B), ROOM4040 (108 B),
ROOM4080 (76 B).

---

## 2. SUCHE NACH EINER ZWEITEN MASKEN-TABELLE (Teilaufgabe 2)

### 2.1 Strukturelle Signatur einer echten `sprite.pri`-Sektion

Aus **allen 480 echten Sektionen / 2750 Gruppen** des Spiels (`e_sig.py`) abgeleitet:
```
Sektion:  u16 groupCount (1..8)     u16 declaredMaskCount (1..105)
Gruppe:   u16 count  u16 base  s16 destX  s16 destY     (8 B)
          base aus { 0x7800, 0x7C00, 0x7C10, 0x8080, 0x8088 }   (nur 5 Werte game-weit)
Maske:    u8 srcX  u8 srcY  u8 dstX  u8 dstY  u16 depth  u16 sizeField
          [+u16 w, u16 h wenn (sizeField & 0xF000) == 0]
```
`base` wird von der Engine **nie gelesen** — FUN_800392d4 schreibt die CLUT-ID als Immediate
`ori v0,zero,0x7800` @**0x80039498** (= `GetClut(0,480)`, weil `0x7800>>6 = 480`, `x=0`).

### 2.2 Vollscan aller 206 RDTs

`e_scan2.py` testet **jeden geraden Byte-Offset** jeder RDT gegen diese Signatur
(Gruppenzahl 1..8, decl 1..105, alle `base` aus der 5er-Menge, |destX/Y| <= 512,
`sum(counts) ~ decl +-4`, alle `depth < 1024` = OT-Größe, alle Rechtecke 1..256):

```
Räume gescannt: 206
Räume mit gültiger pri-Sektions-Signatur AUSSERHALB des sprite.pri-Blocks: 0
```

Innerhalb des Blocks findet der Scan exakt die vom Cut-Record referenzierten Sektionen
(ROOM1170: 4, ROOM1210: 2, ROOM1150: 4, ROOM3090: 6, ROOM40A0: 6) — und in
ROOM1220/3040/3050/30A0/30E0/4040/4080 **null**, weil dort alle Sektionen NULL sind.

> Der schwächere Test "Halbwort `00 78` (=0x7800) irgendwo in der RDT" liefert Treffer in
> MD1-Modelldaten (Dreiecks-TPage/CLUT-Felder, z.B. ROOM1170: 1208 Treffer) und ist **kein**
> Diskriminator — deshalb der strenge Struktur-Test oben.

### 2.3 Sektions-Vergleich: Raum MIT Masken vs. Anomalie-Raum

`e_map.py` — die Sektionsfolge ist **identisch**, nur die Größe des `sprite.pri`-Blocks
unterscheidet sich:

| | ROOM1170 (56 Masken) | ROOM3040 (Anomalie) | ROOM1220 (Anomalie) | ROOM30A0 (Anomalie) |
|---|---|---|---|---|
| Header | 0x000000..0x00005F | 0x000000..0x00005F | 0x000000..0x00005F | 0x000000..0x00005F |
| rid (Cuts) | 0x60, 13x32 B | 0x60, 14x32 B | 0x60, 10x32 B | 0x60, 7x32 B |
| md1tim_ptr | 0x200 (48 B) | 0x220 (16 B) | 0x1A0 (24 B) | 0x140 (16 B) |
| rvd | 0x230 (564 B) | 0x230 (804 B) | 0x1B8 (404 B) | 0x150 (344 B) |
| lit | 0x464 (13x40) | 0x554 (14x40) | 0x34C (10x40) | 0x2A8 (7x40) |
| **sprite.pri** | **0x66C, 1044 B** | **0x784, 116 B** | **0x4DC, 84 B** | **0x3C0, 60 B** |
| sca | 0xA80 | 0x7F8 | 0x530 | 0x3FC |
| blk / flr / scd / msg / snd... | vorhanden | vorhanden | vorhanden | vorhanden |

116 / 84 / 60 B ist jeweils exakt das **Minimum** (`nCut*4 + nCut*4 + 4`). Es fehlt keine Sektion,
es ist keine zusätzliche da.

---

## 3. HEADER-DIRECTORY 0x4C..0x5C (Teilaufgabe 3)

Es gibt **keine unbenutzten** Directory-Einträge. Der Loader fixiert 0x08..0x5C = 22 Dwords
(`addiu v0,v0,0x60` @**0x80039828**), und alle fünf fraglichen Einträge haben belegte Konsumenten:

| Offset | Bedeutung | Leser (Adresse) |
|---|---|---|
| +0x4C | `effect.esp` | `FUN_80019354` — `piVar4 = *(int **)(DAT_800ac778 + 0x4c)` |
| +0x50 | ESP-Ende / EFF | `FUN_80019354` -> `FUN_8001945c(piVar4, *(0x50), piVar4, 8)` |
| +0x54 | `espXX.tim` | `FUN_80019354` -> `FUN_800194f8(*(0x58), *(0x54))` |
| +0x58 | `modelXX.tim` | dito |
| +0x5C | `animation.rbj` | `FUN_8001b3f8` — `piVar5 = *(int **)(DAT_800ac778 + 0x5c)` |

Inhalt in den Anomalie-Räumen (`e_rooms.py`):
* ROOM3040/3041, ROOM3090 und ROOM3091 (bis auf `rbj=0x2510`): **alle fünf = 0** (Raum hat weder
  ESP noch Modell-TIM noch RBJ). Ein 0-Eintrag wird vom Loader übersprungen
  (`beq a0,zero,LAB_80039820` @0x80039808) und bleibt damit ein reiner Null-Zeiger.
* ROOM1220: `esp=0x2650, eff=0x2AEC, esp_tim=0x16DE8, model_tim=0x1A288, rbj=0`.
  Byte-Probe:
  ```
  002650  05 07 FF FF FF FF FF FF 09 00 1D 00 40 78 00 00   ESP-Kopf
  002AEC  08 00 00 00 00 00 07 12 ...                        EFF (4 B bis snd0_edt @0x2AF0)
  016DE8  10 00 00 00 08 00 00 00 2C 00 00 00 00 00 E0 01   Sony-TIM (4bpp+CLUT), CLUT y=0x1E0=480
  01A288  10 00 00 00 09 00 00 00 0C 02 00 00 00 00 E0 01   Sony-TIM (8bpp+CLUT), CLUT y=480
  ```
* ROOM3050/3051 (`esp=0x6744 eff=0x6BE0 esp_tim=0x224DC model_tim=0x2597C rbj=0`),
  ROOM30A0/30A1 (`0x1800 / 0x1C9C / 0x25698 / 0x28B38 / 0`),
  ROOM4040/4041 (`0x1E34 / 0x22D0 / 0x189AC / 0x1BE4C / 0`),
  ROOM4080/4081 (`0x12E4 / 0x1780 / 0x17E5C / 0x1B2FC / 0`),
  ROOM40A0/40A1 (`0x1420 / 0x1A1C / 0x13B98 / 0x17CD8 / 0`): ESP + beide TIMs belegt, `rbj=0`.
* ROOM30E0 (`esp=0x164AC eff=0x16948 esp_tim=0x2C6D4 model_tim=0x2FB74 rbj=0x1528`) und
  ROOM30E1 (`0xE6A4 / 0xEB40 / 0x248CC / 0x27D6C / 0x13F8`): alle fünf belegt.

⇒ **Keiner der Einträge 0x4C..0x5C enthält Maskengeometrie.** Sie sind alle regulär belegt oder 0.

RE15Editor-Gegenprobe: `_64_sprite_pri_address/SpritePriAddress64.java` +
`RdtProcessing.java:304-316` kennt **ebenfalls keinen Pointer** und leitet die Adresse aus
`endOfLightLit + 1` ab; die Format-Notiz `RDT1000 - RDT-FORMAT-INFO.txt`
("1224 - 1299 - sprite.pri (Am aller schwersten zu ermitteln, da kein pointer …)") bzw.
`RDT1170 - RDT-FORMAT-INFO.txt` ("1064-2067 - sprite.pri (E - kein Pointer)") bestätigen Lage
und Fehlen eines zweiten Verweises. Header-Byte 7 heißt dort "Sprite_max - Total value of PRI
cameras sprites (masks) that are used in the room" — in allen Anomalie-Räumen außer
ROOM3090/ROOM40A0 ist er **0**.

---

## 4. BEWEIS: DIE ORIGINAL-ENGINE ZEICHNET DORT NICHTS (Teilaufgabe 4)

### 4.1 Der NULL-Zweig im Maskenbauer FUN_800392d4

```
80039310 24 00 83 8c     lw    v1,0x24(a0)        ; a0 = RDT-Basis, v1 = Kamera-Cut-Tabelle
80039314 40 11 02 00     sll   v0,v0,0x5          ; v0 = aktueller Cut (DAT_800b0fe4) * 32
80039318 21 10 43 00     addu  v0,v0,v1
8003931c 1c 00 4d 8c     lw    t5,0x1c(v0)        ; t5 = pri-Zeiger des Cuts
80039324 00 00 a3 8d     lw    v1,0x0(t5)         ; v1 = erstes u32 der Sektion
80039328 ff ff 02 24     li    v0,-0x1            ; 0xFFFFFFFF
8003932c 03 00 62 14     bne   v1,v0,LAB_8003933c ; ungleich -> normal bauen
80039330 02 54 03 00     _srl  t2,v1,0x10
80039334 57 e5 00 08     j     LAB_8003955c       ; -> RETURN
80039338 00 00 80 a0     _sb   zero,0x0(a0)       ; <<< RDT-Header-Byte[0] = 0
```
Das ist **der** Zweig: bei `FF FF FF FF` wird der Maskenzähler (RDT-Header Byte[0], im RAM
überschrieben) auf **0** gesetzt und die Funktion verlassen — es wird **kein** Prim gebaut,
kein Flag-Byte gesetzt, kein Puffer angefasst.

Zum Vergleich der Normalfall: @**0x80039358** `sb t2,0x0(a0)` schreibt `pri_header>>16` (die
deklarierte Maskenzahl) in dasselbe Byte.

Alle 206 RDT-Dateien tragen auf Platte `hdr[0] == 0` (gemessen, 0 Ausnahmen) — der Wert
entsteht also ausschließlich in FUN_800392d4.

### 4.2 Der Zeichner FUN_80039590 steigt bei 0 sofort aus

```
80039590 0b 80 02 3c     lui   v0,0x800b
80039594 78 c7 42 8c     lw    v0,-0x3888(v0)     ; v0 = RDT-Basis (DAT_800ac778)
800395c0 00 00 54 90     lbu   s4,0x0(v0)         ; s4 = Header-Byte[0] = Maskenzahl
800395cc 2a 00 80 12     beq   s4,zero,LAB_80039678   ; <<< 0 -> ans Funktionsende
800395d0 21 90 00 00     _clear s2
...
8003961c c1 ad 01 0c     jal   SetSprt                (uebersprungen)
80039630 95 00 07 34     ori   a3,zero,0x95           (uebersprungen)
80039634 16 a6 01 0c     jal   SetDrawMode            (uebersprungen)
80039640 56 ae 01 0c     jal   MargePrim              (uebersprungen)
80039660 4e ad 01 0c     jal   AddPrim                (uebersprungen)
...
80039678 40 00 bf 8f     lw    ra,local_8(sp)     ; LAB_80039678 = reines Epilog/return
800396a0 08 00 e0 03     jr    ra
```
Bei `hdr[0]==0` erreicht **kein einziges SPRT-Prim** die Ordering-Table.

Aufrufer sind eindeutig (grep über den kompletten Dump):
* `jal FUN_800392d4` **nur** @0x80021c28 (Kamerawechsel, FUN_80021bbc)
* `jal FUN_80039590` **nur** @0x8001ce54 (Frame-Render)
* `jal FUN_800396a8` **nur** @0x800428f4 (SCD-Opcode 0x45)

Und `+0x1C` des Cut-Records wird im ganzen Spiel **nur** von FUN_800392d4 @0x8003931c gelesen —
die übrigen Konsumenten der Cut-Tabelle (`FUN_80045a64`, `FUN_80053ca4`, `FUN_800460b8`,
`FUN_80021bbc`) lesen ausschließlich +0x02 (Projektion) und +0x04..+0x18 (Auge/Ziel).

### 4.3 Auch der SCD-Schalter kann nichts einschalten

SCD-Opcode **0x45** (Dispatch-Eintrag @0x800745bc der Tabelle `PTR_LAB_800744a8`,
Index `(0x800745bc-0x800744a8)/4 = 0x45`) -> `LAB_800428d4` -> `FUN_800396a8(a0=op1+1, a1=op2)`:
```
800428e4 1c 00 02 8e     lw    v0,0x1c(s0)        ; pc
800428ec 01 00 44 90     lbu   a0,0x1(v0)         ; op1
800428f0 02 00 45 90     lbu   a1,0x2(v0)         ; op2
800428f4 aa e5 00 0c     jal   FUN_800396a8
800428f8 01 00 84 24     _addiu a0,a0,0x1
80042904 03 00 63 24     addiu v1,v1,0x3          ; pc += 3

800396b8 00 00 47 90     lbu   a3,0x0(v0)         ; Header-Byte[0]
800396c4 0a 00 e0 10     beq   a3,zero,LAB_800396f0 ; <<< 0 -> return
```
Ein Skript kann Masken also weder ein- noch ausschalten, wenn es keine gibt.

Zusätzlich (heuristischer SCD-Lauf `e_scd45.py`, Opcode-Längentabelle aus `scd_vm.c`,
Abbruch beim ersten nicht-registrierten Opcode): **kein einziger** Anomalie-Raum benutzt
Opcode 0x45 — im Gegensatz zu ROOM1150 (12x, sauberes
`45 05 00 / 45 06 00 / 45 07 00 / 45 08 00 / 45 09 00 / 45 0A 00` = Gruppen 6..11 AUS,
später `45 05 01 / ... / 45 0A 01` = wieder AN) und ROOM3000 (18x).
*(Als "teilbelegt" gewertet: die Endgrenze des jeweils letzten Skripts ist heuristisch.)*

### 4.4 Sicherheitsnetz-Beweis: `hdr7 == 0` => **alle** Cuts müssen NULL sein

Der Puffer-Allokator **FUN_80039270** dimensioniert die Maskenpuffer aus `hdr[7]`:
```
DAT_800b2584 = arena;                                  ; Flag-Bytes, 4 B je Maske
DAT_800bb4d4 = arena + hdr[7]*4;                       ; Prim-Puffer A, 0x20 B je Maske
DAT_800bb4d8 = DAT_800bb4d4 + hdr[7]*0x20;             ; Prim-Puffer B
DAT_800ac77c = DAT_800bb4d8 + hdr[7]*0x20;             ; neue Arena-Spitze
```
Die Löschschleife im Normalzweig von FUN_800392d4 ist ein **do-while**:
```
8003936c 07 00 47 90     lbu   a3,0x7(v0)         ; a3 = hdr[7]
80039374 ff ff e7 24     addiu a3,a3,-0x1
80039378 00 00 a0 a2     sb    zero,0x0(s5)
8003937c ff 00 e2 30     andi  v0,a3,0xff
80039380 fb ff 40 14     bne   v0,zero,LAB_80039370
80039384 04 00 b5 26     _addiu s5,s5,0x4
```
Mit `hdr[7]==0` liefe sie **255x** und würde 1020 Byte hinter einen 0-Byte-Puffer schreiben.
Dass das nie passiert, ist selbst ein Beleg. Gemessen über alle 206 RDTs:
* `hdr7 == 0` => jeder Cut NULL: **0 Verletzungen**
* `hdr7 == max_über_Cuts(Summe der Gruppen-Counts)`: **206/206**

Die Anomalie-Räume sind also nicht "defekt", sondern konsistent maskenlos gebaut.

---

## 5. WARUM DER ATLAS TROTZDEM IM VRAM LANDET

Der SLD-Upload hängt an einem **völlig getrennten** Zweig in FUN_80021bbc (Kamerawechsel):

```
80021c20 04 00 40 14     bne   v0,zero,LAB_80021c34   ; DAT_800aca38 & 0x100000 -> Masken ueberspringen
80021c28 b5 e4 00 0c     jal   FUN_800392d4           ; (A) MASKEN bauen  <-- liest die RDT
...
80021d6c fc ff 22 8c     lw    v0,-0x4(at)=>DAT_8018fffc   ; Wort im BSS-Chunk-Kopf
80021d74 20 00 40 10     beq   v0,zero,LAB_80021df8        ; (B) kein SLD -> ueberspringen
80021da4 f8 ff 30 8c     lw    s0,-0x8(at)=>DAT_8018fff8   ; Offset des SLD-Unterblocks
80021db8 fa 11 03 0c     jal   FUN_800c47e8                ; SLD entpacken -> 0x801A5800
80021de4 00 00 11 96     lhu   s1,0x0(s0)=>DAT_800aca4c    ; Slot-Zaehler sichern
80021de8 15 00 02 34     ori   v0,zero,0x15                ; VRAM-Slot 0x15 erzwingen
80021dec 9e 3b 01 0c     jal   FUN_8004ee78                ; TIM -> VRAM
80021df0 00 00 02 a6     _sh   v0,0x0(s0)
80021df4 00 00 11 a6     sh    s1,0x0(s0)                  ; Slot-Zaehler wiederherstellen
```

(A) liest die RDT, (B) liest den BSS. **Die beiden Zweige wissen nichts voneinander.**
Deshalb kann ein Cut einen Atlas haben und trotzdem keine Masken — genau die 130 Fälle.

Der erzwungene Slot 0x15 ergibt in **FUN_8004ee78** byte-genau die Sampling-Adresse der Masken:
```
8004eea0 00 00 23 92     lbu   v1,0x0(s1)=>DAT_800aca4c
8004eea8 80 21 03 00     sll   a0,v1,0x6           ; x = slot*64 = 0x15*64 = 1344
8004eeac 10 00 63 2c     sltiu v1,v1,0x10
8004eeb0 02 00 60 14     bne   v1,zero,LAB_8004eebc
8004eeb8 00 fc 84 24     addiu a0,a0,-0x400        ; slot>=0x10 -> x -= 1024  => x = 320
8004eebc 00 00 44 a4     sh    a0,0x0(v0)          ; prect->x = 320
8004eec8 10 00 42 2c     sltiu v0,v0,0x10
8004eecc 01 00 42 38     xori  v0,v0,0x1
8004eed0 00 12 02 00     sll   v0,v0,0x8           ; prect->y = 1<<8 = 256
8004eed4 02 00 62 a4     sh    v0,0x2(v1)
8004eee0 22 a3 01 0c     jal   LoadImage
8004ef38 e0 01 42 24     addiu v0,v0,0x1e0         ; crect->y = CLUT-Zeile + 480
```
und der Maskenzeichner sampelt exakt dort:
```
80039630 95 00 07 34     ori   a3,zero,0x95        ; SetDrawMode tpage 0x95
                                                    ; 0x95 = 1001_0101b: tp=1 (8bpp),
                                                    ; x = (0x95 & 0xF)*64 = 320, y = (bit4)*256 = 256
80039498 00 78 02 34     ori   v0,zero,0x7800      ; CLUT-ID = GetClut(0,480)  (0x7800>>6 = 480)
```
Der Immediate `0x95` kommt im **gesamten** PSX.EXE-Dump **genau einmal** vor (@0x80039630).
=> **Die Maskensprites sind der einzige Konsument der Texturseite (320,256).**
Ohne Masken sampelt niemand den Atlas. Er liegt außerdem außerhalb der beiden Framebuffer
(die bei y=0 bzw. y=240 liegen — `DAT_80072f2e = 0 oder 0xf0`, gesetzt @0x80021e30) und ist auf
dem Bildschirm nicht sichtbar.

Gegenprobe an den extrahierten Dateien: `BSS/ROOM3040/PRI00.TIM` = `magic 0x00000010`,
`flag 0x00000009` (8bpp + CLUT), CLUT-Rect `(0,480) 256x1`, Bild-Rect `128x256` Halbwörter
= **256x256 Pixel 8bpp** — exakt die Geometrie, die tpage 0x95 aufspannt.
(Das `x/y` im TIM selbst ist (0,0) und wird von FUN_8004ee78 @0x8004eebc/0x8004eed4 überschrieben.)

---

## 6. EXAKTE ZÄHLUNG DER BETROFFENEN CUTS

| Raum | nCut | hdr7 | Cuts m. Masken | Cuts m. SLD-Atlas | **ANOMALIE** | Cut-Indizes | PRI##.TIM extrahiert |
|---|---|---|---|---|---|---|---|
| ROOM1220 | 10 | 0 | 0 | 8 | **8** | 0,1,2,3,4,5,6,8 | 7/8 |
| ROOM1221 | 10 | 0 | 0 | 8 | **8** | 0,1,2,3,4,5,6,8 | 7/8 |
| ROOM3040 | 14 | 0 | 0 | 14 | **14** | 0–13 | 14/14 |
| ROOM3041 | 14 | 0 | 0 | 14 | **14** | 0–13 | 14/14 |
| ROOM3050 | 16 | 0 | 0 | 15 | **15** | 0–9, 11–15 | 15/15 |
| ROOM3051 | 16 | 0 | 0 | 15 | **15** | 0–9, 11–15 | 15/15 |
| ROOM3090 | 16 | 26 | 6 | 7 | **1** | 15 | 1/1 |
| ROOM3091 | 16 | 26 | 6 | 7 | **1** | 15 | 1/1 |
| ROOM30A0 | 7 | 0 | 0 | 6 | **6** | 0–5 | 6/6 |
| ROOM30A1 | 7 | 0 | 0 | 6 | **6** | 0–5 | 6/6 |
| ROOM30E0 | 10 | 0 | 0 | 4 | **4** | 6,7,8,9 | 4/4 |
| ROOM30E1 | 10 | 0 | 0 | 4 | **4** | 6,7,8,9 | 4/4 |
| ROOM4040 | 13 | 0 | 0 | 9 | **9** | 2,4,5,6,7,9,10,11,12 | 8/9 |
| ROOM4041 | 13 | 0 | 0 | 9 | **9** | 2,4,5,6,7,9,10,11,12 | 8/9 |
| ROOM4080 | 9 | 0 | 0 | 7 | **7** | 1,2,3,5,6,7,8 | 7/7 |
| ROOM4081 | 9 | 0 | 0 | 7 | **7** | 1,2,3,5,6,7,8 | 7/7 |
| ROOM40A0 | 8 | 35 | 6 | 7 | **1** | 7 | 1/1 |
| ROOM40A1 | 8 | 35 | 6 | 7 | **1** | 7 | 1/1 |
| **SUMME** | | | | | **130** | | 125/130 |

= **65 eindeutige Kamera-Cuts x 2 Spielervarianten** (`ROOM####0` / `ROOM####1`) in
**9 eindeutigen Räumen** (18 RDT-Dateien).

Zwei Klassen:
* **Klasse A — ganzer Raum ohne Maskensystem** (`hdr7 == 0`): ROOM1220, 3040, 3050, 30A0, 30E0,
  4040, 4080. Dort ist der `sprite.pri`-Block auf sein Minimum geschrumpft. **128** der 130 Cuts.
* **Klasse B — Einzel-Cut in einem Raum, der sonst Masken hat**: ROOM3090 Cut 15 (`hdr7=26`,
  6 Cuts mit echten Masken) und ROOM40A0 Cut 7 (`hdr7=35`, 6 Cuts mit echten Masken). **2**
  der 130 Cuts. Hier ist ein einzelner Cut nicht fertig geworden, während die Nachbarn fertig sind.

### Der Atlas-Inhalt ist echt (aber unbenutzt)

Anteil Nicht-Null-Pixel (Index 0 = transparent) der extrahierten Atlanten:

```
ROOM1220  c01 10.6% c02 11.8% c03 6.4% c04 13.1% c05 10.2% c06 5.3% c08 4.8%   (c00: keine TIM extrahiert)
ROOM3040  c00 8.1% c01 27.2% c02 33.6% c03 33.0% c04 30.8% c05 30.5% c06 24.8%
          c07 37.4% c08 42.1% c09 29.6% c10 2.6% c11 30.2% c12 32.4% c13 46.3%
ROOM3050  c00 14.1% c01 33.2% c02 35.3% c03 23.1% c04 24.4% c05 36.0% c06 17.6% c07 26.7%
          c08 6.0% c09 24.4% c11 18.0% c12 8.6% c13 21.4% c14 24.4% c15 52.0%
ROOM3090  c15 4.0%
ROOM30A0  c00 30.7% c01 23.7% c02 4.9% c03 44.1% c04 25.7% c05 14.3%
ROOM30E0  c06 21.1% c07 21.8% c08 18.3% c09 19.2%
ROOM4040  c02 1.9% c04 15.7% c06 17.6% c07 12.3% c09 43.5% c10 18.5% c11 14.5% c12 28.5%
          (c05: keine TIM extrahiert)
ROOM4080  c01 14.2% c02 30.0% c03 11.8% c05 52.2% c06 26.9% c07 14.7% c08 24.8%
ROOM40A0  c07 8.4%
```
Alle 256x256 8bpp mit CLUT (0,480). Es ist gezeichnetes Vordergrund-Material — nur die
Platzierungsdaten (welcher Ausschnitt wohin, mit welcher Tiefe) wurden nie in die RDT geschrieben.

---

## 7. KONSEQUENZ FÜR DEN PORT

**Der Port verhält sich bereits korrekt und darf NICHTS erfinden.**

* `re15_port/engine/src/pri_common.c` —
  `if (group_count == 0xFFFF && mask_count_decl == 0xFFFF) return 0;`
  ist die 1:1-Entsprechung zu `bne v1,v0` @0x8003932c / `sb zero,0x0(a0)` @0x80039338.
* `re15_port/platform/pc/main.c` (~Z. 4454) — `int has_fg = re15_pri_load_cut_atlas(active_cut_idx);`
  lädt den Atlas unabhängig von den Masken (= Zweig (B) @0x80021d74). Das ist das exakte
  Gegenstück zum Original-Verhalten "Atlas in den VRAM, auch ohne Masken".
* `re15_port/platform/pc/main.c` (~Z. 4457) — `if (has_fg && pri_n > 0 && !getenv("RE15_NO_PRI"))`,
  sonst `re15_render_pc_set_pri_rects(NULL,...,0)`. Bei den 130 Cuts ist `pri_n == 0`
  => 0 Rechtecke => kein Overdraw. Das ist exakt `hdr[0]==0` + `beq` @0x800395cc.

Keine Änderung nötig. **Jede Heuristik, die aus dem Atlas Masken *ableiten* würde
(Silhouetten, Bounding-Boxen, "alles was nicht Index 0 ist"), wäre eine Erfindung und
byte-falsch** — die Tiefe (`depth`, der OT-Wortindex @0x80039658) ist im Atlas physisch nicht
enthalten und aus Pixeln prinzipiell nicht rekonstruierbar.

Nebenbefund (nicht Teil dieser Anomalie, aber hier mitgemessen):
**5 der 130 Anomalie-Cuts haben gar keine extrahierte `PRI##.TIM`**
(ROOM1220/1221 Cut 0, ROOM4040/4041 Cut 5). Folgenlos, solange dort ohnehin nichts gezeichnet
wird, aber ein Hinweis auf eine Lücke in der Extraktionspipeline — vgl. ROOM1020/1030/1040/1070,
deren SLD-Blöcke auf 61984 Byte entpacken: `8 + (12+512) + (12 + 256*240) = 61984`
=> **256x240**-Atlas statt 256x256, deshalb vermutlich vom Extraktor verworfen.

---

## 8. WAS NICHT BELEGT IST

* **Warum** die Geometrie fehlt (Zeitmangel, Werkzeugstand, bewusst verworfen) — aus den
  Daten nicht ermittelbar. **NICHT BELEGT.**
* Ob der Atlas eines Anomalie-Cuts *inhaltlich* zum jeweiligen Hintergrund passt (also
  "hätte gezeichnet werden sollen") — nur optisch beurteilbar, kein Byte-Beleg.
  **NICHT BELEGT.**
* Die SCD-Opcode-0x45-Zählung (§4.3) ist **teilbelegt**: Opcode-Länge 0x45 = 3 ist
  disasm-belegt (`addiu v1,v1,0x3` @0x80042904) und der Dispatch-Index ebenfalls
  (`0x800745bc` in `PTR_LAB_800744a8`), aber die Endgrenze des jeweils letzten Skripts
  eines Blocks ist heuristisch (Abbruch beim ersten nicht-registrierten Opcode).
* Die Deutung der Rückwärts-Offsettabelle am Blockende (§1.2) als *Build-Metadaten* ist eine
  Schlussfolgerung aus "kein Leser gefunden"; **belegt** ist nur, dass die Tabelle in 206/206
  RDTs existiert, exakt die Cut-Offsets in Umkehrreihenfolge enthält und mit `hdr[7]` endet,
  und dass kein Engine-Zweig `sca_addr - 4*(nCut+1)` adressiert.

---

## Gegenpruefung

Datum: 2026-09-03 - Gegenpruefender Agent. Methode: jede zitierte Adresse in `ghidra1_V2.txt`
bzw. `RE_15_Quellcode_V2/` geoeffnet; **alle** Datenbehauptungen mit eigenem Python neu erhoben
(nicht die Zahlen des Berichts uebernommen), Quelle `re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT`
(206 echte RDTs, 34 Stub-Dateien < 0x60 B ausgeschlossen).

### WIDERLEGT

#### W1 - Behauptung 1: die Staffel-Formel `pri_offset = lit + nCut*40 + cut*4` gilt NICHT fuer alle 130 Cuts

Die FF-FF-FF-FF-Aussage haelt (siehe B1 unten), die **Formel** nicht. Eigene Nachrechnung ueber
alle 130 Anomalie-Cuts: **126 erfuellen sie, 4 nicht** - genau die beiden Einzel-Cuts in
maskenfaehigen Raeumen (dort haben die vorangehenden Sektionen echte, variable Laenge, also ist die
Schrittweite 4 nicht mehr gegeben):

| RDT | Cut | gespeicherter `pri_offset` | Formel `lit+nCut*40+cut*4` |
|---|---|---|---|
| ROOM3090 (lit=0x5C8, nCut=16) | 15 | **0x0C9C** | 0x0884 |
| ROOM3091 (lit=0x5D0, nCut=16) | 15 | **0x0CA4** | 0x088C |
| ROOM40A0 (lit=0x2B8, nCut=8) | 7 | **0x0CA8** | 0x0414 |
| ROOM40A1 (lit=0x2B8, nCut=8) | 7 | **0x0CA8** | 0x0414 |

Der Bericht widerlegt sich hier selbst: sein eigener BELEG nennt `ROOM40A0: cut7 @0x015C =
0x00000CA8`, waehrend die im selben Satz behauptete Formel 0x0414 liefert.
Roh-Abzug der ROOM40A0 `cut+0x1C`-Werte: `0x3F8, 0x3FC, 0x594, 0x6E4, 0x840, 0xA04, 0xB58, 0xCA8`
- offensichtlich variable Schrittweite.

**Was statt der Formel gilt** (selbst gemessen): `min(pri_offset) == lit + nCut*40` in 206/206
RDTs, und **jeder** Cut-Offset liegt im Intervall `[lit+nCut*40, sca)` - 2188/2188 Cuts.
Die Schrittweite 4 gilt nur in Raeumen mit `hdr7 == 0`.

#### W2 - Behauptung 11 (zweite Haelfte): "Immediate genau einmal -> Atlas vollstaendig unbenutzt" ist ein Fehlschluss

Der Grep-Befund selbst stimmt (`grep ',0x95$'` -> genau 1 Treffer, Zeile 137493,
`80039630 ori a3,zero,0x95`; eigene Zusatzprobe ueber alle 8 Overlay-BINs auf wortausgerichtete
`ori/li rt,zero,0x95`: **0 Treffer**). Der **Schluss** ist trotzdem nicht haltbar:

**(a) Ein Literal-Grep sieht keine per `GetTPage` berechneten TPages.** Es gibt 8 `jal GetTPage`-
Stellen in der EXE; eine davon erzeugt zur Laufzeit **exakt den Wert 0x95**:

```
8002e048 01 00 04 34     ori   a0,zero,0x1        ; tp = 1  (8 bpp)
8002e04c 21 28 00 00     clear a1                 ; abr = 0
8002e050 40 01 06 34     ori   a2,zero,0x140      ; x = 320
8002e054 c4 ac 01 0c     jal   GetTPage           ; y = 256 (a3=0x100, Delay-Slot 8002e058)
```
`GetTPage(1,0,320,256) = (1<<7)|(0<<5)|((256&0x100)>>4)|(320>>6) = 0x80|0x10|0x05 = 0x95`.

**(b) VRAM-Slot 0x15 = (320,256) ist eine GETEILTE Scratch-Seite, kein Maskenreservat.**
`DAT_800aca4c` ist ein globaler VRAM-Cursor, den mehrere Subsysteme vor dem Laden auf denselben
Wert 0x15 setzen - und danach lesen:

```
; Death-Screen  (FUN_8001613c, gerufen aus der Death-Cam-Init LAB_80015284 @80015290)
80016150 addiu a3,a3,0x664                 ; "YOUDIED.TIM"
80016160 15 1f 02 34  ori  v0,zero,0x1f15  ; Slot 0x15 + CLUT-Zeilenversatz 0x1f
8001616c 4c ca 22 a4  sh   v0,-0x35b4(at)=>DAT_800aca4c
80016170 jal FUN_8004ee78                  ; -> laedt nach (320,256)
; und wird unmittelbar danach gesampelt (FUN_80015a80, gerufen @80015298):
80015ae0 ori a0,zero,0x1 / 80015ae4 ori a1,zero,0x1 / 80015ae8 ori a2,zero,0x140
80015aec jal GetTPage   (a3=0x100)  -> Texturseite (320,256), 8 bpp, abr=1  (= 0xB5)
80015c64 ori a1,zero,0x1ff / 80015c68 jal GetClut  -> CLUT (0,511) = 480 + 0x1f

; Config-Screen (FUN_8002dfb0, Tabelleneintrag 80073d9c)
8002dff8 addiu a3,a3,0x9f8                 ; "Config.tim"
8002e008 15 00 02 34  ori  v0,zero,0x15
8002e010 4c ca 22 a4  sh   v0,-0x35b4(at)=>DAT_800aca4c
8002e014 jal FUN_8004ee78                  ; -> laedt EBENFALLS nach (320,256)
```

Damit ist belegt: dieselbe Seite (320,256) wird von mindestens zwei weiteren Subsystemen
beschrieben **und** gelesen. Die Aussage "der Atlas liegt im VRAM auf (320,256) und wird von
nichts gesampelt / ist vollstaendig unbenutzt" ist mit den vorgelegten Belegen **nicht** gezeigt -
gezeigt ist nur, dass ihn der *Maskenzeichner* nicht sampelt (weil `hdr[0]==0`). Ob ihn ein
anderer Konsument sampelt oder ihn nur ueberschreibt, ist aus den vorgelegten Daten
**NICHT BELEGT**.

Nebenbefund (stuetzt die CLUT-Aussage): `80021de8 ori v0,zero,0x15` + `80021df0 sh v0,0x0(s0)`
schreibt ein **Halbwort** 0x0015, setzt also `DAT_800aca4c = 0x15` UND `DAT_800aca4d = 0x00`.
Nur deshalb ergibt `8004ef38 addiu v0,v0,0x1e0` die CLUT-Zeile 480 (der Death-Screen setzt
denselben Cursor auf 0x1f15 und bekommt 511).

#### W3 - Behauptung 12: die Aufteilung ist 126/4, nicht 128/2

Die Raum/Cut-Tabelle stimmt (eigene Erhebung deckt sich 18/18 Raeume, 130/130 Cuts). Die
Klassenaufteilung nicht: Anomalie-Cuts werden pro RDT gezaehlt, also **beide** Spielervarianten.
Gemessen:

* `hdr7 == 0` (ganzer Raum maskenlos): **126** Cuts (ROOM1220/1221 je 8, 3040/3041 je 14,
  3050/3051 je 15, 30A0/30A1 je 6, 30E0/30E1 je 4, 4040/4041 je 9, 4080/4081 je 7)
* Einzel-Cut in maskenfaehigem Raum: **4** Cuts - ROOM3090 **und** ROOM3091 je Cut 15 (hdr7=26),
  ROOM40A0 **und** ROOM40A1 je Cut 7 (hdr7=35)

126 + 4 = 130. Der Bericht nennt 128/2 und zaehlt die Einzel-Cut-Faelle damit nur einfach.

#### W4 - Behauptung 14 (zweite Haelfte): "Raeume mit echten Masken benutzen 0x45 dagegen schon"

Selbst nachdisassembliert mit derselben Laengentabelle (`re15_port/engine/src/scd_vm.c`,
`[0x45] = 3`), linear ueber alle drei SCD-Bloecke (`hdr 0x40/0x44/0x48`) und alle Eintraege
der jeweiligen Zeigertabelle, ueber **alle 206 RDTs**:

| Menge | Anzahl | davon mit Opcode 0x45 |
|---|---|---|
| Raeume MIT echten Maskensektionen | 68 | **10** |
| Raeume OHNE Maskensektionen | 138 | **0** |

Nur **10 von 68** maskenfuehrenden Raeumen benutzen 0x45 - 58 nicht. Gegenbeispiele aus dem
Bericht selbst: **ROOM1170** (4 echte Sektionen, hdr7=56) -> **0x** Opcode 0x45; **ROOM1210**
(2 Sektionen) -> **0x**. Die beiden zitierten Positivbeispiele (ROOM1150 = 12x, ROOM3000 = 18x,
beide reproduziert) tragen die Verallgemeinerung nicht.
Die **Gegenrichtung** haelt und ist die eigentlich tragfaehige Aussage: 0x45 kommt in **keinem**
Raum ohne Masken vor - also auch in keinem Anomalie-Raum (10/10 Anomalie-Raeume 0x, reproduziert).

### Korrekturen an BELEG-Texten (Behauptung selbst bleibt stehen)

* **Behauptung 13**, Nebenrechnung: die Loeschschleife `@0x80039370..0x80039384` ist ein do-while
  mit `andi v0,a3,0xff`. Bei `hdr7 == 0` laeuft sie **256x** (nicht 255x) und beschreibt wegen
  `addiu s5,s5,0x4` einen Bereich von **1024 Byte** (nicht 1020). Die Invarianten selbst sind
  206/206 unabhaengig bestaetigt.
* **Behauptung 3**, Wort "garantiert": `FUN_800392d4` ist gegatet (`@0x80021c20 bne v0,zero`,
  `DAT_800aca38 & 0x100000`), und `hdr[0]` ist ein fortgeschriebenes Laufzeit-Byte. Fuer die 126
  Cuts in `hdr7 == 0`-Raeumen ist das folgenlos (gemessen: **alle 206** RDT-Dateien tragen
  `byte[0] == 0` auf der CD, und in jenen Raeumen ist jeder Cut NULL). Fuer ROOM3090 Cut 15 /
  ROOM40A0 Cut 7 waere "garantiert" nur mit einem Beleg haltbar, dass Bit 0x100000 dort nie
  gesetzt ist - ein Setzer fuer dieses Bit ist in `RE_15_Quellcode_V2/` **nicht auffindbar**
  (einziger Treffer ist der Leser `FUN_80021bbc.c:10`) => **NICHT BELEGT**, in beide Richtungen.
* **Behauptung 6**: der zitierte Scan begrenzte `groupCount` auf 1..8, die echten Sektionen gehen
  aber bis **34** Gruppen (selbst gemessen ueber die 480 Sektionen: groupCount 1..34, decl 1..105,
  destX -134..187, destY -135..149, depth 0..1023). Ich habe den Scan mit 1..34 wiederholt -
  Ergebnis unveraendert **0 Treffer** ausserhalb des Blocks in 206/206 RDTs. Die Behauptung haelt,
  ihr urspruenglicher Beleg war zu eng gefasst.
* **Behauptung 9**: der Negativbefund stuetzt sich nur auf `RE_15_Quellcode_V2/` (EXE); die
  Stage-Overlays wurden nicht durchsucht. Bleibt korrekt als "teilbelegt" markiert.

### BESTAETIGT (unabhaengig nachgemessen)

* **B1** (Kern von Beh. 1): **130/130** Anomalie-Cuts zeigen auf `FF FF FF FF`; **2188/2188**
  Cut-Offsets liegen im `sprite.pri`-Block; **0** Cut traegt `0xFFFFFFFF` im Feld selbst.
* **Beh. 2**: `@80039800..80039830` Header-Schleife MIT Nulltest (`beq a0,zero,LAB_80039820`,
  Schranke `addiu v0,v0,0x60`), `@80039844..80039898` Cut-Schleife **ohne** jeden Test -
  `lw v0,0x1c(a0)` / `addu v0,v0,v1` / `sw v0,0x1c(a0)` woertlich wie zitiert.
* **Beh. 3**: `@8003931c lw t5,0x1c(v0)`, `@80039324 lw v1,0x0(t5)`, `@80039328 li v0,-0x1`,
  `@8003932c bne v1,v0,LAB_8003933c`, `@80039334 j LAB_8003955c`, `@80039338 sb zero,0x0(a0)`
  mit `a0 = DAT_800ac778` (RDT-Basis, geladen `@800392d8`). Zeichner `FUN_80039590`:
  `@800395c0 lbu s4,0x0(v0)` / `@800395cc beq s4,zero,LAB_80039678` vor `jal SetSprt` `@8003961c`,
  `jal SetDrawMode` `@80039634`, `jal AddPrim` `@80039660`; Epilog `@80039678`, `jr ra @800396a0`.
* **Beh. 4**: `800745bc addr LAB_800428d4` in der Tabelle ab `800744a8` => Index 0x45;
  `@800428f4 jal FUN_800396a8`, `@80042904 addiu v1,v1,0x3`; in `FUN_800396a8`
  `@800396b8 lbu a3,0x0(v0)` / `@800396c4 beq a3,zero,LAB_800396f0`.
* **Beh. 5**: `min(pri_offset) == lit + nCut*40` **206/206**; Sektionen kontiguierend bis zur
  Schlusstabelle mit **0 Byte Schlupf**, Blockende == `sca` **206/206**.
* **Beh. 6**: 0 gueltige Sektionssignatur ausserhalb des Blocks (mit erweitertem Scan, s.o.).
  `group[+2]`-Wertemenge unabhaengig reproduziert: {0x7800 x2712, 0x7C00 x26, 0x7C10 x8,
  0x8080 x2, 0x8088 x2} ueber 2750 Gruppen in 480 Sektionen.
* **Beh. 7**: ROOM1220 `hdr[0x4C..0x5C]` = `0x2650 / 0x2AEC / 0x16DE8 / 0x1A288 / 0`; Bytes
  `05 07 ff ff ff ff ff ff`, `08 00 00 00`, `10 00 00 00 08 00 00 00 ... 00 00 e0 01`,
  `10 00 00 00 09 00 00 00` exakt wie zitiert; ROOM3040/3041/3090 alle fuenf = 0
  (Randnotiz: ROOM**3091** hat `rbj = 0x2510`, also nicht 0). Leser vorhanden:
  `FUN_80019354.c:29-32`, `FUN_8001b3f8.c:13`.
* **Beh. 8**: Rueckwaerts-Offsettabelle + abschliessendes `hdr7`-Dword **206/206** exakt
  bestaetigt (Byte-Vergleich gegen die erwartete Liste). ROOM1170 `@0x0A48`:
  `d8 03 00 00 ... 00 00 00 00 38 00 00 00` (lit=0x464, nCut=13 => pri_base 0x66C, sca 0xA80);
  ROOM1220 `@0x0504`: `24 00 00 00 ... 00 00 00 00 00 00 00 00`.
* **Beh. 10**: `@80021c20 bne` (Gate 0x100000) ueberspringt `@80021c28 jal FUN_800392d4`;
  SLD-Zweig unabhaengig davon: `@80021d6c lw v0,-0x4(at)=>DAT_8018fffc`,
  `@80021d74 beq v0,zero,LAB_80021df8`, `@80021da4 lw s0,-0x8(at)`,
  `@80021db8 jal FUN_800c47e8` (Ziel `a2 = 0x801A5800`, gesetzt `@80021da8/80021dac`),
  `@80021de8 ori v0,zero,0x15`, `@80021dec jal FUN_8004ee78`, `@80021df4 sh s1` (Cursor zurueck).
* **Beh. 11 (erste Haelfte)**: `FUN_8004ee78` `@8004eea8 sll a0,v1,0x6` mit `v1 = DAT_800aca4c`,
  `@8004eeac sltiu v1,v1,0x10`, `@8004eeb8 addiu a0,a0,-0x400` => x = 320;
  `@8004eed0 sll v0,v0,0x8` => y = 256; `@8004ef38 addiu v0,v0,0x1e0` => CLUT y = 480.
  Zeichner `@80039630 ori a3,zero,0x95` (8 bpp, x=320, y=256) und `@80039498 ori v0,zero,0x7800`
  (= GetClut(0,480); einziges 0x7800-Immediate im Dump). `BSS/ROOM3040/PRI00.TIM`: magic 0x10,
  flag 0x09, CLUT-Rect (0,480) 256x1, Bild-Rect 128 Halbwoerter x 256 = 256x256 px - bestaetigt.
* **Beh. 13**: `hdr7 == 0 => jeder Cut NULL` **206/206, 0 Verletzungen**;
  `hdr7 == max ueber Cuts von Summe(group.count)` **206/206, 0 Verletzungen**.
* **Beh. 15**: `pri_common.c:48 if (group_count == 0xFFFF && mask_count_decl == 0xFFFF) return 0;`
  sowie in `main.c` `int has_fg = re15_pri_load_cut_atlas(active_cut_idx);` /
  `if (has_fg && pri_n > 0 && !getenv("RE15_NO_PRI"))` ... `else
  re15_render_pc_set_pri_rects(NULL, ..., 0);` - woertlich vorhanden.
* **Beh. 16**: alle sieben Prozentwerte exakt reproduziert (ROOM3040 c13 46.3 %, c08 42.1 %;
  ROOM3050 c15 52.0 %; ROOM4080 c05 52.2 %; ROOM30A0 c03 44.1 %; ROOM4040 c09 43.5 %,
  c02 1.9 %), alle 256x256 8 bpp mit CLUT (0,480).
* **Beh. 17**: als NICHT BELEGT korrekt gekennzeichnet.
