#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht) - LIVE-Savestate-Serie im ORIGINAL (DuckStation, MZD-Disc).

Laedt einen ROOM1170-Savestate per -statefile (Quickload, Skill re15-room-capture), laesst das
Spiel OHNE Eingabe laufen und zieht in kurzen Abstaenden Live-Savestates ueber den
Controller-gebundenen Hotkey SaveSelectedSaveState = SDL-0/LeftShoulder (settings.ini
[Hotkeys], SINGLE-Binding; Technik aus re15_fire_capture.py). Jede Kopie = ein Bild des
laufenden Spiels mit RAM + VRAM.

Auswertung: ds_sign_eval.py (Schriftbereich je Bild + VBlank-Zaehler Vcount @0x800787dc).

  C:/Python310/python.exe re15_port/tools/r34n_g/ds_sign_series.py \
      --state C:/workspace/git/reAi_v2/stage_saves/lamp_near.sav --n 40 --gap 0.25 \
      --outdir <scratch>/caps
Vorher: tasklist | grep -i duckstation  (laeuft eine Instanz -> NICHT starten).
"""
import argparse, os, shutil, subprocess, sys, time
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
SLOT1 = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_1.sav"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"
B = vg.XUSB_BUTTON
T0 = time.monotonic()


def log(m):
    print("[%6.1f] %s" % (time.monotonic() - T0, m), flush=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", required=True)
    ap.add_argument("--load", type=float, default=14.0)
    ap.add_argument("--n", type=int, default=40)
    ap.add_argument("--gap", type=float, default=0.25)
    ap.add_argument("--outdir", required=True)
    ap.add_argument("--press", default="", help="optional: Taste vor der Serie (z.B. U0.4)")
    a = ap.parse_args()
    os.makedirs(a.outdir, exist_ok=True)
    for f in os.listdir(a.outdir):
        if f.startswith("cap_"):
            os.remove(os.path.join(a.outdir, f))

    gp = vg.VX360Gamepad()
    time.sleep(3.0)
    log("vgamepad up")

    def lbsave(dst):
        mt = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        gp.press_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update(); time.sleep(0.10)
        gp.release_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update()
        for _ in range(40):
            time.sleep(0.05)
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > mt:
                break
        time.sleep(0.10)
        shutil.copy2(SLOT1, dst)

    proc = subprocess.Popen([DUCK, "-batch", "-statefile", a.state, CUE])
    log("lade %.0fs ..." % a.load)
    time.sleep(a.load)
    if a.press:
        pm = {"U": B.XUSB_GAMEPAD_DPAD_UP, "D": B.XUSB_GAMEPAD_DPAD_DOWN,
              "L": B.XUSB_GAMEPAD_DPAD_LEFT, "R": B.XUSB_GAMEPAD_DPAD_RIGHT}
        for tok in a.press.split(","):
            tok = tok.strip()
            if not tok:
                continue
            gp.press_button(button=pm[tok[0]]); gp.update(); time.sleep(float(tok[1:] or "0.3"))
            gp.release_button(button=pm[tok[0]]); gp.update(); time.sleep(0.2)
    for i in range(a.n):
        lbsave(os.path.join(a.outdir, "cap_%03d.sav" % i))
        log("cap %d" % i)
        time.sleep(a.gap)
    log("schliessen (graceful, ohne /F)")
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    for _ in range(40):
        time.sleep(0.5)
        if proc.poll() is not None:
            break
    log("fertig")


if __name__ == "__main__":
    main()
