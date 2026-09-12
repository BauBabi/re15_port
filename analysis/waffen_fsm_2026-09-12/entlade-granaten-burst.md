# Entlade-Handler Granaten (Waffen 9/10/11), Burst-Effektvorbau (0x800338A8) und die Granatwerfer-Projektilspur

RE-Dossier 2026-09-12. Quellen: `ghidra1_V2.txt` (RE1.5 PSX.EXE, t_addr=0x80010000 aus `PSX.EXE[0x18]`,
Dateioffset(va) = 0x800 + (va - 0x80010000)), `info/Re1.5/PSX.EXE` (Tabellen-Dumps),
`ghidra_re2_Leon.txt` (RE2-Beleg), `RE15_FUN_CATALOG.md` (FUN_8001f314/FUN_80019700).

Kontext (verifiziert, s. Auftrag): Entlade-Tabelle @0x80074100 (Datei 0x64900), je Waffen-Id ein Handler;
Aufrufort: Standard-Feuer-FSM Substate 2 `jalr @0x800334e0` (Index = DAT_800aca5d*4).

---

## 1. Die drei Granaten-Stubs 0x80033B38 / 0x80033B58 / 0x80033B78 (Waffen 9/10/11)

Alle drei sind BYTE-IDENTISCHE 8-Instruktions-Wrapper (0x20 Bytes) um FUN_8004eae4 (Munition -1).
XREF je genau EINER: Tabellenslot @0x80074124 / @0x80074128 / @0x8007412c (`addr LAB_80033b38/58/78`).
KEIN Effekt-Spawn, KEIN FUN_80011f50 (Schaden), Rueckgabewert von FUN_8004eae4 wird unveraendert
durchgereicht, der Aufrufer (0x800334e8 ff.) liest ihn aber nicht.

### Roh-Listing (LAB_80033b38; 58/78 wortgleich, nur Adressen +0x20/+0x40)
```
80033b38  addiu  sp,sp,-0x18          ; Frame
80033b3c  sw     ra,0x10(sp)
80033b40  jal    FUN_8004eae4         ; Munition der angelegten Waffe -1 (ret 0 = leer)
80033b44  _nop
80033b48  lw     ra,0x10(sp)
80033b4c  addiu  sp,sp,0x18
80033b50  jr     ra                   ; v0 = FUN_8004eae4-Ergebnis (ungenutzt)
80033b54  _nop
```
LAB_80033b58: jal @0x80033b60. LAB_80033b78: jal @0x80033b80. (Bytes je `b9 3a 01 0c` = jal 0x8004eae4.)

### Pseudo-C
```c
void entlade_granate_9(void)  { AmmoDecrement(); }   /* @0x80033b38 */
void entlade_granate_10(void) { AmmoDecrement(); }   /* @0x80033b58 */
void entlade_granate_11(void) { AmmoDecrement(); }   /* @0x80033b78 */
```

**Bedeutung:** Der Granatwerfer-Schuss besteht im Entlade-Schritt NUR aus dem Munitionsabzug.
Muendungseffekte, Projektil und Schaden liegen NICHT hier (Abschnitt 3).

---

## 2. Burst-Handler 0x800338A8: Effekt-Vorbau bis 0x8003396B (Waffen 5/6, 3-Schuss-Burst)

Drei FUN_80019700-Spawns (a0=Effekt-Code, a1=Gun-Pitch DAT_800acabe (lh), a2=Gun-Bone-Matrix
[DAT_800acbdc]+0x7a4, a3=&Offset-Triple sp+0x10/{x,y,z}); danach (bekannt) @0x8003396c FUN_80011f50
+ 3x FUN_8004eae4 (0x80033974/7c/84).

