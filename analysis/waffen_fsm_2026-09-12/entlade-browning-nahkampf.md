# Entlade-Handler Browning HP (W3/W4) + die "Nahkampf-Eintraege" W0/W1/W2

Gruppe: `0x800337BC` (Browning-Entlade), `0x80033C74`, `0x80033D7C`, `0x80033EEC`.
Quelle: `ghidra1_V2.txt` (RE1.5 PSX.EXE, Zeilen 128355-129570 und 242195-242250), Tabellen-Rohbytes
im Dump verifiziert. RE2-Fallback war NICHT noetig - alles aus RE1.5 belegbar.

---

## 0. KERNERGEBNIS: Zwei ueberlappende Tabellen - die "Doppelrolle" ist geklaert

Der Standard-Gun-Handler `0x80032E9C` dispatcht **NICHT** ueber 0x80074100, sondern ueber
**0x800740F4 + Substate*4** (Substate = `DAT_800aca5a`):

```
80032ea0 lbu v0,DAT_800aca5a          ; FSM-Substate
80032eac sll v0,v0,0x2
80032eb0 lui at,0x8007
80032eb4 addiu at,at,0x40f4           ; Tabellenbasis 0x800740F4  <-- Substate-Tabelle!
80032ebc lw  v0,0x0(at)=>PTR_LAB_800740f4
80032ec4 jalr v0
```

Tabelle @0x800740F4 (Rohbytes im Dump, Zeile 242195ff):

| Substate | Adresse  | Eintrag      | Rolle                                  |
|----------|----------|--------------|----------------------------------------|
| 0 | 0x800740F4 | `0x80032F18` | RAISE (mit Phasen ueber DAT_800aca5b)  |
| 1 | 0x800740F8 | `0x80033180` | HOLD                                   |
| 2 | 0x800740FC | `0x80033460` | FIRE/DISCHARGE                         |
| 3 | 0x80074100 | `0x80033C74` | **LOWER**                              |
| 4 | 0x80074104 | `0x80033D7C` | **RELOAD**                             |
| 5 | 0x80074108 | `0x80033EEC` | **L1-RETARGET**                        |

Die **Entlade-Tabelle** hat Basis **0x80074100** und wird mit der **Waffen-Id** (`DAT_800aca5d`)
indiziert - von genau ZWEI Stellen (Ghidra-XREF an `PTR_LAB_80074100`: `800334d8(*), 800345e4(*)`):

```
; Leser 1 - im Standard-FIRE-Sub 0x80033460 (erster Frame von Sub2):
80033490 lbu v1,DAT_800aca5d           ; Waffen-Id
800334c0 sll v1,v1,0x2
800334cc lui at,0x8007
800334d0 addiu at,at,0x4100            ; Basis 0x80074100
800334d8 lw  v0,0x0(at)                ; entlade[waffen_id]
800334e0 jalr v0                       ; EINMAL je Schuss (Phase-Latch DAT_800aca5b @0x80033470-80)

; Leser 2 - im Dauerfeuer-Sub2 0x80034510, identisches Muster:
800345c4 lbu v1,DAT_800aca5d
800345cc sll v1,v1,0x2
800345d8/dc lui/addiu at,0x80074100
800345e4 lw  v0 ; 800345ec jalr v0
```

**Folge:** Die Slots [0..2] der Entlade-Tabelle (0x80074100/104/108) sind IDENTISCH mit den
Substates 3/4/5 der FSM-Tabelle - eine reine **Speicher-Ueberlappung**. Waffen 0-2 (Nahkampf,
Dispatch @0x80074030 -> `0x80034E70`) erreichen die beiden Entlade-Leser NIE, denn die laufen nur
in der Standard- bzw. Dauerfeuer-Maschine. Es gibt KEINE weiteren Xrefs auf 0x80033C74/0x80033D7C/
0x80033EEC ausser der Tabelle (`XREF[1]: 80074100(*)` / `80074104(*)` / `80074108(*)`).

> **Antwort auf die Auftrags-Frage:** `0x80033D7C` ist AUSSCHLIESSLICH der RELOAD-Substate (Sub4)
> der Standard-Gun-FSM. Sein Auftauchen als "Entlade [W1]" ist ein totes Alias durch die
> Tabellen-Ueberlappung; als Waffen-Entlade wird der Slot nie aufgerufen. Dito 0x80033C74 ("[W0]")
> = Sub3 LOWER und 0x80033EEC ("[W2]") = Sub5 RETARGET. Die Port-Benennung ist korrekt.

**Dasselbe Muster nochmal:** Die Nahkampf-Maschine `0x80034E70` dispatcht ueber Basis
**0x80074164** (= 0x80074150 + 0x14):

```
80034eb8 sll v0,v0,0x2                 ; v0 = DAT_800aca5a
80034ebc lui at,0x8007
80034ec0 addiu at,at,0x4164            ; Basis 0x80074164!
80034ec8 lw  v0 ; 80034ed0 jalr
```

