# Entlade-Handler Waffe 12 (Ingram M10) — LAB_800347F8 (0x800347F8–0x80034A2C)

Quelle: `ghidra1_V2.txt` Zeilen 130116–130261 (Listing unten byte-identisch übernommen).
Tabelleneintrag: Entlade-Tabelle @0x80074100 + 12*4 = **@0x80074130** → `LAB_800347f8` (XREF `80074130(*)` im Dump).

## 1. Aufrufweg / FSM-Einbettung

Waffen-Dispatch @0x80074030: Id 12 (Ingram M10) → Dauerfeuer-FSM **0x80034014**
(`lbu DAT_800aca5a` @0x80034018 → Tabelle @0x80074150).

Der Entlade-Handler läuft in **FSM-Substate DAT_800aca5a == 2** = Sub `LAB_80034510`
(Tabellenslot **@0x80074158** = 0x80074150 + 2*4; XREF an LAB_80034510: `80074158(*)`).
Aufrufstelle: `jalr` **@0x800345EC** nach `lw v0, PTR_LAB_80074100[DAT_800aca5d*4]`
(@0x800345C4 `lbu DAT_800aca5d`, @0x800345D8–E4 Tabellenzugriff).

Innere Phase des Feuer-Subs = **DAT_800aca5b**:

| aca5b | Code | Wirkung |
|---|---|---|
| 0 | LAB_80034564 | Seed: `aca5b=1` @0x80034564; Crossfade `DAT_800acae3=7` @0x80034578; **Feuer-Loop-Clip** `DAT_800acae8 = 7 + 3*bit15(acaec) + 6*bit13(acaec)` = **7/10/13** (mitte/hoch/tief) @0x8003457C–A8; fällt DURCH in Phase 1 (gleicher Frame) |
| 1 | LAB_800345AC | `DAT_800acc24=0x5A` @0x800345AC–B4; `DAT_800aca52 \|= 1` @0x800345B8–D4 (Schuss-Latch, liest u.a. Krähen-KI, Port `g_aca52_flags`); **jalr Entlade-Handler @0x800345EC**; danach Pad-Held `DAT_800ac768`: Bit 0x8 → `DAT_800acabe -= 0x18` @0x8003460C–20, Bit 0x2 → `+= 0x18` @0x80034630–44 (Yaw-Schwenk 24/Frame beim Feuern); Bit 0x100 (Feuer) losgelassen → **`DAT_800aca5a = 1`** @LAB_800347DC (0x800347DC–E4); sonst `FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 0, 0x200)` @0x80034654–68 (Keyframe-Anim-Advance); Zielhöhen-Wechsel: Bit 0x10 & !bit15 → `acaec=(acaec&0x1FFF)\|0x8000` @0x80034684–A0, Bit 0x20 & !bit13 → `\|0x2000` @0x800346A4–D4, sonst (weder 0x10 noch 0x20, !bit14) → `\|0x4000` @0x800346F0–28; jeweils `aca5b=0` (Clip-Reseed); Bit 0x40 losgelassen → **`aca5b=2`** @0x8003473C–58 |
| 2 | LAB_80034764 | Feuer-Ende: `aca5b=3` @0x80034764–68; `DAT_800acae9=0` @0x8003477C; `acae3=7` @0x80034784; **Feuer-Ende-Clip** `acae8 = 8 + 3*bit15 + 6*bit13` = **8/11/14** @0x80034788–B4; weiter wie Phase 3 |
| 3 | LAB_800347BC | `FUN_8001f314(acbc4, acbc8, 0, 0x200)` @0x800347BC–D0; Rückgabe != 0 → **`DAT_800aca5a = 1`** @0x800347D4→LAB_800347DC |

**Eintritt in Substate 2** (aus Sub 1 = Ziel-Halten, LAB_80034278): Pad-Held Bit 0x40 @0x800343F0–FC
UND `FUN_8004ea6c() != 0` @0x80034404–0C (= Munition im angelegten Slot > 0: liest Inventar-Slot
`DAT_800b25c8`, Mengen-Byte @0x800B10AD+slot*4, `sltu zero,count` @0x8004ead0–d4) →
`DAT_800aca5a=2` @0x80034414–18, **`DAT_800acae9=0`** @0x8003441C–20, **`DAT_800acaf2=0`** @0x80034424–28.