### Roh-Listing 0x800338A8..0x8003396B
```
800338a8  addiu  sp,sp,-0x30
800338ac  lui    a0,0x202             ;\ Effekt-Code #1 = 0x02020800
800338b0  ori    a0,a0,0x800          ;/  (Bank 2, Id 2, Param 0x800)
800338b4  lui    a1,0x800b
800338b8  lh     a1,DAT_800acabe      ; a1 = Gun-Pitch (Aim-Winkel, s. Anm. unten)
800338bc  addiu  a3,sp,0x10           ; a3 = &offs
800338c0  sw     s0,0x20(sp)
800338c4  lui    s0,0x800b
800338c8  lw     s0,DAT_800acbdc      ; Posebuffer-Basis
800338cc  ori    v0,zero,0x8c         ; x = +0x8c (140)
800338d0  sw     v0,0x10(sp)
800338d4  ori    v0,zero,0x25d        ; y = +0x25d (605)
800338d8  sw     ra,0x28(sp)
800338dc  sw     s1,0x24(sp)
800338e0  sw     v0,0x14(sp)
800338e4  sw     zero,0x18(sp)        ; z = 0
800338e8  addiu  s0,s0,0x7a4          ; s0 = Gun-Bone-Matrix
800338ec  jal    FUN_80019700         ; Spawn #1 (Huelsenauswurf-Klasse, s. Anm.)
800338f0  _move  a2,s0
800338f4  lui    a0,0x302             ;\ Effekt-Code #2 = 0x03020c00
800338f8  ori    a0,a0,0xc00          ;/  (Bank 3, Id 2, Param 0xc00) = Muendungsblitz-Klasse
800338fc  move   a2,s0
80033900  addiu  a3,sp,0x10
80033904  ori    s1,zero,0x91         ; s1 = 0x91 (bleibt fuer #3 liegen)
80033908  lui    a1,0x800b
8003390c  lh     a1,DAT_800acabe
80033910  ori    v0,zero,0x1f4        ; y = +0x1f4 (500)
80033914  sw     v0,0x14(sp)
80033918  li     v0,-0x19             ; z = -0x19 (-25)
8003391c  sw     s1,0x10(sp)          ; x = +0x91 (145)
80033920  jal    FUN_80019700         ; Spawn #2
80033924  _sw    v0,0x18(sp)
80033928  lui    a0,0x402             ;\ Effekt-Code #3 = 0x04020800
8003392c  ori    a0,a0,0x800          ;/  (Bank 4, Id 2, Param 0x800) = Rauch-Klasse
80033930  move   a2,s0
80033934  addiu  a3,sp,0x10
80033938  lui    a1,0x800b
8003393c  lh     a1,DAT_800acabe
80033940  ori    v0,zero,0x109        ; y = +0x109 (265)
80033944  sw     v0,0x14(sp)
80033948  li     v0,-0x32             ; z = -0x32 (-50)
8003394c  sw     s1,0x10(sp)          ; x = +0x91 (145)
80033950  jal    FUN_80019700         ; Spawn #3
80033954  _sw    v0,0x18(sp)
80033958  lui    v0,0x800b            ; --- ab hier bekannter Resolve-Teil ---
8003395c  lw     v0,DAT_800ac784      ; Spieler-Entity
80033960  lui    a0,0x800b
80033964  lbu    a0,DAT_800aca5d      ; Waffen-Id
80033968  lw     a1,0x7c(v0)
8003396c  jal    FUN_80011f50         ; (bekannt) 1 Resolve + danach 3x FUN_8004eae4
```

### Pseudo-C
```c
void entlade_burst_5_6(void) {                     /* @0x800338a8, Tabelle 0x80074114/18 */
    MATRIX *gun = *(long**)&DAT_800acbdc + 0x7a4;  /* Gun-Bone-Matrix */
    short pitch = DAT_800acabe;
    long offs[3];
    offs[0]=0x8c;  offs[1]=0x25d; offs[2]=0;      EffectSpawn(0x02020800, pitch, gun, offs);
    offs[0]=0x91;  offs[1]=0x1f4; offs[2]=-0x19;  EffectSpawn(0x03020c00, pitch, gun, offs);
    offs[0]=0x91;  offs[1]=0x109; offs[2]=-0x32;  EffectSpawn(0x04020800, pitch, gun, offs);
    /* bekannt: FUN_80011f50(DAT_800aca5d, player->ctx7c); 3x FUN_8004eae4(); */
}
```