Die "10-Sub-Dauerfeuer-Tabelle" @0x80074150 ist also in Wahrheit **Dauerfeuer-Subs 0..4**
(0x80034060/0x80034278/0x80034510/0x80034C74/0x80034D68, Basis 0x80074150) ueberlappt mit
**Nahkampf-Subs 0..4** (0x80034EE8/0x800350C4/0x80035314/0x80035424/0x80035538, Basis 0x80074164).
Das erklaert byte-genau die Port-Aussage "sub 3, BOTH machines - gun @0x80033c74 / melee
@0x80035424". (Nahkampf-Kopf-Detail: bei Substate 0 und `!(DAT_800aca54 & 0x4000)` wird sofort
Substate 4 gesetzt, `sb 4` @0x80034ea8.)

Randnotiz Entlade[20] = 0x80034060: 0x80074100 + 20*4 = **0x80074150** = erster Eintrag der
Dauerfeuer-Tabelle - noch eine Ueberlappung; Waffen-Id 20 wuerde als "Entlade" den
Dauerfeuer-Sub0 anspringen. Slots [15..18] sind NULL (Rohbytes 00 @0x8007413C-4B).

---

## 1. `0x800337BC` - Entlade-Handler Waffen 3/4 (Browning HP), 0x800337BC-0x800338A7

Aufrufer: `jalr` @0x800334E0 (Standard-FIRE Sub2, 1x je Schuss am ersten Frame) - fuer W3/W4
der einzige lebende Pfad. Tabellen-Beleg: Rohbytes `bc 37 03 80` @0x8007410C UND @0x80074110.

Umfeld des Aufrufs (Sub2 @0x80033460, erster Frame): Feuer-Clip = `7 + 2*Bit15 + 4*Bit13` des
Aim-Worts `DAT_800acaec` -> **7/9/0xB** (@0x800334A8-BC), `DAT_800acae8`=Clip @0x800334C8,
`DAT_800acae9`=0 @0x8003349C, `DAT_800acae3`=7 @0x800334A4; nach der Rueckkehr:
`DAT_800acc24`=0x5A (Laerm-Timer 90) @0x800334F8, `DAT_800aca52 |= 1` @0x80033504.

### Roh-Listing mit Annotation

```
800337bc addiu sp,sp,-0x30
800337c0 lui   a0,0x200          ; \ Effekt-Code a0 = 0x02000800  (MUENDUNGSFEUER, Familie 2)
800337c4 ori   a0,a0,0x800       ; /
800337c8 lui   a1,0x800b
800337cc lh    a1,DAT_800acabe   ; a1 = Spieler-Yaw (Entity+0x6a, signed)
800337d0 addiu a3,sp,0x10        ; a3 = &Offset-Tripel auf dem Stack
800337d4 sw    s0,0x20(sp)
800337d8 lui   s0,0x800b
800337dc lw    s0,DAT_800acbdc   ; s0 = Posebuffer-Basis
800337e0 ori   v0,zero,0x8c      ; X-Offset 0x8c (140)
800337e4 sw    v0,0x10(sp)
800337e8 ori   v0,zero,0x25d     ; Y-Offset 0x25d (605)
800337ec sw    ra,0x28(sp)
800337f0 sw    s1,0x24(sp)
800337f4 sw    v0,0x14(sp)
800337f8 sw    zero,0x18(sp)     ; Z-Offset 0
800337fc addiu s0,s0,0x7a4       ; s0 = Gun-Bone-Matrix (Posebuffer+0x7a4)
80033800 jal   FUN_80019700      ; Effekt-Spawn #1: MUZZLE (0x02000800, yaw, gunbone, {0x8c,0x25d,0})
80033804 _move a2,s0
80033808 lui   a0,0x300          ; \ Effekt-Code a0 = 0x03000C00  (RAUCH, Familie 3)
8003380c ori   a0,a0,0xc00       ; /
80033810 move  a2,s0             ; wieder Gun-Bone-Matrix
80033814 addiu a3,sp,0x10
80033818 ori   s1,zero,0x91      ; X-Offset 0x91 (145) - s1 wird fuer Spawn #3 WIEDERVERWENDET
8003381c lui   a1,0x800b
80033820 lh    a1,DAT_800acabe   ; yaw
80033824 ori   v0,zero,0x1f4     ; Y-Offset 0x1f4 (500)
80033828 sw    v0,0x14(sp)
8003382c li    v0,-0x19          ; Z-Offset -0x19 (-25)
80033830 sw    s1,0x10(sp)
80033834 jal   FUN_80019700      ; Effekt-Spawn #2: SMOKE (0x03000c00, yaw, gunbone, {0x91,0x1f4,-25})
80033838 _sw   v0,0x18(sp)
8003383c lui   a0,0x400          ; \ Effekt-Code a0 = 0x04000800  (HUELSE, Familie 4)
80033840 ori   a0,a0,0x800       ; /
80033844 move  a2,s0
80033848 addiu a3,sp,0x10
8003384c lui   a1,0x800b
80033850 lh    a1,DAT_800acabe   ; yaw
80033854 ori   v0,zero,0x109     ; Y-Offset 0x109 (265)
80033858 sw    v0,0x14(sp)
8003385c li    v0,-0x32          ; Z-Offset -0x32 (-50)
80033860 sw    s1,0x10(sp)       ; X-Offset 0x91 (aus s1)
80033864 jal   FUN_80019700      ; Effekt-Spawn #3: SHELL EJECT (0x04000800, yaw, gunbone, {0x91,0x109,-50})
80033868 _sw   v0,0x18(sp)
8003386c lui   v0,0x800b
80033870 lw    v0,DAT_800ac784   ; Spieler-Entity-Zeiger
80033874 lui   a0,0x800b
80033878 lbu   a0,DAT_800aca5d   ; a0 = Waffen-Id
8003387c lw    a1,0x7c(v0)       ; a1 = entity+0x7c (Hitbox-Kontext)
80033880 jal   FUN_80011f50      ; SCHADENS-RESOLVER (Waffen-Id, entity+0x7c)
80033884 _nop
80033888 jal   FUN_8004eae4      ; MUNITION -1 (Rueckgabe hier UNGEPRUEFT)
8003388c _nop
80033890 lw    ra,0x28(sp)
80033894 lw    s1,0x24(sp)
80033898 lw    s0,0x20(sp)
8003389c addiu sp,sp,0x30
800338a0 jr    ra
800338a4 _nop
```

