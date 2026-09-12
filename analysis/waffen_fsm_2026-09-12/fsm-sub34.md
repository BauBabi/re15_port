# Dauerfeuer-FSM Sub 3 (0x80034C74) und Sub 4 (0x80034D68) — vollständiges RE

Datum 2026-09-12. Quelle: `ghidra1_V2.txt` (RE1.5 PSX.EXE, Auslieferungsstand `info/Re1.5/PSX.EXE`),
Tabellen per Python direkt aus der EXE gelesen (t_addr=0x80010000 @EXE-Datei-Offset 0x18,
Dateioffset = 0x800 + (va − 0x80010000)).

Kontext: Waffen-Feuer-Dispatch `LAB_80032e44` indexiert Tabelle @`0x80074030` mit der
Waffen-Id `DAT_800aca5d` (@0x80032e60). Ids 0x0C/0x0E/0x13 (Ingram M10 / Flammenwerfer / MC51)
→ `0x80034014` (Dauerfeuer-FSM-Dispatcher). Dieser liest den Substate `DAT_800aca5a`
(@0x80034018) und springt über Tabelle @`0x80074150` (@0x8003402c). Meine Gruppe:

| Index | Tabellen-Slot | Handler | Rolle (Ergebnis dieses RE) |
|---|---|---|---|
| 3 | 0x8007415c | LAB_80034c74 | **Feuer-Ende / Waffe senken** (Clip 6 rückwärts, dann FSM-Exit) |
| 4 | 0x80074160 | LAB_80034d68 | **Nachladen** (Clip 15 vorwärts, Magazin auffüllen + SE) — im Auslieferungsstand UNERREICHBAR (s. §5) |

`0x80034c74` und `0x80034d68` kommen als Pointer **nur** an 0x8007415c/0x80074160 in der EXE vor
(bytegenaue Suche über die ganze EXE; `0x80034278` analog nur @0x80074154). Die Standard-FSM
(`0x80032e9c`, Tabelle @0x800740f4: 0x80032f18/0x80033180/0x80033460, ab Index 3 läuft sie in die
Entlade-Tabelle @0x80074100 hinein) teilt diese Subs also NICHT.

## 0. Schlüssel-Erkenntnis: die „Globals" sind Spieler-Entity-Felder

@0x8001d098/0x8001d0a0: `addiu a0,s0,0x2d0` / `sw a0,0x0(s0)=>DAT_800ac784` mit s0=0x800ac784
→ **`DAT_800ac784` (Spieler-Entity-Zeiger) wird statisch auf `0x800aca54` gesetzt.**
Damit gilt für alle hier benutzten Adressen:

| Adresse | Entity-Feld | Bedeutung | Beleg |
|---|---|---|---|
| DAT_800aca54 | +0x00 | Entity-Flags (Wort). Bit 0x40000000 = „zielt" (gesetzt @0x80032e50/0x80032e64 beim Feuer-Dispatch; auch = Aim-Wort<<16, @0x80011fcc-fe8: `flags = (flags & 0x1fffffff) \| (DAT_800acaec << 16)`). Bit 0x4000 = Messer-FSM-Latch (gesetzt @0x8003575c in Melee-Sub 0x80035538, getestet @0x80034e9c) | s. Zeilen |
| DAT_800aca59 | +0x05 | Spieler-Routine-Index; Dispatch @0x8003068c über Tabelle @0x80073e30 (12 Einträge, [0]=0x80050cb8 Normal, [4]=0x80030af0 …) | 0x8003068c-0x800306ac |
| DAT_800aca5a | +0x06 | Waffen-FSM-Substate (Byte) | 0x80034018 |
| DAT_800aca5b | +0x07 | Init-Latch des aktuellen Substates (0=Erst-Eintritt) | 0x80034c84 |
| DAT_800aca5d | +0x09 | angelegte Waffen-Id (Dispatch-Index 0..20) | 0x80032e60 |
| DAT_800aca88 | +0x34 | Spieler-Weltposition x,y,z (3×int32; +0x34/+0x38/+0x3c). Wird als Positions-Zeiger an den SE-Spawn übergeben; FUN_80045a64 rechnet daraus Distanz zur Kamera (Kamera-Felder +4/+8/+0xc @Kamerablock `*(DAT_800ac778+0x24)+cam*0x20`) für 3D-Panning | 0x800123d0, FUN_80045a64-Disasm |
| DAT_800acabe | +0x6a | Yaw-Winkel des Waffenlaufs/Spielers (0..4095=360°). Konsument: Hit-Scan-Streu-Vektoren werden damit rotiert (`lh a0,DAT_800acabe` → FUN_8004f008(a0=Winkel, in, out), 5× @0x800125bc-0x80012658); Init aus Spawn-Record-Feld +6 @0x8001d8e8 | s. Zeilen |
| DAT_800acae3 | +0x8f | Crossfade-Countdown in Frames (FUN_8001f3bc liest `entity[0x8f]`, blendet per GTE `gte_ldIR0(0x1000 − schritt×countdown)` und dekrementiert) | FUN_8001f3bc |
| DAT_800acae8 | +0x94 | aktueller Anim-Clip-Index (Waffenbank) | FUN_8001f314: `*(byte*)(DAT_800ac784+0x94)` |
| DAT_800acae9 | +0x95 | In-Clip-Frame-Zähler | FUN_8001f3bc-Ende: `entity[0x95]++`, Reset bei Clip-Ende |
| DAT_800acaec | +0x98 | Aim-Wort: Bit15=hoch, Bit14=mitte, Bit13=tief (Sub 1 setzt 0x8000 bei D-Pad-Hoch @0x80034338, 0x2000 bei Runter @0x80034380, 0x4000 sonst @0x800343cc) | s. Zeilen |