### Vergleich Browning-Handler 0x800337BC (Waffen 3/4) — beweist die Code-Feld-Bedeutung
Wortgleiche Struktur, gleiche drei Offset-Triple, NUR Byte1 der Effekt-Codes unterscheidet sich:

| Spawn | Browning 3/4 (@0x800337c0/0x80033808/0x8003383c) | Burst 5/6 (@0x800338ac/0x800338f4/0x80033928) |
|---|---|---|
| #1 | 0x0**200**0800 | 0x0**202**0800 |
| #2 | 0x0**300**0c00 | 0x0**302**0c00 |
| #3 | 0x0**400**0800 | 0x0**402**0800 |

FUN_80019700 zerlegt a0 so: **Bank = a0>>24** (@0x80019728 `srl t8,a0,0x18`),
**Effekt-Id = (a0>>16)&0xff** (@0x8001970c `srl v0,a0,0x10` + @0x80019730 `andi t7,v0,0xff`),
**Param = a0&0xffff** (@0x80019714 `andi s1,a0,0xffff`). Tabellenbasen je Bank:
DAT_800b22d4 (@0x80019750) und DAT_800b2248 (@0x8001976c). Slot-Pool @0x800a73b8
(0x60 Slots, Stride 0x84: @0x8001978c `sltiu v0,t3,0x60`, @0x80019794-9c t3*0x84).
=> Byte1 (0 vs 2) waehlt die Effekt-VARIANTE je Waffenklasse innerhalb derselben drei Baenke 2/3/4.

Anm. Gun-Pitch DAT_800acabe: vorzeichenbehaftetes Halbwort; wird beim Zielen pro Frame um den
Waffen-Schritt aus der 5-Byte-Tabelle @0x80074090 veraendert (Substate-0-Block @0x80033010-48:
`-= tab[(id-1)*5+0]` bei Pad&0x8, `+=` bei Pad&0x2; im Substate 2 halber Schritt `srl v0,1`
@0x800335a4/0x800335f0 aus tab[+1]). Es ist der Elevations-Winkel, den jeder Effekt-Spawn als a1 erhaelt.

---

## 3. Die Projektilspur: wo die Granate wirklich entsteht

### 3.1 Aufrufkette
Waffen 9/10/11 laufen ueber den Standard-Feuer-Handler LAB_80032e9c (Dispatch @0x80074030,
XREFs 0x80074054/58/5c). Dieser ist selbst eine Substate-Maschine ueber DAT_800aca5a
(Dispatch @0x80032ea0-0x80032ec4, Tabelle PTR @0x800740f4):

| Substate | Handler | Rolle |
|---|---|---|
| 0 | LAB_80032f18 (@0x800740f4) | Anlegen/Hochziehen |
| 1 | LAB_80033180 (@0x800740f8) | Ziel-Loop: Feuer-/Reload-Eingabe, Pitch |
| 2 | LAB_80033460 (@0x800740fc) | FEUERN (hier entsteht die Granate) |
| 3 | LAB_80033c74 (@0x80074100) | Feuer-Ende/Erholung |
| 4 | LAB_80033d7c (@0x80074104) | Nachladen |
| 5 | LAB_80033eec (@0x80074108) | (via DAT_800ac762&0x4 @0x800332e8-f8) |

**Tabellen-Doppelnutzung:** Die Substate-Tabelle @0x800740f4 laeuft nahtlos in die Entlade-Tabelle
@0x80074100 hinein — Substate 3/4/5 belegen dieselben Worte wie "Entlade[0..2]". Kollisionsfrei, weil
Waffen-Id 0..2 (Nahkampf, Dispatch 0x80034E70) den Entlade-jalr @0x800334e0 nie erreichen.