### Pseudo-C

```c
void browning_discharge(void)   /* Entlade[3]/[4] @0x800337BC */
{
    s16 yaw     = DAT_800acabe;                 /* Entity+0x6a */
    void *gunbn = *DAT_800acbdc + 0x7a4;        /* Gun-Bone-Matrix */
    FUN_80019700(0x02000800, yaw, gunbn, (s32[3]){0x8c, 0x25d,   0});  /* Muendungsfeuer */
    FUN_80019700(0x03000C00, yaw, gunbn, (s32[3]){0x91, 0x1f4, -25});  /* Rauch */
    FUN_80019700(0x04000800, yaw, gunbn, (s32[3]){0x91, 0x109, -50});  /* Huelse */
    FUN_80011f50(DAT_800aca5d, (*DAT_800ac784)[0x7c/4]);               /* Schaden */
    FUN_8004eae4();                                                    /* Ammo -1 */
}
```

### Port-Verifikation (Auftrag) - ALLE Behauptungen BESTAETIGT

| Port-Behauptung (game_step_common.c ~1346ff) | Dump-Beleg | Ergebnis |
|---|---|---|
| Effekt-Id 2 (Muzzle), Offsets {0x8c,0x25d,0}, "@0x800337e0" | a0=0x02000800 @0x800337C0/C4; 0x8c @0x800337E0; 0x25d @0x800337E8; z=0 @0x800337F8 | KORREKT |
| Effekt-Id 3 (Smoke), {0x91,0x1f4,-25}, "@0x80033818" | a0=0x03000C00 @0x80033808/0C; 0x91 @0x80033818; 0x1f4 @0x80033824; -0x19 @0x8003382C | KORREKT |
| Effekt-Id 4 (Shell), {0x91,0x109,-50}, "@0x8003383c" | a0=0x04000800 @0x8003383C/40; 0x91 via s1 @0x80033860; 0x109 @0x80033854; -0x32 @0x8003385C | KORREKT |
| "NO direct shot SE in the discharge path" | Vollstaendiges Listing enthaelt NUR 3x FUN_80019700 + FUN_80011f50 + FUN_8004eae4 - kein FUN_80045024/Se_on | KORREKT |
| Huelse INLINE beim Schuss (kein Recoil-Watcher) | Spawn #3 @0x8003383C-68 laeuft im selben Aufruf wie Muzzle/Smoke | KORREKT |
| Anker = Gun-Bone-Matrix Posebuffer+0x7a4 | lw DAT_800acbdc @0x800337DC + addiu 0x7a4 @0x800337FC | KORREKT |

**SE/Huelse (Auftrags-Punkt "annotiere den Rest"):** Der Handler enthaelt KEINEN SE-Aufruf - der
Knall kommt datengetrieben aus den ESP-Rows des gespawnten Muzzle-Effekts (Row-VM Routine 9 ->
FUN_80045024(0x01000001,...), im Port bereits so modelliert). Die Huelse ist der DRITTE
Effekt-Spawn (Familie 4, Code 0x04000800) - es gibt keinen separaten Huelsen-Mechanismus.

### Konstanten-Tabelle

