#!/usr/bin/env python3
"""gp_lauf.py - Laeufer der Gegenpruefung: laedt einen Savestate in DuckStation (MZD-Disc), macht KEINE
Eingabe, zieht n Zwischenstaende ueber den Hotkey SaveSelectedSaveState (SDL-0/LeftShoulder, virtueller
Gamepad) und schliesst grazioes (taskkill ohne /F -> SaveStateOnExit -> resume.sav).
Bricht ab, wenn schon ein Emulator laeuft (fremde Instanz NICHT beenden).
Aufruf: gp_lauf.py --state IN.sav --outdir DIR --n 6 --dt 1.5 [--load 14]
"""
import time, sys, subprocess, os, shutil, argparse

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
SDIR = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates"
SLOT1 = os.path.join(SDIR, "HASH-957757946319438E_1.sav")
RESUME = os.path.join(SDIR, "HASH-957757946319438E_resume.sav")
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"
T0 = time.monotonic()


def log(m):
    print("[%6.1f] %s" % (time.monotonic() - T0, m), flush=True)


def fremd():
    out = subprocess.run(["tasklist", "/NH"], capture_output=True, text=True).stdout.lower()
    return ("duckstation" in out) or ("pcsx" in out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", required=True)
    ap.add_argument("--outdir", required=True)
    ap.add_argument("--n", type=int, default=6)
    ap.add_argument("--dt", type=float, default=1.5)
    ap.add_argument("--load", type=float, default=14.0)
    a = ap.parse_args()
    os.makedirs(a.outdir, exist_ok=True)
    if fremd():
        log("ABBRUCH: fremder Emulator laeuft"); sys.exit(9)
    import vgamepad as vg
    B = vg.XUSB_BUTTON
    gp = vg.VX360Gamepad(); time.sleep(3.0)
    resume_mt = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", os.path.abspath(a.state), CUE])
    log("DuckStation pid %d, %s" % (proc.pid, a.state))
    time.sleep(a.load)
    for i in range(a.n):
        t1 = time.monotonic()
        mt = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        gp.press_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update(); time.sleep(0.10)
        gp.release_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update()
        ok = False
        for _ in range(60):
            time.sleep(0.05)
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > mt:
                ok = True; break
        time.sleep(0.25)
        dst = os.path.join(a.outdir, "z_%02d.sav" % i)
        if ok:
            shutil.copy2(SLOT1, dst); log("Zwischenstand %s" % dst)
        else:
            log("WARNUNG: Slot 1 nicht neu (%s)" % dst)
        r = a.dt - (time.monotonic() - t1)
        if r > 0:
            time.sleep(r)
    log("grazioes schliessen")
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    for _ in range(60):
        time.sleep(0.5)
        if proc.poll() is not None:
            break
    time.sleep(1.5)
    for _ in range(30):
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_mt:
            shutil.copy2(RESUME, os.path.join(a.outdir, "z_ende.sav")); log("FERTIG"); return
        time.sleep(0.5)
    log("FEHLER: resume.sav nicht neu"); sys.exit(3)


if __name__ == "__main__":
    main()
