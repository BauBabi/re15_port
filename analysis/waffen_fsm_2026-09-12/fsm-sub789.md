# Waffen-FSM Subs 7/8/9 — 0x80035314 / 0x80035424 / FUN_80035538

Dossier 2026-09-12. Quelle: `ghidra1_V2.txt` (Zeilen 131302–131620), `RE_15_Quellcode_V2/FUN_80035538.c`,
Gegenproben in der EXE selbst (Bank-Loader @0x80036b80ff, Equip @0x80046680ff, Fusstritt-Muster @0x80030bb4).
Alle Adressen RE1.5 `PSX.EXE`.

---

## 0. ERGEBNIS ZUERST: die "Doppelrolle" ist ein Tabellen-Overlap

Die bekannte "10-Sub-Tabelle @0x80074150" des Dauerfeuer-Handlers 0x80034014 sind in Wahrheit
**zwei ueberlappende 5er-Tabellen**:

| Index @0x80074150 | Ziel | Dispatch-Basis 0x80074150 (Dauerfeuer 0x80034014) | Dispatch-Basis 0x80074164 (Nahkampf 0x80034E70) |
|---|---|---|---|
| 0 | 0x80034060 | Sub 0 | — |
| 1 | 0x80034278 | Sub 1 | — |
| 2 | 0x80034510 | Sub 2 | — |
| 3 | 0x80034C74 | Sub 3 | — |
| 4 | 0x80034D68 | Sub 4 | — |
| 5 | 0x80034EE8 | (tot) | **Melee-Sub 0 = RAISE** (Wieder-Anheben, Messer schon in Hand) |
| 6 | 0x800350C4 | (tot) | **Melee-Sub 1 = HOLD** (Clips 8/10/12) |
| 7 | 0x80035314 | (tot) | **Melee-Sub 2 = SLASH** |
| 8 | 0x80035424 | (tot) | **Melee-Sub 3 = LOWER** |
| 9 | 0x80035538 | (tot) | **Melee-Sub 4 = DRAW** (Messer-Ziehen) |

Belege:
- Der Nahkampf-Handler laedt seine Dispatch-Basis @0x80034ec0 `addiu at,at,0x4164` → **0x80074164
  = 0x80074150 + 0x14** (jalr @0x80034ed0; Ghidra-XREF "80034ed0(c)" auf FUN_80035538 ist genau
  dieser Tabellen-Dispatch).
- Der Dauerfeuer-Pfad (0x80034014 + Subs 0..4) schreibt DAT_800aca5a **nur mit 0..4**:
  `1` @0x800341f4, `1` @0x80034244, `1` @0x80034264 (v0=1 aus Delay @0x800340a0), `3` @0x8003430c,
  `2` @0x80034418, `4` @0x80034474, `1` @0x800347e4, `0` @0x80034d40, `1` @0x80034e40.
  **Kein Schreiber setzt 5..9** → Eintraege [5..9] sind ueber 0x80034014 unerreichbar.
- Die Melee-Substates schreiben ihrerseits nur 0..4: `4` @0x80034ea8 (Kalt-Einstieg), `1` @0x800350a4,
  `3` @0x80035144, `2` @0x8003523c, `1` @0x80035400, `0` @0x80035508, `1` @0x80035778.
- Die einzigen aca5a-Schreiber mit Werten 5..9 in der EXE (5 @0x80032168, 9 @0x800321a8/b0,
  6 @0x800321f4, 7 @0x8003220c, 0xa @0x80032280) liegen im **Nachbar-Sub-FSM @0x8003203c**
  (Switch-Jump-Table @0x80010b38, 11 Cases) — ein anderes Action-Sub-FSM, das dieselbe
  Substate-Variable DAT_800aca5a wiederverwendet und **nie ueber die Waffen-Dispatch-Tabelle
  @0x80074030 laeuft** (deren Eintraege sind nur 0x80034E70 / 0x80034014 / 0x80032E9C).

**Fazit:** 0x80035314, 0x80035424 und 0x80035538 sind AUSSCHLIESSLICH Melee-Substates 2/3/4
(Waffen-Ids 0..2, Handler 0x80034E70). Der "Dauerfeuer-Sub-9"-Alias ist toter Tabellenraum.

Melee-FSM-Uebersicht (Kontext, aus 0x80034e70 + Subs):

```
Handler 0x80034E70:  wenn aca5a==0 && !(DAT_800aca54 & 0x4000)  → aca5a=4   (@0x80034e90-a8)
                     dispatch [aca5a] @0x80074164                            (@0x80034eb8-d0)

sub0 RAISE 0x80034EE8: Clip 6 (@0x80034f38), acaec=(…&0x1fff)|0x4000, Auto-Aim FUN_8003703c(0x1388=5000)
                       (@0x80034f78-7c), aca3c|=0xC0; Track-Slew a8f8(Ziel+0x34,0xC0) wenn acaf3&1
                       (@0x80034fb4-c0); fertig → aca5a=1 (Phase-Maschine wie sub4)
sub1 HOLD  0x800350C4: Clip 8 + 2*Hoch + 4*Tief (@0x8003510c-18); Pad 0x100 → aca5a=3 (@0x80035144);
                       Pad 0x10 → acaec|=0x8000 (Hoch, @0x80035170); Pad 0x20 → acaec|=0x2000 (Tief,
                       @0x800351b8); kein 0x30 → acaec|=0x4000 (Neutral, @0x80035204);
                       Pad 0x40 → aca5a=2 (@0x8003523c); Dreh-Schritt = Byte1 @0x80074091 (0x30=48)
sub2 SLASH 0x80035314: DIESES DOSSIER §1
sub3 LOWER 0x80035424: DIESES DOSSIER §2
sub4 DRAW  0x80035538: DIESES DOSSIER §3
```