| Wert | Adresse | Bedeutung |
|---|---|---|
| 0x02000800 | 0x800337C0/C4 | Effekt-Code Muendungsfeuer (Familie 2) |
| 0x03000C00 | 0x80033808/0C | Effekt-Code Rauch (Familie 3) |
| 0x04000800 | 0x8003383C/40 | Effekt-Code Huelsenauswurf (Familie 4) |
| 0x8c / 0x25d / 0 | 0x800337E0 / E8 / F8 | Muzzle-Offset x/y/z (lokal zur Gun-Bone-Matrix) |
| 0x91 / 0x1f4 / -0x19 | 0x80033818 / 24 / 2C | Smoke-Offset x/y/z |
| 0x91 / 0x109 / -0x32 | 0x80033860(s1) / 54 / 5C | Shell-Offset x/y/z |
| +0x7a4 | 0x800337FC | Gun-Bone-Matrix im Posebuffer |
| +0x7c | 0x8003387C | entity+0x7c als a1 des Schadens-Resolvers |

---

## 2. `0x80033C74` - Standard-Gun-FSM **Sub3 = LOWER** (Waffe senken), 0x80033C74-0x80033D7B

Erreicht via FSM-Tabelle [3] @0x80074100. Einstiegs-Schreiber von Substate 3:
`sh 3 -> DAT_800aca5a` @0x80033200 (HOLD: R1 losgelassen) und @0x8003364C (FIRE-Ende ohne R1).

```
80033c74 addiu sp,sp,-0x18
80033c78/7c lui/addiu v1 = &DAT_800aca5b   ; Phasen-Latch
80033c84 lbu   v0,DAT_800aca5b
80033c8c bne   v0,zero,LAB_80033ccc        ; Latch gesetzt -> nur noch Tick-Teil
80033c90 _ori  v0,zero,0x1
   ; ---- erster Frame ----
80033c94 sb    v0,DAT_800aca5b             ; Latch = 1
80033c98 ori   v0,zero,0x6                 ; CLIP 6
80033ca0 sb    v0,DAT_800acae8             ; Motion-Byte (entity+0x94) = Clip 6 (Senken)
80033ca8 lhu   v0,DAT_800acaec             ; Aim-Wort
80033cac ori   v1,zero,0x7
80033cb4 sb    zero,DAT_800acae9           ; In-Clip-Bildzaehler = 0
80033cbc sb    v1,DAT_800acae3             ; Anim-Bank-Modus (entity+0x8f) = 7 (Waffen-Bank)
80033cc0 andi  v0,v0,0x1fff                ; Aim-Wort: Bits 13/14/15 loeschen (hoch/LEVEL/tief)
80033cc8 sh    v0,DAT_800acaec
   ; ---- jeder Frame: manuelles Drehen waehrend des Senkens ----
80033ccc lw    v1,DAT_800ac768              ; Pad-Halte-Wort
80033cd8 andi  v0,v1,0x8
80033cdc beq   v0,zero,LAB_80033d00
80033ce0 _andi v0,v1,0x2
80033ce8 lhu   v0,DAT_800acabe              ; Bit3 gehalten:
80033cf0 addiu v0,v0,-0x18                  ;   yaw -= 0x18 (24)   <- Drehrate LOWER
80033cf8 sh    v0,DAT_800acabe
80033cfc andi  v0,v1,0x2
80033d00 beq   v0,zero,LAB_80033d20
80033d04 _ori  a2,zero,0x1                  ; (Delay-Slot: a2=1 fuer den f314-Call unten)
80033d0c lhu   v0,DAT_800acabe              ; Bit1 gehalten:
80033d14 addiu v0,v0,0x18                   ;   yaw += 0x18 (24)
80033d1c sh    v0,DAT_800acabe
   ; ---- Anim-Advance + Exit ----
80033d24 lw    a0,DAT_800acbc4              ; Anim-Daten
80033d2c lw    a1,DAT_800acbc8              ; Anim-State
80033d30 jal   FUN_8001f314                 ; Keyframe-Advance (a2=1, a3=0x200)
80033d34 _ori  a3,zero,0x200
80033d38 beq   v0,zero,LAB_80033d6c         ; 0 = Clip laeuft noch -> return
80033d3c _li   v1,-0x81
   ; ---- Clip fertig: Aim-Modus komplett verlassen ----
80033d44 lw    v0,DAT_800aca3c              ; Spieler-Flag-Wort
80033d4c sb    zero,DAT_800aca59            ; Control-Lock frei
80033d54 sh    zero,DAT_800aca5a            ; Substate=0 UND Latch(aca5b)=0 (Halbwort!)
80033d58 and   v0,v0,v1                     ; &= ~0x80
80033d5c li    v1,-0x41
80033d60 and   v0,v0,v1                     ; &= ~0x40  (zusammen: ~0xC0, gesetzt @0x80032fc0 ori 0xc0)
80033d68 sw    v0,DAT_800aca3c
80033d6c ... jr ra                          ; Epilog @0x80033d6c-78
```