Nicht-Entity-Globals:

| Adresse | Bedeutung | Beleg |
|---|---|---|
| DAT_800ac768 | **gehaltene logische Tasten** (Bit i = logischer Slot i). Aufgebaut @0x80030464-0x800304e4: PadRead → roh @DAT_800ac758; Keymap `PTR_DAT_80073e1c[DAT_800b0fcc]` (Typ A/B/C/Custom = 0x80073dbc/ddc/dfc/0x800b21cc), 16 Slots: Bit i gesetzt, wenn `roh & keymap[i]` | s. Zeilen |
| DAT_800ac76c | **frisch gedrückte** logische Tasten (Edge): `(DAT_800ac770 ^ DAT_800ac768) & DAT_800ac768` @0x80030550-0x8003057c; DAT_800ac770 = Vorframe-Kopie @0x800304ac | s. Zeilen |
| DAT_800aca3c | globales Spieler-Statuswort. Bits 0x40\|0x80 (=0xC0) beim Feuerbeginn gesetzt (@0x80034108 in Sub 0; @0x80034f90 in Melee-Sub 0x80034ee8), von Sub 3 beim FSM-Exit gelöscht. Bit 0x4000 = Ziel-Lock (gesetzt @0x80031d24 zusammen mit Ziel-Zeiger DAT_800ac788 @0x80031d20) | s. Zeilen |
| DAT_800acbc4 | EMR-Zeiger (Skelett-/Posen-Daten) der geladenen Waffen-Anim-Bank: `PLW-Basis 0x801d7700` + dir[1] (@0x80036bf0-0x80036c04) | s. Zeilen |
| DAT_800acbc8 | EDD-Zeiger (Clip-Verzeichnis) derselben Bank: PLW-Basis + dir[0] (@0x80036bc8-0x80036be4) | s. Zeilen |

Keymap-Default (Typ A @0x80073dbc, 16×u16, PadRead-libetc-Bits: 0x1000=D-Hoch 0x2000=D-Rechts
0x4000=D-Runter 0x8000=D-Links 0x0010=Dreieck 0x0020=Kreis 0x0040=Kreuz 0x0080=Viereck
0x0008=R1 0x0004=L1 0x0002=R2 0x0001=L2, per `Psy-Q_47/INCLUDE/LIBETC.H`):

```
Slot: 0=D-Hoch 1=D-Rechts 2=D-Runter 3=D-Links 4=D-Hoch 5=D-Runter
      6=Viereck(Aktion/Feuer) 7=Viereck 8=R1(Zielen) 9=Kreuz 10=R1 11=L1
      12=D-Links 13=D-Rechts 14=Viereck 15=Kreuz
```
In der FSM benutzt: Bit 0x8 = Slot 3 = **D-Pad LINKS (drehen)**, Bit 0x2 = Slot 1 =
**D-Pad RECHTS**, Bit 0x100 = Slot 8 = **Zielen (R1)**, Bit 0x40 = Slot 6 = **Feuer (Viereck)**,
Bit 0x10/0x20 = Slot 4/5 = Aim hoch/runter.

---

## 1. Sub 3 @0x80034C74–0x80034D64 — „Feuer-Ende / Waffe senken"

### 1.1 Roh-Listing mit Kommentar

