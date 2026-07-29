# RE2 ZOMBIE — ATTACK ARBITRATION + ATTACK SUBSTATES — PORT SPEC
Binary EMZ0.BIN @0x80100000. `self`=s0, `PL`=0x800CFBF8 (s1). All re-disassembled and byte-checked.

## 1. THE LADDER — `DECISION[1]` = 0x80101714

Called from 0x8010118C: `lbu +5` → DECISION[0x8010C88C] → **re-read `lbu +5`** → EXECUTOR[0x8010C8CC] @0x801011D0-EC. **A store here runs its executor the SAME tick.**

```c
void decision_1(Ent*self,a1,a2){            // @0x80101714
  s32 s2 = *(s32*)(self+0x1F0);                       // @0x80101744
  s32 s4 = cone(self,PL.x,PL.z,1024);                 // jal @0x80101748, a3 @0x8010174c; captured @0x8010176c
  s32 s3 = cone(self,PL.x,PL.z, 512);                 // jal @0x80101768, a3 @0x80101754; captured @0x8010178c
       (void)cone(self,PL.x,PL.z,1300);               // jal @0x80101788 — RESULT DEAD (v0 clobbered @0x80101790)
  // cone() = FUN_80015614: returns 0 when target INSIDE ±a3, else +a3/-a3 (@0x80015668-80)

  if (self[0x23E] == 0) {                             // lbu @0x80101790 / bne @0x8010179c
    /* A */ u32 d = *(u32*)(self+0x1F4);              // @0x801017a4
    if ((d & 0xC0000000) &&                           // @0x801017a0/ac/b0
        (d & 0x3FFFFFFF) < 2000 &&                    // @0x801017c0 (sltiu 0x7d0)
        FUN_80015714(self,(s16)self[0x1F8],256)==0){  // @0x801017cc-d8
        *(u32*)(self+4)=0x00000E01; return;           // @0x801017dc/e0/e4  *** ONLY EARLY RETURN ***
    }
    /* B */ // self[0x23E]==0 re-tested @0x801017e8/f0 — DEAD, drop it
    if ((s16)s3 == 0                                  // sll @0x801017f4 / bne @0x801017f8
     && (u32)s2 < 2000                                // sltiu @0x801017fc / beq @0x80101800  (UNSIGNED)
     && self[0x106] != PL[0x106]                      // lbu @0x80101808/0c, beq-away @0x80101814 (10620006 = BEQ)
     && PL[0x1D3] == 0)                               // whole byte, lbu @0x8010181c / bne @0x80101824
       *(u32*)(self+4)=0x00000E01;                    // @0x80101828/2c   (no return — falls through)
  }
  /* C */ if ((*(s16*)(self+0x1D4) & 0xC000)          // lh @0x80101830 / andi @0x80101838
           &&  (*(u32*)(self+0x110) & 1))             // lw @0x80101844 / andi @0x8010184c
       *(u32*)(self+4)=0x00000A01;                    // @0x80101854/58
  /* D */ if ((u32)s2 < 3500                          // sltiu 0xdac @0x8010185c (bytes ac 0d 42 2e)
           &&  (s16)s4 != 0                           // beq-away @0x80101868  → OUTSIDE ±1024 required
           && (*(u16*)0x800CFBF6 & 0x15)              // lhu @0x80101874 / andi @0x8010187c
           && (rng() & 3) == 0)                       // jal 0x80015FE8 @0x80101888 / andi @0x80101890 / bne @0x80101894
       *(u32*)(self+4)=0x00000C01;                    // @0x8010189c/a0
  /* E */ if ((u32)s2 < 2500                          // sltiu 0x9c4 @0x801018a4
           &&  (s16)s4 != 0                           // beq-away @0x801018b0
           && (*(u16*)0x800CFBF6 & 0x17)              // andi @0x801018c4
           && (rng() & 1) == 0)                       // jal @0x801018d0 / andi @0x801018d8 / bne @0x801018dc
       *(u32*)(self+4)=0x00000C01;                    // @0x801018e0/e4
  // D and E are NESTED, not tiers: at s2<2500 BOTH run and BOTH draw the RNG.

  if (PL[0x8] != 15) {                                // lbu @0x801018e8 / addiu 15 @0x801018ec / beq @0x801018f0
    /* G */ if ((u32)s2 < 1200                        // sltiu 0x4b0 @0x801018f4 / beq @0x801018f8
             && !(PL[0x1D3] & 0x80)                   // andi @0x80101908 / bne @0x8010190c
             && self[0x106] == PL[0x106]) {           // bne-away @0x80101920
        if (!(*(u16*)(self+0x21A) & 0x20)             // lhu @0x80101928 / andi @0x80101930 / bne→G2 @0x80101934
         && FUN_80015758(self+0x38,PL+0x38,(s16)self[0x76]+256,256)==0){ // jal @0x80101948, a2 @0x8010194c, a3=256 @0x80101944
             *(u32*)(self+4)=0x00000301;              // @0x80101954/58
             PL[0x1D3] |= 0x80; }                     // @0x80101964/68     (falls into G2)
   G2:  if (!(*(u16*)(self+0x21A) & 0x40)             // andi @0x80101974 / bne @0x80101978
         && FUN_80015758(self+0x38,PL+0x38,(s16)self[0x76]-256,256)==0){ // jal @0x8010198c, a2 @0x80101990
             *(u32*)(self+4)=0x00000301;              // @0x80101998/9c
             PL[0x1D3] |= 0x80; }                     // @0x801019a8, sb @0x801019b0 (delay of j @0x801019ac)
     }
  } else {
    /* J */ if (self[0x23E]==0                        // lbu @0x801019b4 / bne @0x801019bc
             && (s16)s3 == 0                          // sll @0x801019c0 / bne @0x801019c4
             && (u32)s2 < 2000                        // sltiu @0x801019c8 / beq @0x801019cc
             && PL[0x1D3] == 0)                       // lbu @0x801019d4 / bne @0x801019dc
        *(u32*)(self+4)=0x00000E01;                   // @0x801019e0/e4   (J == B minus the +0x106 test)
  }
  /* K */ if (*(s16*)(PL+0x156) == -32768             // lh @0x801019e8 / addiu -32768 @0x801019ec / bne @0x801019f0
           && (s16)s3 == 0                            // sll @0x801019f4 / bne @0x801019f8
           && (u32)s2 < 1000) {                       // sltiu 0x3e8 @0x801019fc / beq @0x80101a00
        *(u32*)(self+4) = 0x00060801;                 // lui 0x6 @0x80101a04 + ori 0x801 @0x80101a0c / sw @0x80101a10
        *(u16*)(self+0x10E) |= 0x4000;                // lhu @0x80101a08 / ori @0x80101a14 / sh @0x80101a18
  }
}
```
**Winner rule:** last writer wins — 9 stores to +0x4 (0x801017E4, 82C, 858, 8A0, 8E4, 958, 99C, 9E4, A10); only 0x801017E4 returns (`j 0x80101a1c` @0x801017e0). No instruction in the function reads +0x4..+0x7, so sequential C is exact. If no block fires, +0x4 is untouched.
**RNG call COUNT is behaviour:** 0..2 draws/tick (D @0x80101888, E @0x801018d0), only after their first three gates. `FUN_80015FE8`: `s=[0x800CE318]; hi=(s>>7)&0xFF; lo=(hi+s)&0xFF; s=lo|(hi<<8) (16-bit); return lo` (@0x80015fe8-0x80016018). Seed 0xD2706CA4 @0x8002b91c.
**Word→bytes (LE):** 0x0E01→+4=1,+5=14; 0x0A01→+5=10; 0x0C01→+5=12; 0x0301→+5=3; 0x00060801→+5=8,+6=6. Every `sw` **zeroes +6/+7** — load-bearing (executors dispatch on +6).

