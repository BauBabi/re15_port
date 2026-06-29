#!/usr/bin/env python3
"""re15_enemy_state.py - dump the live ENEMY-AI state from an RE1.5 (MZD) savestate.

Reads the enemy array DAT_800acc2c (stride 0x1f4) + the player block and labels
each enemy's FSM state from the byte-true dispatch map RE'd 2026-06-29 (see
RE15_FUN_CATALOG.md "Enemy AI" section + the reai-v2-foundation-combat memory):

  main state  entity+0x4  -> PTR_FUN_801217a0[+0x4]:
                0=init(FUN_8011d84c) 1=active-AI(FUN_8011d9f4) 2=hurt 3=death
                4=idle(0x80050be8)            (2/3 also written by the damage resolver)
  sub-state   entity+0x9  ((&0xf) -> PTR_FUN_801217b4[idx]; 0 = sub-behaviour 0 =
                FUN_8011da48, the active humanoid AI; live STAGE1 enemies use 0)
  anim-phase  entity+0x5  (FUN_8011da48 dispatches DAT_801217b8[+0x5]; the attack-
                commit 0x701 sets +0x5=7 = the lunge anim-phase)
  action idx  entity+0x0 low byte (model-instance action table @0x80071d40;
                0x16..0x19 = the lunge action sequence = the mid-attack HITBOX window)
  hitbox dims *(entity+0x78): u16 radius_min@+6, height@+8, radius_max@+10

Reuses re15_ss.Ram (decompress + PSX.EXE-signature RAM-base). Stdlib + zstandard.
Usage:  python re15_enemy_state.py <savestate.sav>
"""
import sys, os
from re15_ss import Ram

ENEMY_BASE = 0x800acc2c
STRIDE     = 0x1f4
COUNT_ADDR = 0x800aca4e
MAX_SCAN   = 16

MAIN = {0: "init", 1: "active-AI", 2: "hurt", 3: "death", 4: "idle"}

def label(s4, s5, act_lo):
    base = MAIN.get(s4, "state%d" % s4)
    if s4 == 1:
        if 0x16 <= act_lo <= 0x19:
            return "ATTACK (lunge action 0x%02x = hitbox window)" % act_lo
        if s5 == 7:
            return "active: attack lunge anim-phase (+0x5=7)"
        return "active: approach (+0x5=%d)" % s5
    return base

def main():
    if len(sys.argv) < 2:
        print("usage: re15_enemy_state.py <savestate.sav>"); return 1
    r = Ram(sys.argv[1])
    print("file         :", os.path.basename(sys.argv[1]))
    print("RAM base off : 0x%x" % r.base)
    print("player HP    : %d   (+0x9a of the player block)" % r.s16(0x800acaee))
    print("player state : %d   (player block +0x4; 7 = death/grabbed)" % r.u8(0x800aca58))
    print("active count : %d   (DAT_800aca4e)" % r.u8(COUNT_ADDR))
    print()
    cols = ("slot", "type", "+0x4", "+0x5", "+0x9(&f)", "act", "dist", "flags", "hitbox", "label")
    print("%-4s %-5s %-5s %-5s %-9s %-5s %-7s %-7s %-9s %s" % cols)
    print("-" * 92)
    for i in range(MAX_SCAN):
        a = ENEMY_BASE + i * STRIDE
        if (r.u32(a) & 1) == 0:
            continue
        typ = r.u8(a + 0x8); s4 = r.u8(a + 4); s5 = r.u8(a + 5); s9 = r.u8(a + 9)
        act_lo = r.u32(a) & 0xff; dist = r.u32(a + 0x1d0); fl = r.u16(a + 0x1d8)
        hb = "-"
        p = r.u32(a + 0x78)
        if 0x80000000 <= p < 0x80200000:
            rmin = r.u16(p + 6); h = r.u16(p + 8); rmax = r.u16(p + 10)
            hb = "%d/%d%s" % (rmin, h, "" if rmin == rmax else "/%d" % rmax)
        print("%-4d 0x%02x  %-5d %-5d 0x%02x(%d)  0x%02x  %-7d 0x%-5x %-9s %s" % (
            i, typ, s4, s5, s9, s9 & 0xf, act_lo, dist, fl, hb, label(s4, s5, act_lo)))
    return 0

if __name__ == "__main__":
    sys.exit(main())