### 3.2 Substate 2 (LAB_80033460) im Detail
```
80033470  lbu  v0,DAT_800aca5b       ; Schuss-Latch
80033478  bne  v0,zero,LAB_80033508  ; schon gefeuert -> nur noch Pflege
  ; --- erster Frame des Substate 2: ---
80033480  sb   1,DAT_800aca5b        ; Latch setzen
8003349c  sb   zero,DAT_800acae9     ; Anim-Cursor = 0
800334a4  sb   7,DAT_800acae3        ; (Konstante 7 @0x80033494)
800334a8-bc                          ; Feuer-Clip aus Aim-Wort DAT_800acaec:
                                     ;   7 + 2*bit15(hoch) + 4*bit13(tief)
800334c8  sb   v0,DAT_800acae8       ; = Clip 7 (Mitte) / 9 (hoch) / 11 (tief)
800334d8  lw   v0,[0x80074100 + id*4]
800334e0  jalr v0                    ; ENTLADE-HANDLER (fuer 9/10/11: nur Munition-1)
800334f8  sb   0x5a,DAT_800acc24     ; 90 (@0x800334f0)
800334fc  ori  v0,v0,1               ; DAT_800aca52 |= 1 (@0x80033504)
LAB_80033508:                        ; --- jeder Frame: ---
8003350c-58                          ; NUR Waffe 8 (Schrot): Cursor 3/5/7 -> je FUN_80011f50
8003355c-0x80033600                  ; Pitch: Pad&0x8 -> -, Pad&0x2 -> + (halber Schritt tab[+1]>>1)
80033604  bne  (Pad&0x100),LAB_80033658  ; Taste 0x100 GEHALTEN -> Feuerphase laeuft weiter
8003360c-3c                          ; sonst: Cursor > tab[(id-1)*5+2] (@0x80033634, GL: 0x0a)?
8003364c  sh   3,DAT_800aca5a        ;   ja -> Substate 3 (Feuer-Ende) + return (@0x80033650)
LAB_80033658:
80033668  jal  FUN_8001f314(DAT_800acbc4,DAT_800acbc8,0,0x200)  ; Waffenbank-Anim +1 Tick
                                     ; (EMR/EDD der Waffenbank; Katalog: Keyframe-Advance)
80033670  beq  v0,zero,+2            ; Clip fertig ->
8003367c  sh   1,DAT_800aca5a        ;   Substate 1 (zurueck in den Ziel-Loop)
LAB_80033680:                        ; --- GRANATWERFER-PROJEKTIL ---
80033684  lbu  v1,DAT_800aca5d
80033688  ori  v0,zero,0x9
8003368c  bne  v1,v0,return          ; NUR WAFFE 9!
80033690  _ori v0,zero,0x13          ; Frame-Konstante 19
80033698  lbu  v1,DAT_800acae9       ; Anim-Cursor
800336a0  bne  v1,v0,LAB_80033700    ; Cursor==19?
800336ac  lhu  v0,DAT_800acaec
800336b4  andi v0,v0,0x8000          ;   und Aim HOCH (Bit15)?
800336bc  _lui a0,0x40d
800336c0  ori  a0,a0,0x1000          ;   -> Effekt 0x040D1000 = GRANATE
800336cc  lh   a1,DAT_800acabe       ;   a1 = Gun-Pitch
800336d4  lw   a2,DAT_800acbdc
800336d8  ori  v0,zero,0x12c         ;   y = +0x12c (300)  (@0x800336dc sw 0x14(sp))
800336e0  ori  v0,zero,0x320         ;   z = +0x320 (800)  (@0x800336e8 sw 0x18(sp))
800336e4  sw   zero,0x10(sp)         ;   x = 0
800336ec  jal  FUN_80019700
800336f0  _addiu a2,a2,0x7a4         ;   a2 = Gun-Bone-Matrix
LAB_80033700:                        ; Cursor==0x16 (22, @0x800336a4/0x800336fc)?
80033714  andi v0,v0,0x4000          ;   und Aim MITTE (Bit14)?
8003371c  _lui a0,0x40d              ;   -> Effekt 0x040D1000
80033720  ori  a0,a0,0x1000
80033738  ori  v0,zero,0x1f4         ;   Offsets x=0,y=0,z=+0x1f4 (500) @0x8003373c-44
80033748  jal  FUN_80019700
LAB_80033750:                        ; Cursor==0x18 (24, @0x80033758)?
80033770  andi v0,v0,0x2000          ;   und Aim TIEF (Bit13)?
80033778  _lui a0,0x40d              ;   -> Effekt 0x040D1000
8003377c  ori  a0,a0,0x1000
80033794  ori  v0,zero,0x12c         ;   Offsets x=0,y=0,z=+0x12c (300) @0x80033798-a0
800337a4  jal  FUN_80019700
800337ac  lw ra / jr ra              ; Ende Substate 2 (@0x800337ac-b8)
```