```
LAB_80034c74                              XREF: 0x8007415c (Tabelle @0x80074150, Index 3)
80034c74  addiu  sp,sp,-0x18
80034c78  lui    v1,0x800b
80034c7c  addiu  v1,v1,-0x35a5            ; v1 = &DAT_800aca5b (entity+0x07, Init-Latch)
80034c80  sw     ra,0x10(sp)
80034c84  lbu    v0,0x0(v1)               ; Init-Latch lesen
80034c88  nop
80034c8c  bne    v0,zero,LAB_80034cb8     ; schon initialisiert -> Tick-Teil
80034c90  _ori   v0,zero,0x1
; ---- Erst-Eintritt ----
80034c94  sb     v0,0x0(v1)               ; Init-Latch := 1
80034c98  ori    v0,zero,0x6              ; *** Clip-Nr. 6 ***
80034c9c  lui    at,0x800b
80034ca0  sb     v0,-0x3518(at)           ; DAT_800acae8 (entity+0x94) := 6
80034ca4  ori    v0,zero,0x7              ; *** Crossfade 7 Frames ***
80034ca8  lui    at,0x800b
80034cac  sb     zero,-0x3517(at)         ; DAT_800acae9 (entity+0x95) := 0 (Frame-Zaehler)
80034cb0  lui    at,0x800b
80034cb4  sb     v0,-0x351d(at)           ; DAT_800acae3 (entity+0x8f) := 7
; ---- jeder Frame ----
LAB_80034cb8
80034cb8  lui    v1,0x800b
80034cbc  lw     v1,-0x3898(v1)           ; v1 = DAT_800ac768 (gehaltene log. Tasten)
80034cc0  nop
80034cc4  andi   v0,v1,0x8                ; Slot 3 = D-Pad LINKS?
80034cc8  beq    v0,zero,LAB_80034cec
80034ccc  _andi  v0,v1,0x2                ; (Delay) v0 = Slot 1 = D-Pad RECHTS
80034cd0  lui    v0,0x800b
80034cd4  lhu    v0,-0x3542(v0)           ; DAT_800acabe (entity+0x6a, Yaw)
80034cd8  nop
80034cdc  addiu  v0,v0,-0x18              ; *** Yaw -= 0x18 (24/4096 = 2,11 Grad/Frame) ***
80034ce0  lui    at,0x800b
80034ce4  sh     v0,-0x3542(at)           ; zurueckschreiben
80034ce8  andi   v0,v1,0x2                ; RECHTS neu laden (v0 wurde ueberschrieben)
LAB_80034cec
80034cec  beq    v0,zero,LAB_80034d0c
80034cf0  _ori   a2,zero,0x1              ; (Delay, laeuft IMMER) a2 := 1 = RUECKWAERTS
80034cf4  lui    v0,0x800b
80034cf8  lhu    v0,-0x3542(v0)           ; Yaw
80034cfc  nop
80034d00  addiu  v0,v0,0x18               ; *** Yaw += 0x18 ***
80034d04  lui    at,0x800b
80034d08  sh     v0,-0x3542(at)
LAB_80034d0c
80034d0c  lui    a0,0x800b
80034d10  lw     a0,-0x343c(a0)           ; a0 = DAT_800acbc4 (EMR der Waffenbank)
80034d14  lui    a1,0x800b
80034d18  lw     a1,-0x3438(a1)           ; a1 = DAT_800acbc8 (EDD der Waffenbank)
80034d1c  jal    FUN_8001f314             ; Anim-Frame-Stepper; a2=1 -> Clip RUECKWAERTS
80034d20  _ori   a3,zero,0x200            ; *** Crossfade-Schrittweite 0x200 (=0x1000/8) ***
80034d24  beq    v0,zero,LAB_80034d58     ; 0 = Clip laeuft noch -> return
80034d28  _li    v1,-0x81                 ; (Delay) v1 = ~0x80
; ---- Clip zu Ende: FSM-Exit ----
80034d2c  lui    v0,0x800b
80034d30  lw     v0,-0x35c4(v0)           ; DAT_800aca3c (Status-Wort)
80034d34  lui    at,0x800b
80034d38  sb     zero,-0x35a7(at)         ; *** DAT_800aca59 (entity+0x05) := 0 -> Spieler-Routine 0 (Normal, 0x80050cb8 via Tabelle @0x80073e30) ***
80034d3c  lui    at,0x800b
80034d40  sh     zero,-0x35a6(at)         ; *** DAT_800aca5a UND DAT_800aca5b := 0 (EIN Halbwort-Store loescht Substate+Init) ***
80034d44  and    v0,v0,v1                 ; Status &= ~0x80
80034d48  li     v1,-0x41                 ; v1 = ~0x40
80034d4c  and    v0,v0,v1                 ; Status &= ~0x40
80034d50  lui    at,0x800b
80034d54  sw     v0,-0x35c4(at)           ; *** DAT_800aca3c: Bits 0x80|0x40 geloescht (Gegenstueck zu ori 0xC0 @0x80034108) ***
LAB_80034d58
80034d58  lw     ra,0x10(sp)
80034d5c  addiu  sp,sp,0x18
80034d60  jr     ra
80034d64  _nop
```

### 1.2 Pseudo-C

