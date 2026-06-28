#!/usr/bin/env python3
"""re15_capture.py - one-shot: drive DuckStation to load a room, then immediately
verify the savestate (globals + black-frame check + framebuffer PNG). Wraps
re15_mzd_load_room.py and re15_ss.py so a capture self-checks. If the display is
black it tells you to raise --postload instead of you discovering it later.

Usage (args forwarded to the driver):
  re15_capture.py --out room.sav [--right N] [--triangle M] [--postload S] [--png room.png]
"""
import sys, os, subprocess, argparse
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re15_ss import Ram, vram_png, display_nonblack_fraction

HERE = os.path.dirname(os.path.abspath(__file__))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True)
    ap.add_argument("--right", type=int, default=0)
    ap.add_argument("--triangle", type=int, default=0)
    ap.add_argument("--postload", type=float, default=6.0)
    ap.add_argument("--boot", type=float, default=64.0)
    ap.add_argument("--png", default=None)
    a = ap.parse_args()

    cmd = [sys.executable, os.path.join(HERE, "re15_mzd_load_room.py"),
           "--out", a.out, "--right", str(a.right), "--triangle", str(a.triangle),
           "--postload", str(a.postload), "--boot", str(a.boot)]
    print("[capture] running driver:", " ".join(cmd))
    if subprocess.run(cmd).returncode != 0:
        sys.exit("driver failed")

    print("\n[verify] %s" % a.out)
    r = Ram(a.out)
    frac = display_nonblack_fraction(r)
    print("  display     : %.0f%% non-black" % (100 * frac))
    print("  player HP   : %d (DAT_800acaee)" % r.s16(0x800acaee))
    print("  player state: %d (DAT_800aca58)" % r.u8(0x800aca58))
    print("  fade level  : 0x%x (DAT_800b5568)" % r.u8(0x800b5568))
    if a.png:
        vram_png(r, a.png); print("  PNG         : %s" % a.png)
    if frac < 0.10:
        print("  !! WARNING: display is black -> captured mid-load. "
              "Re-run with a larger --postload (cross-stage needs >=16).")
        sys.exit(2)
    print("  OK: room rendered.")

if __name__ == "__main__":
    main()