Zweiter Leser der Entlade-Tabelle: Standard-Feuer-FSM 0x80032E9C, Sub [2]=`LAB_80033460`
(Tabelle @0x800740F4, Slot @0x800740FC), `jalr` @0x800334E0 — erreicht 0x800347F8 aber nie,
da der Waffen-Dispatch @0x80074030 die Id 12 ausschließlich auf 0x80034014 mappt.

## 2. Roh-Listing mit Annotation

```
; --- Prolog -------------------------------------------------------------
800347f8 addiu  sp,sp,-0x30
800347fc sw     s1,0x24(sp)
80034800 lui    s1,0xaaaa            ; s1 = 0xAAAAAAAB (Magic: /3 bzw. /6)
80034804 sw     s2,0x28(sp)
80034808 lui    s2,0x800b
8003480c addiu  s2,s2,-0x3517        ; s2 = &DAT_800ACAE9 (In-Clip-Bildzaehler)
80034810 sw     ra,0x2c(sp)
80034814 sw     s0,0x20(sp)
; --- Block 1: alle 3 Clip-Frames Muendungsfeuer --------------------------
80034818 lbu    a0,0x0(s2)           ; a0 = DAT_800acae9
8003481c ori    s1,s1,0xaaab
80034820 multu  a0,s1                ; hi = a0*0xAAAAAAAB
80034824 mfhi   v1
80034828 srl    v1,v1,0x1            ; v1 = a0/3
8003482c sll    v0,v1,0x1
80034830 addu   v0,v0,v1             ; v0 = (a0/3)*3
80034834 subu   a0,a0,v0             ; a0 = a0 % 3
80034838 andi   a0,a0,0xff
8003483c bne    a0,zero,LAB_80034908 ; (acae9 % 3) != 0 -> kein FX-Spawn
80034840 _lui   v0,0x9249            ; (Delay: Magic-Oberteil fuer /7 in Block 3)
80034844 lui    a0,0x201
80034848 ori    a0,a0,0x800          ; a0 = 0x02010800  MUENDUNGSFEUER (fx-Id 2, Sub 1, Scale 0x800)
8003484c addiu  a3,sp,0x10           ; a3 = &Offset-Triple
80034850 lui    v0,0x800b
80034854 lw     v0,DAT_800acbdc      ; Posebuffer-Basis
80034858 lui    a1,0x800b
8003485c lh     a1,DAT_800acabe      ; a1 = Spieler-Yaw (Basis-Yaw-Seed des Effekts)
80034860 lui    t0,0x800b
80034864 lbu    t0,DAT_800aca5c      ; Charakter-Byte
80034868 addiu  s0,v0,0x7a4          ; s0 = Gun-Bone-Matrix (+0x7A4)
8003486c move   a2,s0
80034870 andi   t0,t0,0x4            ; Bit 2: 0=Leon, 4=Elza
80034874 sltu   t0,zero,t0           ; t0 = elza (0/1)
80034878 sll    v1,t0,0x2
8003487c addu   v1,v1,t0
80034880 sll    v1,v1,0x1            ; v1 = elza*10
80034884 ori    v0,zero,0x46
80034888 subu   v0,v0,v1             ; X = 0x46(70) - elza*10  -> 70/60
8003488c sll    v1,t0,0x4
80034890 subu   v1,v1,t0
80034894 sll    v1,v1,0x1            ; v1 = elza*30
80034898 sw     v0,0x10(sp)          ; ofs.x
8003489c ori    v0,zero,0x41a
800348a0 subu   v0,v0,v1             ; Y = 0x41A(1050) - elza*30 -> 1050/1020
800348a4 sw     v0,0x14(sp)          ; ofs.y
800348a8 li     v0,-0x3c
800348ac subu   v0,v0,v1             ; Z = -0x3C(-60) - elza*30 -> -60/-90
800348b0 jal    FUN_80019700         ; Effekt-Spawn Muendungsfeuer
800348b4 _sw    v0,0x18(sp)          ; ofs.z (Delay-Slot)
; --- Block 2: alle 6 Clip-Frames Rauch (gleiche Offsets) -----------------
800348b8 lbu    a0,0x0(s2)           ; a0 = DAT_800acae9
800348c0 multu  a0,s1
800348c4 mfhi   v1
800348c8 srl    v1,v1,0x2            ; v1 = a0/6
800348cc sll    v0,v1,0x1
800348d0 addu   v0,v0,v1
800348d4 sll    v0,v0,0x1            ; v0 = (a0/6)*6
800348d8 subu   a0,a0,v0             ; a0 = a0 % 6
800348dc andi   a0,a0,0xff
800348e0 bne    a0,zero,LAB_80034908
800348e4 _lui   v0,0x9249
800348e8 lui    a0,0x300
800348ec ori    a0,a0,0xb00          ; a0 = 0x03000B00  RAUCH (fx-Id 3, Scale 0xB00)
800348f0 lui    a1,0x800b
800348f4 lh     a1,DAT_800acabe      ; Yaw
800348f8 move   a2,s0                ; Gun-Bone-Matrix
800348fc jal    FUN_80019700         ; Effekt-Spawn Rauch
80034900 _addiu a3,sp,0x10           ; Offsets aus Block 1 wiederverwendet (%6==0 => %3==0)
80034904 lui    v0,0x9249
; --- Block 3: alle 7 Ticks von acaf2 Huelsenauswurf ----------------------
LAB_80034908:                        ; XREF: 8003483c(j), 800348e0(j)
80034908 lui    s0,0x800b
8003490c addiu  s0,s0,-0x350e        ; s0 = &DAT_800ACAF2 (freilaufender Feuertakt-Zaehler)
80034910 lbu    v1,0x0(s0)
80034914 ori    v0,v0,0x2493         ; v0 = 0x92492493 (Magic: signed /7)
80034918 sll    v1,v1,0x18
8003491c sra    a0,v1,0x18           ; a0 = (s8)acaf2
80034920 mult   a0,v0
80034924 sra    v1,v1,0x1f
80034928 mfhi   v0
8003492c addu   v0,v0,a0
80034930 sra    v0,v0,0x2
80034934 subu   v0,v0,v1             ; v0 = a0/7 (signed)
80034938 sll    v1,v0,0x3
8003493c subu   v1,v1,v0             ; v1 = (a0/7)*7
80034940 subu   a0,a0,v1             ; a0 = a0 % 7
80034944 sll    a0,a0,0x18
80034948 bne    a0,zero,LAB_800349ac ; (acaf2 % 7) != 0 -> keine Huelse
8003494c _lui   a0,0x400
80034950 ori    a0,a0,0x800          ; a0 = 0x04000800  HUELSENAUSWURF (fx-Id 4, Scale 0x800)
80034954 addiu  a3,sp,0x10
80034958 lui    a1,0x800b
8003495c lh     a1,DAT_800acabe      ; Yaw
80034960 lui    a2,0x800b
80034964 lw     a2,DAT_800acbdc
80034968 ori    v0,zero,0x64
8003496c sw     v0,0x14(sp)          ; Y = 0x64 (100), charakterunabhaengig
80034970 lui    v0,0x800b
80034974 lbu    v0,DAT_800aca5c      ; Charakter-Byte
80034978 addiu  a2,a2,0x7a4          ; a2 = Gun-Bone-Matrix
8003497c andi   v0,v0,0x4
80034980 sltu   v0,zero,v0           ; elza (0/1)
80034984 sll    v1,v0,0x2
80034988 addu   v1,v1,v0
8003498c sll    v1,v1,0x3            ; v1 = elza*40
80034990 ori    v0,zero,0xc8
80034994 subu   v0,v0,v1             ; X = 0xC8(200) - elza*40 -> 200/160
80034998 sw     v0,0x10(sp)          ; ofs.x
8003499c li     v0,-0x14
800349a0 subu   v0,v0,v1             ; Z = -0x14(-20) - elza*40 -> -20/-60
800349a4 jal    FUN_80019700         ; Effekt-Spawn Huelse
800349a8 _sw    v0,0x18(sp)          ; ofs.z
; --- Block 4: Takt-Kern: Zaehler++, Munition, Schaden --------------------
LAB_800349ac:                        ; XREF: 80034948(j)
800349ac lbu    v0,0x0(s0)           ; v0 = DAT_800acaf2
800349b0 lui    v1,0x800b
800349b4 lbu    v1,DAT_800acae9      ; v1 = In-Clip-Bildzaehler
800349b8 addiu  v0,v0,0x1
800349bc andi   v1,v1,0x4            ; Munitions-Gate: Bit 2 des Clip-Frames
800349c0 bne    v1,zero,LAB_80034a14 ; Bit gesetzt (Frames 4-7 je 8er-Fenster) -> nichts
800349c4 _sb    v0,0x0(s0)           ; DAT_800acaf2++ IMMER (Delay-Slot)
800349c8 jal    FUN_8004eae4         ; Munition -1 (ret 0 = Magazin leer)
800349cc _nop
800349d0 beq    v0,zero,LAB_800349fc ; leer -> Leer-Zweig
800349d4 _ori   v0,zero,0x2          ; (Delay: v0=2, wird im Leer-Zweig zu aca5b)
800349d8 lui    v0,0x800b
800349dc lw     v0,DAT_800ac784      ; Spieler-Entity
800349e0 lui    a0,0x800b
800349e4 lbu    a0,DAT_800aca5d      ; a0 = angelegte Waffen-Id (12)
800349e8 lw     a1,0x7c(v0)          ; a1 = [entity+0x7C]
800349ec jal    FUN_80011f50         ; SCHADEN pro verbrauchter Patrone
800349f0 _nop
800349f4 j      LAB_80034a14
800349f8 _nop
; --- Block 5: Magazin leer -----------------------------------------------
LAB_800349fc:                        ; XREF: 800349d0(j)
800349fc lui    at,0x800b
80034a00 sb     v0,DAT_800aca5b      ; innere Phase = 2 -> Feuer-Ende (Clip 8/11/14)
80034a04 lui    a0,0x101
80034a08 ori    a0,a0,0x1            ; a0 = 0x01010001 (SE gepackt, Sample-Id 1)
80034a0c jal    FUN_80045024         ; Se_on-Kern: Leerklick-SE
80034a10 _addiu a1,s0,-0x6a          ; a1 = &DAT_800ACA88 (Spieler-Weltposition, 3D-Pan)
; --- Epilog ---------------------------------------------------------------
LAB_80034a14:                        ; XREF: 800349c0(j), 800349f4(j)
80034a14 lw     ra,0x2c(sp)
80034a18 lw     s2,0x28(sp)
80034a1c lw     s1,0x24(sp)
80034a20 lw     s0,0x20(sp)
80034a24 addiu  sp,sp,0x30
80034a28 jr     ra
80034a2c _nop
```

