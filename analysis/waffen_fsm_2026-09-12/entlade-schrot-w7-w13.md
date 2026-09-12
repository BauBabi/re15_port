# Entlade-Handler Waffe 7 (Super Redhawk), 8 (Remington M870), 13 (SPAS-12)

RE-Dossier 2026-09-12. Quelle: `ghidra1_V2.txt` (RE1.5 PSX.EXE, Zeilen 129110-129315),
Rohbytes `info/Re1.5/PSX.EXE` (t_addr=0x80010000, Dateioffset(va) = 0x800 + va - 0x80010000),
ESP-Daten `re15_port/shared_assets/PSX/DATA/CORE00.ESP`.

**Einordnung:** Die drei Funktionen sind Eintraege der **Entlade-Tabelle @0x80074100**
(Dateioffset 0x64900, Rohbytes verifiziert):
`[7]=0x800339A4 @0x8007411C, [8]=0x80033A58 @0x80074120, [13]=0x80033B98 @0x80074134`.
Ghidra-XREFs bestaetigen je Handler GENAU EINEN Aufrufer (die Tabelle):
`LAB_800339a4 XREF[1]: 8007411c(*)`, `LAB_80033a58 XREF[1]: 80074120(*)`,
`LAB_80033b98 XREF[1]: 80074134(*)`.
Sie laufen im Standard-Feuer-FSM 0x80032E9C beim Schuss (Entladung) und enthalten NUR den
Effekt-/Abrechnungs-Teil: Effekt-Spawns + 1x Schadens-Resolve + 1x Munition-1.
Der Schrot-MEHRFACH-Resolve (Frames 3/5/7) liegt @0x80033508-58 im Standard-FSM, NICHT hier.

## Waffen-Identitaeten (belegt, nicht geraten)

Byte-true Item-Namenskatalog (Glyph-Blob **DAT_800c4a28**, decodiert ueber die
Item-Prompt-Fontmap; Port: `re15_port/engine/src/inventory_common.c:246-259`):

| Item-Id | Name | Effekt-Beleg aus dem Handler |
|---|---|---|
| 0x07 | **SUPER REDHAWK** (Ruger, .44-Magnum-REVOLVER) | KEIN Huelsenauswurf-Effekt (kein Effekt-4-Call im ganzen Bereich 0x800339A4-0x80033A57) — Revolverhuelsen bleiben in der Trommel. Offsets identisch zur Browning HP, nur groessere Effekt-Scales. |
| 0x08 | **REMINGTON M870** (Schrotflinte) | Muendung sub 3 (Grossblitz, 5 Streams), Huelse sub 3 (Schrothuelse, eigene Init-Routine 38). |
| 0x0D | **SPAS-12** (zweite Schrotflinte) | Byte-GLEICHE Effekt-Codes wie die M870, nur andere Gun-Bone-Offsets (laengerer Lauf). |

Die Aufgaben-Vermutung "7=Remington? 13=Magnum?" ist damit widerlegt: 7 = Magnum-Revolver,
8 = Remington, 13 = SPAS-12.

## Gemeinsames ABI (Vorwissen, verifiziert — analysis/bite_blood_fx.md §1 + RE15_ESP_ROWMACHINE.md §4)

`FUN_80019700` = ESP-Effekt-Spawner:
- **a0 gepackt** (Decode @0x80019704-1c): `effekt_id = a0>>24`, `sub = (a0>>16)&0xff`,
  `scale16 = a0&0xffff` (Q12, 0x1000 = 1.0). Effekt-Ids der globalen Bank CORE00.ESP:
  2 = Muendungsfeuer, 3 = Rauch, 4 = Huelsenauswurf.
- **a1** -> Slot +0x2e = Basis-Yaw-Seed; hier immer `lh DAT_800acabe` = **Spieler-Yaw**
  (identisch Spieler-Entity +0x6a; Schreiber @0x8001d8dc-e8).
- **a2** = Matrix-Zeiger (32 B kopiert, Translation @+0x14); hier `[DAT_800acbdc]+0x7a4`
  = **Gun-Bone-Matrix** des Spieler-Posebuffers.
- **a3** = Zeiger auf lokales Offset-Triple `sp+0x10/0x14/0x18` (-> Slot +0x40/44/48,
  angewandt als R*v+T, FUN_80019e20).