## 2. FIELDS

| off | w | role |
|---|---|---|
| self+0x04 | u32 | routine word: +4 state idx (table 0x8010C830 @0x801004cc/e0), +5 sub idx, +6 phase. PROVEN by readers @0x801004cc, @0x801011a8/d0, @0x80104770 |
| self+0x76 | s16 | facing yaw (0..4095); read by FUN_80015714 @0x80015714, written @0x80104870 |
| self+0x106 | u8 | UNPROVEN. Writer @0x80036974 stores `sign(v)-trunc(v/1800)` from FUN_8004FBA0(self+0x84,450,16384) @0x8003692c; +1 at @0x8003ea50 |
| self+0x110 | u32 | UNPROVEN value. Writer FUN_8003567C: `=0` unconditionally @0x8003569C (delay slot), else `=FUN_8004C1BC(...)` @0x800356D8; called from zombie root @0x80100638 |
| self+0x144/146/148 | s16×3 | per-frame translation, rewritten from keyframe root motion by FUN_80015E7C @0x80015fd8-e4, consumed by FUN_800152C8 @0x80015304 |
| self+0x14C..14F | u8×4 | +0x14C clip id, +0x14D frame (incremented/wrapped by FUN_8002959C @0x80029b28-4c), +0x14E flags UNPROVEN |
| self+0x156 | s16 | HP. PROVEN: `lhu/subu/sh 342` @0x80040248-5c, clamp 0 @0x800402b0 |
| self+0x158 | s16 | dual use: turn delta (12) / mash meter (3). Writers @0x8010480c, @0x80102828 |
| self+0x1D2 | u8 | HURT column = zone + 3*(attack_word>>28). PROVEN @0x80047114/0x80047320/0x80047330 |
| self+0x1D3 | u8 | UNPROVEN. Generic per-entity byte; low7 self-decrement @0x80100484-98; bit0x80 set by G @0x80101968/0x801019b0 |
| self+0x1D4 | s16 | UNPROVEN. No writer in EMZ0; EXE 0xC000-capable writers @0x80055D90, @0x800570F4, @0x80080D90 |
| self+0x1F0 | s32 | 2D distance to PL. PROVEN writer @0x800265d4/e0 (isqrt of dx²+dz² over +0x38/+0x40); refreshed only if (self+0)&1 and !((+0x10E)&0x8000) @0x8002657c-9c |
| self+0x1F4 | u32 | bits31-30 = rank 1/2/3 (@0x80065788/7E4/840), bits29-0 = isqrt distance to R (@0x8006560c/20). R = entity with +0x8==69 or 79 (@0x80060608-18) |
| self+0x1F8 | s16 | bearing(self→R), 0..4095 (@0x8006576c-0x800657a4) |
| self+0x21A | u16 | bit0x20 blocks the +256 probe, bit0x40 blocks the −256 probe (5 identical reader pairs). Semantics UNPROVEN. Writers @0x801052E4/F4/FC, @0x80102A34/40, @0x80105E60/6C |
| self+0x23E | u8 | saturating countdown; reload 60 @0x80104e28/2c, decrement @0x80100470-80. **No initializer anywhere** |
| self+0x10E | u16 | low6 = variant (odd = crawl, table 0x8010C854 @0x8010115c-74); bit0x4000 set by K, cleared @0x80104f0c; role of 0x4000 UNPROVEN |
| PL+0x8 | u8 | UNPROVEN (compared to 15 @0x801018ec) |
| PL+0x156 | s16 | player HP (same writer as above, a2=0x800CFBF8 @0x800401e0) |
| PL+0x1B4 | ptr | grabber backlink; written @0x80102710. NOTE zombie root rewrites its OWN +0x1B4 = PL each tick @0x80100500 |
| 0x800CFBF6 | u16 | UNPROVEN bitfield. EMZ0 never writes it (6 reads only). EXE sets bit1 @0x8003D6B4 etc., clears 0-4 @0x8003BFE0 |
| 0x800CE318 | u16 | RNG state |