## 3. Pseudo-C

```c
/* Entlade-Handler Ingram M10 (Waffe 12). Laeuft 1x pro Frame in FSM-Substate
 * DAT_800aca5a==2 (Feuer-Loop-Sub LAB_80034510), innere Phase DAT_800aca5b in {0,1}.
 * Beim Eintritt in Substate 2: acae9=0 @0x8003441c-20, acaf2=0 @0x80034424-28. */
void ingram_unload_800347f8(void)
{
    s32 ofs[3];                                    /* sp+0x10/14/18 = x/y/z rel. Gun-Bone */
    int elza;

    if (DAT_800acae9 % 3 == 0) {                   /* @0x80034818-3c */
        elza  = (DAT_800aca5c & 4) ? 1 : 0;        /* @0x80034864-74: 0=Leon, 1=Elza */
        ofs[0] = 0x46  - elza*10;                  /* @0x80034884-98  x: 70 / 60     */
        ofs[1] = 0x41A - elza*30;                  /* @0x8003489c-a4  y: 1050 / 1020 */
        ofs[2] = -0x3C - elza*30;                  /* @0x800348a8-b4  z: -60 / -90   */
        FUN_80019700(0x02010800, (s16)DAT_800acabe,        /* Muendungsfeuer @0x80034844-48 */
                     DAT_800acbdc + 0x7a4, ofs);           /* Gun-Bone @0x80034868 */
        if (DAT_800acae9 % 6 == 0)                 /* @0x800348b8-e0 */
            FUN_80019700(0x03000B00, (s16)DAT_800acabe,    /* Rauch @0x800348e8-ec */
                         DAT_800acbdc + 0x7a4, ofs);       /* gleiche Offsets */
    }
    if ((s8)DAT_800acaf2 % 7 == 0) {               /* @0x80034910-48 (signed /7) */
        elza  = (DAT_800aca5c & 4) ? 1 : 0;        /* @0x80034974-80 */
        ofs[0] = 0xC8  - elza*40;                  /* @0x80034984-98  x: 200 / 160 */
        ofs[1] = 0x64;                             /* @0x80034968-6c  y: 100       */
        ofs[2] = -0x14 - elza*40;                  /* @0x8003499c-a8  z: -20 / -60 */
        FUN_80019700(0x04000800, (s16)DAT_800acabe,        /* Huelse @0x8003494c-50 */
                     DAT_800acbdc + 0x7a4, ofs);
    }
    DAT_800acaf2++;                                /* @0x800349c4 (Delay-Slot, IMMER) */

    if ((DAT_800acae9 & 4) == 0) {                 /* @0x800349b4-c0: Clip-Frames 0-3 je 8 */
        if (FUN_8004eae4() != 0) {                 /* Munition -1 @0x800349c8 */
            FUN_80011f50(DAT_800aca5d,             /* Schaden PRO Patrone @0x800349ec */
                         *(u32*)(DAT_800ac784 + 0x7c));   /* @0x800349d8-e8 */
        } else {                                   /* Magazin leer */
            DAT_800aca5b = 2;                      /* @0x800349fc-a00: Phase 2 = Feuer-Ende */
            FUN_80045024(0x01010001, &DAT_800aca88); /* @0x80034a04-10: Leerklick-SE, 3D-Pan */
        }
    }
}
```

