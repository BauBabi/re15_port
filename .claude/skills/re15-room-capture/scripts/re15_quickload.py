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
  - `-statefile` reliably quick-loads for menu-shots, stepping AND the Square-LOAD
    (verified end-to-end 2026-06-30 — Square is RELIABLE after the Up,Up,Down menu
    re-activation the script does; allow --postload >=8s for a room / >=16s for a
    cross-stage load before the close, quickload skips asset-settle so the frame
    can stall late in that window).
  - WORKING live-engage capture (ROOM1140 = 0x114 BRIEFING, player alive, all 5
    zombies wake):  re15_quickload.py --left 16 --postload 10 --path "R0.5,U2,R0.3,U6"
    Artifact: stage_saves/mzd_stage1_engage_live.sav. Debug-JUMP numbers are HEX
    (--left 16 = 0x114 BRIEFING; --left 10 = 0x11A SEWER EXIT -- a 6-step trap).
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
SLOT1  = os.path.join(r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates",
                      "HASH-957757946319438E_1.sav")
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
    ap.add_argument("--after", type=float, default=0.0)   # post-path stand-still settle (let a zombie grab)
    ap.add_argument("--path", default="")   # directed move after load, e.g. "R0.4,U2.5,L0.6,U6"
                                            # tokens: U/D forward/back, L/R turn, X cross, S square; <secs> hold
    ap.add_argument("--fire", type=int, default=0)   # after path: hold R1 (aim) + tap Square N times.
                                                     # The input injection WORKS (verified runs 2026-06-30). BUT two
                                                     # gotchas make it useless for capturing the muzzle in the BRIEFING:
                                                     #  (1) the MUZZLE is 1 frame -> gone by save-time (save is at close);
                                                     #  (2) the briefing player has weapon DAT_800aca5d=1 (knife/melee, NOT
                                                     #      the pistol w2) -> no muzzle, no ranged hit, effect pool stays 0.
                                                     # To capture an ESP effect: use a GUN-equipped gameplay room AND a
                                                     # PERSISTENT effect (blood/death-gore), or save ON the effect frame.
    ap.add_argument("--script", default="")   # FRAME-EXACT input, e.g. "R60,U120" — counts are FRAMES.
                                              # Unlike --path (wall-clock holds) this pauses the
                                              # emulator and steps it ONE FRAME AT A TIME with the
                                              # FrameAdvance hotkey while the pad holds that frame's
                                              # bits, so a 60-frame token really is 60 frames.
                                              # WHY: --path is NOT frame-exact and the error is huge —
                                              # a nominal 2.00s hold of R (=60 frames) moved the
                                              # player yaw (0x800ACABE) by 323 units, about 3.4
                                              # frames' worth. The port meanwhile gets its
                                              # RE15_INPUT_SCRIPT frame-exactly, so the two sides
                                              # were fed DIFFERENT input and every difference looked
                                              # like a port bug (it produced two phantom root causes).
                                              # Requires in settings.ini [Hotkeys]:
                                              #   FrameAdvance = SDL-0/+LeftTrigger
                                              #   TogglePause  = SDL-0/+RightTrigger
                                              # (controller, NOT keyboard — the frontend filters
                                              # injected keystrokes; same reason Save/Screenshot are
                                              # on shoulder buttons.)
    ap.add_argument("--save-hotkey", action="store_true")   # DIAGNOSTIC: after --path (emulator
                                              # RUNNING, never paused) tap the save hotkey and report
                                              # whether slot 1 was written. This separates "the
                                              # binding/pad is broken" from "hotkeys do not fire while
                                              # PAUSED" — the two hypotheses left for the frame-exact
                                              # capture. Run it with a trivial --path so it is quick.
    ap.add_argument("--out", required=True)
    args = ap.parse_args()
    T0 = time.monotonic()
    script_saved = False

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
            # DIRECTED move (TANK CONTROL): U/D = forward/back, L/R ROTATE the player (NOT strafe),
            # X = cross, S = square. Rotation is RELATIVE TO CAMERA: in 0x114 BRIEFING the spawn faces
            # the camera, so R rotates toward SCREEN-LEFT = toward the feeding zombies in the left aisle
            # (between conference table and wall). Walking straight hits the table. VERIFIED path:
            #   --path "R0.5,U2,R0.3,U6"   (R=face left to zombies, U=past table edge, R=into aisle, U=down it)
            # Two-R (R..,U..,R..,U..) is more robust than R-then-L (the counter-rotation overcorrects).
            # Keep rotations short (~0.3-0.5s; >0.7s overrotates). Verify each try with the PNG (re15_ss.py)
            # + distances with re15_enemy_state.py (target: nearest zombie dist<4000).
            PMAP = {"U": B.XUSB_GAMEPAD_DPAD_UP, "D": B.XUSB_GAMEPAD_DPAD_DOWN,
                    "L": B.XUSB_GAMEPAD_DPAD_LEFT, "R": B.XUSB_GAMEPAD_DPAD_RIGHT,
                    "X": B.XUSB_GAMEPAD_A, "S": B.XUSB_GAMEPAD_X}
            for tok in args.path.split(","):
                tok = tok.strip()
                if not tok: continue
                btn = PMAP.get(tok[0].upper()); secs = float(tok[1:] or "0.5")
                if btn is None: continue
                log("PATH %s %.2fs" % (tok[0].upper(), secs)); hold_btn(btn, secs)
            if args.after > 0:
                # POST-PATH SETTLE: stand still (no input) so a woken zombie can approach + GRAB the
                # (stationary) player before the save — needed to capture a mid-grab / struggle frame.
                log("AFTER settle %.0fs (let a zombie reach + grab)" % args.after); time.sleep(args.after)
        if args.script:
            # FRAME-EXACT stepping. Runs on the SAME proven navigation above — rebuilding that
            # navigation in a separate script is exactly how the first attempt failed (its menu taps
            # used 0.06s gaps instead of the 0.30-0.60s this driver uses, so the room never loaded
            # and the capture came back with an empty enemy roster).
            SMAP = {"U": B.XUSB_GAMEPAD_DPAD_UP, "D": B.XUSB_GAMEPAD_DPAD_DOWN,
                    "L": B.XUSB_GAMEPAD_DPAD_LEFT, "R": B.XUSB_GAMEPAD_DPAD_RIGHT,
                    "X": B.XUSB_GAMEPAD_A, "A": B.XUSB_GAMEPAD_X,
                    "M": B.XUSB_GAMEPAD_RIGHT_SHOULDER}
            # Shoulder BUTTONS, not trigger axes: the axis bindings (SDL-0/+LeftTrigger) never fired
            # — measured with the emulator RUNNING as well as paused, so it was the binding and not
            # the pause. Shoulder buttons are the pattern this config already used successfully
            # (Screenshot / ToggleMediaCapture were bound that way), and the skill notes them as the
            # reliable ones while L3/R3 are not.
            def trig(which, hold=0.06, gap=0.06):
                b = B.XUSB_GAMEPAD_LEFT_SHOULDER if which == "L" else B.XUSB_GAMEPAD_RIGHT_SHOULDER
                gp.press_button(button=b); gp.update(); time.sleep(hold)
                gp.release_button(button=b); gp.update(); time.sleep(gap)
            # No explicit pause needed: DuckStation's FrameAdvance pauses on its first use and then
            # steps one frame per press. TogglePause could not be made to work from the virtual pad
            # (tried +RightTrigger and LeftShoulder), FrameAdvance can — so the run stays paused from
            # the first step onward and is captured with the save hotkey, never via SaveStateOnExit
            # (which does not fire while paused).
            log("STEP (FrameAdvance pausiert implizit beim ersten Druck)")
            held = set()
            for tok in args.script.split(","):
                tok = tok.strip()
                if not tok: continue
                i = len(tok)
                while i > 0 and tok[i-1].isdigit(): i -= 1
                letters, n = tok[:i].upper(), int(tok[i:] or "1")
                want = {SMAP[c] for c in letters if c in SMAP}
                for b in held - want: gp.release_button(button=b)
                for b in want - held: gp.press_button(button=b)
                held = want; gp.update(); time.sleep(0.10)
                log("SCRIPT %-4s x%d frames" % (letters or "W", n))
                for _ in range(n): trig("L")
            for b in held: gp.release_button(button=b)
            gp.update(); time.sleep(0.2)
            # EXPLICIT save WHILE STILL PAUSED. SaveStateOnExit does NOT fire when the emulator is
            # paused at shutdown — proven: the run reported "resume nicht neu geschrieben". Saving here
            # also keeps the captured state EXACT: unpausing first would let ~15 uncontrolled frames
            # run before the save, which defeats the whole point of frame-exact stepping.
            # SaveSelectedSaveState is on SDL-0/LeftShoulder (controller, since the frontend filters
            # injected keystrokes) and writes slot 1 -> <HASH>_1.sav.
            # A plain tap does NOT work while paused: DuckStation processes hotkeys inside its frame
            # loop, so with the emulator paused the press is never seen (measured — slot 1 kept its
            # days-old timestamp). So HOLD the save button and drive ONE FrameAdvance: that frame
            # processes the input, the save fires, and exactly one extra frame is spent — which the
            # port side can match by adding one frame to its script.
            # CAPTURE. SaveSelectedSaveState does NOT fire while the emulator is paused (measured:
            # slot 1 kept its days-old timestamp across three attempts, with a plain tap AND with the
            # button held across a FrameAdvance) — DuckStation services hotkeys inside the frame loop.
            # So UNPAUSE and let SaveStateOnExit do it, which is the path already proven by the
            # --path runs. Cost, stated openly: the ~0.4s between unpause and process exit is
            # uncontrolled (~12 frames) with NO input held, so the capture is frame-exact up to that
            # idle tail. The port side matches it by appending the same idle frames; anything tighter
            # needs a hotkey that works while paused, which this build does not appear to offer.
            # CAPTURE while paused: HOLD the save hotkey (RightTrigger) and drive ONE FrameAdvance.
            # That frame is the only one DuckStation services input on, so the save fires there.
            log("SAVE slot1 (hold RT + 1 frame)")
            slot1_m = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
            gp.press_button(button=B.XUSB_GAMEPAD_RIGHT_SHOULDER); gp.update(); time.sleep(0.15)
            trig("L", 0.06, 0.35)
            gp.release_button(button=B.XUSB_GAMEPAD_RIGHT_SHOULDER); gp.update(); time.sleep(1.0)
            for _ in range(20):
                if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > slot1_m:
                    log("SAVE ok"); script_saved = True; break
                time.sleep(0.5)
            if not script_saved:
                log("FAIL: Slot-1-Savestate wurde nicht geschrieben"); sys.exit(4)

        if args.save_hotkey:
            log("SAVE-HOTKEY-TEST (Emulator LAEUFT, nicht pausiert)")
            slot1_m = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
            gp.press_button(button=B.XUSB_GAMEPAD_RIGHT_SHOULDER); gp.update(); time.sleep(0.20)
            gp.release_button(button=B.XUSB_GAMEPAD_RIGHT_SHOULDER); gp.update(); time.sleep(1.5)
            ok = os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > slot1_m
            log("ERGEBNIS: Slot 1 %s" % ("GESCHRIEBEN -> Binding und Pad sind in Ordnung, das Problem ist ausschliesslich der PAUSIERTE Zustand"
                                         if ok else "NICHT geschrieben -> Binding oder Pad ist das Problem, nicht die Pause"))

        if args.fire > 0:
            # Hold R1 (aim) then tap Square (fire) N times. R1 = right shoulder; Square = XUSB_X
            # in this pad mapping (the same button the menu LOAD used). The aim/raise needs a beat
            # to settle before the discharge registers.
            log("FIRE: hold R1 (aim) + Square x%d" % args.fire)
            gp.press_button(button=B.XUSB_GAMEPAD_RIGHT_SHOULDER); gp.update(); time.sleep(0.7)
            for s in range(args.fire):
                tap(B.XUSB_GAMEPAD_X, hold=0.13, gap=0.45)
            gp.release_button(button=B.XUSB_GAMEPAD_RIGHT_SHOULDER); gp.update(); time.sleep(0.2)
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
    if script_saved:
        # The frame-exact path already wrote an EXACT paused-state savestate; the resume file
        # is irrelevant (it is not even written when the emulator exits paused).
        shutil.copy2(SLOT1, args.out)
        log("DONE (slot1) -> %s (%d bytes)" % (args.out, os.path.getsize(args.out)))
        return
    fresh = False
    for _ in range(30):
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_mtime:
            fresh = True; break
        time.sleep(0.5)
    if not fresh:
        # HARD FAIL instead of silently copying a STALE resume file. This bit hard once: a --script
        # run came back with active_count=0 and a nonsense player position, which read like a broken
        # capture of the room — it was actually an untouched resume state from an earlier session,
        # because SaveStateOnExit had not fired. A capture tool that quietly returns old data is
        # worse than one that crashes.
        log("FAIL: SaveStateOnExit hat %s NICHT neu geschrieben — kein frischer Zustand." % RESUME)
        log("      (Passiert u.a., wenn der Emulator beim Schliessen noch PAUSIERT war.)")
        sys.exit(3)
    shutil.copy2(RESUME, args.out)
    log("DONE -> %s (%d bytes)" % (args.out, os.path.getsize(args.out)))

if __name__ == "__main__":
    main()