```c
void autofire_sub3_waffe_senken(void)   /* @0x80034c74 */
{
    if (!entity->init_latch /*+0x07*/) {            /* @0x80034c84 */
        entity->init_latch = 1;                     /* @0x80034c94 */
        entity->clip      = 6;    /* @0x80034c98: Anhebe-Clip 6, wird gleich rueckwaerts gespielt */
        entity->frame     = 0;    /* @0x80034cac */
        entity->crossfade = 7;    /* @0x80034ca4/0x80034cb4: 7-Frame-Blend */
    }
    if (held & 0x008) entity->yaw -= 0x18;          /* D-Links,  @0x80034cdc */
    if (held & 0x002) entity->yaw += 0x18;          /* D-Rechts, @0x80034d00 */

    /* Clip 6 RUECKWAERTS abspielen (a2=1 @0x80034cf0-Delay-Slot, laeuft immer) */
    if (FUN_8001f314(EMR /*DAT_800acbc4*/, EDD /*DAT_800acbc8*/, 1, 0x200)) {
        /* Clip fertig -> kompletter FSM-Exit */
        entity->routine   = 0;                      /* @0x80034d38: zurueck zu Routine 0 */
        *(u16*)&entity->substate = 0;               /* @0x80034d40: Substate+Init in einem Store */
        DAT_800aca3c &= ~(0x80|0x40);               /* @0x80034d44-0x80034d54 */
    }
}
```

### 1.3 Konstanten

| Wert | Adresse | Bedeutung |
|---|---|---|
| Clip 6 | 0x80034c98 | Absenk-Animation = Bank-Clip 6, rückwärts |
| Crossfade 7 | 0x80034ca4 / 0x80034cb4 | Blend-Frames beim Clip-Wechsel |
| ±0x18 | 0x80034cdc / 0x80034d00 | Dreh-Rate im Zielen: 24/4096·360° = 2,109°/Frame |
| a2=1 | 0x80034cf0 | FUN_8001f314-Richtung: 1 = rückwärts (`frame = n−1−zaehler`, s. FUN_8001f314 @0x8001f314: `uVar2 = (count − entity[0x95]) − 1`) |
| a3=0x200 | 0x80034d20 | Crossfade-Schrittweite (0x1000/8; FUN_8001f3bc: `gte_ldIR0(0x1000 − 0x200·countdown)`) |
| ~0x80, ~0x40 | 0x80034d28 / 0x80034d48 | gelöschte Statusbits in DAT_800aca3c |

**Eintritt in Sub 3:** einzig @0x800342f8-0x8003430c (Sub 1 = 0x80034278): sobald
`!(DAT_800ac768 & 0x100)` — **Zielen-Taste (Slot 8 = R1) losgelassen** — wird Substate := 3
geschrieben (sh @0x8003430c, löscht dabei auch das Init-Latch).

---

## 2. Sub 4 @0x80034D68–0x80034E6C — „Nachladen" (im Auslieferungsstand toter Zustand)

### 2.1 Roh-Listing mit Kommentar