**Pseudo-C:**
```c
void gun_sub3_lower(void) {
    if (!DAT_800aca5b) {                       /* erster Frame @0x80033c8c */
        DAT_800aca5b = 1;
        DAT_800acae8 = 6;                      /* Senken-Clip 6 @0x80033c98 */
        DAT_800acae9 = 0;                      /* @0x80033cb4 */
        DAT_800acae3 = 7;                      /* W-Bank-Modus @0x80033cbc */
        DAT_800acaec &= 0x1fff;                /* Elevation-Bits weg @0x80033cc0 */
    }
    if (DAT_800ac768 & 8) DAT_800acabe -= 0x18;   /* Drehen 24/Frame @0x80033cf0 */
    if (DAT_800ac768 & 2) DAT_800acabe += 0x18;   /* @0x80033d14 */
    if (FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 1, 0x200)) {  /* Clip zu Ende */
        DAT_800aca59 = 0;                      /* @0x80033d4c */
        *(u16*)&DAT_800aca5a = 0;              /* Substate+Latch = 0 @0x80033d54 */
        DAT_800aca3c &= ~0xC0;                 /* Aim-Flags weg @0x80033d58-60 */
    }
}
```

| Wert | Adresse | Bedeutung |
|---|---|---|
| Clip 6 | 0x80033C98 | Senken-Animation (W-Bank) |
| 7 | 0x80033CBC | entity+0x8f Anim-Bank-Modus = Waffen-Bank |
| 0x1fff | 0x80033CC0 | Aim-Wort-Maske: loescht Bits 13/14/15 |
| 0x18 | 0x80033CF0 / 0x80033D14 | Drehrate 24/Frame waehrend LOWER |
| a2=1 | 0x80033D04 | FUN_8001f314-Richtungsparameter (Katalog: fwd/rev) |
| ~0xC0 | 0x80033D3C+0x80033D5C | Flag-Clear in DAT_800aca3c (Set: ori 0xC0 @0x80032FC0) |

---

## 3. `0x80033D7C` - Standard-Gun-FSM **Sub4 = RELOAD**, 0x80033D7C-0x80033EEB

Erreicht via FSM-Tabelle [4] @0x80074104. Einstiegs-Schreiber: `sh 4 -> DAT_800aca5a`
@0x80033378 - das Leer+Press-Edge-Gate (Reserve vorhanden UND Waffen-Id < 9, `sltiu 0x9`
@0x80033368). **Doppelrolle: KEINE** - siehe §0.

```
80033d90 lbu   v0,DAT_800aca5b
80033d98 bne   v0,zero,LAB_80033ddc         ; Latch -> Tick-Teil
   ; ---- erster Frame ----
80033da0 sb    1,DAT_800aca5b
80033da4 ori   v0,zero,0xd                  ; CLIP 0xD (13) = Nachlade-Animation
80033dac sb    v0,DAT_800acae8
80033db4 lhu   v0,DAT_800acaec
80033dc0 sb    zero,DAT_800acae9            ; Bildzaehler = 0
80033dc8 sb    7,DAT_800acae3               ; W-Bank-Modus
80033dcc andi  v0,v0,0x1fff                 ; Elevation-Bits weg ...
80033dd0 ori   v0,v0,0x4000                 ; ... und Bit14 = LEVEL setzen
80033dd8 sh    v0,DAT_800acaec
   ; ---- jeder Frame: Drehen 24/Frame (identisch zu Sub3) ----
80033de0-80033e2c  DAT_800ac768&8 -> yaw-=0x18 @0x80033e00 ; &2 -> yaw+=0x18 @0x80033e24
   ; ---- SONDERFALL Waffe 7 (Super Redhawk): Speedloader-Abwurf bei Bild 10 ----
80033e34 lbu   v1,DAT_800aca5d              ; Waffen-Id
80033e38 ori   v0,zero,0x7
80033e3c bne   v1,v0,LAB_80033e94           ; nur Waffe 7
80033e40 _clear a2                          ; (a2=0 fuer f314 unten)
80033e48 lbu   v1,DAT_800acae9              ; In-Clip-Bildzaehler
80033e4c ori   v0,zero,0xa
80033e50 bne   v1,v0,LAB_80033e94           ; nur bei Bild 10
80033e54 _lui  a0,0x406
80033e58 ori   a0,a0,0x800                  ; Effekt-Code 0x04060800 (Familie 4, Sub 6 = Speedloader)
80033e5c addiu a3,sp,0x10
80033e64 lh    a1,DAT_800acabe              ; yaw
80033e6c lw    a2,DAT_800acbdc
80033e70 ori   v0,zero,0x91                 ; X 0x91
80033e74 sw    v0,0x10(sp)
80033e78 ori   v0,zero,0x1f4                ; Y 0x1f4
80033e7c sw    v0,0x14(sp)
80033e80 li    v0,-0x19                     ; Z -0x19
80033e84 sw    v0,0x18(sp)
80033e88 jal   FUN_80019700                 ; Spawn (0x04060800, yaw, gunbone+0x7a4, {0x91,0x1f4,-25})
80033e8c _addiu a2,a2,0x7a4
80033e90 clear a2
   ; ---- Anim-Advance + Reload-Abschluss ----
80033e9c lw    a0,DAT_800acbc4
80033ea4 lw    a1,DAT_800acbc8
80033ea8 jal   FUN_8001f314                 ; (a2=0, a3=0x200)
80033eb0 beq   v0,zero,LAB_80033ed8         ; Clip laeuft noch -> return
80033eb4 _ori  v0,zero,0x1
80033ebc sh    v0,DAT_800aca5a              ; Substate = 1 (HOLD), Latch = 0 (Halbwort 0x0001)
80033ec0 jal   FUN_8004ebdc                 ; MAGAZIN AUFFUELLEN (Reserve -> Clip)
80033ec8 lui   a0,0x103
80033ecc ori   a0,a0,0x1                    ; SE-Code 0x01030001 = ARMS-Bank Record 3 (Reload-SE)
80033ed0 jal   FUN_80045024                 ; Se_on-Player (positional)
80033ed4 _addiu a1,s0,-0x13c                ; a1 = &DAT_800aca88 (Spieler-Positionsblock)
80033ed8 ... jr ra                          ; Epilog @0x80033ed8-e8
```