### Pseudo-C (Substate 2, granatenrelevanter Teil)
```c
void std_fire_substate2(void) {                       /* LAB_80033460 */
    if (!DAT_800aca5b) {                              /* erster Frame */
        DAT_800aca5b = 1; DAT_800acae9 = 0; DAT_800acae3 = 7;
        DAT_800acae8 = 7 + 2*aim_hi + 4*aim_lo;       /* Feuer-Clip 7/9/11 */
        unload_table[DAT_800aca5d]();                 /* 9/10/11: nur Ammo-1 */
        DAT_800acc24 = 0x5a; DAT_800aca52 |= 1;
    }
    /* ... Waffe-8-Mehrfachresolve, Pitch ... */
    if (!(pad_held & 0x100)) {                        /* Taste losgelassen */
        if (DAT_800acae9 > wtab[(id-1)*5+2]) {        /* GL: > 10 */
            DAT_800aca5a = 3; return;                 /* Feuer-Ende */
        }
    }
    if (FUN_8001f314(EMR, EDD, 0, 0x200)) DAT_800aca5a = 1;   /* Clip fertig */
    if (DAT_800aca5d == 9) {                          /* NUR Waffe 9 */
        if (DAT_800acae9 == 19 && (aim & 0x8000)) spawn(0x040d1000, {0,300,800});
        if (DAT_800acae9 == 22 && (aim & 0x4000)) spawn(0x040d1000, {0,0,500});
        if (DAT_800acae9 == 24 && (aim & 0x2000)) spawn(0x040d1000, {0,0,300});
    }
}
```

**Mechanik:** Die Granate ist Effekt-Entity **0x040D1000** (Bank 4, Effekt 0x0d, Param 0x1000) aus dem
FUN_80019700-Pool @0x800a73b8. Sie verlaesst den Lauf NICHT beim Entladen (Frame 0), sondern an dem
Anim-Frame, an dem der Rueckstoss-Clip die Muendung freigibt — je Elevation frueher/spaeter und mit
anderem Muendungs-Offset: hoch=Frame 19/(0,300,800), Mitte=Frame 22/(0,0,500), tief=Frame 24/(0,0,300).
Voraussetzung: Substate 2 lebt bei Cursor >10 nur weiter, solange Pad-Bit 0x100 gehalten ist (@0x80033604).
Da genau eines der Aim-Bits 15/14/13 gesetzt ist, feuert genau EIN Spawn pro Schuss.
(Nebenbefund: Bit14 von DAT_800acaec = Aim MITTE — ergaenzt die bekannte Bit15/13-Belegung.)