Pad-Bits in DAT_800ac768 (Engine-Pad-Wort), wie hier verwendet: 0x2/0x8 = Aim-Drehung
(+/− auf DAT_800acabe), 0x10/0x20 = Zielen hoch/runter, 0x40 = Stich ausloesen,
0x100 = Aim-Release → LOWER. DAT_800acaec-Bits: 0x8000 hoch, 0x4000 neutral, 0x2000 tief
(Formel-Beleg: `srl 0xf → *2` und `srl 0xb → &4`, s. §1).

---

## 1. Sub 7 / Melee-Sub 2 = **MESSER-SLASH** — 0x80035314..0x80035418 (Pad-Bytes bis 0x80035423)

### 1.1 Roh-Listing mit Annotation

```
LAB_80035314                              XREF: 0x8007416c (Tabelleneintrag)
80035314  addiu  sp,sp,-0x20
80035318  lui    v1,0x800b
8003531c  addiu  v1,v1,-0x35a5            ; v1 = &DAT_800aca5b  (Phase im Substate)
80035320  sw     ra,0x18(sp)
80035324  lbu    v0,0x0(v1)               ; Phase lesen
80035328  nop
8003532c  bne    v0,zero,LAB_80035384     ; Phase!=0 → Tick
80035330  _addiu a1,v1,0x2d               ; a1 = &DAT_800aca88 (Spieler-Positionsblock, x@+0/z@+8)
; ---- Phase 0: Init ----
80035334  lui    a0,0x105                 ; SE-Code 0x0105_0001 aufbauen
80035338  ori    v0,zero,0x1
8003533c  sb     v0,0x0(v1)               ; Phase := 1
80035340  lui    v1,0x800b
80035344  lhu    v1,-0x3514(v1)           ; DAT_800acaec = Aim-Wort
80035348  ori    v0,zero,0x7
8003534c  lui    at,0x800b
80035350  sb     zero,-0x3517(at)         ; DAT_800acae9 := 0   (In-Clip-Frame-Zaehler)
80035354  lui    at,0x800b
80035358  sb     v0,-0x351d(at)           ; DAT_800acae3 := 7   (Anim-Frac/Interp-Init)
8003535c  srl    v0,v1,0xf                ; Bit15 (Hoch = 0x8000)
80035360  sll    v0,v0,0x1                ;   *2
80035364  addiu  v0,v0,0x7                ; Basis-Clip 7
80035368  srl    v1,v1,0xb
8003536c  andi   v1,v1,0x4                ; Bit13 (Tief = 0x2000) → 4
80035370  addu   v0,v0,v1                 ; Clip = 7 + 2*Hoch + 4*Tief  → 7 / 9 / 11
80035374  lui    at,0x800b
80035378  sb     v0,-0x3518(at)           ; DAT_800acae8 := Clip (W-Bank!)
8003537c  jal    FUN_80045024             ; SE abspielen: Schwung-Sound — IMMER (Hit wie Whiff)
80035380  _ori   a0,a0,0x1                ;   a0 = 0x0105_0001 (Klasse 1, SE-Index 5, low=1)
                                          ;   a1 = &DAT_800aca88 (Distanz-Attenuation)
; ---- Tick (Phase 1) ----
LAB_80035384
80035384  lui    v0,0x800b
80035388  lbu    v0,-0x3517(v0)           ; DAT_800acae9 = Frame im Clip
8003538c  nop
80035390  addiu  v0,v0,-0x6
80035394  sltiu  v0,v0,0x6                ; (frame-6) < 6u  ⇔  frame ∈ [6..11]
80035398  beq    v0,zero,LAB_800353d4     ; ausserhalb → kein Schaden
8003539c  _addiu a1,sp,0x10               ; a1 = sp+0x10 (Positions-Triple)
; ---- Schadens-Fenster: Frames 6..11, JEDEN Frame ein Resolve ----
800353a0  lui    v1,0x800b
800353a4  lw     v1,-0x3424(v1)           ; DAT_800acbdc = Posebuffer-Basis
800353a8  lui    a0,0x800b
800353ac  lbu    a0,-0x35a3(a0)           ; a0 = DAT_800aca5d (Waffen-Id, 1=Messer)
800353b0  lhu    v0,0x7b8(v1)             ; Gun-Bone-Matrix.t[0] (Matrix @+0x7a4, t @+0x14)
800353b4  nop
800353b8  sh     v0,0x10(sp)              ;   → pos.x
800353bc  lhu    v0,0x7bc(v1)             ; t[1]
800353c0  nop
800353c4  sh     v0,0x12(sp)              ;   → pos.y
800353c8  lhu    v0,0x7c0(v1)             ; t[2]
800353cc  jal    FUN_80011f50             ; Schadens-Resolver(waffen_id, &pos)
800353d0  _sh    v0,0x14(sp)              ;   → pos.z (Delay-Slot)
; ---- Anim vorwaerts, Ende → HOLD ----
LAB_800353d4
800353d4  clear  a2                       ; a2 = 0 → VORWAERTS
800353d8  lui    a0,0x800b
800353dc  lw     a0,-0x343c(a0)           ; DAT_800acbc4 (W-Bank-Animdaten, Loader @0x80036c04)
800353e0  lui    a1,0x800b
800353e4  lw     a1,-0x3438(a1)           ; DAT_800acbc8 (W-Bank-Pose,     Loader @0x80036be4)
800353e8  jal    FUN_8001f314             ; Keyframe-Anim-Advance
800353ec  _ori   a3,zero,0x200            ;   Schrittweite 0x200
800353f0  beq    v0,zero,LAB_80035404     ; nicht fertig → weiter
800353f4  _clear a0
800353f8  ori    v0,zero,0x1
800353fc  lui    at,0x800b
80035400  sh     v0,-0x35a6(at)           ; DAT_800aca5a := 1 (16-bit: Substate=1 HOLD, Phase=0)
LAB_80035404
80035404  jal    FUN_800369f8             ; Wurzelbewegung/Fusslock anwenden
80035408  _ori   a1,zero,0x1              ;   Kanal 1 (vgl. 0x80050c94: anim_flags&2-Kanal)
8003540c  lw     ra,0x18(sp)
80035410  addiu  sp,sp,0x20
80035414  jr     ra
80035418  _nop
8003541c  (4 Fuell-Bytes 08 00 e0 03 / 00 00 00 00 bis 0x80035423 — verwaistes jr ra/nop-Paar)
```