## 3. SUBSTATE 12 and SUBSTATE 3

### EXECUTOR[12] = 0x80104748 — no damage, no HP store (jal set: 15558,15614,5BD6C,2959C,15E7C,152C8)
```
phase +0x6 dispatch @0x80104770-b4:  0→0x801047B8 (falls into 1), 1→0x8010482C, 2→0x80104884 (falls into 3), 3→0x8010489C, else exit
P0: *(u32*)(self+0x14C)=0x00070019;  // clip 0x19, frame 0  — lui 0x7 @0x80104790 (delay!) + ori 0x19 @0x801047b8 / sw @0x801047c0
    self[6]=1 @0x801047c8; FUN_80015558(self,PL.x,PL.z,32) @0x801047dc;
    self+0x158 = cone(self,PL.x,PL.z,190) @0x801047f8, stored in delay slot @0x8010480c (UNCONDITIONAL);
    if(self[0x239]==0){ FUN_8005BD6C(10,self) @0x80104814; self[0x239]=150 @0x80104820; }
    self+0x144 = 11 @0x80104828  ← DEAD for motion (FUN_80015E7C overwrites +0x144 in P3)
P1: if(cone(self,PL.x,PL.z,320)==0){ self+0x158=0 @0x8010484c; self[6]=2 @0x80104850; }   // @0x80104838-40
    yaw += self+0x158 every tick  (@0x8010485c-70, sh in delay slot of the anim call)
    if(FUN_8002959C(self,a1,a2,512)!=0) self[6]=2   // @0x8010486c-80  (clip 0x19 wrapped; +0x158 NOT cleared)
P2: *(u32*)(self+0x14C)=0x0007001B @0x80104884-8c; self[6]=3 @0x80104894; falls into P3 same tick
P3: FUN_80015558(self,PL.x,PL.z,32) @0x801048a4; FUN_80015E7C(self,a1,a2,0) @0x801048b8;
    if(FUN_8002959C(self,a1,a2,512)!=0) *(u32*)(self+4)=0x00000101 @0x801048d8/dc;
    FUN_800152C8(self,0) @0x801048e4;
    if(self[0x14D]==25) *(u32*)(self+4)=0x00000101 @0x801048f4-fc
```
Exits only via those two sites → state 1 / sub 1. No timer.
`DECISION[12]` = 0x8010460C: `+0x6==3 && (u32)+0x1F0<1300 (@0x8010463c) && !(PL[0x1D3]&0x80) && +0x14D<11 (@0x80104668) && self[0x106]==PL[0x106] && (PL[0x8]^0xF)!=0` → the same two ±256 probes (@0x801046b0-0x80104718) → `sw 0x00000301` + `PL[0x1D3]|=0x80`.

