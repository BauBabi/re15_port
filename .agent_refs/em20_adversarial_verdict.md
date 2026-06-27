# em20 (0x20) Adversarial Verification Verdict

## type_hex
0x20 = em20 MODEL id. VERIFIED NOT the entity dispatch-class byte. FUN_8011e064 installs the
6 STAGE1 enemy inits at table slots 0x80072cac/cb4/cc0/cc8/cd0/cd8 = entity[2] dispatch classes
0x40/0x42/0x45/0x47/0x49/0x4B (computed (slot-0x80072bac)/4). Runtime class is 0x40-family, NOT 0x20.
Using .type=0x20 as a def KEY is fine, but the spec framing "entity[2]=class -> em20 init" conflates
the model number with the dispatch class. Which of the 6 slots is em20 is itself UNPROVEN from cited sources.

## name
em20 Large biped (rooms 1190/1230/3060; arg3 0/64/65). REFUTE the "33-node" claim: bones=15
(em20_animations.json boneCount=15; FUN_8011d598 hierarchy idx0..14). 28 anims CONFIRMED.

## archetype
RE15_ARCH_WALKER_GRABBER CONFIRMED. Melee contact-gated lunge (entity[0x1c2]&1), no ranged/no prone-eat.
arg3 fast-start flag (entity[9]&0x40 -> entity[1]=4, entity[5]=6) VERIFIED byte-identical in all 4 init
variants FUN_8011c4ac/cab8/d598/db70. HIGH for archetype shape; walk/attack/hit/death clip indices
UNCONFIRMED (no em20 savestate) - keep flagged, do not ship guessed clips.

## fsm
CONFIRMED: dispatch FUN_8001a50c (pool &DAT_800acc2c, stride 0x7d words=0x1f4, table at
&DAT_80072bac+entity[2]*4); install FUN_8011e064 (6 inits 0x8011c5a0/cb70/d140/d6d4/dc68/e22c, sound
handler DAT_800ac998=&LAB_8011efc4); wrapper 0x8011bda0 (pause gate !(DAT_800aca40&0x20000000) &&
!(entity[9]&0x20); state dispatch @0x8011be38 lui 0x8012/addiu 0x1598 -> table @0x80121598 by entity[4];
anim advance func_0x8003b0a4(entity[0x34], *(entity[0x78]+6), 4); tail func_0x8001b064); active state0
FUN_8011dd18.
STATE TABLE @0x80121598 values [0]0x8011c6dc [1]0x8011c884 [2]0x8011ca48 [3]0x8011ca90 [4]0x80050be8
[5]0x8011c950 [6]0x8004f100 [7]0x8004f3a4 are RAM-sourced (a room1140 ZOMBIE save); disasm proves only
the ADDR + entity[4] indexing. Slots 0/1/5 independently confirmed by overlay bodies (FUN_8011c6dc
nested-dispatch entity[9]&0xf @0x8012167c then entity[5] @0x80121680/16a0/16c0/16e0; FUN_8011c884 fires
func_0x800369f8(0,0)/(0,1) on entity[0x1c4]&1/&2; FUN_8011c950 APPROACH dist=func_0x80065f60(dx2+dz2)
from (DAT_800aca88-entity[0x34]),(0x800aca90-entity[0x3c]) -> entity[0x1d0] then subtable @0x80121738).
Slots 2/3 engine tails; 4/6/7 not in slice (MEDIUM).
REFUTED: generic walker substates 0x8004f100/0x8004f5e8 are UNDISASSEMBLED (raw ?? bytes in
ghidra1_V2.txt lines 170157/171413), so their ranges 0x5dc/0x4b0/0x40/0x7d1 are UNVERIFIED, NOT disasm-cited.

## anim_map
CONFIRMED byte-true: FUN_8001f314 reads *(byte*)(DAT_800ac784+0x94) as the clip index into the EDD
table (param2+clip*4) and +0x95 as frame, so clip identity == entity[0x94] (same mechanism as zombie).
INIT clip=2 CONFIRMED but CITATION ADDR WRONG: the entity[0x94]=2 write is `sb 2,0x94` at 0x8011bfc0
(em20_disasm.txt line 139), NOT 0x8011d678. The init funcs FUN_8011c4ac/cab8/d598/db70 instead write
entity[0x25]=2 (a separate base-clip slot) + entity[0x95]/0x8f/0x93=0, and load
func_0x8001f314(entity[0x21],entity[0x5b],0,0x200); they do NOT touch 0x94.
EDD re-parsed (em20_animations.json): 28 clips, 15 bones, fps30; frame counts match spec EXACTLY:
0:49 1:18 2:30 3:14 4:14 5:14 6:18 7:15 8:30 9:35 10:35 11:14 12:42 13:14 14:44 15:14 16:27 17:14
18:50 19:29 20:17 21:13 22:20 23:29 24:18 25:40 26:30 27:99.
ACTION->CLIP map UNRESOLVED (spec correctly flags): clip set by entity[5]/[6] subtables
(entity[0x1e]=DAT_80121664/734/79c/8d4, RAM); pinning walk/attack/hit/death needs an em20 savestate
(1190/1230/3060). None of the 9 HASH saves are em20. Structural clip guesses are INFERENCE, not proof.

