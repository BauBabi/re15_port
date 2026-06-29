#!/usr/bin/env python3
"""re15_mzd_load_room.py - drive DuckStation through the RE1.5 (MZD) boot + debug
menu to JUMP-load a room, then graceful-close so SaveStateOnExit writes the
savestate. Game input goes via a virtual X360 pad (vgamepad/ViGEm) bound to
SDL-0 in DuckStation's Pad1; keyboard injection is filtered by the game.

Sequence (timings from reai-v2-duckstation-dynamic-re memory):
  boot -> Up x3 + Cross (New Game) -> Cross (Leon) -> Cross -> in ROOM1240
  -> Back (DEBUG MENU) -> Down (JUMP) -> Triangle xM -> Right xN -> Square (load)
  -> [provoke] -> graceful close -> resume.sav -> copy to --out.

Optional modes (added 2026-06-29 — enemy/AI capture):
  --jump       force the debug JUMP even at right=0/triangle=0 (load the default
               JUMP room, or — with --menushot — just read the menu).
  --menushot   stop IN the JUMP menu WITHOUT loading; the savestate captures the
               menu screen. Dump its VRAM PNG (re15_ss.py) and read the shown
               "JUMP <room> <name>" to map --right -> room. The debug JUMP list
               holds ALL rooms.  ⚠️ DPad-Right steps DOWN the list; a long hold
               AUTO-REPEATS and overshoots to the list end — keep STEP_HOLD short
               and verify each step with --menushot.
  --provoke S  after loading, walk the player forward (DPad-Up) with rotate sweeps
               for S seconds so an enemy detects the player and enters its
               approach/attack AI (to capture a live-combat / mid-lunge frame).
"""
import time, sys, subprocess, os, shutil, argparse
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
RESUME = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_resume.sav"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"
B = vg.XUSB_BUTTON
STEP_HOLD = 0.05   # short tap for ONE precise room/stage step (avoid menu auto-repeat)

def log(m): print("[%6.1f] %s" % (time.monotonic()-T0, m), flush=True)

def main():
    global T0
    ap = argparse.ArgumentParser()
    ap.add_argument("--boot", type=float, default=64.0)
    ap.add_argument("--right", type=int, default=0)     # rooms to step within the stage
    ap.add_argument("--triangle", type=int, default=0)  # stages to advance
    ap.add_argument("--postload", type=float, default=6.0)  # settle after Square (cross-stage needs more)
    ap.add_argument("--jump", action="store_true")      # force the JUMP even at right=0
    ap.add_argument("--menushot", action="store_true")  # capture the JUMP menu WITHOUT loading
    ap.add_argument("--provoke", type=float, default=0.0)  # seconds of post-load provocation movement
    ap.add_argument("--out", required=True)
    args = ap.parse_args()
    T0 = time.monotonic()

    gp = vg.VX360Gamepad()
    time.sleep(3.0)                       # ViGEm registration -> DuckStation sees SDL-0
    log("vgamepad up")

    def tap(btn, hold=0.13, gap=0.6):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update(); time.sleep(gap)
    def hold_btn(btn, secs):
        gp.press_button(button=btn); gp.update(); time.sleep(secs)
        gp.release_button(button=btn); gp.update(); time.sleep(0.2)

    proc = subprocess.Popen([DUCK, "-batch", CUE])
    log("DuckStation launched pid=%d, booting %.0fs..." % (proc.pid, args.boot))
    time.sleep(args.boot)

    log("title: Up x3 + Cross (New Game)")
    for _ in range(3): tap(B.XUSB_GAMEPAD_DPAD_UP, gap=0.45)
    tap(B.XUSB_GAMEPAD_A)
    time.sleep(12.0)
    log("Cross (Leon, default)")
    tap(B.XUSB_GAMEPAD_A)
    time.sleep(3.0)
    tap(B.XUSB_GAMEPAD_A)
    log("loading into ROOM1240 (18s)...")
    time.sleep(18.0)

    if args.jump or args.triangle or args.right:
        log("Back -> DEBUG MENU")
        tap(B.XUSB_GAMEPAD_BACK, gap=1.5)
        log("Down -> JUMP line")
        tap(B.XUSB_GAMEPAD_DPAD_DOWN, gap=1.0)
        for i in range(args.triangle):
            log("Triangle -> stage +1 (%d/%d)" % (i+1, args.triangle))
            tap(B.XUSB_GAMEPAD_Y, hold=STEP_HOLD, gap=0.8)
        for i in range(args.right):
            log("Right -> room +1 (%d/%d)" % (i+1, args.right))
            tap(B.XUSB_GAMEPAD_DPAD_RIGHT, hold=STEP_HOLD, gap=0.55)
        if args.menushot:
            log("MENU SHOT: capturing JUMP menu WITHOUT loading"); time.sleep(1.5)
        else:
            log("Square -> LOAD room"); tap(B.XUSB_GAMEPAD_X, gap=0.6)
            log("waiting for room load (%.0fs)..." % args.postload)
            time.sleep(args.postload)

    if args.provoke > 0 and not args.menushot:
        log("PROVOKE %.0fs (walk forward + rotate sweeps)" % args.provoke)
        t_end = time.monotonic() + args.provoke
        seq = 0
        while time.monotonic() < t_end:
            hold_btn(B.XUSB_GAMEPAD_DPAD_UP, 2.5)
            hold_btn(B.XUSB_GAMEPAD_DPAD_RIGHT if seq % 2 == 0 else B.XUSB_GAMEPAD_DPAD_LEFT, 0.8)
            seq += 1

    resume_mtime = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    log("graceful close (taskkill, no /F -> WM_CLOSE -> SaveStateOnExit)")
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    for _ in range(40):
        time.sleep(0.5)
        if proc.poll() is not None:
            break
    time.sleep(2.0)  # flush

    for _ in range(30):
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_mtime:
            break
        time.sleep(0.5)
    if not (os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_mtime):
        log("WARNING: resume.sav not refreshed!")
    shutil.copy2(RESUME, args.out)
    log("DONE -> %s (%d bytes)" % (args.out, os.path.getsize(args.out)))

if __name__ == "__main__":
    main()