### 1.2 Pseudo-C

```c
void melee_slash_80035314(void)            /* Melee-Sub 2 */
{
    if (DAT_800aca5b == 0) {                              /* Phase 0: Init */
        DAT_800aca5b = 1;                                 /* @0x8003533c */
        DAT_800acae9 = 0;                                 /* Frame-Zaehler @0x80035350 */
        DAT_800acae3 = 7;                                 /* Anim-Frac    @0x80035358 */
        DAT_800acae8 = 7 + 2*((DAT_800acaec>>15)&1)       /* Clip 7/9/11  @0x8003535c-78 */
                         + ((DAT_800acaec>>11)&4);
        FUN_80045024(0x1050001, &DAT_800aca88);           /* Schwung-SE, IMMER @0x8003537c */
    }
    if ((uint8_t)(DAT_800acae9 - 6) < 6) {                /* Frames 6..11 @0x8003538c-98 */
        int16_t pos[3] = { pose[0x7b8/2], pose[0x7bc/2], pose[0x7c0/2] };  /* Gun-Bone.t */
        FUN_80011f50(DAT_800aca5d, pos);                  /* Schaden je Frame @0x800353cc */
    }
    if (FUN_8001f314(DAT_800acbc4, DAT_800acbc8, /*dir*/0, 0x200))
        *(uint16_t*)&DAT_800aca5a = 1;                    /* → HOLD (Melee-Sub 1) @0x80035400 */
    FUN_800369f8(0, 1);                                   /* Root-Motion Kanal 1 @0x80035404-08 */
}
```

### 1.3 Konstanten

| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x80035364 | 7 | Basis-Clip des Stichs (W-Bank) |
| 0x80035360 | +2 | bei Aim-Hoch (acaec Bit15 = 0x8000) → Clip 9 |
| 0x8003536c-70 | +4 | bei Aim-Tief (acaec Bit13 = 0x2000) → Clip 11 |
| 0x80035334+0x80035380 | 0x0105_0001 | Schwung-SE (FUN_80045024: a0>>24=Klasse 1 @0x80045028, (a0>>16)&0xff=SE-Index 5 @0x80045078-7c, low=1) |
| 0x80035390-94 | [6..11] | Schadens-Fenster (frame-6 < 6u), 6 Resolves bei voller Fensterdauer |
| 0x800353b0/bc/c8 | +0x7b8/bc/c0 | Posebuffer: Gun-Bone-Matrix-Translation (Matrix @+0x7a4, t@+0x14) |
| 0x800353ec | 0x200 | Anim-Schrittweite (FUN_8001f314 a3) |
| 0x800353d4 | a2=0 | VORWAERTS-Abspielen |
| 0x800353f8-400 | aca5a=1 | Clip fertig → HOLD, Phase 0 (sh loescht aca5b mit) |
| 0x80035358 | 7 | DAT_800acae3 Anim-Frac-Init (Port: anim_frac=7) |
| 0x80035408 | a1=1 | FUN_800369f8 Kanal 1 (Slash traegt Root-Motion auf Kanal 1; HOLD/LOWER/DRAW nutzen Kanal 0) |

Uebergaenge: Clip-Ende → Substate 1 (HOLD). Kein anderer Ausgang; R1-Release wird erst im HOLD
ausgewertet (@0x80035130) — der Stich spielt immer zu Ende.

---

## 2. Sub 8 / Melee-Sub 3 = **LOWER (Waffe senken, Action-Exit)** — 0x80035424..0x80035534

### 2.1 Roh-Listing mit Annotation

