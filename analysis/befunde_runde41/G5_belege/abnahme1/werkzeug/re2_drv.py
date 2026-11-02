"""re2_drv.py - Abnahme G5A: DuckStation mit RE2 (re2leon.cue) per virtuellem Pad fernsteuern.
Befehle kommen zeilenweise aus einer Befehlsdatei (wird angehaengt):
  tap BTN [hold] [gap] | hold BTN secs | press BTN | release BTN | sleep s | close | quit
BTN: A B X Y START BACK UP DOWN LEFT RIGHT LB RB
Start:  python re2_drv.py CMDFILE LOGFILE [--state SAV]
"""
import sys, time, os, subprocess
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\re2leon.cue"
B = vg.XUSB_BUTTON
BTN = {"A": B.XUSB_GAMEPAD_A, "B": B.XUSB_GAMEPAD_B, "X": B.XUSB_GAMEPAD_X, "Y": B.XUSB_GAMEPAD_Y,
       "START": B.XUSB_GAMEPAD_START, "BACK": B.XUSB_GAMEPAD_BACK, "UP": B.XUSB_GAMEPAD_DPAD_UP,
       "DOWN": B.XUSB_GAMEPAD_DPAD_DOWN, "LEFT": B.XUSB_GAMEPAD_DPAD_LEFT, "RIGHT": B.XUSB_GAMEPAD_DPAD_RIGHT,
       "LB": B.XUSB_GAMEPAD_LEFT_SHOULDER, "RB": B.XUSB_GAMEPAD_RIGHT_SHOULDER}

cmdf, logf = sys.argv[1], sys.argv[2]
state = None
if "--state" in sys.argv:
    state = sys.argv[sys.argv.index("--state") + 1]
T0 = time.monotonic()
L = open(logf, "a", buffering=1)
def log(m):
    L.write("[%7.2f] %s\n" % (time.monotonic() - T0, m))

gp = vg.VX360Gamepad()
time.sleep(3.0)
log("pad up")
args = [DUCK, "-batch"]
if state:
    args += ["-statefile", state]
args += [CUE]
proc = subprocess.Popen(args)
log("duck pid=%d args=%s" % (proc.pid, args))
open(cmdf, "a").close()
pos = 0
while True:
    if proc.poll() is not None:
        log("duck exited rc=%s" % proc.returncode)
        break
    with open(cmdf) as f:
        f.seek(pos)
        lines = f.readlines()
        pos = f.tell()
    for ln in lines:
        p = ln.split()
        if not p:
            continue
        c = p[0]
        log("cmd " + ln.strip())
        if c == "tap":
            b = BTN[p[1]]; h = float(p[2]) if len(p) > 2 else 0.13; g = float(p[3]) if len(p) > 3 else 0.5
            gp.press_button(button=b); gp.update(); time.sleep(h)
            gp.release_button(button=b); gp.update(); time.sleep(g)
        elif c == "hold":
            b = BTN[p[1]]
            gp.press_button(button=b); gp.update(); time.sleep(float(p[2]))
            gp.release_button(button=b); gp.update(); time.sleep(0.1)
        elif c == "press":
            gp.press_button(button=BTN[p[1]]); gp.update()
        elif c == "release":
            gp.release_button(button=BTN[p[1]]); gp.update()
        elif c == "sleep":
            time.sleep(float(p[1]))
        elif c == "close":
            subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
            for _ in range(60):
                time.sleep(0.5)
                if proc.poll() is not None:
                    break
            log("closed rc=%s" % proc.poll())
        elif c == "snap":
            src = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\SLUS-00748_1.sav"
            m0 = os.path.getmtime(src) if os.path.exists(src) else 0
            b = BTN["RB"]
            gp.press_button(button=b); gp.update(); time.sleep(0.12)
            gp.release_button(button=b); gp.update()
            for _ in range(40):
                time.sleep(0.1)
                if os.path.exists(src) and os.path.getmtime(src) > m0:
                    break
            time.sleep(0.6)
            import shutil
            shutil.copy2(src, p[1])
            log("snap -> %s (neu=%s)" % (p[1], os.path.getmtime(src) > m0))
        elif c == "quit":
            log("quit")
            sys.exit(0)
        log("done " + c)
    time.sleep(0.05)
log("end")
