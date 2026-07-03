#!/usr/bin/env python3
"""re15_advance.py - advance a DuckStation savestate forward N seconds with NO INPUT,
then graceful-close so SaveStateOnExit writes the advanced state; copy it to --out.

This is the parity reference generator: load a gameplay save (e.g. mzd_stage1_engage_live.sav),
let the game run untouched for N seconds (the AI advances, player idle), and capture where the
original ends up. Diff against the port's RE15_PLAYER_POS injected run at the matching frame.

  python re15_advance.py --state ../../.../mzd_stage1_engage_live.sav --secs 6 --out adv_t6.sav
"""
import argparse, os, shutil, subprocess, time

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
RESUME = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_resume.sav"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", required=True)          # gameplay savestate to load
    ap.add_argument("--load", type=float, default=14.0) # wait for the state to load
    ap.add_argument("--secs", type=float, default=6.0)  # seconds to advance (NO input)
    ap.add_argument("--out", required=True)
    a = ap.parse_args()

    print("[advance] launch -statefile %s" % a.state, flush=True)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", a.state, CUE])
    time.sleep(a.load)
    print("[advance] running %.1fs with NO input..." % a.secs, flush=True)
    time.sleep(a.secs)                                  # <-- the game advances untouched
    print("[advance] graceful close (SaveStateOnExit)", flush=True)
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    # wait for a fresh resume state
    for _ in range(40):
        time.sleep(0.5)
        if os.path.exists(RESUME) and (time.time() - os.path.getmtime(RESUME)) < 20:
            break
    time.sleep(1.0)
    shutil.copy(RESUME, a.out)
    print("[advance] wrote %s" % a.out, flush=True)

if __name__ == "__main__":
    main()
