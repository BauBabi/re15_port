# Dauerfeuer-FSM Sub 2 — 0x80034510 bis 0x80034C73 (RE1.5 PSX.EXE)

Gruppe: Sub 2 der Dauerfeuer-Zustandsmaschine (Tabelle @0x80074150, Eintrag [2] @0x80074158
→ LAB_80034510; Dispatcher 0x80034014 via `lbu DAT_800aca5a`).

Der Bereich enthält **drei** Funktionen:

| Funktion | Bereich | Rolle |
|---|---|---|
| `LAB_80034510` | 0x80034510–0x800347F4 | FSM **Sub 2 = FEUER-Schleife** (innere Substates 0..3 in `DAT_800aca5b`) |
| `LAB_800347F8` | 0x800347F8–0x80034A2C | **Entlade-Routine Waffe 12** (Ingram M10; Entlade-Tabelle @0x80074100[12] = XREF 0x80074130) |
| `0x80034A30` | 0x80034A30–0x80034C70 | **Entlade-Routine Waffe 19** (MC51; @0x80074100[19] = 0x8007414C). Im Ghidra-Dump NUR Rohbytes (`??`) — hier per `re15_disasm.py dis 0x80034a30` disassembliert. Bytes 0x80034C71–0x80034C73 = Padding vor Sub 3 (0x80034C74). |

Wichtige Vorab-Zuordnung (Spieler-Entity-Basis = 0x800ACA54, belegt über 0x800ACA88 = Spieler+0x34,
enemy_ai_common.c:7088; FUN_8001f314 liest `DAT_800ac784+0x94/+0x95`):