```
LAB_80035424                              XREF: 0x80074170 (Tabelleneintrag)
80035424  addiu  sp,sp,-0x18
80035428  lui    v1,0x800b
8003542c  addiu  v1,v1,-0x35a5            ; &DAT_800aca5b
80035430  sw     ra,0x10(sp)
80035434  lbu    v0,0x0(v1)
80035438  nop
8003543c  bne    v0,zero,LAB_8003547c     ; Phase!=0 → Tick
80035440  _ori   v0,zero,0x1
; ---- Phase 0: Init ----
80035444  sb     v0,0x0(v1)               ; Phase := 1
80035448  ori    v0,zero,0x6
8003544c  lui    at,0x800b
80035450  sb     v0,-0x3518(at)           ; DAT_800acae8 := 6  (RAISE-Clip, wird RUECKWAERTS gespielt)
80035454  lui    v0,0x800b
80035458  lhu    v0,-0x3514(v0)           ; DAT_800acaec
8003545c  ori    v1,zero,0x7
80035460  lui    at,0x800b
80035464  sb     zero,-0x3517(at)         ; DAT_800acae9 := 0
80035468  lui    at,0x800b
8003546c  sb     v1,-0x351d(at)           ; DAT_800acae3 := 7
80035470  andi   v0,v0,0x1fff             ; Aim-Bits 15/14/13 LOESCHEN
80035474  lui    at,0x800b
80035478  sh     v0,-0x3514(at)           ; DAT_800acaec := neutralisiert
; ---- Tick ----
LAB_8003547c
8003547c  lui    v1,0x800b
80035480  lw     v1,-0x3898(v1)           ; DAT_800ac768 = Pad-Wort
80035484  nop
80035488  andi   v0,v1,0x8
8003548c  beq    v0,zero,LAB_800354b0
80035490  _andi  v0,v1,0x2
80035494  lui    v0,0x800b
80035498  lhu    v0,-0x3542(v0)           ; DAT_800acabe (Aim-Yaw)
8003549c  nop
800354a0  addiu  v0,v0,-0x18              ; Pad 0x8: Yaw -= 24 (FESTER Schritt, nicht Tabelle!)
800354a4  lui    at,0x800b
800354a8  sh     v0,-0x3542(at)
800354ac  andi   v0,v1,0x2
LAB_800354b0
800354b0  beq    v0,zero,LAB_800354d0
800354b4  _ori   a2,zero,0x1              ; !! Delay-Slot laeuft IMMER: a2 := 1 = RUECKWAERTS
800354b8  lui    v0,0x800b
800354bc  lhu    v0,-0x3542(v0)
800354c0  nop
800354c4  addiu  v0,v0,0x18               ; Pad 0x2: Yaw += 24
800354c8  lui    at,0x800b
800354cc  sh     v0,-0x3542(at)
LAB_800354d0
800354d0  lui    a0,0x800b
800354d4  lw     a0,-0x343c(a0)           ; DAT_800acbc4
800354d8  lui    a1,0x800b
800354dc  lw     a1,-0x3438(a1)           ; DAT_800acbc8
800354e0  jal    FUN_8001f314             ; Clip 6 RUECKWAERTS (a2=1) abspielen
800354e4  _ori   a3,zero,0x200
800354e8  beq    v0,zero,LAB_80035520     ; nicht fertig → weiter
800354ec  _clear a0
; ---- Clip ausgelaufen: Action-Exit ----
800354f0  lui    v0,0x800b
800354f4  lw     v0,-0x35c4(v0)           ; DAT_800aca3c (Spieler-Flagwort)
800354f8  li     v1,-0x81
800354fc  lui    at,0x800b
80035500  sb     zero,-0x35a7(at)         ; DAT_800aca59 := 0  (Action-Latch loeschen)
80035504  lui    at,0x800b
80035508  sh     zero,-0x35a6(at)         ; DAT_800aca5a := 0, DAT_800aca5b := 0
8003550c  and    v0,v0,v1                 ; &= ~0x80
80035510  li     v1,-0x41
80035514  and    v0,v0,v1                 ; &= ~0x40   (zusammen: aca3c &= ~0xC0)
80035518  lui    at,0x800b
8003551c  sw     v0,-0x35c4(at)
LAB_80035520
80035520  jal    FUN_800369f8             ; Root-Motion Kanal 0
80035524  _clear a1
80035528  lw     ra,0x10(sp)
8003552c  addiu  sp,sp,0x18
80035530  jr     ra
80035534  _nop
```

### 2.2 Pseudo-C

```c
void melee_lower_80035424(void)            /* Melee-Sub 3; Einstieg: HOLD sieht Pad 0x100 @0x80035144 */
{
    if (DAT_800aca5b == 0) {
        DAT_800aca5b = 1;
        DAT_800acae8 = 6;                                 /* Clip 6 @0x80035448-50 */
        DAT_800acae9 = 0;  DAT_800acae3 = 7;
        DAT_800acaec &= 0x1fff;                           /* Aim hoch/neutral/tief loeschen @0x80035470 */
    }
    if (DAT_800ac768 & 8) DAT_800acabe -= 0x18;           /* Aim-Drehung, FEST 24 @0x800354a0 */
    if (DAT_800ac768 & 2) DAT_800acabe += 0x18;           /* @0x800354c4 */
    if (FUN_8001f314(DAT_800acbc4, DAT_800acbc8, /*dir*/1, 0x200)) {   /* RUECKWAERTS @0x800354b4 */
        DAT_800aca59 = 0;                                 /* Action-Latch @0x80035500 */
        *(uint16_t*)&DAT_800aca5a = 0;                    /* Substate+Phase @0x80035508 */
        DAT_800aca3c &= ~0xC0;                            /* Aim-Flags @0x800354f8-1c */
    }
    FUN_800369f8(0, 0);                                   /* Kanal 0 @0x80035520-24 */
}
```

### 2.3 Konstanten

| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x80035448-50 | Clip 6 | RAISE-Clip, rueckwaerts abgespielt = Senken |
| 0x800354b4 | a2=1 | FUN_8001f314 RUECKWAERTS (Delay-Slot laeuft auf beiden Pfaden) |
| 0x80035470 | &0x1fff | Aim-Wort-Bits 0x8000/0x4000/0x2000 loeschen |
| 0x800354a0 / 0x800354c4 | ±0x18 (24) | Aim-Dreh-Schritt FEST (nicht Waffen-Tabelle; HOLD nutzt Byte1=48 @0x80074091, DRAW/RAISE Byte0=24 @0x80074090) |
| 0x80035500 | aca59=0 | Action-Latch (Port sub_state_1) loeschen |
| 0x80035508 | aca5a=aca5b=0 | Substate 0, Phase 0 (16-bit-sh) |
| 0x800354f8+0x80035510 | ~0x80, ~0x40 | DAT_800aca3c &= ~0xC0 — die beim RAISE/DRAW gesetzten Aim-Flags (|=0xC0 @0x80034f90 bzw. @0x800355ec) |
| 0x800354e4 | 0x200 | Anim-Schrittweite |