```
LAB_80034d68                              XREF: 0x80074160 (Tabelle @0x80074150, Index 4)
80034d68  addiu  sp,sp,-0x18
80034d6c  sw     s0,0x10(sp)
80034d70  lui    s0,0x800b
80034d74  addiu  s0,s0,-0x35a5            ; s0 = &DAT_800aca5b (Init-Latch)
80034d78  sw     ra,0x14(sp)
80034d7c  lbu    v0,0x0(s0)
80034d80  nop
80034d84  bne    v0,zero,LAB_80034dc8
80034d88  _ori   v0,zero,0x1
; ---- Erst-Eintritt ----
80034d8c  sb     v0,0x0(s0)               ; Init-Latch := 1
80034d90  ori    v0,zero,0xf              ; *** Clip-Nr. 15 (Nachlade-Anim) ***
80034d94  lui    at,0x800b
80034d98  sb     v0,-0x3518(at)           ; DAT_800acae8 := 15
80034d9c  lui    v0,0x800b
80034da0  lhu    v0,-0x3514(v0)           ; DAT_800acaec (Aim-Wort)
80034da4  ori    v1,zero,0x7              ; *** Crossfade 7 ***
80034da8  lui    at,0x800b
80034dac  sb     zero,-0x3517(at)         ; DAT_800acae9 := 0
80034db0  lui    at,0x800b
80034db4  sb     v1,-0x351d(at)           ; DAT_800acae3 := 7
80034db8  andi   v0,v0,0x1fff             ; Aim-Bits 15/14/13 loeschen
80034dbc  ori    v0,v0,0x4000             ; Bit 14 = MITTE setzen
80034dc0  lui    at,0x800b
80034dc4  sh     v0,-0x3514(at)           ; *** Aim-Wort auf "geradeaus" gezwungen ***
; ---- jeder Frame ----
LAB_80034dc8
80034dc8  lui    v1,0x800b
80034dcc  lw     v1,-0x3898(v1)           ; DAT_800ac768
80034dd0  nop
80034dd4  andi   v0,v1,0x8                ; D-Pad LINKS
80034dd8  beq    v0,zero,LAB_80034dfc
80034ddc  _andi  v0,v1,0x2
80034de0  lui    v0,0x800b
80034de4  lhu    v0,-0x3542(v0)           ; Yaw
80034de8  nop
80034dec  addiu  v0,v0,-0x18              ; Yaw -= 0x18
80034df0  lui    at,0x800b
80034df4  sh     v0,-0x3542(at)
80034df8  andi   v0,v1,0x2
LAB_80034dfc
80034dfc  beq    v0,zero,LAB_80034e1c
80034e00  _clear a2                       ; (Delay, laeuft IMMER) a2 := 0 = VORWAERTS
80034e04  lui    v0,0x800b
80034e08  lhu    v0,-0x3542(v0)
80034e0c  nop
80034e10  addiu  v0,v0,0x18               ; Yaw += 0x18
80034e14  lui    at,0x800b
80034e18  sh     v0,-0x3542(at)
LAB_80034e1c
80034e1c  lui    a0,0x800b
80034e20  lw     a0,-0x343c(a0)           ; EMR
80034e24  lui    a1,0x800b
80034e28  lw     a1,-0x3438(a1)           ; EDD
80034e2c  jal    FUN_8001f314             ; Clip 15 VORWAERTS steppen
80034e30  _ori   a3,zero,0x200            ; Crossfade-Schrittweite 0x200
80034e34  beq    v0,zero,LAB_80034e5c     ; laeuft noch -> return
80034e38  _ori   v0,zero,0x1
; ---- Clip zu Ende: Magazin fuellen ----
80034e3c  lui    at,0x800b
80034e40  sh     v0,-0x35a6(at)           ; *** DAT_800aca5a := 1 (Halbwort: Substate=1, Init-Latch=0) -> zurueck in Feuer-Bereitschaft (0x80034278) ***
80034e44  jal    FUN_8004ebdc             ; *** Magazin aus Vorrat auffuellen (s. §4.3) ***
80034e48  _nop
80034e4c  lui    a0,0x103
80034e50  ori    a0,a0,0x1                ; *** SE-Code 0x01030001: Bank 1, SE 3, low=1 -> positional ***
80034e54  jal    FUN_80045024             ; SE abspielen (SsUtKeyOnV; low-Byte!=0 -> FUN_80045a64(pos) 3D-Panning)
80034e58  _addiu a1,s0,0x2d               ; a1 = 0x800aca5b+0x2d = &DAT_800aca88 = Spieler-Position (entity+0x34)
LAB_80034e5c
80034e5c  lw     ra,0x14(sp)
80034e60  lw     s0,0x10(sp)
80034e64  addiu  sp,sp,0x18
80034e68  jr     ra
80034e6c  _nop
```

### 2.2 Pseudo-C

```c
void autofire_sub4_nachladen(void)      /* @0x80034d68 */
{
    if (!entity->init_latch) {
        entity->init_latch = 1;                     /* @0x80034d8c */
        entity->clip      = 15;                     /* @0x80034d90 */
        entity->frame     = 0;                      /* @0x80034dac */
        entity->crossfade = 7;                      /* @0x80034da4/0x80034db4 */
        entity->aim_word  = (entity->aim_word & 0x1fff) | 0x4000;
                       /* @0x80034db8-0x80034dc4: hoch/tief raus, Mitte rein */
    }
    if (held & 0x008) entity->yaw -= 0x18;          /* @0x80034dec */
    if (held & 0x002) entity->yaw += 0x18;          /* @0x80034e10 */

    /* Clip 15 VORWAERTS (a2=0 @0x80034e00-Delay-Slot) */
    if (FUN_8001f314(EMR, EDD, 0, 0x200)) {
        *(u16*)&entity->substate = 1;               /* @0x80034e3c-40: -> Sub 1, Init frisch */
        FUN_8004ebdc();                             /* @0x80034e44: Magazin += min(Kapazitaet, Vorrat) */
        FUN_80045024(0x01030001, &entity->pos /*0x800aca88*/);  /* @0x80034e4c-58: Nachlade-SE, 3D */
    }
}
```

### 2.3 Konstanten

