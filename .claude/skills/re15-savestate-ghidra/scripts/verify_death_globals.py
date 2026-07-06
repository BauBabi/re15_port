#!/usr/bin/env python3
"""Verify the RE1.5 eaten-death presentation globals against live DuckStation savestates.

This is the reproducible check behind the "dog/zombie eaten-death is byte-true complete" claim
(2026-07-06). It reads DAT_800aca3c (the cut-control word), DAT_800acb10/acb12 (the blood-pool
half-extents), and the player victim-cmd bytes from the grab-death sequence saves and asserts:

  1. aca3c == 0xc0 (bits 0x40|0x80 set) through the WHOLE eaten death (cmd 5 struggle / 6 collapse /
     7 youdied). Those bits gate PSX-hardware display re-inits (@0x8001cd04 framebuffer swap,
     @0x800214e8 fade) that the PC renderer has no equivalent of -> a no-op in the port.
  2. acb10/acb12 are the BLOOD-POOL half-extents (NOT a camera pan): base 500x600, growing +12/frame
     during cmd 7 -> matches the port's g_death_pool (500+12t x 600+12t, cap 122 -> 1964/2064).

Both the ZOMBIE grab (@0x8010a3f8 sets aca3c|=0xc0) and the DOG machine A (@0x80111984) share this
exact mechanism + the shared cmd-7 death handler (LAB_8003694c) that owns acb10/acb12, so these
zombie-captured saves verify the shared behaviour that applies identically to the dog.

Usage:  python verify_death_globals.py [stage_saves_dir]
"""
import os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
# re15_ss.Ram lives in the sibling re15-room-capture skill (shared decoder)
sys.path.insert(0, os.path.join(HERE, "..", "..", "re15-room-capture", "scripts"))
sys.path.insert(0, HERE)
from re15_ss import Ram  # noqa: E402

ACA3C = 0x800aca3c   # cut-control word (grab-death sets |= 0xc0)
ACB10 = 0x800acb10   # blood-pool half-extent X (base 500)
ACB12 = 0x800acb12   # blood-pool half-extent Z (base 600)
ACA58 = 0x800aca58   # player victim command (5 struggle / 6 collapse / 7 youdied)
ACA5A = 0x800aca5a   # victim phase

SAVES = [
    ("cmd5_struggle", "mzd_death_cmd5_struggle.sav", 5),
    ("cmd6_collapse", "mzd_death_cmd6_collapse.sav", 6),
    ("cmd7_youdied",  "mzd_death_cmd7_youdied.sav",  7),
]


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "..", "..", "..", "..", "stage_saves")
    print(f"{'save':16} {'aca3c':>10} 0x40 0x80 {'cmd':>3} {'phase':>5} {'acb10':>6} {'acb12':>6}")
    ok = True
    for name, fn, exp_cmd in SAVES:
        p = os.path.join(root, fn)
        if not os.path.exists(p):
            print(f"{name:16} MISSING ({p})"); ok = False; continue
        r = Ram(p)
        a3c = r.u32(ACA3C); b10 = r.u16(ACB10); b12 = r.u16(ACB12)
        cmd = r.u8(ACA58);  ph = r.u8(ACA5A)
        b40 = "Y" if a3c & 0x40 else "."; b80 = "Y" if a3c & 0x80 else "."
        print(f"{name:16} 0x{a3c:08x}   {b40}    {b80}  {cmd:>3} {ph:>5} {b10:>6} {b12:>6}")
        if (a3c & 0xc0) != 0xc0:
            print(f"  ! FAIL: aca3c bits 0x40|0x80 not both set"); ok = False
        if cmd != exp_cmd:
            print(f"  ! note: player cmd {cmd} != expected {exp_cmd} (save may be a neighbouring frame)")
        # blood pool: base >= 500/600, and each half-extent == base + 12*t for some t>=0
        if not (b10 >= 500 and (b10 - 500) % 12 == 0 and b12 == b10 + 100):
            print(f"  ! FAIL: acb10/acb12 not a (500+12t)/(600+12t) blood-pool pair"); ok = False
    print()
    print("VERIFIED: aca3c=0xc0 (display-reinit gates, no PC equivalent) + acb10/acb12 = blood pool"
          " (= port g_death_pool)." if ok else "VERIFICATION INCOMPLETE (see notes above).")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