### EXECUTOR[3] = 0x801025EC (GRAB) — shared by upright sub 3 and crawl sub 1 (table 0x8010C910/0x8010C91C)
Row `s5 = (self+0x10E & 1) + ((self[8]==23 || self[8]==17) ? 2 : 0)` @0x8010266c-90.
Data copied to stack: `0x8010000C = 0B 0B 0E 0E 0B 0B 0E 0E` (clips, sp+16) and `0x80100014 = 10 14 01 05 10 1E 01 0A` (frame,damage pairs, sp+64). Phase table @0x8010001C, 10 entries, guarded `sltiu +0x6,0xa` @0x8010269c.
```
P0 @0x801026C0: clip = tbl[2*s5] (0x0B upright / 0x0E crawl); +0x14C = 0x000F0000 + clip (ADD @0x801026d8-e0); +0x6=1;
   FUN_80015558(self,PL.x,PL.z,2048) @0x801026f4;
   PL+0x1B4 = self @0x80102710; PL+0x4 = (FUN_80015910(self,PL)<<8)|5 @0x80102718-28;
   if(s5&1) PL[0x5]+=2 @0x80102738-44;  PL+0x188=self+0x188 @0x80102748-50; PL+0x18C=self+0x18C;
   PL[0x1D3]|=0x80 @0x8010275c-60; self[0x1D3]=15 @0x8010276c-70; self[0]|=0x1000; PL[0]|=0x1000;
   moan gated by +0x239 then +0x239=150 @0x801027c0; FUN_800395B8(20,0,250,0)
P1 @0x801027D8: +0x6 += FUN_8002959C(self,a1,a2,256)   // 1→2 exactly on seize-clip wrap (@0x801027f8-0x80102810)
P2 @0x80102814: +0x6=3; +0x158=148 @0x80102828/2c; *(u32*)(self+0x14C) = clip+1 (0x0C / 0x0F) @0x80102830/34
                 — FULL WORD, so +0x14D/14E/14F all cleared. Falls into P3.
P3 @0x80102838: FUN_80015CB8(...,0); FUN_8002959C(...,256) @0x80102858;
   m = FUN_8001598C()           // @0x80102860; returns ([0x800CE310] & 0x34F)!=0, a RISING-EDGE pad word (@0x80039384)
   +0x158 = (u16)+0x158 - 2 - 5*m   // sll2+addu @0x80102868/70, addiu -2 @0x80102874, subu @0x80102878, sh @0x8010287c
   if ((s16)+0x158 < 0) { self[6]=4; PL[6]=4; }   // sll16 @0x80102880 / bgez @0x80102884 / @0x8010288c-94
   // no input → 148,146,… first negative on the 75th P3 tick
   if (self[0x14D] == tbl64[2*s5])                // 16 / 1 / 16 / 1  — @0x801028a0-ac
        { sfx; FUN_800401D4(a0 = tbl64[2*s5+1] = 20/5/30/10, a1 = s5&1); }   // @0x801028f4-fc
   // the meter test FALLS INTO this compare — escaping on the bite frame still takes the hit
   r = ret; if(r&1){ self[6]=4; PL[6]=4; PL[0x16A]=1; }        // @0x80102904-1c  ← normal kill
            if(r&2){ *(u32*)(self+4)=0x00000601 (delay @0x80102938);
                     PL+0x4=(FUN_80015910(self,PL)<<8)|6 → 0x800CFBFC @0x80102950;
                     [0x800CFB74]|=0x100 @0x8010295c; }        // ← overkill only
P4 @0x80102968: +0x6=5, +0x158=0, +0x16A=0, +0x14C = clip + 0x00070002 (release clip 0x0D/0x10)
P5→P7 (+0x14D==7) →P8 (+0x6 += clip-end) →P9 @0x80102EB4: *(u32*)(self+4)=0x00000001 (state 1, sub 0)
Other terminals: 0x00000501 @0x80102D2C, 0x00000007 @0x80102BF4.  **Substate 3 NEVER writes 0x0101.**
```
`FUN_800401D4` scales a0 before subtracting: ×1.5 if a0<41 && ([0x800CFB74]&0x40 || HP>=121) @0x800401ec-210; if [0x800D482A]==3 then ×5 (a0<30) else ×2 (a0<60) @0x80040218-44; `HP -= a0` @0x80040248-5c. Return enum {0 survived, 1 killing blow, 2 overkill/already-dead} @0x80040264-0x800402e8.