| Wert | Adresse | Bedeutung |
|---|---|---|
| Clip 15 (0xf) | 0x80034d90 | Nachlade-Animation = Bank-Clip 15, vorwärts |
| Crossfade 7 | 0x80034da4 / 0x80034db4 | Blend-Frames |
| &0x1fff \| 0x4000 | 0x80034db8 / 0x80034dbc | Aim-Wort auf „Mitte" zwingen (Bit15 hoch, Bit14 mitte, Bit13 tief) |
| ±0x18 | 0x80034dec / 0x80034e10 | Dreh-Rate wie Sub 3 |
| a2=0 | 0x80034e00 | Clip vorwärts |
| a3=0x200 | 0x80034e30 | Crossfade-Schrittweite |
| Substate:=1 | 0x80034e3c-0x80034e40 | Folgezustand = Feuer-Bereitschaft 0x80034278 |
| 0x01030001 | 0x80034e4c/0x80034e50 | Nachlade-SE: Bank 1 (SE-Tabelle @0x801fcd00, FUN_80045024 case 1), Index 3, positional an Spieler-Position |

---

## 3. Bonus im Adressbereich: Nahkampf-Dispatcher LAB_80034E70 (0x80034e70–0x80034ee4)

XREF: Tabelle @0x80074030, Einträge [0][1][2] (Waffen-Ids 0..2 = Nahkampf).

```
80034e70  addiu  sp,sp,-0x18
80034e74  lui    v1,0x800b
80034e78  addiu  v1,v1,-0x35a6            ; &DAT_800aca5a
80034e7c  sw     ra,0x10(sp)
80034e80  lhu    v0,0x0(v1)               ; HALBWORT: Substate+Init zusammen
80034e84  nop
80034e88  bne    v0,zero,LAB_80034eac     ; FSM schon aktiv -> direkt dispatchen
80034e8c  _nop
80034e90  lui    v0,0x800b
80034e94  lw     v0,-0x35ac(v0)           ; DAT_800aca54 = entity->flags
80034e98  nop
80034e9c  andi   v0,v0,0x4000             ; Messer-Latch-Bit?
80034ea0  bne    v0,zero,LAB_80034eac
80034ea4  _ori   v0,zero,0x4
80034ea8  sb     v0,0x0(v1)               ; Latch nicht gesetzt -> Substate := 4 (Erst-Einstieg)
LAB_80034eac
80034eac  lui    v0,0x800b
80034eb0  lbu    v0,-0x35a6(v0)           ; Substate
80034eb4  nop
80034eb8  sll    v0,v0,0x2
80034ebc  lui    at,0x8007
80034ec0  addiu  at,at,0x4164             ; *** Tabellen-Basis 0x80074164 = Eintrag 5 der Dauerfeuer-Tabelle! ***
80034ec4  addu   at,at,v0
80034ec8  lw     v0,0x0(at)
80034ecc  nop
80034ed0  jalr   v0
80034ed4  _nop
80034ed8  lw     ra,0x10(sp)
80034edc  addiu  sp,sp,0x18
80034ee0  jr     ra
80034ee4  _nop
```

**Struktur-Befund:** Die 10-Einträge-Tabelle @0x80074150 ist in Wahrheit ZWEI überlappende FSMs:
Einträge 0–4 (0x80034060/0x80034278/0x80034510/0x80034c74/0x80034d68) = Dauerfeuer-Substates 0–4,
Einträge 5–9 (0x80034ee8/0x800350c4/0x80035314/0x80035424/0x80035538) = **Nahkampf-Substates 0–4**
(Basis 0x80074164 @0x80034ec0). Der Nahkampf startet beim allerersten Mal in Substate 4
(0x80035538), das @0x8003575c `entity->flags |= 0x4000` setzt; danach beginnt jeder Angriff in
Substate 0 (0x80034ee8). Der Dauerfeuer-Dispatcher 0x80034014 ruft nach dem Sub noch
`FUN_800369f8(0,1)` (@0x80034044-4c), der Nahkampf-Dispatcher NICHT.

---

## 4. Callee-Belege

### 4.1 FUN_8001f314 — Anim-Frame-Stepper (a0=EMR, a1=EDD, a2=Richtung, a3=Blend-Schritt)

Decompile `RE_15_Quellcode_V2/FUN_8001f314.c` (Kernzeilen, gegen Disasm plausibel):
`uVar1 = *(u32*)(EDD + entity[0x94]*4)` = Clip-Wort (low16 = Frame-Anzahl, high16 = Offset);
`a2==0 → frame = entity[0x95]`, `a2!=0 → frame = (anzahl − entity[0x95]) − 1` (**rückwärts**);
Frame-Zeiger → `entity+0x168`; Bit 0x8000 im Frame-Wort entscheidet FUN_8001f3bc vs FUN_8001f8b4.
Beide Callees enden mit `entity[0x95]++; return (anzahl <= entity[0x95]) und dann Reset auf 0`
→ **Rückgabe ≠ 0 = Clip zu Ende, Frame-Zähler steht wieder auf 0.**
FUN_8001f3bc verrechnet den Crossfade: `countdown = entity[0x8f]`; wenn >0:
`gte_ldIR0(0x1000 − a3·countdown)` Alt-Pose + `gte_ldIR0(a3·countdown)` Neu-Pose, dann
`entity[0x8f]--`. Mit a3=0x200 und Start 7 → Blend-Gewichte 7/8, 6/8, … pro Frame.

