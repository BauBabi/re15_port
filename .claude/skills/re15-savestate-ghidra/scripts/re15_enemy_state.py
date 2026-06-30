#!/usr/bin/env python3
"""re15_enemy_state.py - dump the live ENEMY-AI state from an RE1.5 (MZD) savestate.

Reads the enemy array DAT_800acc2c (stride 0x1f4) + the player block and labels each
enemy from the byte-true dispatch map for the LIVE STAGE1 zombies (types 0x10/0x11/0x16),
RE'd + savestate-verified 2026-06-29..30 (see reai-v2-foundation-combat memory + HANDOVER):

  per-frame loop FUN_8001a50c -> @0x80072bac[+0x8 type] -> (for 0x10/0x11/0x16) FUN_80100424
  main state  entity+0x4  -> @0x8011f7b4[+0x4]:  0=init 1=active 2=hurt 3=death (7=corpse)
  sub-state   entity+0x5  -> the m0 brain @0x8011f840[+0x5] (DECIDE) / @0x8011f890[+0x5] (ANIM):
                0=search 2=ENGAGE 3/4=GRAB 5/6=forward-walk 7=TURN-to-face 0xc=feeding
                0x13=engage+a (+0x90&3)-gated forward-walk pre-check
  variant     entity+0x9 (&0xf) -> @0x8011f80c[..] (m0=0 is the briefing combat path;
                6=feeding 8=lying are the pre-wake variants)
  hitbox      *(entity+0x78): u16 radius_min@+6, height@+8, radius_max@+10 (zombie=400/1440)

  IMPORTANT: this is the @0x8011f7b4 family. Type 0x47 (Elliot) uses a DIFFERENT parallel
  table @0x801217a0 with a different +0x5 meaning (+0x5=7 there = lunge, NOT turn) — do not
  mix them. The @0x8011f840/890 dispatch tables are STATIC in STAGE1.BIN (read with
  re15_disasm.py table; overlay off = addr-0x80100000, NO 0x800 header) and == live RAM;
  re15_runtime_table.py cross-checks them against a savestate.

  --ai also dumps the AI-internal fields the combat port needs and evaluates the +0x5=0x13
  forward-walk gate (so it never has to be re-derived in ad-hoc python):
    +0x90 forward-walk gate flag   +0x6a rot_y    +0x94 motion(clip)
    +0x6 sub-step   +0x1d4 hurt_clip   +0x1dc hit_stun(s16)
  forward-walk fires (in +0x5=0x13/9/10) only when (+0x90 & 3) != 0 AND
    ((((+0x90 & 0xf0)<<4) - rot_y + 512) & 0xfff) < 1024   (FUN_8010561c @0x80105638..58)

Reuses re15_ss.Ram. Stdlib + zstandard.
Usage:  python re15_enemy_state.py <savestate.sav> [--ai]
"""
import sys, os, argparse
from re15_ss import Ram

ENEMY_BASE = 0x800acc2c
STRIDE     = 0x1f4
COUNT_ADDR = 0x800aca4e
MAX_SCAN   = 16

MAIN = {0: "init", 1: "active", 2: "hurt", 3: "death", 4: "idle", 7: "corpse"}
SUB5 = {0: "search", 1: "search-alt", 2: "ENGAGE", 3: "GRAB(face)", 4: "GRAB(behind)",
        5: "walk", 6: "walk", 7: "TURN-to-face", 0xc: "feeding",
        0x13: "engage+fw-precheck"}

def label(s4, s5):
    if s4 != 1:
        return MAIN.get(s4, "state%d" % s4)
    return "active: " + SUB5.get(s5, "+0x5=0x%x" % s5)

def fw_gate(p90, rot):
    """Evaluate the +0x5=0x13 forward-walk gate (FUN_8010561c)."""
    if (p90 & 3) == 0:
        return False, None
    ang = (((p90 & 0xf0) << 4) - rot + 512) & 0xfff
    return (ang < 1024), ang

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sav")
    ap.add_argument("--ai", action="store_true", help="also dump AI-internal fields + forward-walk gate")
    a = ap.parse_args()
    r = Ram(a.sav)
    print("file         :", os.path.basename(a.sav))
    print("RAM base off : 0x%x" % r.base)
    print("player HP    : %d   (+0x9a of the player block)" % r.s16(0x800acaee))
    print("player state : %d   (player block +0x4; 7 = death/grabbed)" % r.u8(0x800aca58))
    print("active count : %d   (DAT_800aca4e)" % r.u8(COUNT_ADDR))
    print()
    if not a.ai:
        cols = ("slot", "type", "+0x4", "+0x5", "+0x9(&f)", "act", "dist", "flags", "hitbox", "label")
        print("%-4s %-5s %-5s %-5s %-9s %-5s %-7s %-7s %-9s %s" % cols)
        print("-" * 96)
    else:
        cols = ("slot", "type", "+0x4", "+0x5", "dist", "+0x90", "rot", "fw?", "mot", "+0x6", "hurtC", "stun", "label")
        print("%-4s %-5s %-5s %-5s %-7s %-6s %-7s %-4s %-4s %-4s %-5s %-6s %s" % cols)
        print("-" * 104)
    for i in range(MAX_SCAN):
        a0 = ENEMY_BASE + i * STRIDE
        if (r.u32(a0) & 1) == 0:
            continue
        typ = r.u8(a0 + 0x8); s4 = r.u8(a0 + 4); s5 = r.u8(a0 + 5); s9 = r.u8(a0 + 9)
        act_lo = r.u32(a0) & 0xff; dist = r.u32(a0 + 0x1d0); fl = r.u16(a0 + 0x1d8)
        hb = "-"; p = r.u32(a0 + 0x78)
        if 0x80000000 <= p < 0x80200000:
            rmin = r.u16(p + 6); h = r.u16(p + 8); rmax = r.u16(p + 10)
            hb = "%d/%d%s" % (rmin, h, "" if rmin == rmax else "/%d" % rmax)
        if not a.ai:
            print("%-4d 0x%02x  %-5d 0x%-3x 0x%02x(%d)  0x%02x  %-7d 0x%-5x %-9s %s" % (
                i, typ, s4, s5, s9, s9 & 0xf, act_lo, dist, fl, hb, label(s4, s5)))
        else:
            p90 = r.u8(a0 + 0x90); rot = r.s16(a0 + 0x6a); mot = r.u8(a0 + 0x94)
            sub6 = r.u8(a0 + 0x6); hurtc = r.u8(a0 + 0x1d4); stun = r.s16(a0 + 0x1dc)
            gate, ang = fw_gate(p90, rot)
            fws = "YES" if gate else ("no" if (p90 & 3) else "-")
            print("%-4d 0x%02x  %-5d 0x%-3x %-7d 0x%02x   %-7d %-4s 0x%02x 0x%02x 0x%-3x %-6d %s" % (
                i, typ, s4, s5, dist, p90, rot, fws, mot, sub6, hurtc, stun, label(s4, s5)))
    if a.ai:
        print("\nfw? = forward-walk gate (FUN_8010561c): YES=active, no=+0x90&3 set but angle fails,")
        print("      '-' = +0x90&3==0 (dormant = the briefing zombies; 0x13 then behaves like engage).")
    return 0

if __name__ == "__main__":
    sys.exit(main())