## 4. Konstanten-Tabelle

| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x80034844-48 | 0x02010800 | Effekt Muendungsfeuer: fx-Id 2, Sub 1, Scale 0x800 (Pistole nutzt 0x02000800 → Sub 1 = Dauerfeuer-Variante) |
| 0x8003483c | %3 | Muendungsfeuer nur wenn `DAT_800acae9 % 3 == 0` (Magic 0xAAAAAAAB @0x80034800/1c) |
| 0x80034884-b4 | 70/1050/-60 bzw. 60/1020/-90 | Muendungs-Offset x/y/z am Gun-Bone; −10/−30/−30 wenn Elza (`DAT_800aca5c&4` @0x80034870) |
| 0x800348e8-ec | 0x03000B00 | Effekt Rauch: fx-Id 3, Scale 0xB00 (Pistole: 0x03000C00) |
| 0x800348e0 | %6 | Rauch nur wenn `DAT_800acae9 % 6 == 0` (Offsets aus dem %3-Block wiederverwendet) |
| 0x8003494c-50 | 0x04000800 | Effekt Huelsenauswurf: fx-Id 4, Scale 0x800 (identisch zum Pistolen-Pfad) |
| 0x80034914/48 | %7 | Huelse nur wenn `(s8)DAT_800acaf2 % 7 == 0` (Magic 0x92492493, signed) |
| 0x80034968-a8 | 200/100/-20 bzw. 160/100/-60 | Huelsen-Offset x/y/z; −40/0/−40 wenn Elza |
| 0x800349bc | & 4 | Munitions-/Schadens-Gate: nur Clip-Frames mit `(DAT_800acae9 & 4)==0` (= 4 Patronen je 8 Clip-Frames) |
| 0x800349c4 | +1 | `DAT_800acaf2++` jeden Handler-Frame (Delay-Slot, unbedingt) |
| 0x800349d4 / 0x80034a00 | 2 | Bei leerem Magazin: innere Phase `DAT_800aca5b = 2` (Feuer-Ende-Clip 8/11/14 via LAB_80034764) |
| 0x80034a04-08 | 0x01010001 | SE-Wort fuer FUN_80045024 (Se_on-Kern; Byte3=Voice-Gate, Low16=Sample 1) — Leerklick |
| 0x80034a10 | &DAT_800aca88 | SE-Positionszeiger = Spieler-Weltposition (X@aca88/Z@aca90) fuer Distanz-Pan |
| 0x80034414-28 | Substate-2-Eintritt | `aca5a=2`, `acae9=0`, `acaf2=0` (aus Sub 1; Bedingung Pad-Held&0x40 @0x800343f8 + FUN_8004ea6c()!=0 = Munition>0 @0x80034404-0c) |
| 0x80034564-a8 | Clips 7/10/13 | Feuer-Loop-Clip = 7 + 3*(acaec bit15, hoch) + 6*(acaec bit13, tief); `acae3=7` |
| 0x80034764-b4 | Clips 8/11/14 | Feuer-Ende-Clip = 8 + 3*bit15 + 6*bit13; `acae9=0`, `acae3=7` |
| 0x8003460c-44 | ±0x18 | Yaw-Schwenk `DAT_800acabe ∓ 24`/Frame bei Pad-Held Bit 0x8/0x2 waehrend Dauerfeuer |
| 0x800345ac-b4 | 0x5A | `DAT_800acc24 = 90` jeden Feuer-Frame (Leser NICHT gefunden, s. §6) |
| 0x800345c8-d4 | \|=1 | `DAT_800aca52 \|= 1` jeden Feuer-Frame (Schuss-Latch; Leser u.a. Kraehen-KI @0x80112700, Port `g_aca52_flags`) |