### 4.2 FUN_80045024 — SE-Spawn (verifiziert gegen Decompile + §0-Positionsbeleg)

a0 gepackt: `bank<<24 | index<<16 | … | low`. Bank 1 → SE-Deskriptoren @0x801fcd00 (case 1).
`low != 0` → `FUN_80045a64(a1)` rechnet Distanz/Pan aus `a1[0..2]` (x,y,z) gegen die aktive
Kamera (`*(DAT_800ac778+0x24) + DAT_800b0fe4*0x20`, Felder +4/+8/+0xc) — deshalb übergibt Sub 4
`&DAT_800aca88` (Spieler-Position). Danach `SsUtKeyOnV(kanal, vabId, prog, ton, …)`.

### 4.3 FUN_8004ebdc — Magazin-Auffüllen (ROH-DISASM, Decompile teilweise stale!)

```
8004ebf0  lbu  v0,DAT_800b25c8            ; ausgeruesteter Inventar-Slot
8004ec08  lbu  v1,DAT_800b10ac[slot*4]    ; Item-Typ der Waffe (Inventar-Stride 4: +0 Typ, +1 Menge, +2 Flag)
8004ec28  lw   v0,0x80074dac + typ*0xc    ; Zeiger auf Munitions-Typ-Liste (Record-Feld +4)
8004ec30  lbu  a0,0x0(v0)                 ; erster Munitions-Item-Typ
8004ec34  jal  FUN_8004dfec               ; Inventar-Slot mit diesem Typ suchen
8004ec7c  lbu  v1,0x80074da8 + typ*0xc    ; *** Magazin-Kapazitaet (Record-Feld +0) ***
8004ec8c  lbu  a2,DAT_800b10ad[ammoslot*4]; Vorratsmenge
8004ec94  sltu v0,v1,a2 / beq ...         ; Kapazitaet < Vorrat?
  ja  @8004ecac-ecec: Waffe.Menge += Kapazitaet; Vorrat -= Kapazitaet
  nein@8004ed04-ed50: Waffe.Menge += Vorrat; FUN_8004947c(ammoslot); Typ/Menge/Flag(ammoslot) := 0
```

Kapazitäts-Tabelle @0x80074da8 (Stride 0xc, Index = **Item-Typ**, aus der EXE gedumpt):
Typ 12 → 100, Typ 14 → 100, Typ 19 → 100 (die drei Dauerfeuer-Waffen), Typ 3–6 → 15,
Typ 7 → 6, Typ 8 → 7, Typ 13 → 12, Typ 18 → 4 usw.; Feld +4 = Zeiger 0x80074c88…0x80074d88
auf die Munitions-Typ-Listen.

### 4.4 FUN_8004dfec — Inventar-Suche (ROH-DISASM; **Decompile-Datei ist FALSCH**)

`RE_15_Quellcode_V2/FUN_8004dfec.c` behauptet `return 0xffffffff` — der echte Code
@0x8004dfec-0x8004e050 ist eine Schleife:

```
8004dff0  lbu  v0,DAT_800b0fbc            ; Anzahl belegter Inventar-Slots
8004dff8  beq  v0,zero -> return -1
8004e01c  lbu  v0,DAT_800b10ac[i*4]       ; Item-Typ von Slot i
8004e024  bne  v0,a0 -> weiter            ; Typ-Vergleich
8004e02c  j    exit; _sra v0,(i<<24),24   ; gefunden: return (s8)i
8004e048  li   v0,-0x1                    ; nicht gefunden: return -1
```

→ „Decompiles verifizieren"-Regel bestätigt: bei diesen kleinen Inventar-Helfern zählt nur der Disasm.

### 4.5 FUN_8004eb70 — „Vorrat vorhanden?" (Gate vor Substate 4; ROH-DISASM)

```
8004eb74  lbu DAT_800b25c8 ; Slot -> 8004eb90 lbu Item-Typ -> 8004ebb0 lw Munitions-Listen-Ptr
8004ebb8  lbu a0,0x0(ptr)  ; erster Munitions-Typ
8004ebbc  jal FUN_8004dfec
8004ebc4  sll v0,v0,0x18 / slt v0,zero,v0 ; return (s8-Slot > 0)
```