Uebergaenge: Clip-Ende → Substate 0 + aca59=0 = **kompletter Aim-Action-Exit** (das gesenkte
Messer bleibt via DAT_800aca54-Bit 0x4000 "in der Hand"; naechstes R1 geht dann in Sub 0 RAISE,
nicht in Sub 4 DRAW — Gate @0x80034e90-a8).

---

## 3. Sub 9 / Melee-Sub 4 = **DRAW (Messer-Ziehen mit Mesh-Attach-Event)** — FUN_80035538, 0x80035538..0x80035798

Ghidra-XREFs: 0x80034ed0(c) (= jalr ueber Melee-Tabelle 0x80074164), 0x80074174(*) (Tabelleneintrag).
Decompile `RE_15_Quellcode_V2/FUN_80035538.c` vorhanden, aber dort sind die Posebuffer-Stores als
`uRam0000076c…` verstuemmelt (Basis DAT_800acbdc nicht aufgeloest) — das Listing unten ist massgeblich.

### 3.1 Roh-Listing mit Annotation

```
FUN_80035538
80035538  addiu  sp,sp,-0x18
8003553c  sw     s0,0x10(sp)
80035540  lui    s0,0x800b
80035544  addiu  s0,s0,-0x35a5            ; s0 = &DAT_800aca5b (Phase)
80035548  sw     ra,0x14(sp)
8003554c  lbu    v1,0x0(s0)
80035550  ori    v0,zero,0x1
80035554  beq    v1,v0,LAB_80035600       ; Phase 1 → Tick
80035558  _slti  v0,v1,0x2
8003555c  beq    v0,zero,LAB_80035574     ; Phase >=2 → Phase-2-Pruefung
80035560  _nop
80035564  beq    v1,zero,LAB_80035588     ; Phase 0 → Init
80035568  _ori   v0,zero,0x1
8003556c  j      LAB_80035780             ; (unerreichbar: Phase<2 && !=0 && !=1)
80035570  _clear a0
LAB_80035574
80035574  ori    v0,zero,0x2
80035578  beq    v1,v0,LAB_80035774       ; Phase 2 → Uebergang HOLD
8003557c  _ori   v0,zero,0x1
80035580  j      LAB_80035780             ; Phase >2: nur Root-Motion
80035584  _clear a0
; ---- Phase 0: Init ----
LAB_80035588
80035588  sb     v0,0x0(s0)               ; Phase := 1
8003558c  ori    v0,zero,0xd
80035590  lui    at,0x800b
80035594  sb     v0,-0x3518(at)           ; DAT_800acae8 := 0x0D  (DRAW-Clip 13 der W-Bank)
80035598  lui    v0,0x800b
8003559c  lhu    v0,-0x3514(v0)           ; DAT_800acaec
800355a0  ori    v1,zero,0x7
800355a4  lui    at,0x800b
800355a8  sb     zero,-0x3517(at)         ; DAT_800acae9 := 0
800355ac  lui    at,0x800b
800355b0  sb     v1,-0x351d(at)           ; DAT_800acae3 := 7
800355b4  lui    at,0x800b
800355b8  sh     zero,-0x3520(at)         ; DAT_800acae0 := 0   (Vorwaerts-Speed aus)
800355bc  andi   v0,v0,0x1fff
800355c0  ori    v0,v0,0x4000             ; Aim-Wort := neutral (Bit14)
800355c4  lui    at,0x800b
800355c8  sh     v0,-0x3514(at)           ; DAT_800acaec
800355cc  jal    FUN_8003703c             ; AUTO-AIM-LATCH: naechstes Ziel suchen
800355d0  _ori   a0,zero,0x7d0            ;   Radius 2000 (RAISE-Sub 0 nutzt 5000 @0x80034f7c)
800355d4  lui    a0,0x108                 ; SE-Code 0x0108_0001 aufbauen
800355d8  ori    a0,a0,0x1
800355dc  lui    v1,0x800b
800355e0  lw     v1,-0x35c4(v1)           ; DAT_800aca3c
800355e4  lui    at,0x800b
800355e8  sb     v0,-0x350d(at)           ; DAT_800acaf3 := Scan-Ergebnis (1=lebendes Ziel, 2=K3, 0=nichts)
800355ec  ori    v1,v1,0xc0
800355f0  lui    at,0x800b
800355f4  sw     v1,-0x35c4(at)           ; DAT_800aca3c |= 0xC0 (Aim-Flags)
800355f8  jal    FUN_80045024             ; Zieh-SE
800355fc  _addiu a1,s0,0x2d               ;   a1 = &DAT_800aca88, a0 = 0x0108_0001 (SE-Index 8)
; ---- Tick (Phase 1; Init faellt hier durch) ----
LAB_80035600
80035600  lui    v0,0x800b
80035604  lbu    v0,-0x350d(v0)           ; DAT_800acaf3
80035608  nop
8003560c  andi   v0,v0,0x1
80035610  beq    v0,zero,LAB_80035628     ; Bit0 nur bei Rueckgabe 1 = LEBENDES Ziel gelatcht
80035614  _ori   a1,zero,0xc0
80035618  lui    a0,0x800b
8003561c  lw     a0,-0x3404(a0)           ; DAT_800acbfc = Ziel-Entity (player+0x1a8, von FUN_8003703c)
80035620  jal    FUN_8001a8f8             ; AUTO-TRACK: Yaw-Slew auf Zielposition
80035624  _addiu a0,a0,0x34               ;   a0 = &ziel->x (+0x34), Slew-Rate 0xC0
LAB_80035628
80035628  lui    a0,0x800b
8003562c  lw     a0,-0x3898(a0)           ; DAT_800ac768
80035630  nop
80035634  andi   v0,a0,0x8
80035638  beq    v0,zero,LAB_80035684
8003563c  _andi  v0,a0,0x2
; Pad 0x8: Aim-Yaw -= Waffen-Schritt
80035640  lui    v1,0x800b
80035644  lbu    v1,-0x35a3(v1)           ; DAT_800aca5d (Waffen-Id)
80035648  nop
8003564c  addiu  v1,v1,-0x1
80035650  sll    v0,v1,0x2
80035654  addu   v0,v0,v1                 ; (id-1)*5
80035658  lui    at,0x8007
8003565c  addiu  at,at,0x4090             ; Tabelle 0x80074090, 5-Byte-Records
80035660  addu   at,at,v0
80035664  lbu    v1,0x0(at)               ; Byte0 = 0x18 = 24 (Messer-Record)
80035668  lui    v0,0x800b
8003566c  lhu    v0,-0x3542(v0)           ; DAT_800acabe
80035670  nop
80035674  subu   v0,v0,v1
80035678  lui    at,0x800b
8003567c  sh     v0,-0x3542(at)
80035680  andi   v0,a0,0x2
LAB_80035684
80035684  beq    v0,zero,LAB_800356cc
80035688  _clear a2                       ; a2 := 0 = VORWAERTS (Delay-Slot, laeuft immer)
; Pad 0x2: Aim-Yaw += Waffen-Schritt  (0x8003568c-c8, spiegelbildlich zu oben)
8003568c…c8  … DAT_800acabe += tabelle74090[(aca5d-1)*5]
LAB_800356cc
800356cc  lui    a0,0x800b
800356d0  lw     a0,-0x343c(a0)           ; DAT_800acbc4
800356d4  lui    a1,0x800b
800356d8  lw     a1,-0x3438(a1)           ; DAT_800acbc8
800356dc  jal    FUN_8001f314             ; Clip 13 vorwaerts
800356e0  _ori   a3,zero,0x200
800356e4  lui    v1,0x800b
800356e8  lbu    v1,-0x35a5(v1)           ; Phase
800356ec  lui    a0,0x800b
800356f0  lw     a0,-0x3444(a0)           ; DAT_800acbbc = Zeiger auf AKTUELLES Keyframe-Flags-Wort
800356f4  addu   v1,v1,v0                 ; Phase += f314-Rueckgabe (fertig → 2)
800356f8  lui    at,0x800b
800356fc  sb     v1,-0x35a5(at)
80035700  lw     v0,0x0(a0)               ; Keyframe-Flags lesen
80035704  lui    v1,0x1
80035708  and    v0,v0,v1                 ; Bit 0x10000 = ATTACH-EVENT im Draw-Clip
8003570c  beq    v0,zero,LAB_80035780     ; kein Event → fertig fuer diesen Frame
80035710  _clear a0
; ---- Keyframe-Event: Messer-Hand-Mesh einwechseln ----
80035714  lui    v0,0x800b
80035718  lw     v0,-0x3424(v0)           ; DAT_800acbdc = Posebuffer
8003571c  lui    v1,0x800c
80035720  lw     v1,-0x4b4c(v1)           ; DAT_800bb4b4 ┐ die 4 vom Bank-Loader
80035724  lui    a0,0x800c                ;              │ beiseitegelegten Zeiger
80035728  lw     a0,-0x41fc(a0)           ; DAT_800bbe04 │ (Stow @0x80036d74-8c,
8003572c  lui    a1,0x800c                ;              │  nur fuer aca5d==1
80035730  lw     a1,-0x7678(a1)           ; DAT_800b8988 │  @0x80036d34-38)
80035734  lui    a2,0x800c                ;              │
80035738  lw     a2,-0x4270(a2)           ; DAT_800bbd90 ┘
8003573c  sw     v1,0x76c(v0)             ; pose+0x76c := bb4b4  (Hand-Teil A, = plw+0xc  beim Stow)
80035740  sw     a0,0x774(v0)             ; pose+0x774 := bbe04  (Hand-Teil B, = plw+0x28 beim Stow)
80035744  sw     a1,0x770(v0)             ; pose+0x770 := b8988
80035748  sw     a2,0x778(v0)             ; pose+0x778 := bbd90
8003574c  lui    v0,0x800b
80035750  lw     v0,-0x35ac(v0)           ; DAT_800aca54
80035754  lui    v1,0x800b
80035758  lw     v1,-0x387c(v1)           ; DAT_800ac784 = Spieler-Entity
8003575c  ori    v0,v0,0x4000
80035760  lui    at,0x800b
80035764  sw     v0,-0x35ac(at)           ; DAT_800aca54 |= 0x4000  ("Messer in der Hand"-Latch)
80035768  lw     v0,0x188(v1)             ; entity+0x188 = Kine/Posebuffer-Zeiger (== DAT_800acbdc)
8003576c  j      LAB_8003577c
80035770  _sw    zero,0xa14(v0)           ; kine+0xa14 := 0 (Attach-Sperre AUF; Equip setzt 1 @0x800466b4)
; ---- Phase 2: Uebergang ----
LAB_80035774
80035774  lui    at,0x800b
80035778  sh     v0,-0x35a6(at)           ; DAT_800aca5a := 1 (v0=1 aus Delay 0x8003557c) → HOLD
LAB_8003577c
8003577c  clear  a0
LAB_80035780
80035780  jal    FUN_800369f8             ; Root-Motion Kanal 0
80035784  _clear a1
80035788  lw     ra,0x14(sp)
8003578c  lw     s0,0x10(sp)
80035790  addiu  sp,sp,0x18
80035794  jr     ra
80035798  _nop
```