Abrechnung (in allen drei Handlern identisch):
- `FUN_80011f50(a0 = lbu DAT_800aca5d [Waffen-Id], a1 = lw [DAT_800ac784 + 0x7c])` = 1x Schadens-Resolve.
- `FUN_8004eae4()` = Munition -1 (Rueckgabe ungeprueft), genau 1x.
- **KEIN SE-Aufruf im Handler.** Der Knall ist DATENGETRIEBEN: Row-Stream 0 des
  Muendungs-Effekts traegt Routine 9 (@0x80017654) = `FUN_80045024(0x01000001, &weltpos)`
  (SE-Code HART CODIERT in der Routine, ARMS-Bank Record 0) auf dem 2. Tick des Slots.

---

## 1. Waffe 7 = SUPER REDHAWK — Entlade-Handler 0x800339A4-0x80033A57

### Roh-Listing (ghidra1_V2.txt:129110-129154)

```
                     LAB_800339a4                XREF[1]: 8007411c(*)  ; Entlade-Tabelle [7]
800339a4 addiu  sp,sp,-0x28
800339a8 lui    a0,0x200
800339ac ori    a0,a0,0xe00          ; a0 = 0x02000E00: Effekt 2 (MUENDUNG) sub 0, scale 0x0E00 (0.875)
800339b0 lui    a1,0x800b
800339b4 lh     a1,DAT_800acabe      ; a1 = Spieler-Yaw (Basis-Yaw-Seed)
800339b8 addiu  a3,sp,0x10           ; a3 = Offset-Triple
800339bc sw     s0,0x20(sp)
800339c0 lui    s0,0x800b
800339c4 lw     s0,DAT_800acbdc      ; Posebuffer-Basis
800339c8 ori    v0,zero,0x8c
800339cc sw     v0,0x10(sp)          ; off[0] = 0x8C  (140)
800339d0 ori    v0,zero,0x25d
800339d4 sw     ra,0x24(sp)
800339d8 sw     v0,0x14(sp)          ; off[1] = 0x25D (605)
800339dc sw     zero,0x18(sp)        ; off[2] = 0
800339e0 addiu  s0,s0,0x7a4          ; s0 = Gun-Bone-Matrix (+0x7a4)
800339e4 jal    FUN_80019700         ; SPAWN Muendungsfeuer
800339e8 _move  a2,s0
800339ec lui    a0,0x300
800339f0 ori    a0,a0,0x1000         ; a0 = 0x03001000: Effekt 3 (RAUCH) sub 0, scale 0x1000 (1.0)
800339f4 move   a2,s0
800339f8 addiu  a3,sp,0x10
800339fc lui    a1,0x800b
80033a00 lh     a1,DAT_800acabe      ; Spieler-Yaw
80033a04 ori    v0,zero,0x91
80033a08 sw     v0,0x10(sp)          ; off[0] = 0x91  (145)
80033a0c ori    v0,zero,0x1f4
80033a10 sw     v0,0x14(sp)          ; off[1] = 0x1F4 (500)
80033a14 li     v0,-0x19
80033a18 jal    FUN_80019700         ; SPAWN Rauch
80033a1c _sw    v0,0x18(sp)          ; off[2] = -0x19 (-25)
80033a20 lui    v0,0x800b
80033a24 lw     v0,DAT_800ac784      ; Spieler-Entity
80033a28 lui    a0,0x800b
80033a2c lbu    a0,DAT_800aca5d      ; a0 = angelegte Waffen-Id (7)
80033a30 lw     a1,0x7c(v0)          ; a1 = [entity+0x7c]
80033a34 jal    FUN_80011f50         ; 1x SCHADENS-RESOLVE
80033a38 _nop
80033a3c jal    FUN_8004eae4         ; 1x MUNITION -1
80033a40 _nop
80033a44 lw     ra,0x24(sp)
80033a48 lw     s0,0x20(sp)
80033a4c addiu  sp,sp,0x28
80033a50 jr     ra
80033a54 _nop
```

### Pseudo-C

```c
void entlade_w07_super_redhawk(void)   /* @0x800339A4, Tabelle @0x8007411C */
{
    s32 off[3];
    u8 *gunbone = *(u8**)DAT_800acbdc + 0x7a4;              /* Gun-Bone-Matrix */

    off[0]=0x8C; off[1]=0x25D; off[2]=0;                    /* @0x800339c8-dc */
    FUN_80019700(0x02000E00, (s16)DAT_800acabe, gunbone, off);  /* Muendung, scale 0.875 */

    off[0]=0x91; off[1]=0x1F4; off[2]=-0x19;                /* @0x80033a04-1c */
    FUN_80019700(0x03001000, (s16)DAT_800acabe, gunbone, off);  /* Rauch, scale 1.0 */

    /* KEIN Huelsen-Effekt: Revolver. */
    FUN_80011f50(DAT_800aca5d, *(u32*)(DAT_800ac784 + 0x7c));   /* @0x80033a34 */
    FUN_8004eae4();                                             /* @0x80033a3c: Patrone -1 */
}
```

