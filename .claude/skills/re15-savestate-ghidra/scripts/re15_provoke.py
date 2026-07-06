#!/usr/bin/env python3
"""re15_provoke.py - provoke an enemy attack WITHOUT navigation, then time-sample it.

Uses re15_ss_patch.py to teleport an enemy next to the player + clear its skip-tick gate
(+0x9 & 0x20), loads the repacked save in DuckStation, runs it deterministically for a set
of durations (a filmstrip of the same playthrough), and reads each enemy slot's +0x5/clip/hp.

  python re15_provoke.py BASE.sav --slot 1 --dx 0 --dz -2500 --unskip \
        --times 14,16,18,20 --outdir scratch_leap

The enemy struct base is 0x800acc2c + slot*0x1f4; position +0x34/+0x3c, +0x9 skip-gate.
Player position cache is 0x800aca88/0x800aca90 (READ-ONLY: it is refreshed from the player
entity each frame, so move the ENEMY, not the player).
"""
import sys, os, time, subprocess, shutil, argparse, struct
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from re15_ss import Ram

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
RESUME = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_resume.sav"
IMG  = "duckstation-qt-x64-ReleaseLTCG.exe"
EB, ST = 0x800acc2c, 0x1f4
CLIP = {3: "cr3", 4: "crawl4", 5: "crawl5", 6: "sel6", 18: "BITE", 19: "HEAVY", 20: "LEAP", 22: "idle", 14: "leap?"}

def run_once(state, seconds, out):
    rmt0 = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    p = subprocess.Popen([DUCK, "-batch", "-statefile", os.path.abspath(state), CUE])
    time.sleep(seconds)
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    for _ in range(30):
        time.sleep(1)
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > rmt0:
            break
    shutil.copy2(RESUME, out)

def read_state(sav, slots):
    r = Ram(sav)
    line = "pl(%d,%d)hp=%d" % (r.s16(0x800aca88), r.s16(0x800aca90), r.s16(0x800acaee))
    hit = []
    for s in slots:
        b = EB + s * ST
        c = r.u8(b + 0x94)
        line += "  m%d[+0x5=%d clip=%s air=%d dist=%d]" % (
            s, r.u8(b + 5), CLIP.get(c, str(c)), r.u8(b + 0x1e0), r.s16(b + 0x1d4))
        if c in (14, 20) or r.u8(b + 0x1e0):
            hit.append(s)
    return line, hit

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("base")
    ap.add_argument("--slot", type=int, action="append", default=[], help="enemy slot to move (repeatable)")
    ap.add_argument("--dx", type=int, action="append", default=[], help="enemy X (absolute) per --slot")
    ap.add_argument("--dz", type=int, action="append", default=[], help="enemy Z (absolute) per --slot")
    ap.add_argument("--rot", type=int, action="append", default=[], help="enemy rot_y per --slot (optional)")
    ap.add_argument("--unskip", action="store_true", help="clear +0x9 bit 0x20 (0x30->0x10) on each --slot")
    ap.add_argument("--times", default="16,18,20", help="comma list of run durations (seconds)")
    ap.add_argument("--watch", type=int, action="append", default=[], help="extra slots to read (not moved)")
    ap.add_argument("--outdir", default="scratch_provoke")
    args = ap.parse_args()

    os.makedirs(args.outdir, exist_ok=True)
    patches = []
    for i, s in enumerate(args.slot):
        b = EB + s * ST
        if i < len(args.dx): patches.append("0x%x:2:%d" % (b + 0x34, args.dx[i]))
        if i < len(args.dz): patches.append("0x%x:2:%d" % (b + 0x3c, args.dz[i]))
        if i < len(args.rot): patches.append("0x%x:2:%d" % (b + 0x6a, args.rot[i]))
        if args.unskip: patches.append("0x%x:1:0x10" % (b + 0x9))
    prov = os.path.join(args.outdir, "provoke.sav")
    subprocess.run([sys.executable, os.path.join(HERE, "re15_ss_patch.py"), args.base, prov] + patches, check=True)

    slots = sorted(set(args.slot + args.watch))
    for t in [int(x) for x in args.times.split(",")]:
        out = os.path.join(args.outdir, "t%d.sav" % t)
        run_once(prov, t, out)
        line, hit = read_state(out, slots)
        print("[t%2d] %s%s" % (t, line, "   <<< LEAP/AIRBORNE" if hit else ""))

if __name__ == "__main__":
    main()