## 5. Belege fuer Fremd-Funktionen (nicht neu hergeleitet)

- FUN_80019700 = Effekt-Spawn (a0=Code, a1=Basis-Yaw-Seed, a2=Bone-Matrix, a3=Offset-Triple):
  Auftrags-Kontext + `analysis/bite_blood_fx.md` (a1→Slot+0x2e) + Port-Feuerpfad
  `game_step_common.c:1349/1428` (fx-Id-Encoding Id/Sub/Scale, fx4=Huelse).
  ⚠️ `RE15_FUN_CATALOG.md` beschreibt FUN_80019700 abweichend als Enemy-Hitbox-Setup —
  Diskrepanz dort klaeren, hier nicht aufgeloest.
- fx-Id-Semantik 2=Muendungsfeuer / 3=Rauch / 4=Huelse: `player_common.c:123` (Pistolen-Pfad,
  0x02000800/0x03000C00/0x04000800).
- FUN_8004eae4 = Munition−1 (ret 0=leer): Auftrags-Kontext; Kopf @0x8004eae4 liest denselben
  Inventar-Slot `DAT_800b25c8` wie FUN_8004ea6c.
- FUN_8004ea6c = Munitionsstand des angelegten Slots > 0 (selbst gelesen: @0x8004ea74 `lbu
  DAT_800b25c8`, @0x8004eacc `lbu DAT_800b10ad[slot*4]`, @0x8004ead4 `sltu v0,zero,count`).