**Vergleich Browning HP (@0x800337BC, bereits portiert):** identische Offsets
({0x8C,0x25D,0} Muendung @0x800337e0, {0x91,0x1F4,-0x19} Rauch @0x80033818) — dieselbe
Hand-Pose —, aber groessere Scales (Muendung 0x0E00 statt 0x0800, Rauch 0x1000 statt 0x0C00)
und der Huelsen-Call (0x04000800 @0x8003383c der Browning) FEHLT komplett.

### Konstanten

| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x800339a8-ac | 0x02000E00 | Effekt 2 (Muendung), sub 0, scale 0x0E00 = 0.875 Q12 |
| 0x800339c8-dc | {0x8C, 0x25D, 0} | Muendungs-Offset im Gun-Bone-Raum |
| 0x800339ec-f0 | 0x03001000 | Effekt 3 (Rauch), sub 0, scale 0x1000 = 1.0 |
| 0x80033a04-1c | {0x91, 0x1F4, -0x19} | Rauch-Offset |
| 0x80033a34 | jal FUN_80011f50 | genau 1 Schadens-Resolve |
| 0x80033a3c | jal FUN_8004eae4 | genau 1 Patrone |
| — | (kein 0x04xxxxxx) | KEIN Huelsenauswurf -> Revolver |

---

## 2. Waffe 8 = REMINGTON M870 — Entlade-Handler 0x80033A58-0x80033B37

### Roh-Listing (ghidra1_V2.txt:129156-129211)

```
                     LAB_80033a58                XREF[1]: 80074120(*)  ; Entlade-Tabelle [8]
80033a58 addiu  sp,sp,-0x30
80033a5c lui    a0,0x203
80033a60 ori    a0,a0,0xf00          ; a0 = 0x02030F00: Effekt 2 (MUENDUNG) sub 3, scale 0x0F00 (0.9375)
80033a64 lui    a1,0x800b
80033a68 lh     a1,DAT_800acabe      ; Spieler-Yaw
80033a6c addiu  a3,sp,0x10
80033a70 sw     s1,0x24(sp)
80033a74 ori    s1,zero,0xa0         ; s1 = 0xA0 (x-Offset, WIEDERVERWENDET fuer Huelse)
80033a78 sw     s0,0x20(sp)
80033a7c lui    s0,0x800b
80033a80 lw     s0,DAT_800acbdc
80033a84 ori    v0,zero,0x528
80033a88 sw     v0,0x14(sp)          ; off[1] = 0x528 (1320)
80033a8c ori    v0,zero,0x3
80033a90 sw     ra,0x28(sp)
80033a94 sw     s1,0x10(sp)          ; off[0] = 0xA0 (160)
80033a98 sw     v0,0x18(sp)          ; off[2] = 3
80033a9c addiu  s0,s0,0x7a4          ; Gun-Bone-Matrix
80033aa0 jal    FUN_80019700         ; SPAWN Muendungs-Grossblitz (sub 3 = 5 Slots)
80033aa4 _move  a2,s0
80033aa8 lui    a0,0x300
80033aac ori    a0,a0,0x1400         ; a0 = 0x03001400: Effekt 3 (RAUCH) sub 0, scale 0x1400 (1.25)
80033ab0 move   a2,s0
80033ab4 addiu  a3,sp,0x10
80033ab8 lui    a1,0x800b
80033abc lh     a1,DAT_800acabe
80033ac0 ori    v0,zero,0x500
80033ac4 jal    FUN_80019700         ; SPAWN Rauch
80033ac8 _sw    v0,0x14(sp)          ; NUR off[1] = 0x500 neu; off[0]=0xA0/off[2]=3 ERBEN vom Muendungs-Triple
80033acc lui    a0,0x403
80033ad0 ori    a0,a0,0x920          ; a0 = 0x04030920: Effekt 4 (HUELSE) sub 3, scale 0x0920 (0.5703)
80033ad4 move   a2,s0
80033ad8 addiu  a3,sp,0x10
80033adc lui    a1,0x800b
80033ae0 lh     a1,DAT_800acabe
80033ae4 ori    v0,zero,0x208
80033ae8 sw     v0,0x14(sp)          ; off[1] = 0x208 (520)
80033aec ori    v0,zero,0x50
80033af0 sw     s1,0x10(sp)          ; off[0] = 0xA0 (s1)
80033af4 jal    FUN_80019700         ; SPAWN Schrothuelse
80033af8 _sw    v0,0x18(sp)          ; off[2] = 0x50 (80)
80033afc lui    v0,0x800b
80033b00 lw     v0,DAT_800ac784      ; Spieler-Entity
80033b04 lui    a0,0x800b
80033b08 lbu    a0,DAT_800aca5d      ; Waffen-Id (8)
80033b0c lw     a1,0x7c(v0)
80033b10 jal    FUN_80011f50         ; 1x SCHADENS-RESOLVE (Grundschuss; Streuung liegt @0x80033508-58!)
80033b14 _nop
80033b18 jal    FUN_8004eae4         ; 1x MUNITION -1
80033b1c _nop
80033b20 lw     ra,0x28(sp)
80033b24 lw     s1,0x24(sp)
80033b28 lw     s0,0x20(sp)
80033b2c addiu  sp,sp,0x30
80033b30 jr     ra
80033b34 _nop
```

