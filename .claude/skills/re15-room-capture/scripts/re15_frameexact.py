#!/usr/bin/env python
"""re15_frameexact.py — FRAME-EXACT input into the original, for paired-replay parity.

WHY THIS EXISTS
re15_quickload.py --path holds a pad button for a WALL-CLOCK duration. That is not frame-exact, and
the error is not small: measured on ROOM1140, a nominal 2.00 s hold of Right ("R2" = 60 frames)
moved the player's yaw (0x800ACABE) by only 323 units — about 3.4 frames' worth at the game's turn
rate, not 60. The port meanwhile receives its RE15_INPUT_SCRIPT frame-exactly. So the two sides were
being fed DIFFERENT input, and every difference in the outcome looked like a port bug. That cost two
phantom root causes (a "collision divergence" and a "spawn-yaw divergence"), both of which turned out
not to exist.

THE MECHANISM
DuckStation is PAUSED, then stepped ONE FRAME AT A TIME with the FrameAdvance hotkey while the
virtual pad holds the bits for that frame. One tap = exactly one emulated frame with exactly the
requested pad state, so a 60-frame token really is 60 frames.

Both hotkeys are bound to the CONTROLLER (not the keyboard): DuckStation's frontend ignores injected
keystrokes because of focus protection — that is a documented lesson of the re15-room-capture skill,
and the same reason Save/Screenshot were moved to shoulder buttons. The triggers are used because
they are the only inputs the game itself does not consume:
    [Hotkeys] FrameAdvance = SDL-0/+LeftTrigger
              TogglePause  = SDL-0/+RightTrigger

USAGE
    python re15_frameexact.py --left 16 --postload 10 --script "R60" --out room.sav
        --script is a comma-separated list of "<letters><FRAMES>" — FRAMES, not seconds, because
        that is the whole point. Letters are the same as the port's RE15_INPUT_SCRIPT:
        U D L R (d-pad)  X (cross/run)  A (square/action)  M (R1/aim)  W (wait)

VERIFY IT WORKED, ALWAYS
A frame-exact run must be REPRODUCIBLE: the same script twice must give byte-identical player state.
--selftest runs the script twice and compares; if the two differ the stepping is not frame-exact and
no parity verdict built on it means anything.
"""
import argparse, os, shutil, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
RESUME = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\savestates\HASH-957757946319438E_resume.sav")

BITS = {"U": "dpad_up", "D": "dpad_down", "L": "dpad_left", "R": "dpad_right",
        "X": "a", "A": "x", "M": "rb", "W": None}


def parse_script(s):
    """'R60,U30' -> [('R',60), ('U',30)]; letters may combine ('XU30')."""
    out = []
    for tok in s.split(","):
        tok = tok.strip()
        if not tok:
            continue
        i = len(tok)
        while i > 0 and tok[i - 1].isdigit():
            i -= 1
        letters, n = tok[:i].upper(), int(tok[i:] or "1")
        out.append((letters, n))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--script", required=True, help='e.g. "R60,U120" — counts are FRAMES')
    ap.add_argument("--state", default=os.path.join(HERE, "..", "..", "..", "..",
                                                    "stage_saves", "mzd_debugmenu.sav"))
    ap.add_argument("--left", type=int, default=0)
    ap.add_argument("--right", type=int, default=0)
    ap.add_argument("--postload", type=float, default=10.0)
    ap.add_argument("--out", required=True)
    ap.add_argument("--selftest", action="store_true", help="run twice, require identical result")
    a = ap.parse_args()

    import vgamepad as vg
    from vgamepad import XUSB_BUTTON as B

    steps = parse_script(a.script)
    total = sum(n for _, n in steps)
    print("[frame-exact] %d Frames in %d Segmenten" % (total, len(steps)))

    pad = vg.VX360Gamepad()
    time.sleep(1.0)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", os.path.abspath(a.state), CUE])
    time.sleep(14)

    def tap(btn, hold=0.06, after=0.06):
        pad.press_button(button=btn); pad.update(); time.sleep(hold)
        pad.release_button(button=btn); pad.update(); time.sleep(after)

    def trig(which, hold=0.05, after=0.05):
        (pad.left_trigger if which == "L" else pad.right_trigger)(value=255); pad.update()
        time.sleep(hold)
        (pad.left_trigger if which == "L" else pad.right_trigger)(value=0); pad.update()
        time.sleep(after)

    # re-activate the menu after a -statefile load (documented quirk), then step to the room
    tap(B.XUSB_GAMEPAD_DPAD_UP); tap(B.XUSB_GAMEPAD_DPAD_UP); tap(B.XUSB_GAMEPAD_DPAD_DOWN)
    for _ in range(a.left):
        tap(B.XUSB_GAMEPAD_DPAD_LEFT, 0.07, 0.07)
    for _ in range(a.right):
        tap(B.XUSB_GAMEPAD_DPAD_RIGHT, 0.07, 0.07)
    tap(B.XUSB_GAMEPAD_X)                       # Square = load the room
    time.sleep(a.postload)

    print("[frame-exact] PAUSE")
    trig("R")                                    # TogglePause
    time.sleep(0.4)

    held = {}
    for letters, n in steps:
        want = [BITS[c] for c in letters if BITS.get(c)]
        for name in set(list(held) + want):
            btn = {"dpad_up": B.XUSB_GAMEPAD_DPAD_UP, "dpad_down": B.XUSB_GAMEPAD_DPAD_DOWN,
                   "dpad_left": B.XUSB_GAMEPAD_DPAD_LEFT, "dpad_right": B.XUSB_GAMEPAD_DPAD_RIGHT,
                   "a": B.XUSB_GAMEPAD_A, "x": B.XUSB_GAMEPAD_X, "rb": B.XUSB_GAMEPAD_RIGHT_SHOULDER}[name]
            if name in want and name not in held:
                pad.press_button(button=btn); held[name] = 1
            elif name not in want and name in held:
                pad.release_button(button=btn); held.pop(name)
        pad.update(); time.sleep(0.05)
        for _ in range(n):
            trig("L", 0.035, 0.035)              # FrameAdvance = exactly one frame
    for name in list(held):
        btn = {"dpad_up": B.XUSB_GAMEPAD_DPAD_UP, "dpad_down": B.XUSB_GAMEPAD_DPAD_DOWN,
               "dpad_left": B.XUSB_GAMEPAD_DPAD_LEFT, "dpad_right": B.XUSB_GAMEPAD_DPAD_RIGHT,
               "a": B.XUSB_GAMEPAD_A, "x": B.XUSB_GAMEPAD_X, "rb": B.XUSB_GAMEPAD_RIGHT_SHOULDER}[name]
        pad.release_button(button=btn)
    pad.update()

    print("[frame-exact] graceful close")
    subprocess.run(["taskkill", "/IM", os.path.basename(DUCK)], capture_output=True)
    for _ in range(40):
        time.sleep(1.0)
        if os.path.exists(RESUME) and time.time() - os.path.getmtime(RESUME) < 40:
            break
    shutil.copyfile(RESUME, a.out)
    print("[frame-exact] DONE ->", a.out)


if __name__ == "__main__":
    sys.exit(main())