### 3.2 Pseudo-C

```c
void FUN_80035538(void)                    /* Melee-Sub 4 = DRAW; Einstieg: Kalt-Entry
                                              aca5a==0 && !(aca54&0x4000) → 4 @0x80034e90-a8 */
{
    switch (DAT_800aca5b) {
    case 0:                                               /* Init, faellt in den Tick durch */
        DAT_800aca5b = 1;
        DAT_800acae8 = 0x0d;                              /* DRAW-Clip 13 @0x8003558c-94 */
        DAT_800acae9 = 0;  DAT_800acae3 = 7;
        DAT_800acae0 = 0;                                 /* keine Vorwaertsbewegung @0x800355b8 */
        DAT_800acaec = (DAT_800acaec & 0x1fff) | 0x4000;  /* Aim neutral @0x800355bc-c8 */
        DAT_800acaf3 = FUN_8003703c(2000);                /* Auto-Aim-Latch, Radius 0x7d0 @0x800355cc-d0 */
        DAT_800aca3c |= 0xC0;                             /* @0x800355ec */
        FUN_80045024(0x1080001, &DAT_800aca88);           /* Zieh-SE @0x800355f8 */
        /* fallthrough */
    case 1:                                               /* Tick */
        if (DAT_800acaf3 & 1)                             /* nur lebendes Ziel (Scan-rv 1) */
            FUN_8001a8f8(DAT_800acbfc + 0x34, 0xC0);      /* Auto-Track-Slew @0x80035618-24 */
        if (DAT_800ac768 & 8) DAT_800acabe -= tbl_74090[(DAT_800aca5d-1)*5];  /* @0x80035664-7c */
        if (DAT_800ac768 & 2) DAT_800acabe += tbl_74090[(DAT_800aca5d-1)*5];  /* @0x800356b0-c8 */
        DAT_800aca5b += FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 0, 0x200);   /* fertig → Phase 2 */
        if (*DAT_800acbbc & 0x10000) {                    /* Keyframe-ATTACH-Event @0x80035700-08 */
            pose[0x76c] = DAT_800bb4b4;  pose[0x774] = DAT_800bbe04;  /* Messer-Hand-Mesh rein */
            pose[0x770] = DAT_800b8988;  pose[0x778] = DAT_800bbd90;  /* @0x8003573c-48 */
            DAT_800aca54 |= 0x4000;                       /* in-hand-Latch @0x8003575c-64 */
            *(int*)(*(int*)(DAT_800ac784+0x188) + 0xa14) = 0;   /* Attach-Sperre auf @0x80035768-70 */
        }
        break;
    case 2:
        *(uint16_t*)&DAT_800aca5a = 1;                    /* → HOLD (Melee-Sub 1) @0x80035774-78 */
        break;
    }
    FUN_800369f8(0, 0);                                   /* Kanal 0 @0x80035780-84 */
}
```