**Pseudo-C:**
```c
void gun_sub4_reload(void) {
    if (!DAT_800aca5b) {                                   /* @0x80033d98 */
        DAT_800aca5b = 1;
        DAT_800acae8 = 0x0d;                               /* Reload-Clip 13 @0x80033da4 */
        DAT_800acae9 = 0;  DAT_800acae3 = 7;
        DAT_800acaec = (DAT_800acaec & 0x1fff) | 0x4000;   /* Elevation -> LEVEL @0x80033dcc-d0 */
    }
    if (DAT_800ac768 & 8) DAT_800acabe -= 0x18;            /* @0x80033e00 */
    if (DAT_800ac768 & 2) DAT_800acabe += 0x18;            /* @0x80033e24 */
    if (DAT_800aca5d == 7 && DAT_800acae9 == 10)           /* Waffe 7, Bild 10 @0x80033e3c/50 */
        FUN_80019700(0x04060800, DAT_800acabe,
                     *DAT_800acbdc + 0x7a4, (s32[3]){0x91, 0x1f4, -25});
    if (FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 0, 0x200)) {
        *(u16*)&DAT_800aca5a = 1;                          /* -> HOLD @0x80033ebc */
        FUN_8004ebdc();                                    /* Refill @0x80033ec0 */
        FUN_80045024(0x01030001, &DAT_800aca88);           /* Reload-SE @0x80033ec8-d4 */
    }
}
```

| Wert | Adresse | Bedeutung |
|---|---|---|
| Clip 0xD | 0x80033DA4 | Nachlade-Animation (W-Bank) - Port-Behauptung "clip 0xD" KORREKT |
| 0x4000 | 0x80033DD0 | Aim-Wort Bit14 = Elevation LEVEL (wie Sub0-Init @0x80032F9C) |
| 0x18 | 0x80033E00 / 0x80033E24 | Drehrate 24/Frame waehrend RELOAD |
| 7 / 0xA | 0x80033E38 / 0x80033E4C | Waffe 7 (Super Redhawk), In-Clip-Bild 10 |
| 0x04060800 | 0x80033E54/58 | Effekt-Code Speedloader/Magazin-Abwurf |
| {0x91,0x1f4,-0x19} | 0x80033E70/78/80 | Abwurf-Offset (identisch zum Smoke-Tripel des Browning) |
| Substate 1 | 0x80033EBC | Rueckkehr in HOLD (sh 0x0001 loescht zugleich den Latch) |
| 0x01030001 | 0x80033EC8/CC | Reload-SE: ARMS-Bank Record 3 |
| &DAT_800aca88 | 0x80033ED4 | Positions-Arg des SE (s0-0x13c = 0x800acbc4-0x13c) |

---

## 4. `0x80033EEC` - Standard-Gun-FSM **Sub5 = L1-RETARGET**, 0x80033EEC-0x80034013

Erreicht via FSM-Tabelle [5] @0x80074108. Einstiegs-Schreiber: `sh 5 -> DAT_800aca5a`
@0x800332F8, Bedingung `DAT_800ac762 & 4` (Press-Edge-Wort, L1) im HOLD.