Achtung: liefert nur dann true, wenn die Munition in Slot **> 0** liegt (Slot 0 und „nicht
gefunden" (−1) ergeben beide false — `slt zero, idx<<24`).

---

## 5. Erreichbarkeit von Sub 4: doppelt verriegelt tot im Auslieferungsstand

Der EINZIGE Schreiber von Substate 4 ist Sub 1 @0x80034468-0x80034474:

```
80034434  lw   v0,DAT_800ac76c            ; frisch gedrueckte logische Tasten
80034440  andi v0,v0,0x40                 ; Feuer (Slot 6 = Viereck) frisch gedrueckt?
80034444  beq  -> LAB_80034490
80034448  _ori v0,zero,0x12               ; *** Waffen-Id-Vergleichswert 0x12 = 18 ***
8003444c  lbu  v1,DAT_800aca5d            ; angelegte Waffen-Id
80034458  bne  v1,v0 -> LAB_80034490      ; nur Waffe 0x12 darf nachladen
80034460  jal  FUN_8004eb70               ; Vorrat da?
80034468  beq  v0,zero -> LAB_80034480    ; nein -> Leer-Klick-SE (a0=0x101xxxx @80034480)
8003446c  _ori v0,zero,0x4
80034474  sh   v0,DAT_800aca5a            ; Substate := 4
```

Kontext dieser Stelle: sie liegt hinter `FUN_8004ea6c()==0` (@0x80034404-0x8003440c) —
Magazin leer (FUN_8004ea6c @0x8004ea6c: Menge des ausgerüsteten Slots ≠ 0, mit
Kombi-Item-Weiche `DAT_800b10ae[slot]==2 → slot−1` und Sperr-Slot 0x80).

Beide Riegel:
1. **Waffen-Id-Riegel:** In diese FSM dispatchen nur Ids 0x0C/0x0E/0x13 (Tabelle @0x80074030,
   aus der EXE gedumpt). Der Vergleich verlangt 0x12 → nie wahr. (Waffe 0x12 dispatcht nach
   0x80032e9c und hat in der Entlade-Tabelle @0x80074100 den Eintrag NULL.)
2. **Munitions-Riegel:** Selbst bei Id 0x12 müsste FUN_8004eb70 true liefern — dazu müsste die
   Munitions-Typ-Liste des Item-Typs einen im Inventar (Slot>0) vorhandenen Typ nennen.

**Folgerung:** Sub 4 (Nachladen) ist fertig implementierter, aber im MZD-Auslieferungsstand
unerreichbarer Code — ein Überbleibsel einer Ausbaustufe mit einer vierten Dauerfeuer-/
Magazin-Waffe (Id 0x12). Die drei ausgelieferten Dauerfeuer-Waffen feuern direkt aus dem
Vorrat (Kapazität 100) und kennen nur Leer-Klick statt Nachladen.

## 6. FSM-Übergangsbild (belegte Kanten)

```
Sub 0 (0x80034060)  --Init/Feuerstart, setzt aca3c|=0xC0 @0x80034108-->  Sub 1
Sub 1 (0x80034278)  --R1 (Slot 8, Bit 0x100) losgelassen @0x800342f8--> Sub 3 (@0x8003430c)
Sub 1               --Feuer gehalten + FUN_8004ea6c()!=0 @0x800343f8-0x80034418--> Sub 2 (schiessen)
Sub 1               --Magazin leer + Feuer-Edge + Id==0x12 + Vorrat @0x80034434-74--> Sub 4  [TOT]
Sub 2 (0x80034510)  --Ende @0x800347dc-e4--> Sub 1
Sub 3 (0x80034c74)  --Clip 6 rueckwaerts fertig--> Substate 0 + Routine 0 + aca3c&=~0xC0  [FSM-Exit]
Sub 4 (0x80034d68)  --Clip 15 fertig--> Substate 1 + Magazin auffuellen + SE 0x01030001
```

## 7. Port-Relevanz (re15_port)

- `platform/pc/main.c`-Vermutung „Clips 14/15 = Feuer-Ende/Halten-runter — PRÜFEN" ist
  **widerlegt**: Das Feuer-Ende der Dauerfeuer-Waffen ist **Clip 6 rückwärts**
  (@0x80034c98 + a2=1 @0x80034cf0), nicht Clip 14/15. Clip 15 ist die (tote) Nachlade-Anim
  (@0x80034d90).
- Während Absenken (Sub 3) und Nachladen (Sub 4) bleibt die D-Pad-L/R-Drehung mit ±0x18/Frame
  aktiv (@0x80034cdc/0x80034d00 bzw. 0x80034dec/0x80034e10) — identische Rate wie in den
  Feuer-Substates.
- FSM-Exit muss ZUSÄTZLICH Routine-Index (entity+0x05) auf 0 und Statusbits 0x40|0x80 in
  DAT_800aca3c löschen — sonst bleibt der Spieler im Waffen-Zustand hängen.
