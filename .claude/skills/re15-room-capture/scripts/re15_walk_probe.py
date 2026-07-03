#!/usr/bin/env python3
"""re15_walk_probe.py - COORDINATE-PARITY probe: load a gameplay savestate, HOLD one D-pad direction
for N seconds (NO debug-menu nav), save. Read the player coord with re15_ss.py and compare to the PC
port driven with the same hold (RE15_INPUT_SCRIPT). Same input -> same coords, or the port is wrong.

  python re15_walk_probe.py --state ..\..\..\stage_saves\mzd_stage1_briefing.sav --hold U --secs 2 --out walk.sav
"""
import argparse, os, shutil, subprocess, time
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
RESUME = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_resume.sav"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", required=True)
    ap.add_argument("--load", type=float, default=14.0)
    ap.add_argument("--hold", default="U")          # U/D/L/R (one direction, held)
    ap.add_argument("--secs", type=float, default=2.0)
    ap.add_argument("--out", required=True)
    a = ap.parse_args()

    PMAP = {"U": vg.XUSB_BUTTON.XUSB_GAMEPAD_DPAD_UP,  "D": vg.XUSB_BUTTON.XUSB_GAMEPAD_DPAD_DOWN,
            "L": vg.XUSB_BUTTON.XUSB_GAMEPAD_DPAD_LEFT,"R": vg.XUSB_BUTTON.XUSB_GAMEPAD_DPAD_RIGHT}
    btn = PMAP[a.hold.upper()]

    gp = vg.VX360Gamepad(); time.sleep(3.0)          # pad before DuckStation (SDL-0 enumeration)
    print("[walk] launch -statefile %s" % a.state, flush=True)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", a.state, CUE])
    time.sleep(a.load)
    print("[walk] HOLD %s for %.1fs" % (a.hold, a.secs), flush=True)
    gp.press_button(button=btn); gp.update(); time.sleep(a.secs)
    gp.release_button(button=btn); gp.update(); time.sleep(0.3)
    print("[walk] graceful close", flush=True)
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    for _ in range(40):
        time.sleep(0.5)
        if os.path.exists(RESUME) and (time.time() - os.path.getmtime(RESUME)) < 20: break
    time.sleep(1.0)
    shutil.copy(RESUME, a.out)
    print("[walk] wrote %s" % a.out, flush=True)

if __name__ == "__main__":
    main()