### 3.3 Waffen 10/11 und 15..18 — im Auslieferungsstand OHNE Projektil
- **10/11:** Gate @0x8003368c prueft ausschliesslich `id == 9`. Es gibt in der ganzen EXE nur die drei
  `lui a0,0x40d` @0x800336bc/0x8003371c/0x80033778 (grep ueber ghidra1_V2.txt). Alle 9 Callsites von
  FUN_80011f50 (XREF-Liste @ghidra1:75820: 80012418/80033554/80033880/8003396c/80033a34/80033b10/
  80033c50/800349ec/800353cc) sind fuer andere Waffen-Ids gated. => Waffen 10/11 ziehen Munition ab
  (Stubs) und bewirken sonst NICHTS — kein Projektil, kein Schaden. Unfertiger Prototyp-Stand.
- **15..18:** Entlade-Slots @0x8007413c-0x8007414b sind NULL (PSX.EXE-Dump, Datei 0x6493c-0x6494b).
  Der Standard-Handler wuerde @0x800334e0 `jalr 0` ausfuehren -> Absturz. Es existiert KEINE
  Werfer-Logik fuer diese Ids in der EXE; die gesuchte "andere Stelle" fuer Werfer-Projektile
  existiert im Auslieferungsstand nicht.
- Reload ist fuer alle Ids >= 9 gesperrt: @0x80033368 `sltiu v0,id,9`, @0x8003336c verzweigt bei
  id>=9 an FUN_80045024(0x1010001, DAT_800aca88) (SE, @0x80033358/0x80033390) statt Substate 4 zu
  setzen (@0x80033378 nur id<9).

### 3.4 RE2-Beleg (ausdruecklich beauftragter Fallback)
`ghidra_re2_Leon.txt` @0x800218c8: `lui a0,0x40d` / @0x800218cc `ori a0,a0,0x2800` -> @0x800218e4
`jal FUN_8001cbe8` mit a1 = lh 0x22(entity) (Winkel), a2 = &Offset-Triple (DAT_8009db44),
a3 = entity+0x34 — dieselbe Spawner-Signatur wie RE1.5 FUN_80019700 und derselbe Code-Aufbau
Bank 4/Effekt 0x0d (Nachbar-Cases @0x80021888 0x03142000, @0x80021920 0x030f2000). RE2 fuehrt
die Bank-4/0x0d-Effektklasse also identisch; die RE1.5-Struktur ist kein Einzelfall.

---

## 4. Konstanten-Tabellen

### Stubs 9/10/11
| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x80033b40 / 0x80033b60 / 0x80033b80 | jal 0x8004eae4 | Munition -1, einzige Wirkung |
| 0x80074124 / 0x80074128 / 0x8007412c | 0x80033b38/58/78 | Tabellenslots (einzige XREFs) |

### Burst-Vorbau 0x800338A8
| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x800338ac/b0 | 0x02020800 | Effekt #1 (Bank2/Id2), Offs (0x8c,0x25d,0) @0x800338cc-e4 |
| 0x800338f4/f8 | 0x03020c00 | Effekt #2 (Bank3/Id2), Offs (0x91,0x1f4,-0x19) @0x80033904-24 |
| 0x80033928/2c | 0x04020800 | Effekt #3 (Bank4/Id2), Offs (0x91,0x109,-0x32) @0x80033940-54 |
| 0x800338b8 u.a. | lh DAT_800acabe | a1 = Gun-Pitch fuer jeden Spawn |
| 0x800338c8+0x800338e8 | [DAT_800acbdc]+0x7a4 | Gun-Bone-Matrix (a2) |