| Global | = Spieler-Feld | Bedeutung (Beleg) |
|---|---|---|
| DAT_800aca5a | +0x06 | Dauerfeuer-FSM-Substate (Dispatcher 0x80034014) |
| DAT_800aca5b | +0x07 | **innerer** Substate von Sub 2 (0..3), nur hier benutzt |
| DAT_800aca5c | +0x08 | Charakter-Byte (Leon 0/Elza 1; `&4`-Test @0x80104000-0C RE'd als Charakter-Zweig) |
| DAT_800aca5d | +0x09 | angelegte Waffen-Id |
| DAT_800acabe | +0x6a | Spieler-Yaw (Heading-Akkumulator; a1 der Effekt-Spawns) |
| DAT_800acae3 | +0x8f | FRAC-Crossfade-Zähler (von FUN_8001f314 dekrementiert; climb_common.c:47) |
| DAT_800acae8 | +0x94 | aktueller Clip-Index (FUN_8001f314 @0x8001f324 `lbu 0x94`) |
| DAT_800acae9 | +0x95 | In-Clip-Bildzähler (FUN_8001f314 POST-increment, Wrap bei Clip-Ende) |
| DAT_800acaec | +0x98 | Aim-Wort: Bit15=hoch, Bit13=tief, Bit14=neutral |
| DAT_800acaf2 | +0x9e | freilaufender Feuer-Frame-Zähler (nur Entlade-Routinen) |
| DAT_800aca88 | +0x34 | Spieler-Positionsvektor (a1 des Se_on-Klick) |
| DAT_800acc24 | +0x1d0 | „kürzlich gefeuert“-Byte, siehe §Unklar |

Virtuelles Pad-Wort `DAT_800ac768` (Preset 0 @0x80073dbc, FUN_80030444 @0x800304b8-e4;
pad_common.c): Bit 0x2=RECHTS, 0x8=LINKS, 0x10=HOCH(zielen), 0x20=RUNTER(zielen),
0x40=QUADRAT(=Anlegen-Taste), 0x100=R1(=Abzug).

---

## 1. LAB_80034510 — FSM Sub 2: die Dauerfeuer-Schleife

### 1.1 Roh-Listing (annotiert)

```
; --- Dispatch über inneren Substate DAT_800aca5b -------------------------------
80034510 addiu sp,sp,-0x18
80034514 lui   a0,0x800b
80034518 addiu a0,a0,-0x35a5          ; a0 = &DAT_800aca5b (innerer Substate)
8003451c sw    ra,0x10(sp)
80034520 lbu   v1,0x0(a0)             ; v1 = aca5b
80034524 ori   v0,zero,0x1
80034528 beq   v1,v0,LAB_800345ac     ; ==1 -> FEUER-Frame
8003452c _slti v0,v1,0x2
80034530 beq   v0,zero,LAB_80034548   ; >=2 -> weiter unten
80034534 _nop
80034538 beq   v1,zero,LAB_80034564   ; ==0 -> FEUER-EINSTIEG
8003453c _ori  v0,zero,0x1            ; (Delay: v0=1 fuer den sb im Case 0)
80034540 j     LAB_800347e8           ; sonst: return
80034548 ori   v0,zero,0x2
8003454c beq   v1,v0,LAB_80034764     ; ==2 -> RELEASE-EINSTIEG
80034550 _ori  v0,zero,0x3            ; (Delay: v0=3 fuer den sb im Case 2)
80034554 beq   v1,v0,LAB_800347bc     ; ==3 -> RELEASE-WARTEN
80034558 _clear a2                    ; (Delay: a2=0 fuer den f314-Call)
8003455c j     LAB_800347e8           ; sonst: return

; --- Case 0 @0x80034564: FEUER-EINSTIEG (Clip setzen) --------------------------
80034564 sb    v0,0x0(a0)             ; aca5b = 1
80034568/6c lhu a0,DAT_800acaec       ; a0 = Aim-Wort
80034570 ori   v0,zero,0x7
80034574/78 sb  v0,DAT_800acae3       ; +0x8f Crossfade = 7            <<< @0x80034570
8003457c srl   v0,a0,0xf              ; v0 = Bit15 (hoch) 0/1
80034580-88 v1 = v0*3 + 7             ; Clip-Basis 7, hoch: +3         <<< @0x80034588
8003458c-90 a0 = (aim&0x2000)!=0      ; Bit13 (tief) 0/1
80034594-a0 v1 += a0*6                ; tief: +6
800345a4/a8 sb  v1,DAT_800acae8       ; +0x94 Clip = 7|10|13 (FEUER level/hoch/tief)
;   ACHTUNG: DAT_800acae9 (+0x95 Frame) wird HIER NICHT genullt.
;   (faellt durch in Case 1)

; --- Case 1 @0x800345ac: FEUER-Frame -------------------------------------------
800345ac ori   v0,zero,0x5a
800345b0/b4 sb  v0,DAT_800acc24       ; +0x1d0 = 90 ("kuerzlich gefeuert", s. §Unklar)
800345b8/bc lhu v0,DAT_800aca52
800345c0/c4 lbu v1,DAT_800aca5d       ; v1 = Waffen-Id
800345c8 ori   v0,v0,0x1
800345cc sll   v1,v1,0x2
800345d0/d4 sh  v0,DAT_800aca52       ; aca52 |= 1  (Schuss-Latch; Kraehe liest es @0x8004f17c)
800345d8-e4 at = 0x80074100 + wid*4   ; ENTLADE-Tabelle
800345e4 lw    v0,0x0(at)             ; v0 = Entlade-Routine der Waffe
800345ec jalr  v0                     ; >>> ENTLADEN (jeden Frame!) — Takt liegt DORT
800345f4/f8 lw  v1,DAT_800ac768       ; v1 = virtuelles Pad HELD
80034600 andi  v0,v1,0x8              ; LINKS?
80034604 beq   v0,zero,LAB_80034628
8003460c-20 DAT_800acabe -= 0x18      ; drehen im Feuern: -24/Frame    <<< @0x80034618
80034624 andi  v0,v1,0x2              ; RECHTS?
80034628 beq   v0,zero,LAB_8003464c
80034630-44 DAT_800acabe += 0x18      ; +24/Frame                      <<< @0x8003463c
80034648 andi  v0,v1,0x100            ; R1 = ABZUG noch gehalten?
8003464c beq   v0,zero,LAB_800347dc   ; NEIN -> DAT_800aca5a=1 (Abbruch Abzug-loslassen)
80034650 _clear a2
80034654-68 FUN_8001f314([DAT_800acbc4],[DAT_800acbc8],0,0x200)
;        = Anim-Advance aus dem PLW-Waffenbank-Paar: sampelt +0x95, POST-inkrement,
;          dekrementiert +0x8f, Wrap am Clip-Ende. Rueckgabe hier IGNORIERT (Feuer-Clip looppt).
8003466c-78 v0 = DAT_800ac768 & 0x10  ; HOCH gehalten?
8003467c beq  v0,zero,LAB_800346a4
80034684-94 if (acaec & 0x8000) -> LAB_800346a4   ; schon hoch -> weiter
80034698/a0 v0 = (acaec&0x1fff)|0x8000            ; Aim-Wort: HOCH setzen
8003469c j    LAB_800346d8
800346a4-b4 v0 = held & 0x20         ; RUNTER gehalten?
800346b4 beq  v0,zero,LAB_800346f0
800346bc-cc if (acaec & 0x2000) -> LAB_800346f0   ; schon tief -> weiter
800346d0/d4 v0 = (acaec&0x1fff)|0x2000            ; TIEF setzen
800346d8/dc sb zero,DAT_800aca5b     ; innerer Substate -> 0: Clip NEU waehlen
800346e0/e4 sh v0,DAT_800acaec       ;   (Aim-Level-Wechsel MITTEN im Feuern;
800346e8 j   LAB_800347e8            ;    +0x95 wird dabei NICHT genullt!)
800346f0-fc v0 = held & 0x30         ; weder hoch noch runter?
80034700 bne  v0,zero,LAB_8003473c
80034708-18 if (acaec & 0x4000) -> LAB_8003473c   ; schon neutral -> weiter
80034720-28 acaec = (acaec&0x1fff)|0x4000         ; NEUTRAL setzen
8003472c/30 sb zero,DAT_800aca5b     ; -> Substate 0 (Clip 7 neu)
80034734 j    LAB_800347e8
8003473c-48 v0 = held & 0x40         ; QUADRAT (Anlegen) noch gehalten?
8003474c bne  v0,zero,LAB_800347e8   ; ja -> fertig fuer diesen Frame
80034750 _ori v0,zero,0x2
80034754/58 sb v0,DAT_800aca5b       ; NEIN -> innerer Substate 2 (RELEASE)
8003475c j    LAB_800347e8

; --- Case 2 @0x80034764: RELEASE-EINSTIEG --------------------------------------
80034764 ori   v0,zero,0x3           ; (v0 kam als 3 aus dem Delay-Slot, hier erneut)
80034768 sb    v0,0x0(a0)            ; aca5b = 3
8003476c/70 lhu a0,DAT_800acaec
80034774 ori   v0,zero,0x7
80034778/7c sb  zero,DAT_800acae9    ; +0x95 Frame = 0  (EINZIGER Reset in Sub 2!)
80034780/84 sb  v0,DAT_800acae3      ; Crossfade = 7                   <<< @0x80034774
80034788-b4 Clip = 8 + 3*hoch + 6*tief  ; 8|11|14 (RELEASE level/hoch/tief)
800347b0/b4 sb  v1,DAT_800acae8
800347b8 clear a2                    ; (faellt durch in Case 3)

; --- Case 3 @0x800347bc: RELEASE-WARTEN ----------------------------------------
800347bc-d0 v0 = FUN_8001f314([DAT_800acbc4],[DAT_800acbc8],0,0x200)
800347d4 beq  v0,zero,LAB_800347e8   ; Clip noch nicht fertig -> warten
; --- LAB_800347dc: Exit zu Aussen-Substate 1 -----------------------------------
800347dc ori   v0,zero,0x1
800347e0/e4 sh  v0,DAT_800aca5a      ; !! HALFWORD-Store: aca5a=1 UND aca5b=0
                                     ;    (little-endian raeumt den inneren Substate mit ab)
; --- LAB_800347e8: Epilog -------------------------------------------------------
800347e8-f4 lw ra / addiu sp / jr ra
```

### 1.2 Pseudo-C

```c
void fsm_sub2_feuerschleife(void)   /* @0x80034510, aca5a==2 */
{
    switch (aca5b) {                                    /* innerer Substate */
    case 0:  /* FEUER-EINSTIEG @0x80034564 */
        aca5b = 1;
        acae3 = 7;                                      /* Crossfade @0x80034570 */
        acae8 = 7 + 3*(acaec>>15) + 6*((acaec>>13)&1);  /* Clip 7|10|13 @0x80034588/94 */
        /* acae9 (+0x95) laeuft weiter — KEIN Reset */
        /* fallthrough */
    case 1:  /* FEUER-Frame @0x800345ac */
        acc24 = 0x5a;                                   /* 90 @0x800345ac */
        aca52 |= 1;                                     /* Schuss-Latch @0x800345c8 */
        entlade_tabelle_0x80074100[aca5d]();            /* JEDEN Frame @0x800345ec */
        if (held & 0x8)   acabe -= 0x18;                /* LINKS  @0x80034618 */
        if (held & 0x2)   acabe += 0x18;                /* RECHTS @0x8003463c */
        if (!(held & 0x100)) { aca5a = 1; aca5b = 0; return; }  /* ABZUG los @0x8003464c/0x800347e4 */
        f314(*acbc4, *acbc8, 0, 0x200);                 /* Anim-Advance @0x80034664 (Wrap ignoriert) */
        if ((held & 0x10) && !(acaec & 0x8000)) { acaec = (acaec&0x1fff)|0x8000; aca5b = 0; return; }
        if ((held & 0x20) && !(acaec & 0x2000)) { acaec = (acaec&0x1fff)|0x2000; aca5b = 0; return; }
        if (!(held & 0x30) && !(acaec & 0x4000)) { acaec = (acaec&0x1fff)|0x4000; aca5b = 0; return; }
        if (!(held & 0x40)) aca5b = 2;                  /* ANLEGEN los @0x8003474c-58 */
        return;
    case 2:  /* RELEASE-EINSTIEG @0x80034764 */
        aca5b = 3;
        acae9 = 0;                                      /* @0x8003477c — einziger Frame-Reset */
        acae3 = 7;                                      /* @0x80034774 */
        acae8 = 8 + 3*(acaec>>15) + 6*((acaec>>13)&1);  /* Clip 8|11|14 @0x80034794 */
        /* fallthrough */
    case 3:  /* RELEASE-WARTEN @0x800347bc */
        if (f314(*acbc4, *acbc8, 0, 0x200))             /* Wrap = Clip fertig @0x800347d4 */
            { aca5a = 1; aca5b = 0; }                   /* sh @0x800347e4 */
        return;
    }
}
```

### 1.3 Übergänge (Sub 2)

| Ereignis | Bedingung (Adresse) | Ziel |
|---|---|---|
| Feuer-Einstieg | aca5b==0 (@0x80034538) | Clip 7/10/13, aca5b=1, faellt in Feuer-Frame |
| Abzug (R1, 0x100) losgelassen | @0x8003464c | **DAT_800aca5a = 1** (sh @0x800347e4, nullt aca5b mit) |
| Aim-Level-Wechsel (0x10/0x20/0x30) | @0x8003467c/b4/00 | aca5b=0 → naechster Frame neuer Feuer-Clip (Frame-Zähler läuft weiter) |
| Anlegen (QUADRAT, 0x40) losgelassen | @0x8003474c | aca5b=2 → Release-Clip 8/11/14 |
| Magazin leer | in der Entlade-Routine (@0x800349fc bzw. @0x80034c34) | aca5b=2 + Leer-Klick Se_on(0x01010001) |
| Release-Clip fertig | f314-Wrap @0x800347d4 | **DAT_800aca5a = 1** |

---

## 2. LAB_800347F8 — Entlade-Routine Waffe 12 (Ingram M10)

Aufgerufen jeden Feuer-Frame aus Sub-2-Case-1 (@0x800345ec) UND vom Standard-Feuer-Pfad
(gleiche Tabelle, jalr @0x800334e0). Hier liegt der **Feuertakt**.

### 2.1 Roh-Listing (annotiert, gekürzt um Prolog/Epilog)

```
800347f8-80034814  Prolog; s1=0xAAAAAAAB (Magic /3 bzw. /6), s2=&DAT_800acae9
80034818 lbu   a0,0(s2)               ; a0 = Frame im Clip (+0x95)
80034820-38 a0 = a0 % 3               ; (multu 0xAAAAAAAB, hi>>1, *3, sub)
8003483c bne  a0,zero,LAB_80034908    ; nur bei Frame%3==0:
; --- Effekt 1: MUENDUNGSFEUER, jeden 3. Clip-Frame ------------------------------
80034844/48 a0 = 0x02010800           ; ESP id 2 (muzzle), SUB 1, scale 0x800  <<< @0x80034844
8003484c a3 = sp+0x10                 ; Offset-Triple
80034850/54 v0 = DAT_800acbdc         ; Posebuffer-Basis
80034858/5c a1 = (lh)DAT_800acabe     ; Yaw
80034860/64 t0 = DAT_800aca5c         ; Charakter-Byte
80034868 s0 = v0 + 0x7a4              ; Gun-Bone-Matrix
80034870/74 t0 = (t0&4)?1:0           ; Charakter-Zweig c
80034878-98 [sp+0x10] = 0x46 - 10*c   ; x = 70-10c                     <<< @0x80034884
8003488c-a4 [sp+0x14] = 0x41a - 30*c  ; y = 1050-30c                   <<< @0x8003489c
800348a8-b4 [sp+0x18] = -0x3c - 30*c  ; z = -60-30c                    <<< @0x800348a8
800348b0 jal  FUN_80019700            ; Effekt-Spawn
; --- Effekt 2: RAUCH, jeden 6. Clip-Frame ---------------------------------------
800348b8-d8 a0 = acae9 % 6            ; (hi>>2, *6)
800348e0 bne  a0,zero,LAB_80034908
800348e8/ec a0 = 0x03000b00           ; ESP id 3 (smoke), sub 0, scale 0xb00  <<< @0x800348e8
800348f0-f8 a1 = acabe, a2 = s0       ; Offsets sp+0x10 werden WIEDERVERWENDET
800348fc jal  FUN_80019700            ;   (gueltig: %6==0 impliziert %3==0)
; --- Effekt 3: HUELSENAUSWURF, jeder 7. Feuer-Frame ------------------------------
80034908/0c s0 = &DAT_800acaf2        ; freilaufender Feuer-Zaehler (+0x9e)
80034910-44 a0 = (s8)acaf2 % 7        ; SIGNIERTE Division (Magic 0x92492493)
80034948 bne  a0,zero,LAB_800349ac
8003494c/50 a0 = 0x04000800           ; ESP id 4 (shell), sub 0, scale 0x800  <<< @0x8003494c
80034954-64 a3=sp+0x10, a1=acabe, a2=DAT_800acbdc+0x7a4
80034968/6c [sp+0x14] = 0x64          ; y = 100                        <<< @0x80034968
80034970-98 [sp+0x10] = 0xc8 - 40*c   ; x = 200-40c                    <<< @0x80034990
8003499c-a8 [sp+0x18] = -0x14 - 40*c  ; z = -20-40c                    <<< @0x8003499c
800349a4 jal  FUN_80019700
; --- Zaehler + SCHUSS-Gate -------------------------------------------------------
800349ac lbu  v0,0(s0)                ; v0 = acaf2
800349b0/b4 lbu v1,DAT_800acae9
800349b8 addiu v0,v0,0x1
800349bc andi v1,v1,0x4               ; BIT 2 des Clip-Frames          <<< @0x800349bc
800349c0 bne  v1,zero,LAB_80034a14    ; Bit gesetzt -> KEIN Schuss dieses Frame
800349c4 _sb  v0,0(s0)                ; (Delay: acaf2++ IMMER)
800349c8 jal  FUN_8004eae4            ; Munition -1 (Inventar-Slot; ret 0 = leer)
800349d0 beq  v0,zero,LAB_800349fc
800349d4 _ori v0,zero,0x2
; Munition da -> SCHADEN:
800349d8/dc v0 = DAT_800ac784         ; Spieler-Entity
800349e0/e4 a0 = DAT_800aca5d         ; Waffen-Id
800349e8 a1 = [entity+0x7c]           ; Aim-Ptr
800349ec jal  FUN_80011f50            ; Schadens-Resolver (Aim-Cone ueber Gegner)
800349f4 j    LAB_80034a14
; Magazin leer:
800349fc-a00 sb v0(=2),DAT_800aca5b   ; innerer Substate -> 2 (RELEASE)  <<< @0x800349fc
80034a04/08 a0 = 0x01010001           ; Se_on Bank 1 / Record 1 / positional  <<< @0x80034a04
80034a0c jal  FUN_80045024            ; LEER-KLICK
80034a10 _a1 = s0 - 0x6a              ; = &DAT_800aca88 (Spieler-Position)
80034a14-2c Epilog
```

### 2.2 Pseudo-C (gilt strukturell auch für §3)

```c
void entlade_ingram(void)   /* @0x800347f8, Tabelle 0x80074100[12] */
{
    int c = (aca5c & 4) ? 1 : 0;                       /* Charakter-Zweig */
    if (acae9 % 3 == 0) {
        fx(0x02010800, acabe, gunbone, 70-10*c, 1050-30*c, -60-30*c);   /* Muendung */
        if (acae9 % 6 == 0)
            fx(0x03000b00, acabe, gunbone, /*gleiche Offsets*/);        /* Rauch */
    }
    if ((int8_t)acaf2 % 7 == 0)
        fx(0x04000800, acabe, gunbone, 200-40*c, 100, -20-40*c);        /* Huelse */
    acaf2++;                                            /* immer (Delay-Slot @0x800349c4) */
    if (!(acae9 & 4)) {                                 /* SCHUSS-Gate @0x800349bc */
        if (FUN_8004eae4())                             /* Munition -1 */
            FUN_80011f50(aca5d, player->aim_ptr_0x7c);  /* Schaden */
        else {
            aca5b = 2;                                  /* leer -> RELEASE @0x800349fc */
            Se_on(0x01010001, &player_pos_0x800aca88);  /* Klick @0x80034a04 */
        }
    }
}
```

---

## 3. 0x80034A30 — Entlade-Routine Waffe 19 (MC51)

Im Ghidra-Dump NICHT disassembliert (nur `?? XXh`-Rohbytes ab Zeile ~130168) — nur über den
Funktionspointer @0x8007414C erreichbar. Disassembliert mit
`re15_disasm.py dis 0x80034a30 148`. **Byte-identische Struktur zu §2**, Unterschiede NUR in
den Effekt-Offsets:

| Konstante | Ingram (W12) | MC51 (W19) | Adresse (MC51) |
|---|---|---|---|
| Muendung x | 70−10c | **65** (s5=0x41) | @0x80034a94 |
| Muendung y | 1050−30c | **1300** (s3=0x514) | @0x80034a98 |
| Muendung z | −60−30c | **90−160c** (s2=0x5a; c*160 @0x80034ad0-d8) | @0x80034ab4/adc |
| Rauch-Offsets | wiederverwendet | neu geschrieben, identisch zur Muendung | @0x80034b38-54 |
| Huelse x/y/z | 200−40c / 100 / −20−40c | **145 / 530 / 25** (0x91/0x212/0x19, ohne c-Zweig) | @0x80034bc4/bcc/bd4 |

Takt/Gates identisch: Muendung `acae9%3==0` (@0x80034a64-80), Rauch `acae9%6==0`
(@0x80034af0-b10), Huelse `(s8)acaf2%7==0` (@0x80034b6c-a4), `acaf2++` immer
(Delay @0x80034bfc), Schuss-Gate `!(acae9&4)` (@0x80034bf4-f8), Munition FUN_8004eae4
(@0x80034c00), Schaden FUN_80011f50(aca5d,[entity+0x7c]) (@0x80034c10-28), leer →
`aca5b=2` (@0x80034c34-38) + Se_on(0x01010001,&0x800aca88) (@0x80034c3c-48).
`jr ra` @0x80034c6c; 0x80034c71-73 Padding.

Waffe 14 (Flammenwerfer) hat ihre eigene Entlade-Routine @0x800C45A8 (Tabelle [14]) —
**außerhalb dieses Bereichs**, Takt dort separat zu RE'en.

---

## 4. Feuertakt — die Gesamtrechnung (W0C/W13-Bänke)

Clip-Längen byte-true aus den PLW-EDDs gelesen (Datei `shared_assets/PSX/PLD/PL00W0C.PLW`,
dir[0]=EDD @0x8; Paare u16 count/u16 offset):

```
PL00W0C == PL00W13 (16 Clips): [20,31,39,1,50,30,16, 9,19,1, 9,18,1, 9,18,1]
PL00W0E (Flammenwerfer):       [20,31,39,1,50,30,15,21, 5,1,21, 5,1,21, 5,1]
```

| Clip | W0C/W13 Länge | Nutzung durch Sub 2 |
|---|---|---|
| 7 / 10 / 13 | 9 / 9 / 9 | **FEUER** level/hoch/tief (@0x80034588/94, Basis 7 + 3*hoch + 6*tief) |
| 8 / 11 / 14 | 19 / 18 / 18 | **RELEASE** level/hoch/tief (@0x80034794, Basis 8) |
| 9 / 12 / 15 | 1 / 1 / 1 | von Sub 2 NICHT benutzt (1-Frame-Halteposen; andere Subs) |

Damit pro 9-Frame-Feuer-Clip-Zyklus (acae9 = 0..8, f314-Wrap):
- **Schuss (Munition−1 + Schadens-Resolve): Frames 0,1,2,3,8** — alle Frames mit Bit 2 == 0
  (`andi 0x4` @0x800349bc / @0x80034bf4) → 5 Schuss je 9 Frames ≈ 16,7 Schuss/s bei 30 fps.
  Frames 4-7 = Pause.
- **Mündungsfeuer: Frames 0,3,6** (`%3` @0x80034820-3c) — 0x02010800 = ESP-id 2, **Sub 1**
  (Pistolen-Standardfeuer nutzt Sub 0: 0x02000800 @game_step-RE), Scale 0x800.
- **Rauch: Frames 0,6** (`%6` @0x800348c0-e0) — 0x03000b00, Scale 0xb00 (Pistole: 0x0c00).
- **Hülse: jeder 7. Wert von acaf2** (freilaufend, `%7` signiert @0x80034910-44) — 0x04000800,
  unabhängig vom Clip-Zyklus. acaf2 ist u8 und wird als s8 dividiert: nach dem Überlauf
  127→−128 verschiebt sich die 7er-Phase (−128%7=−2, byte-true Randeffekt).

Aim-Wechsel mitten im Feuern (Case 1 → aca5b=0) nullt **weder** acae9 **noch** acaf2 —
der Schusstakt läuft phasengleich weiter; nur der Clip und der 7-Frame-Crossfade (acae3=7)
werden neu gesetzt. Einziger acae9-Reset in Sub 2: Release-Einstieg @0x8003477c.

---

## 5. Konstanten-Tabelle (vollständig)

| Wert | Bedeutung | Adresse |
|---|---|---|
| 7 | Crossfade-Frames (+0x8f) bei Feuer-Einstieg | @0x80034570 (ori), sb @0x80034578 |
| 7 / +3 / +6 | Feuer-Clip-Basis / hoch / tief → 7,10,13 | @0x80034588 (addiu 7), @0x80034580-a0 |
| 0x5a (90) | „kürzlich gefeuert“-Byte +0x1d0 | @0x800345ac, sb @0x800345b4 |
| 1 (Bit 0) | aca52-Schuss-Latch | ori @0x800345c8, sh @0x800345d4 |
| 0x80074100 | Entlade-Tabelle (Index = Waffen-Id) | lui/addiu @0x800345d8-dc |
| 0x18 (24) | Dreh-Rate Yaw/Frame beim Feuern (LINKS 0x8 / RECHTS 0x2) | @0x80034618 / @0x8003463c |
| 0x100 | Abzug (R1, virtuell) | andi @0x80034648 |
| 0x10/0x20/0x30 | Aim hoch/runter/beides (virtuell) | @0x80034678 / @0x800346b0 / @0x800346fc |
| 0x8000/0x2000/0x4000 | Aim-Wort-Bits hoch/tief/neutral (&0x1fff-Maske) | @0x800346a0 / @0x800346d4 / @0x80034720 |
| 0x40 | Anlegen-Taste (QUADRAT, virtuell) | andi @0x80034748 |
| 0x200 | f314-Crossfade-Schritt | ori a3 @0x80034668 / @0x800347d0 |
| 8 / +3 / +6 | Release-Clip-Basis → 8,11,14 | @0x80034794 |
| 1 | Exit: sh 1 → aca5a=1 UND aca5b=0 | @0x800347dc-e4 |
| 3 / 6 / 7 | Effekt-Perioden Mündung/Rauch/Hülse | @0x80034820-3c / @0x800348c0-e0 / @0x80034910-44 |
| 0x02010800 | Mündungsfeuer (ESP id 2, Sub 1, Scale 0x800) | @0x80034844-48 (W19: @0x80034a88-8c) |
| 0x03000b00 | Rauch (id 3, Sub 0, Scale 0xb00) | @0x800348e8-ec (W19: @0x80034b18-1c) |
| 0x04000800 | Hülse (id 4, Sub 0, Scale 0x800) | @0x8003494c-50 (W19: @0x80034ba8-ac) |
| 70−10c / 1050−30c / −60−30c | W12 Mündungs-Offset x/y/z (c = aca5c&4) | @0x80034884 / @0x8003489c / @0x800348a8 |
| 200−40c / 100 / −20−40c | W12 Hülsen-Offset | @0x80034990 / @0x80034968 / @0x8003499c |
| 65 / 1300 / 90−160c | W19 Mündungs-Offset | @0x80034a94 / @0x80034a98 / @0x80034ab4+adc |
| 145 / 530 / 25 | W19 Hülsen-Offset | @0x80034bc4 / @0x80034bcc / @0x80034bd4 |
| 0x4 | Schuss-Gate: Bit 2 von +0x95 muss 0 sein | andi @0x800349bc / @0x80034bf4 |
| 2 | Leer → innerer Substate 2 (Release) | @0x800349d4+9fc / @0x80034c0c+c34 |
| 0x01010001 | Leer-Klick Se_on Bank1/Rec1/positional, a1=&0x800aca88 | @0x80034a04-0c / @0x80034c3c-48 |
| 0x7a4 | Gun-Bone-Matrix-Offset im Posebuffer (DAT_800acbdc) | @0x80034868 / @0x80034978 / @0x80034ac0 / @0x80034be0 |
| 0x7c | Aim-Ptr-Feld im Spieler-Entity (a1 des Schadens-Resolvers) | lw @0x800349e8 / @0x80034c20 |

Aufruf-Semantik (belegt): FUN_8001f314(a0=EDD-Ptr `[0x800acbc4]`, a1=Bank-Ptr `[0x800acbc8]`,
a2=0 vorwärts, a3=0x200 Crossfade) — liest Clip aus +0x94, Frame aus +0x95
(RE_15_Quellcode_V2/FUN_8001f314.c), Rückgabe ≠0 = Clip-Wrap. FUN_8004eae4 dekrementiert
das Munitions-Byte des angelegten Inventar-Slots (`DAT_800b10ad[slot*4]`, 2-Slot-Items via
`DAT_800b10ae[slot*4]==2` → slot−1), ret 0 = leer. FUN_80019700: a0 = (id<<24)|(sub<<16)|scale,
a1=Winkel, a2=Matrix, a3=Offset-Triple (sp+0x10/14/18). FUN_80045024 = Se_on
(Bank=a0>>24, Record=(a0>>16)&0xff, Low-Byte=positional; @0x80045028/78/80).

---

## 6. Befund-Korrektur für den Port

`re15_port/engine/src/enemy_ai_common.c:5509` bezeichnet den aca52-Setter „@0x800345c8“ als
„player knockdown command FSM — NOT PORTED YET“. **Falsch zugeordnet:** 0x800345c8 ist der
Dauerfeuer-Case-1 (dieses Dossier), und der zweite Setter @0x800334e8-504 liegt im
Standard-Feuer-Sub LAB_80033460 (Tabelleneintrag @0x800740fc), ebenfalls DIREKT nach dem
Entlade-jalr @0x800334e0 — beide Stellen setzen zusätzlich acc24=0x5a. Bit 0 von
DAT_800aca52 ist also ein **„Spieler feuert (Schuss-Lärm)“-Latch**, kein Knockdown-Latch;
die Krähe (Typ 0x4b, Leser @0x8004f17c) reagiert damit auf Dauerfeuer/Schüsse. Löschstellen
unverändert: @0x80031c44 (`&= 0xfffe`, Spieler-Normal-Handler) und FUN_8003ecec (Raum-Init).

## 7. Unklar (auch mit RE2-Fallback nicht abschließend)

- **Leser von DAT_800acc24** (+0x1d0 des Spieler-Entity, = 90 pro Feuer-Frame): Der
  Ghidra-Dump listet nur 3 Writes (@0x8003198c =0, @0x800334f8 =0x5a, @0x800345b4 =0x5a),
  keinen Read; Overlay-Decompilate (`grep acc24 RE_15_Quellcode_Overlays/`) leer. Bei
  Gegnern ist +0x1d0 der Distanz-Cache — der Spieler durchläuft die Gegner-Tick-Schleife
  nicht. Möglich: toter/Debug-Wert oder Leser über berechneten Zeiger. Nicht auflösbar
  ohne dynamischen Watchpoint (re15-pcsx-watchpoint auf 0x800acc24-Read).
- **Visuelle Identität von ESP-Sub 1 der Mündung (0x02010800)** vs. Sub 0 der Pistole:
  Row-Inhalt der ESP-Bank nicht hier dekodiert (Row-VM re15_esp.c, eigener Bereich).
- Ob der Zustand, der Sub 2 betritt (aca5a←2, in Sub 0/1 außerhalb dieses Bereichs),
  +0x95 vorher nullt — Sub 2 selbst tut es beim Feuer-Einstieg NICHT (nur @0x8003477c
  beim Release). Für die Phase des 5-aus-9-Schusstakts beim allerersten Zyklus ist das
  relevant und gehört in die Sub-0/1-Dossiers.