### Pseudo-C

```c
void entlade_w08_remington_m870(void)  /* @0x80033A58, Tabelle @0x80074120 */
{
    s32 off[3];
    u8 *gunbone = *(u8**)DAT_800acbdc + 0x7a4;

    off[0]=0xA0; off[1]=0x528; off[2]=3;                    /* @0x80033a84-98 */
    FUN_80019700(0x02030F00, (s16)DAT_800acabe, gunbone, off);  /* Muendung sub 3, scale 0.9375 */

    off[1]=0x500;                                           /* @0x80033ac0/c8 — x/z bleiben! */
    FUN_80019700(0x03001400, (s16)DAT_800acabe, gunbone, off);  /* Rauch (0xA0,0x500,3), scale 1.25 */

    off[0]=0xA0; off[1]=0x208; off[2]=0x50;                 /* @0x80033ae4-f8 */
    FUN_80019700(0x04030920, (s16)DAT_800acabe, gunbone, off);  /* Schrothuelse sub 3, scale 0.5703 */

    FUN_80011f50(DAT_800aca5d, *(u32*)(DAT_800ac784 + 0x7c));   /* @0x80033b10: NUR der Grundschuss */
    FUN_8004eae4();                                             /* @0x80033b18: EINE Patrone */
}
```

**Abgrenzung Streuung:** Die drei ZUSAETZLICHEN Resolves der Schrotflinte (Rueckstoss-Bilder
3/5/7) liegen im Standard-FSM @0x80033508-58 (`lbu DAT_800aca5d`/`ori v0,8`/`bne` ->
`lbu DAT_800acae9` -> `beq 3/beq 5/bne 7` -> `jal FUN_80011f50`; Memory
reai-v2-waffen-banken). Der Entlade-Handler traegt davon NICHTS — 1 Resolve, 1 Patrone.

### Konstanten

| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x80033a5c-60 | 0x02030F00 | Effekt 2 (Muendung), **sub 3**, scale 0x0F00 = 0.9375 |
| 0x80033a74/94, 0x80033a84-98 | {0xA0, 0x528, 3} | Muendungs-Offset |
| 0x80033aa8-ac | 0x03001400 | Effekt 3 (Rauch), sub 0, scale 0x1400 = 1.25 |
| 0x80033ac0/c8 | {0xA0, 0x500, 3} | Rauch-Offset (x/z vom Muendungs-Triple geerbt) |
| 0x80033acc-d0 | 0x04030920 | Effekt 4 (Huelse), **sub 3** = Schrothuelse, scale 0x0920 |
| 0x80033ae4-f8 | {0xA0, 0x208, 0x50} | Huelsen-Offset |
| 0x80033b10 | jal FUN_80011f50 | 1 Resolve (Streuung extern @0x80033508-58) |
| 0x80033b18 | jal FUN_8004eae4 | 1 Patrone |

---

## 3. Waffe 13 = SPAS-12 — Entlade-Handler 0x80033B98-0x80033C73

### Roh-Listing (ghidra1_V2.txt:129261-129315)

