"""re2c_drv.py - DuckStation mit RE2 Claire PAL (Resident Evil 2 Claire.cue, SLES_109.72) per virtuellem Pad fernsteuern.
Befehle zeilenweise aus einer Befehlsdatei (wird angehaengt):
  tap BTN [hold] [gap] | hold BTN secs | press BTN | release BTN | sleep s
  snap OUT.sav   (RB = SaveSelectedSaveState, neueste *_1.sav kopieren, Vorschaubild OUT.png)
  cap            (LB = ToggleMediaCapture)
  mark ID | close | quit
BTN: A(Kreuz) B(Kreis) X(Viereck) Y(Dreieck) START BACK(Select) UP DOWN LEFT RIGHT LB RB
Start:  python re2c_drv.py CMDFILE LOGFILE [--state SAV]
"""
import sys, time, os, subprocess, glob, shutil
import vgamepad as vg
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Resident Evil 2 Claire.cue"
SSDIR = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates"
B = vg.XUSB_BUTTON
BTN = {"A": B.XUSB_GAMEPAD_A, "B": B.XUSB_GAMEPAD_B, "X": B.XUSB_GAMEPAD_X, "Y": B.XUSB_GAMEPAD_Y,
       "START": B.XUSB_GAMEPAD_START, "BACK": B.XUSB_GAMEPAD_BACK, "UP": B.XUSB_GAMEPAD_DPAD_UP,
       "DOWN": B.XUSB_GAMEPAD_DPAD_DOWN, "LEFT": B.XUSB_GAMEPAD_DPAD_LEFT, "RIGHT": B.XUSB_GAMEPAD_DPAD_RIGHT,
       "LB": B.XUSB_GAMEPAD_LEFT_SHOULDER, "RB": B.XUSB_GAMEPAD_RIGHT_SHOULDER}

cmdf, logf = sys.argv[1], sys.argv[2]
state = sys.argv[sys.argv.index("--state") + 1] if "--state" in sys.argv else None
if state:
    state = os.path.abspath(state)  # DuckStation -statefile scheitert still mit Vorwaertsstrichen (gemessen 2026-11-02)
T0 = time.monotonic()
L = open(logf, "a", buffering=1)


def log(m):
    L.write("[%8.3f] %s\n" % (time.monotonic() - T0, m))


def newest():
    fs = glob.glob(os.path.join(SSDIR, "*_1.sav"))
    return max(fs, key=os.path.getmtime) if fs else None


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
        elif c in ("snap", "cap"):
            n0 = newest(); m0 = os.path.getmtime(n0) if n0 else 0
            b = BTN["RB" if c == "snap" else "LB"]
            gp.press_button(button=b); gp.update(); time.sleep(0.12)
            gp.release_button(button=b); gp.update()
            if c == "snap":
                ok = False; n = None
                for _ in range(50):
                    time.sleep(0.1)
                    n = newest()
                    if n and os.path.getmtime(n) > m0:
                        ok = True; break
                time.sleep(0.3)
                if ok:
                    shutil.copy2(n, p[1])
                    try:
                        from re2c_ss import SS
                        SS(p[1]).shot(p[1][:-4] + ".png")
                    except Exception as e:
                        log("shot-fehler %s" % e)
                log("snap -> %s (neu=%s, quelle=%s)" % (p[1], ok, n if ok else None))
            else:
                time.sleep(0.3)
        elif c == "mark":
            pass
        elif c == "close":
            subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
            for _ in range(60):
                time.sleep(0.5)
                if proc.poll() is not None:
                    break
            log("closed rc=%s" % proc.poll())
        elif c == "quit":
            log("quit")
            sys.exit(0)
        log("done " + c + (" " + p[1] if len(p) > 1 else ""))
    time.sleep(0.02)
log("end")