### Granatwerfer (Waffe 9), Substate 2
| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x8003368c | id==9 | einziges Waffen-Gate des Projektil-Spawns |
| 0x80033690 | 0x13 (19) | Spawn-Frame Aim HOCH |
| 0x800336a4/0x800336fc | 0x16 (22) | Spawn-Frame Aim MITTE |
| 0x80033758 | 0x18 (24) | Spawn-Frame Aim TIEF |
| 0x800336b4 / 0x80033714 / 0x80033770 | 0x8000/0x4000/0x2000 | Aim-Bits hoch/Mitte/tief (DAT_800acaec) |
| 0x800336bc-c0 (3x) | 0x040d1000 | Granaten-Projektil-Effekt (Bank4/Id 0x0d/Param 0x1000) |
| 0x800336d8-e8 | (0,0x12c,0x320) | Muendungs-Offset hoch (x,y,z) |
| 0x80033738-44 | (0,0,0x1f4) | Muendungs-Offset Mitte |
| 0x80033794-a0 | (0,0,0x12c) | Muendungs-Offset tief |
| 0x80033634 | tab[(id-1)*5+2] | Feuer-Ende-Frame (GL-Zeile: 0x0a) |
| 0x80033604 | Pad&0x100 | haelt Substate 2 ueber Cursor 10 hinaus offen |

### 5-Byte-Waffentabelle @0x80074090 (PSX.EXE Datei-Offset 0x64890, Zeile = (id-1)*5)
```
id  1: 18 30 00 01 01      id  8: 18 30 0a 00 01      id 13: 18 30 0a 01 01
id  3: 18 30 07 01 01      id  9: 18 30 0a 01 00      id 19: 18 30 0a 01 01
id  4: 18 30 07 01 01      id 10: 18 30 0a 01 00      id 2/14/15/16/17/18/20: 00 ..
id 5/6/7: 18 30 0a 01 01   id 11: 18 30 0a 01 00
Byte0 @+0 = Pitch-Schritt (Ziel-Loop), Byte1 @+1 = Pitch-Schritt-Basis Substate 2 (>>1),
Byte2 @+2 = Feuer-Ende-Frame; Byte3(+3)/Byte4(+4): u.a. @0x80032edc-efc gelesen
(Anim-Bank-Selektor via DAT_800aca5c&0x4), nicht Teil dieser Gruppe.
```

## 5. Offen / mit RE1.5+RE2 nicht abschliessbar
- Was Effekt 0x040d1000 (Bank 4, Id 0x0d) im Effekt-System TUT (Flugbahn/Bogenwurf, Aufschlag,
  Explosionsschaden): liegt in den Bank-4-Effektdaten/Routinen hinter DAT_800b22d4/DAT_800b2248
  (zur Laufzeit vom Overlay gepatcht, On-Disc-Slots teils null lt. FUN_CATALOG) — separater RE-Gang
  (ESP-Row-VM). FUN_80011f50 ist dafuer nachweislich NICHT der Traeger (Callsite-Liste vollstaendig).
- Ob Waffen 10/11 als eigene Granatarten GEPLANT waren (z.B. Flamm-/Saeuregranate) oder das Projektil
  nur fuer Id 9 fertig wurde, ist statisch nicht entscheidbar — sicher ist nur: im Auslieferungsstand
  spawnen sie nichts und schaden nicht.
- Bedeutung des Param-Halbworts (0x800/0xc00/0x1000) in den Effekt-Codes: FUN_80019700 haelt es in s1
  (@0x80019714); die Verwendung tiefer in der Funktion wurde nicht zu Ende verfolgt.
- FUN_8004ea6c (Magazin-Pruefung, !=0 -> Substate 2 @0x80033324) und FUN_8004eb70 (Reserve-Pruefung
  vor Reload @0x8003334c) sind aus der Verwendung benannt, nicht selbst disassembliert.
- Katalog-Spannung FUN_80019700: RE15_FUN_CATALOG nennt es "Enemy hitbox-data setup"; dieselbe
  Funktion ist der generische typ-indizierte Pool-Spawner (Pool @0x800a73b8, 0x60 Slots, Stride 0x84)
  — beide Sichten nutzen dieselben Tabellen DAT_800b2248/DAT_800b22d4; die gespawnten Entities tragen
  Hitbox-Masse (+0x78/+0x7c @0x80019894/0x800198b0), was fuer ein aufschlagendes Granaten-Projektil
  stimmig ist. Nicht zu Ende geklaert.