- FUN_80011f50 = Schadens-Resolver (a0=Waffen-Id, a1=[entity+0x7c]): Auftrags-Kontext.
- FUN_80045024 = Se_on-Kern: `RE15_FUN_CATALOG.md` (LAB_80041624 packt (bank<<24)|(vol<<16)|id,
  Voice-Gate `if 0x20<v return` in FUN_80045024, L/R-Pan via FUN_80045a64).
- DAT_800acabe = Spieler-Yaw (Entity+0x6a): `player_common.c:37/459`, `analysis/bite_blood_fx.md`.
- DAT_800aca5c = Charakter/Modell-Byte, Bit2: 0=Leon/4=Elza: `platform/pc/main.c:1671/936`,
  `enemy_ai_re2_zombie.c:1742`.
- DAT_800aca88/90 = Spieler-Welt-X/Z: `RE15_FUN_CATALOG.md` (FUN_8011d6d4).
- DAT_800acae3 = Anim-Crossfade-Byte (Entity+0x8f): `climb_common.c:47`.
- FUN_8001f314 = Keyframe-Anim-Advance: `RE15_FUN_CATALOG.md`.
- RE2-Fallback war fuer diese Funktion NICHT noetig (alle Aufrufe/Globals in RE1.5 belegbar).

## 6. Offen / nicht klaerbar

1. **DAT_800acc24** (Seed 0x5A=90 je Feuer-Frame @0x800345b0-b4 und @0x800334f4-f8; genullt
   @0x8003198c): im gesamten EXE-Dump nur 3 Schreib-Xrefs, KEIN Leser; auch
   `RE_15_Quellcode_Overlays/` und `RE_15_Quellcode_V2/` ohne Treffer. Zweck unbelegt
   (Vermutung "Schuss-gehoert-Timer" waere Raten — nicht uebernommen).
2. Klang-Identitaet von SE 0x01010001 (welches VAB-Sample konkret): funktional Leerklick
   (laeuft NUR im Leer-Zweig @0x800349fc), Sample-Inhalt nicht verifiziert.
3. Physische Tasten hinter den virtuellen Held-Bits 0x2/0x8/0x10/0x20/0x40/0x100 in
   DAT_800ac768 (virtuelles Held-Wort, Mapping @0x8003051c): Registerwirkung exakt belegt,
   Button-Namen hier nicht verifiziert.
4. Katalog-Diskrepanz FUN_80019700 (s. §5).