### 3.3 Der Attach-Mechanismus komplett (Gegenstellen ausserhalb der Funktion)

1. **Equip** (@0x80046680-b4): `DAT_800aca5d := neue Waffen-Id` @0x80046688;
   `DAT_800aca54 &= ~0x4000` @0x80046694-a0 (li -0x4001 = Maske 0xFFFFBFFF);
   `kine(DAT_800acbdc)+0xa14 := 1` @0x800466ac-b4 (Delay-Slot des jal FUN_80036b68 = Bank-Lade-Kick).
   → Nach jedem Equip ist das Messer "nicht in der Hand", der Draw spielt erneut.
2. **Bank-Loader** (FUN_80036b80-Familie, @0x80036d04-9c): setzt normal
   `pose+0x76c := plw+0xc`, `pose+0x774 := plw+0x28` (@0x80036d14-1c, Waffen-Hand-Mesh).
   **NUR wenn `DAT_800aca5d == 1`** (@0x80036d34-38, Messer): die vier Zeiger pose+0x76c/774/770/778
   werden nach `DAT_800bb4b4 / DAT_800bbe04 / DAT_800b8988 / DAT_800bbd90` **beiseitegelegt**
   (@0x80036d74-8c) und durch die Blosse-Hand-Zeiger `DAT_800b2b2c / DAT_800b521c / DAT_800b2848 /
   DAT_800b2b48` ersetzt (@0x80036d90-9c).
3. **Draw-Event** (diese Funktion, @0x8003573c-70): tauscht beim Keyframe-Bit 0x10000 die
   beiseitegelegten Messer-Hand-Zeiger zurueck ein — der Griff zum Messer wird im richtigen
   Animations-Frame sichtbar — und setzt Latch + `+0xa14=0`.
4. `+0xa14` wird ausserdem @0x80031abc genullt (wenn DAT_800aca5c ∈ {4,5,6}; Nachbar-Action,
   nicht weiter verfolgt). Einzige Setzer-Stelle auf 1: der Equip @0x800466b4.