```
                     LAB_80033b98                XREF[1]: 80074134(*)  ; Entlade-Tabelle [13]
80033b98 addiu  sp,sp,-0x28
80033b9c lui    a0,0x203
80033ba0 ori    a0,a0,0xf00          ; a0 = 0x02030F00: Effekt 2 sub 3, scale 0x0F00 — BYTE-GLEICH M870
80033ba4 lui    a1,0x800b
80033ba8 lh     a1,DAT_800acabe      ; Spieler-Yaw
80033bac addiu  a3,sp,0x10
80033bb0 sw     s0,0x20(sp)
80033bb4 lui    s0,0x800b
80033bb8 lw     s0,DAT_800acbdc
80033bbc ori    v0,zero,0x17c
80033bc0 sw     v0,0x10(sp)          ; off[0] = 0x17C (380)
80033bc4 ori    v0,zero,0x636
80033bc8 sw     v0,0x14(sp)          ; off[1] = 0x636 (1590) — laengster Lauf aller Handfeuerwaffen
80033bcc li     v0,-0x1a4
80033bd0 sw     ra,0x24(sp)
80033bd4 sw     v0,0x18(sp)          ; off[2] = -0x1A4 (-420)
80033bd8 addiu  s0,s0,0x7a4          ; Gun-Bone-Matrix
80033bdc jal    FUN_80019700         ; SPAWN Muendungs-Grossblitz
80033be0 _move  a2,s0
80033be4 lui    a0,0x300
80033be8 ori    a0,a0,0x1400         ; a0 = 0x03001400: Rauch sub 0, scale 1.25 — BYTE-GLEICH M870
80033bec move   a2,s0
80033bf0 addiu  a3,sp,0x10
80033bf4 lui    a1,0x800b
80033bf8 lh     a1,DAT_800acabe
80033bfc ori    v0,zero,0x5d2
80033c00 jal    FUN_80019700         ; SPAWN Rauch
80033c04 _sw    v0,0x14(sp)          ; NUR off[1] = 0x5D2 (1490) neu; off[0]=0x17C/off[2]=-0x1A4 ERBEN
80033c08 lui    a0,0x403
80033c0c ori    a0,a0,0x920          ; a0 = 0x04030920: Schrothuelse sub 3 — BYTE-GLEICH M870
80033c10 move   a2,s0
80033c14 addiu  a3,sp,0x10
80033c18 lui    a1,0x800b
80033c1c lh     a1,DAT_800acabe
80033c20 ori    v0,zero,0x118
80033c24 sw     v0,0x10(sp)          ; off[0] = 0x118 (280)
80033c28 ori    v0,zero,0x190
80033c2c sw     v0,0x14(sp)          ; off[1] = 0x190 (400)
80033c30 li     v0,-0xa0
80033c34 jal    FUN_80019700         ; SPAWN Huelse
80033c38 _sw    v0,0x18(sp)          ; off[2] = -0xA0 (-160)
80033c3c lui    v0,0x800b
80033c40 lw     v0,DAT_800ac784
80033c44 lui    a0,0x800b
80033c48 lbu    a0,DAT_800aca5d      ; Waffen-Id (13)
80033c4c lw     a1,0x7c(v0)
80033c50 jal    FUN_80011f50         ; 1x SCHADENS-RESOLVE
80033c54 _nop
80033c58 jal    FUN_8004eae4         ; 1x MUNITION -1
80033c5c _nop
80033c60 lw     ra,0x24(sp)
80033c64 lw     s0,0x20(sp)
80033c68 addiu  sp,sp,0x28
80033c6c jr     ra
80033c70 _nop
```

### Pseudo-C

```c
void entlade_w0d_spas12(void)          /* @0x80033B98, Tabelle @0x80074134 */
{
    s32 off[3];
    u8 *gunbone = *(u8**)DAT_800acbdc + 0x7a4;

    off[0]=0x17C; off[1]=0x636; off[2]=-0x1A4;              /* @0x80033bbc-d4 */
    FUN_80019700(0x02030F00, (s16)DAT_800acabe, gunbone, off);  /* Muendung sub 3 (wie M870) */

    off[1]=0x5D2;                                           /* @0x80033bfc/c04 — x/z bleiben! */
    FUN_80019700(0x03001400, (s16)DAT_800acabe, gunbone, off);  /* Rauch (0x17C,0x5D2,-0x1A4) */

    off[0]=0x118; off[1]=0x190; off[2]=-0xA0;               /* @0x80033c20-38 */
    FUN_80019700(0x04030920, (s16)DAT_800acabe, gunbone, off);  /* Schrothuelse sub 3 */

    FUN_80011f50(DAT_800aca5d, *(u32*)(DAT_800ac784 + 0x7c));   /* @0x80033c50 */
    FUN_8004eae4();                                             /* @0x80033c58 */
}
```

