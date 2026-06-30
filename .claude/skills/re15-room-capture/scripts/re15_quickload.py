#!/usr/bin/env python3
"""re15_quickload.py - QUICK-LOAD a savestate via DuckStation `-statefile` (skips
the ~64s boot+intro), then navigate the debug JUMP menu + (menu-shot | load +
provoke) + graceful-close -> save. ~20-40s per run vs ~110s for the boot path.

Workflow:
  1. ONCE, create a debug-menu base state with the boot driver:
       re15_mzd_load_room.py --jump --menushot --out menu.sav
     (this stops in the JUMP menu, cursor on "JUMP <room> <name>").
  2. Then quick-load that state repeatedly to read/step the menu or load a room:
       re15_quickload.py --state menu.sav --right N --menushot --out probe.sav
       re15_quickload.py --state menu.sav --right N --provoke 14 --out room.sav

Caveats:
  - `-statefile` reliably quick-loads for MENU-SHOTS and stepping; the actual
    Square-LOAD does not always take after a state-load (use the boot driver
    re15_mzd_load_room.py for a guaranteed load).
  - The debug JUMP list holds ALL rooms. DPad-Right steps DOWN the list; a long
    hold AUTO-REPEATS and overshoots to the list end, so STEP_HOLD is short and
    you should verify each step with --menushot.
  - A base state captured during the live opening briefing is TIME-SENSITIVE (the
    briefing keeps playing during the load wait); prefer a base state taken in a
    stable room.
"""
import time, sys, subprocess, os, shutil, argparse
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
RESUME = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_resume.sav"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"
B = vg.XUSB_BUTTON
STEP_HOLD = 0.04   # short tap = ONE precise room step (a long hold auto-repeats to the list end)

def log(m): print("[%6.1f] %s" % (time.monotonic()-T0, m), flush=True)

def main():
    global T0
    ap = argparse.ArgumentParser()
    # The DURABLE debug-menu quickload base (committed): quick-loading it lands directly on the
    # JUMP line in ~40s (vs ~110s for a cold boot). Default so a capture run is zero-config from the
    # repo root. Regenerate with: re15_mzd_load_room.py --jump --menushot --out stage_saves/mzd_debugmenu.sav
    ap.add_argument("--state", default="stage_saves/mzd_debugmenu.sav")   # savestate to quick-load
    ap.add_argument("--load", type=float, default=16.0)  # wait after launch for the state to load
    ap.add_argument("--right", type=int, default=0)      # rooms to step RIGHT (down the JUMP list)
    ap.add_argument("--left", type=int, default=0)       # rooms to step LEFT (up the JUMP list)
    ap.add_argument("--triangle", type=int, default=0)   # stages to advance (Triangle cycles the stage)
    ap.add_argument("--menushot", action="store_true")
    ap.add_argument("--postload", type=float, default=8.0)
    ap.add_argument("--provoke", type=float, default=0.0)
    ap.add_argument("--path", default="")   # directed move after load, e.g. "R0.4,U2.5,L0.6,U6"
                                            # tokens: U/D forward/back, L/R turn, X cross, S square; <secs> hold
    ap.add_argument("--out", required=True)
    args = ap.parse_args()
    T0 = time.monotonic()

    gp = vg.VX360Gamepad()
    time.sleep(3.0)
    log("vgamepad up")
    def tap(btn, hold=0.13, gap=0.6):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update(); time.sleep(gap)
    def hold_btn(btn, secs):
        gp.press_button(button=btn); gp.update(); time.sleep(secs)
        gp.release_button(button=btn); gp.update(); time.sleep(0.2)

    proc = subprocess.Popen([DUCK, "-batch", "-statefile", args.state, CUE])
    log("DuckStation launched pid=%d, loading state %.0fs..." % (proc.pid, args.load))
    time.sleep(args.load)

    # The DEBUG-MENU flow (user-confirmed 2026-06-30): you land in the DEBUG menu at the TOP; press
    # DOWN once to reach JUMP; change rooms with single SHORT Left/Right presses; switch stage with
    # TRIANGLE; LOAD with SQUARE. CRITICAL after a -statefile restore: a bare Right is IGNORED — the
    # menu input must be RE-ACTIVATED by a navigation press first. So normalise to the top (Up x2) then
    # press Down -> JUMP (this re-engages stepping), exactly the boot driver's fresh-nav sequence.
    for _ in range(2): tap(B.XUSB_GAMEPAD_DPAD_UP, hold=0.06, gap=0.30)   # -> DEBUG menu top
    tap(B.XUSB_GAMEPAD_DPAD_DOWN, hold=0.06, gap=0.50)                    # -> JUMP line (re-activates input)
    for i in range(args.triangle):                                        # Triangle = stage select
        log("Triangle -> stage +1 (%d/%d)" % (i+1, args.triangle)); tap(B.XUSB_GAMEPAD_Y, hold=0.06, gap=0.55)
    for i in range(args.right):                                           # short individual Right presses
        log("Right -> room (%d/%d)" % (i+1, args.right)); tap(B.XUSB_GAMEPAD_DPAD_RIGHT, hold=0.08, gap=0.45)
    for i in range(args.left):
        log("Left -> room (%d/%d)" % (i+1, args.left)); tap(B.XUSB_GAMEPAD_DPAD_LEFT, hold=0.08, gap=0.45)
    if args.menushot:
        log("MENU SHOT (tri=%d right=%d left=%d), no load" % (args.triangle, args.right, args.left)); time.sleep(1.2)
    else:
        log("Square -> LOAD room"); tap(B.XUSB_GAMEPAD_X, gap=0.6)
        log("settle %.0fs" % args.postload); time.sleep(args.postload)
        if args.path:
            # DIRECTED move (room-specific, e.g. around an obstacle): U/D = forward/back, L/R = turn,
            # X = cross, S = square. The 0x114 BRIEFING ROOM needs a table detour (user: a bit right,
            # turn left, then straight): --path "R0.4,U2.5,L0.7,U6"
            PMAP = {"U": B.XUSB_GAMEPAD_DPAD_UP, "D": B.XUSB_GAMEPAD_DPAD_DOWN,
                    "L": B.XUSB_GAMEPAD_DPAD_LEFT, "R": B.XUSB_GAMEPAD_DPAD_RIGHT,
                    "X": B.XUSB_GAMEPAD_A, "S": B.XUSB_GAMEPAD_X}
            for tok in args.path.split(","):
                tok = tok.strip()
                if not tok: continue
                btn = PMAP.get(tok[0].upper()); secs = float(tok[1:] or "0.5")
                if btn is None: continue
                log("PATH %s %.2fs" % (tok[0].upper(), secs)); hold_btn(btn, secs)
        if args.provoke > 0:
            log("PROVOKE %.0fs" % args.provoke)
            t_end = time.monotonic() + args.provoke; seq = 0
            while time.monotonic() < t_end:
                hold_btn(B.XUSB_GAMEPAD_DPAD_UP, 2.5)
                hold_btn(B.XUSB_GAMEPAD_DPAD_RIGHT if seq % 2 == 0 else B.XUSB_GAMEPAD_DPAD_LEFT, 0.8)
                seq += 1

    resume_mtime = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    log("graceful close")
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    for _ in range(40):
        time.sleep(0.5)
        if proc.poll() is not None: break
    time.sleep(2.0)
    for _ in range(30):
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_mtime: break
        time.sleep(0.5)
    shutil.copy2(RESUME, args.out)
    log("DONE -> %s (%d bytes)" % (args.out, os.path.getsize(args.out)))

if __name__ == "__main__":
    main()