`DAT_800acbbc` = Zeiger auf das Flags-Wort des aktuellen Keyframes (kein statischer Schreiber im
Dump — wird vom Anim-System indirekt gepflegt). Gleiche Nutzung als Event-Kanal: Fusstritt-SE
`*ptr & 0x4000` mit Fussauswahl `bit 0x1000` → `FUN_80045630(0, 7-3*bit)` @0x80030bb4-e0 (Gehen)
und @0x80035884-b0 (Action-8-FSM). Bit 0x10000 ist das Attach-Event des Draw-Clips; WELCHER
Keyframe des Clips 13 es traegt, steht in den PLW-EDD-Assetdaten (PL00W01), nicht im Code.

### 3.4 Konstanten

| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x8003558c-94 | Clip 0x0D (13) | DRAW-Clip der Melee-W-Bank (PLW-verifiziert 15 Frames, Port player_common.c) |
| 0x800355b8 | acae0=0 | Vorwaerts-Speed aus (Spieler wurzelt) |
| 0x800355bc-c0 | (…&0x1fff)\|0x4000 | Aim-Wort: hoch/tief loeschen, Neutral-Bit 14 setzen |
| 0x800355d0 | 0x7d0 (2000) | Auto-Aim-Latch-Radius des DRAW (RAISE-Sub0: 0x1388=5000 @0x80034f7c; Gun-FSM: 30000) |
| 0x800355d4+d8 | 0x0108_0001 | Zieh-SE (Klasse 1, SE-Index 8; Feld-Split FUN_80045024 @0x80045028/78) |
| 0x800355ec | \|=0xC0 | DAT_800aca3c Aim-Flags (Gegenstelle: LOWER loescht @0x800354f8-1c) |
| 0x80035614+24 | Slew 0xC0, Ziel+0x34 | FUN_8001a8f8 Auto-Track (Ziel-Entity aus DAT_800acbfc; +0x34=x, +0x3c=z — Scan @0x800370c8-f8) |
| 0x8003565c / 0x80035664 | 0x80074090, Byte0=0x18 | Aim-Dreh-Schritt je Waffe (5-Byte-Records; Messer-Record Byte0 = 24) |
| 0x80035704-08 | 0x10000 | Keyframe-Event-Bit "Waffe attachen" im Flags-Wort *DAT_800acbbc |
| 0x8003573c-48 | +0x76c/774/770/778 | Posebuffer-Slots der Hand-Bone-Meshteile (Loader-Gegenstelle @0x80036d14-1c/90-9c) |
| 0x8003575c-64 | aca54\|=0x4000 | "Messer in der Hand"-Latch (Kalt-Entry-Gate @0x80034e9c; Equip-Reset @0x8004669c) |
| 0x80035770 | kine+0xa14=0 | Attach-Sperre aufheben (Equip setzt 1 @0x800466b4) |
| 0x800356e0 | 0x200 | Anim-Schrittweite |
| 0x80035688 | a2=0 | VORWAERTS |
| 0x80035774-78 | aca5a=1 | Phase 2 → HOLD (ein Frame Verzoegerung nach Clip-Ende, weil Phase-2 erst im Folge-Frame faellt) |

Uebergaenge: Phase 0 → 1 (sofort, Init faellt in Tick durch); Phase 1 → 2 bei Clip-Ende
(f314-Rueckgabe addiert auf Phase @0x800356f4); Phase 2 → Substate 1 (HOLD). Kein Abbruchpfad:
der Draw spielt immer zu Ende, R1-Release wirkt erst im HOLD.

---

## 4. Port-Abgleich (re15_port)

- `engine/src/player_common.c` bildet alle drei Subs bereits ab und ist in Clips/SE/Fenster/Radien
  deckungsgleich mit diesem Dossier: SLASH 7/9/11 + SE 0x1050001 + Fenster 6..11
  (re15_player_fire_start / re15_player_slash_window), LOWER Clip 6 rueckwaerts (RE15_AIM_LOWER),
  DRAW Clip 0xD + SE 0x1080001 + Latch-Radius 2000/5000 + Slew 0xC0 (Aim-Block ~Z.700-770).
- **Divergenz-Kandidat (nicht Teil dieses Auftrags, nur Befund):** `platform/pc/main.c` ~Z.7559
  rendert das In-Hand-Waffen-Mesh "UNCONDITIONALLY when aca5d!=0". Das Original zeigt beim
  **Messer (aca5d==1)** bis zum Draw-Event die blosse Hand (Loader-Stow @0x80036d74-9c,
  Attach erst @0x8003573c-48). Der Port besitzt den Latch (`s_knife_in_hand`), konsultiert ihn
  im Render-Attach aber nicht.
- Die 16-Clip-Frage der Dauerfeuer-Baenke (W0C/W0E/W13, Clips 14/15) ist von diesem Dossier
  unberuehrt: Clips 13/14/15 der MELEE-Baenke bzw. Dauerfeuer-Subs 0-4 sind andere Baustellen;
  die hier behandelten Subs 7-9 laufen nie fuer W0C/W0E/W13 (§0).

## 5. RE2-Abgleich

Nicht benoetigt — alle Fragen liessen sich RE1.5-intern schliessen (Stow/Swap-Zeigersymmetrie
Loader↔Draw ist ein woertlicher 4-Zeiger-Match). Versucht wurde: Suche nach den SE-Literalen
0x1050001/0x1080001 in `ghidra_re2_Leon.txt` und `RE2_Quellcode_V2/` — kein Treffer (RE2 packt
SE-Aufrufe anders); keine benannten Messer-FSM-Decompiles im RE2-Bestand.