```
80033f00 lbu   v0,DAT_800aca5b
80033f08 bne   v0,zero,LAB_80033f84         ; Latch -> Dreh-Teil
80033f0c _ori  v0,zero,0x7
   ; ---- erster Frame ----
80033f14 lhu   v1,DAT_800acaec              ; Aim-Wort
80033f1c sb    v0,DAT_800acae3              ; W-Bank-Modus = 7
80033f24 sb    1,DAT_800aca5b               ; Latch
80033f2c sb    zero,DAT_800acae9            ; Bildzaehler = 0
80033f30 srl   v0,v1,0xf                    ; \
80033f34 sll   v0,v0,0x1                    ;  } Clip = 8 + 2*Bit15 + 4*Bit13
80033f38 addiu v0,v0,0x8                    ;  } -> 8 (level) / 10 (hoch) / 12 (tief)
80033f3c srl   v1,v1,0xb                    ;  }   = die HOLD-Clips
80033f40 andi  v1,v1,0x4                    ; /
80033f44 addu  v0,v0,v1
80033f4c sb    v0,DAT_800acae8
80033f50 jal   FUN_80037250                 ; NAECHSTES ZIEL waehlen (Arg 0x7530 wird IGNORIERT,
80033f54 _ori  a0,zero,0x7530               ;   a0 sofort ueberschrieben @0x80037250)
80033f5c sb    v0,DAT_800acaf3              ; Ziel-Latch-Byte
80033f60 sll   v0,v0,0x18
80033f64 bne   v0,zero,LAB_80033f84         ; Ziel gefunden -> Dreh-Teil
80033f68 _ori  v0,zero,0x101
   ; ---- KEIN Ziel: sofort zurueck in HOLD (Phase 1, Init uebersprungen) ----
80033f70 sh    v0,DAT_800aca5a              ; 0x0101: Substate=1, Latch(aca5b)=1
80033f78 sb    zero,DAT_800acae3            ; Anim-Bank-Modus = 0
80033f7c j     LAB_80034000                 ; return
   ; ---- Ziel vorhanden: jeder Frame drehen ----
80033f84 lui/addiu s0 = &DAT_800acbfc       ; Ziel-Zeiger-Slot
80033f8c lw    a0,DAT_800acbfc
80033f90 ori   a1,zero,0xc8
80033f94 jal   FUN_8001a8f8                 ; Yaw-Slew Richtung Ziel, Rate 0xC8 (200)/Frame
80033f98 _addiu a0,a0,0x34                  ;   (Ziel-Entity+0x34 = Positionsblock)
80033fa0 lw    v0,DAT_800ac768              ; Pad-Halte-Wort
80033fa8 andi  v0,v0,0x100
80033fac bne   v0,zero,LAB_80033fc8         ; R1 (0x100) noch gehalten?
80033fb0 _ori  a1,zero,0x64
   ; R1 losgelassen -> LOWER
80033fb4 ori   v0,zero,0x3
80033fbc sh    v0,DAT_800aca5a              ; Substate = 3 (LOWER), Latch = 0
80033fc0 j     LAB_80034000
   ; R1 gehalten: schon aufs Ziel ausgerichtet?
80033fc8 lw    a0,DAT_800acbfc
80033fcc jal   FUN_8001a9cc                 ; Arc-Test (Ziel-Entity+0x34, Kegel 0x64=100)
80033fd0 _addiu a0,a0,0x34
80033fd4 bne   v0,zero,LAB_80033fe8         ; !=0 = noch nicht im Kegel -> weiterdrehen
80033fd8 _clear a2
80033fdc ori   v0,zero,0x1
80033fe4 sh    v0,DAT_800aca5a              ; ausgerichtet: Substate = 1 (HOLD), Latch = 0
80033fe8 lw    a0,DAT_800acbc4
80033ff4 lw    a1,DAT_800acbc8
80033ff8 jal   FUN_8001f314                 ; Anim-Advance (Dreh-Clip laeuft weiter; a2=0, a3=0x200)
80033ffc _ori  a3,zero,0x200
80034000 ... jr ra                          ; Epilog @0x80034000-10
```

**Pseudo-C:**
```c
void gun_sub5_retarget(void) {
    if (!DAT_800aca5b) {                                       /* @0x80033f08 */
        DAT_800acae3 = 7;  DAT_800aca5b = 1;  DAT_800acae9 = 0;
        DAT_800acae8 = 8 + 2*(DAT_800acaec>>15)                /* HOLD-Clip 8/10/12 */
                         + ((DAT_800acaec>>11)&4);             /* @0x80033f30-44 */
        DAT_800acaf3 = FUN_80037250(/*arg ignoriert*/);        /* naechstes Ziel @0x80033f50 */
        if (!DAT_800acaf3) {                                   /* kein Ziel: */
            *(u16*)&DAT_800aca5a = 0x101;                      /*  HOLD, Phase 1 @0x80033f70 */
            DAT_800acae3 = 0;                                  /*  @0x80033f78 */
            return;
        }
    }
    FUN_8001a8f8(DAT_800acbfc + 0x34, 0xC8);                   /* zudrehen, 200/Frame @0x80033f94 */
    if (!(DAT_800ac768 & 0x100)) {                             /* R1 losgelassen @0x80033fa8 */
        *(u16*)&DAT_800aca5a = 3;  return;                     /* -> LOWER @0x80033fbc */
    }
    if (FUN_8001a9cc(DAT_800acbfc + 0x34, 0x64) == 0)          /* im +-100-Kegel @0x80033fcc */
        *(u16*)&DAT_800aca5a = 1;                              /* -> HOLD @0x80033fe4 */
    FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 0, 0x200);        /* @0x80033ff8 */
}
```