## sound_map
CONFIRMED structurally: chain situation -> func_0x800453d0(ID) -> tone(ID+1) -> VagAtr -> em_NN(=VAG-1);
overlay handler DAT_800ac998=&LAB_8011efc4 (FUN_8011e064); SE flush FUN_8011c884 reads entity[0x1c4]&1
-> func_0x800369f8(0,0) and &2 -> func_0x800369f8(0,1) (em20_disasm.txt lines 300-320). Concrete
situation->ID->em_NN UNRESOLVED: needs an em20 savestate for scripts/em_sound_map.py; the zombie ID map
is a TEMPLATE only.

## attack
CONFIRMED (FUN_8011dd18.c): DETECT 15000 (func_0x800509e4 arg0); locked-on scale 0x578=1400 / free 700
(func_0x8005070c); turn-rate 0x514=1300 (func_0x800509e4 arg2); ATTACK GATE (entity[0x1c2]&1) contact AND
DAT_800acae0>100 global aggro -> entity[4]=2/entity[5]=4/entity[6]=0/FUN_8011e634() = melee LUNGE/GRAB
on contact, not ranged. FUN_8011e634 body not in slice -> attack clip + lunge frames UNCONFIRMED.
UNVERIFIED (undisassembled substates 0x8004f100/0x8004f5e8): aggro 0x4b0=1200, pursue 0x5dc=1500,
walk 0x7d1=2001, contact 0x40=64. turn-engage 0x800=2048 (FUN_8011c654/d740) MEDIUM.
COLLISION CONFIRMED EXACTLY (FUN_8011d598 + disasm 0x8011bf90-bfb0): work@entity[0x62]+0x5f8 w=0x40(64)
h=0x30(48) d=0x2c8(712); +0x5f4=0 +0x5f6=0 +0x5fe=0x138(312). Shadow func_0x8001af5c(0,0,600,700,...,0x808080).
hp entity[0x9e]=0x78(120), entity[0x1b9]=8, entity[0x9a]=0xffff, entity[1]=1, status|=0x40000000,
entity[0x6e]=0 CONFIRMED. entity[0x6a]=&DAT_800aca54 PLAYER pointer CONFIRMED -> REFUTES entity[0x6a]=heading.
Knockback via shared FUN_80012d60 / PTR_LAB_800741a8 (framework-consistent, not traced for em20, MEDIUM).
Front/back polarity via shared FUN_8001a7a8 (assumed).

## def_row
SHIP (disasm-cited): type=0x20, arch=WALKER_GRABBER, clip_idle=2 (entity[0x94]=2 @0x8011bfc0, 30f),
rise_frames=30, turn_rate=1300, detect=15000, collision w64/h48/d712, hp entity[0x9e]=120.
UNVERIFIED placeholders (do NOT ship as fact; need em20 savestate): clip_walk, clip_attack, clip_eat,
clip_stagger, clip_death, loco_clip, aggro/disturb=1200, lunge=1500, bite/grab/eat=64, step,
turn_engage=2048, cos_cone2, lunge_frames/lunge_hit/recover_frames, se_attack/se_eat/se_kick.
NO new archetype step fn (reuses walker_grabber); ONE new arg3 fast-start flag.

## citations
extracted/PSX/EMD/CDEMD0/em20.emd (166912=0x28c00); extracted/PSX/EMD/CDEMD0/em20_animations.json
(boneCount=15, fps30, 28 clips, frame counts re-verified); RE_15_Quellcode_V2/FUN_8001a50c.c,
FUN_8001f314.c; RE_15_Quellcode_Overlays/STAGE1/FUN_8011e064.c, FUN_8011d598.c, FUN_8011db70.c,
FUN_8011cab8.c, FUN_8011c4ac.c, FUN_8011dd18.c; .agent_refs/em20_disasm.txt; ghidra1_V2.txt lines
88539 (FUN_8001ab9c), 170157 & 171413 (0x8004f100/0x8004f5e8 undisassembled).

## confidence
MIXED. Mechanism HIGH; concrete clip/sound/distance numbers LOW/UNPROVEN.
5 REFUTED/CORRECTED vs the spec:
(1) "33-node" -> 15 bones.
(2) entity[0x94]=2 citation "0x8011d678" -> actual 0x8011bfc0; inits write entity[0x25]=2 not 0x94.
(3) class "0x20" / table "0x80072be0+" -> slots 0x80072cac+ = classes 0x40/0x42/0x45/0x47/0x49/0x4B; 0x20 is the model id.
(4) walker substate ranges 0x4b0/0x5dc/0x7d1/0x40 "disasm-cited" -> substates undisassembled, values UNVERIFIED.
(5) entity[0x6a]=heading -> actually the PLAYER target pointer (&DAT_800aca54).
CORRECTLY FLAGGED by the spec (agree): action->clip map, step speed, situation->ID->em_NN all need an
em20 savestate (1190/1230/3060); none of the 9 HASH saves qualify.