### Konstanten

| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x80033b9c-a0 | 0x02030F00 | Muendung sub 3, scale 0x0F00 (identisch M870 @0x80033a5c) |
| 0x80033bbc-d4 | {0x17C, 0x636, -0x1A4} | Muendungs-Offset (laengerer Lauf als M870 {0xA0,0x528,3}) |
| 0x80033be4-e8 | 0x03001400 | Rauch sub 0, scale 0x1400 (identisch M870) |
| 0x80033bfc/c04 | {0x17C, 0x5D2, -0x1A4} | Rauch-Offset (x/z geerbt) |
| 0x80033c08-0c | 0x04030920 | Schrothuelse sub 3 (identisch M870 @0x80033acc) |
| 0x80033c20-38 | {0x118, 0x190, -0xA0} | Huelsen-Offset (M870: {0xA0, 0x208, 0x50}) |
| 0x80033c50 | jal FUN_80011f50 | 1 Resolve |
| 0x80033c58 | jal FUN_8004eae4 | 1 Patrone |

---

## 4. ESP-Daten-Anhang: Was sub 3 konkret bedeutet (CORE00.ESP, selbst geparst)

Parser-Formeln = Loader FUN_8001923c/FUN_8001945c/FUN_80019700 (Layout dokumentiert in
`re15_port/engine/src/re15_esp.c:157-181`). Id-Header der globalen Bank @Dateioffset 0:
`{3,8,0,2,4}`. Rowblock je Effekt = eff_start + (count_a*2+count_b+2)*4:

| Effekt | eff_start | rowblk | Streams sub 0 | Streams sub 3 |
|---|---|---|---|---|
| 2 Muendung | 0x0F00 | 0x0FC8 | **2** (R8->R9->R0; R10->R0) | **5** (R8->R9->R0; 4x R10->R0) — sub-3-Basis @0x1200 |
| 3 Rauch | 0x0008 | 0x0384 | 1 (R10->R0) | 1 (R10->R0) — Handler nutzen ohnehin sub 0 |
| 4 Huelse | 0x1728 | 0x18C0 | 1 (**R16**->R11) | 1 (**R38**->R11) — sub-3-Basis @0x1A00 |

Konsequenzen (datenbelegt):
- **Muendung sub 3 = Grossblitz:** 5 gespawnte Slots je Schuss statt 2. Stream 0 traegt wie
  bei der Handfeuerwaffe die Kette Routine 8 (Anzeige + Sekundaerblitz-Kette) -> **Routine 9
  @0x80017654 = der positionale KNALL** `FUN_80045024(0x01000001, &weltpos)` + Laerm-Latch
  `0x800b5358 := 1` auf dem 2. Slot-Tick. Die SE-Id 0x01000001 (ARMS-Bank Record 0) ist IN
  DER ROUTINE hart codiert — Magnum, M870 und SPAS-12 feuern also denselben Knall-SE-Weg wie
  die Handfeuerwaffe; im Handler selbst steht KEIN SE-Call.
- **Huelse sub 3 = Schrothuelse:** eigener Init (Routine 38 statt 16), danach dieselbe
  R11-Physik (RNG-Streuung + Schwerkraft + Boden-Bounce mit Klick-SE, Row-Daten).
- Der CLUT-Zusatz `(sub>>3)*0x40` (@0x8001987c-88) ist fuer sub 3 = 0 (3>>3=0), sub 3
  waehlt also NUR den Row-Stream, keine andere Palette.

## 5. Offene Punkte

- **Routine 38** (Init der Schrothuelse, Dispatch-Tabelle @0x80071d40[38]) ist hier nur als
  Row-Selektor belegt, ihr Koerper wurde nicht disassembliert (ausserhalb dieser Gruppe;
  Routine 16 = 2-Tick-Eject-Hold ist aus dem Blut/Huelsen-Trace wf_a18487d9 bekannt).
- Die Achsen-Semantik des Offset-Triples ist Gun-Bone-lokal (R*v+T); welche Komponente
  entlang des Laufs zeigt, haengt an der Bindepose der jeweiligen W-Bank und wurde hier
  nicht separat vermessen (fuer den Port unerheblich: Triple wird 1:1 an den Spawner gereicht).

RE2-Fallback wurde NICHT benoetigt — alle Aussagen sind RE1.5-nativ belegt (Disasm +
PSX.EXE-Rohbytes + CORE00.ESP-Daten + Item-Namenskatalog DAT_800c4a28).