### Anhang: `FUN_80037250` (0x80037250-0x80037354) - "naechstes Ziel" fuer Sub5

Das Stack-Argument 0x7530 wird NICHT benutzt (a0 sofort ueberschrieben @0x80037250/54; anders als
`FUN_8003703c(radius)`, der Radius-Scan aus Sub0). Ablauf:
- Aktueller Ziel-Index = (`DAT_800acbfc` - 0x800acc2c) / 0x1F4 (Magic-Div 0x10624DD3 @0x8003725C/68,
  Gegner-Stride 0x1F4, Array-Basis `DAT_800acc2c`).
- Schleife ab Index+1, Wrap bei 0x14 (20 Slots, @0x800372B4) zurueck auf die Basis (@0x800372C4-C8);
  Budget = `lbu DAT_800aca4e` (@0x80037278, dekrementiert je Fehlschlag @0x8003730C).
- Kandidat gueltig wenn: Flags-Wort Bit0 gesetzt (@0x800372D4), HP `lh +0x9a` >= 0 (@0x800372E0-E8),
  `lbu +0x9 & 0x60 == 0` (@0x800372F0-F8).
- Treffer: `DAT_800acbfc` = &Slot (@0x8003734C), Rueckgabe Index+1; sonst 0.

| Wert | Adresse | Bedeutung |
|---|---|---|
| Clip 8/10/12 | 0x80033F30-44 | HOLD-Clips (level/hoch/tief) als Dreh-Pose |
| 0x7530 | 0x80033F54 | totes Argument (30000) - von FUN_80037250 ignoriert |
| 0x101 | 0x80033F68/70 | Substate 1 + Latch 1: HOLD ohne Neu-Init |
| 0xC8 | 0x80033F90 | Yaw-Slew-Rate 200/Frame Richtung Ziel |
| 0x100 | 0x80033FA8 | Pad-Bit R1 im Halte-Wort DAT_800ac768 |
| 0x64 | 0x80033FB0 | Arc-Test-Kegel +-100 (FUN_8001a9cc) |
| +0x34 | 0x80033F98/D0 | Ziel-Entity-Positionsblock |
| 0x14 / 0x1F4 | 0x800372B4 / 0x800372BC | 20 Gegner-Slots, Stride 500 |
| +0x9a / +0x9&0x60 | 0x800372E0 / F0-F8 | Ziel-Filter: HP>=0, Terminal-Flags frei |

---

## 5. FSM-Uebergaenge der Standard-Gun-Maschine (belegt)

| Von | Bedingung | Nach | Schreiber |
|---|---|---|---|
| Sub0 Phase2 | (Raise fertig) | 1 HOLD | sh @0x8003316C (v0=1 @0x80032F58) |
| HOLD | dpad-tief | 1, acaec=(&0xBFFF)\|0x2000 | @0x800330FC-0x8003310C |
| HOLD | dpad-hoch | 1, acaec\|=0x8000 | @0x8003314C-5C |
| HOLD | R1 losgelassen | 3 LOWER | sh 3 @0x80033200 |
| HOLD | L1-Edge (DAT_800ac762&4) | 5 RETARGET | sh 5 @0x800332F8 |
| HOLD | Square gehalten + Magazin>0 | 2 FIRE | sh 2 @0x80033328 |
| HOLD | leer + Edge + Reserve + Id<9 | 4 RELOAD | sh 4 @0x80033378 (sltiu 9 @0x80033368) |
| FIRE | Recoil-Clip fertig | 1 HOLD | sh 1 @0x8003367C (Auto-Refire-Schleife) |
| FIRE | (Zaehler-Bedingung, ohne R1) | 3 LOWER | sh 3 @0x8003364C |
| LOWER | Clip 6 fertig | 0 + Exit (aca59=0, aca3c&=~0xC0) | @0x80033D4C-68 |
| RELOAD | Clip 0xD fertig | 1 HOLD (+Refill+SE) | @0x80033EBC-D4 |
| RETARGET | kein Ziel | 1 HOLD (Phase 1) | sh 0x101 @0x80033F70 |
| RETARGET | R1 losgelassen | 3 LOWER | sh 3 @0x80033FBC |
| RETARGET | im Kegel 0x64 | 1 HOLD | sh 1 @0x80033FE4 |

Globals-Zuordnung (Spieler-Block-Basis 0x800aca54): `DAT_800acabe`=+0x6a Yaw,
`DAT_800acae0`=+0x8c Vorwaerts-Speed, `DAT_800acae3`=+0x8f Anim-Bank-Modus (64 Schreiber,
kein annotierter Leser - wird ueber den Entity-Zeiger gelesen), `DAT_800acae8`=+0x94 Motion-Clip,
`DAT_800acae9`=+0x95 Bildzaehler, `DAT_800acaec`=+0x98 Aim-Wort.