## 4. WHAT IS STILL UNKNOWN — with the next xref

1. **0x800CFBF6** (masks 0x15/0x17/0x04) — role unproven. Next: the ~13 writers of **0x800CFBDC** (gates the low-5 clear @0x8003BFC0) + a savestate watch on 0x800CFBF6 across one player frame. Note 0x80065C88 is shared by player and zombie tables → not proven player-exclusive.
2. **self+0x106** — quantizer proven, semantics not. Next: any *reader* other than the equality tests; FUN_8004FBA0 @0x8004fba0 args (self+0x84, 450, 16384).
3. **self+0x1D4 bits 0xC000** — next: decode which member ID maps to offset 468 in the setter jump table reaching @0x80055D90 (getter twin @0x8005608C), and identify `*[0x800AB21C]` + the caller of the bulk copy @0x80080BFC.
4. **self+0x110 value** — next: RE **FUN_8004C1BC** (globals 0x800D3908, 0x800C3B6C, 0x800CE324, 0x800CE330).
5. **PL+0x156 == -32768 (block K)** — no writer of 0x8000 to 0x800CFD4E exists; damage clamps 0 @0x800402b0, heal clamps to [0x800CFD5A] @0x80040340. Next: PCSX-Redux watchpoint on 0x800CFD4E for the whole session. **Until then K is unreachable-by-proof — port it but expect it never to fire.**
6. **self+0x10E bit 0x4000** (set by K) — 3 readers (@0x80100514, @0x80103824, @0x80103C9C) each skipping a `+0x6=` store; meaning unproven. Next: what 0x80103930 / 0x80103E24 do.
7. **PL+0x8 == 15** — identity of the id unproven. Next: the writer of PL+0x8 in the EXE.
8. **self+0x21A bits 0x20/0x40** — proven only as probe blockers. Next: the +0x1D0&0x80 branch @0x801052C0-F4 that selects data ptr +2064 vs +1548 off self+0x198.
9. **self+0x23E spawn value** — no initializer in either binary. Next: the entity-slot allocator feeding FUN_8001AD3C (field-by-field init @0x8001B0A0-15x).
10. **PL+0x1D3 release** — none of the 11 whole-byte clears is tied to grab exit. Next: watchpoint 0x800CFDCB across a live grab; candidates @0x801006C8, @0x8010499C, @0x8003C248.
11. **a3 of FUN_8002959C (256 vs 512)** — only the upper 16 bits are tested @0x800295b8-c4, so both take the same branch. Role unproven.
12. **[0x800D482A]** (damage ×5/×2 multiplier) — readers only, no writer found. Next: search all COMMON/BIN overlays for a store at that address.
13. **self+0x1F0 staleness** — refresh gated @0x8002657c-9c; unproven when those bits are clear in practice. Next: savestate sample.
14. **DECISION[0] @0x80101294 and DECISION[2] @0x80101F7C** use the SAME globals at different thresholds (0xBB8/0x7D0, masks 0x15/0x17/0x04). Not covered here — port separately, do not share code.
15. **Near-clone trap** @0x801013A4-D0: same lhu/andi 0x17/rng/0x0C01 but `beq` polarity — fires when bit0 **!=** 0. Do not reuse block E's implementation.

## 5. PORT ORDER

1. **RNG FUN_80015FE8** as a 16-bit global + 8-bit return. Proof: log the state after N calls from a fixed seed and diff against a PSX savestate read of 0x800CE318.
2. **cone helpers** FUN_80015614 / FUN_80015714 / FUN_80015758 with the *return 0 = inside* convention. Proof: unit-test the three branch outcomes (0, +a3, −a3) over a yaw sweep; assert `cone(...,512)==0` exactly on ±45°.
3. **Routine word as one u32**, and the two dispatch tables 0x8010C88C/0x8010C8CC with the **re-read of +5** between them. Proof: a decision that writes 0x0C01 must run executor 12 in the same tick — assert +0x6 transitions 0→1 in one frame.
4. **The ladder verbatim** (blocks A..K, single early return, last-writer-wins, RNG called exactly at D/E). Proof: instrument the port to dump `(s2, s3, s4, 0x800CFBF6, rng_calls, final +0x4)` per tick, and compare against a PSX trace at the same room/position.
5. **+0x23E countdown** (decrement @0x80100470, reload 60 @0x80104E2C) — it gates A/B/J. Proof: after any substate-14 completion, blocks A/B/J must be dead for exactly 60 ticks.
6. **EXECUTOR[12]** four phases, incl. the fall-throughs (0→1, 2→3 same tick) and the two exits. Proof: clip sequence 0x19 → 0x1B, and exit on frame 25 or clip wrap, never on a timer.
7. **EXECUTOR[3] P0–P2** (clip table 0x8010000C, player handoff, +0x158 = 148). Proof: PL+0x4 low byte becomes 5 (+2 when crawling) on the grab frame.
8. **Mash meter + bite** (−2/tick, −5/edge, `bgez` on the sign-extended s16; damage at frame 16/1 with arg 20/5/30/10). Proof: with zero input the release fires on P3 tick 75; with the pad held nothing changes (edge-only, mask 0x34F).
9. **FUN_800401D4 scaling + return enum**, then the r&1 / r&2 split (0x0601 only on overkill). Proof: HP delta for arg 20 at HP 121 = 30, not 20.
10. **Block K last** — behind an assert, since its −32768 gate has no proven producer (see Unknown #5).