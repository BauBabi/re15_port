#!/usr/bin/env python3
"""lauf.py - Runde 34 / re_absturz_original.md §2  (DuckStation-Treiber fuer den Granatenversuch)

Laedt einen Savestate per `-statefile` in DuckStation (MZD-Disc = Auslieferungsstand, §2.1),
fuehrt eine Eingabe-Folge ueber einen virtuellen Gamepad (vgamepad/ViGEm = SDL-0) aus,
zieht unterwegs Zwischen-Savestates ueber den Hotkey SaveSelectedSaveState (= SDL-0/LeftShoulder,
settings.ini) und schliesst am Ende GRAZIOES (taskkill ohne /F -> SaveStateOnExit -> resume.sav).

Folge (Kommas trennen Schritte):
  W<s>              warten s Sekunden
  T<KNOPF>          kurz tippen (0.13 s)
  D<KNOPF>          druecken und halten
  U<KNOPF>          loslassen
  H<KNOPF>:<s>      s Sekunden halten
  S<name>           Zwischen-Savestate (LB) -> <outdir>/<name>.sav
  F<n>:<dt>:<name>  n Zwischen-Savestates im Abstand dt s -> <name>_00.sav ...
KNOPF: START SELECT SQ(Viereck) CR(Kreuz) TRI CI R1 UP DOWN LEFT RIGHT

Aufruf: python lauf.py --state IN.sav --outdir DIR --final NAME --seq "W2,TSTART,W4"
Vorher prueft das Skript, dass KEIN DuckStation laeuft (fremde Instanz -> Abbruch, nicht beenden).
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


def laeuft_schon():
    out = subprocess.run(["tasklist", "/NH"], capture_output=True, text=True).stdout.lower()
    return ("duckstation" in out) or ("pcsx" in out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", required=True)
    ap.add_argument("--outdir", required=True)
    ap.add_argument("--final", default="final")
    ap.add_argument("--load", type=float, default=14.0)
    ap.add_argument("--seq", default="")
    args = ap.parse_args()
    os.makedirs(args.outdir, exist_ok=True)
    if laeuft_schon():
        log("ABBRUCH: es laeuft bereits ein Emulator (nicht von mir) -> spaeter erneut")
        sys.exit(9)

    import vgamepad as vg
    B = vg.XUSB_BUTTON
    KN = {"START": B.XUSB_GAMEPAD_START, "SELECT": B.XUSB_GAMEPAD_BACK,
          "SQ": B.XUSB_GAMEPAD_X, "CR": B.XUSB_GAMEPAD_A, "TRI": B.XUSB_GAMEPAD_Y,
          "CI": B.XUSB_GAMEPAD_B, "R1": B.XUSB_GAMEPAD_RIGHT_SHOULDER,
          "UP": B.XUSB_GAMEPAD_DPAD_UP, "DOWN": B.XUSB_GAMEPAD_DPAD_DOWN,
          "LEFT": B.XUSB_GAMEPAD_DPAD_LEFT, "RIGHT": B.XUSB_GAMEPAD_DPAD_RIGHT}
    gp = vg.VX360Gamepad()
    time.sleep(3.0)
    log("vgamepad bereit")

    def lbsave(dst):
        mt = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        gp.press_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update(); time.sleep(0.10)
        gp.release_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update()
        ok = False
        for _ in range(60):
            time.sleep(0.05)
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > mt:
                ok = True
                break
        time.sleep(0.25)
        if ok:
            shutil.copy2(SLOT1, dst)
            log("  Zwischenstand -> %s" % dst)
        else:
            log("  WARNUNG: Slot 1 nicht neu geschrieben (%s)" % dst)
        return ok

    resume_mt = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", os.path.abspath(args.state), CUE])
    log("DuckStation pid=%d, lade %s (%.0f s)" % (proc.pid, args.state, args.load))
    time.sleep(args.load)

    for tok in [t.strip() for t in args.seq.split(",") if t.strip()]:
        c, rest = tok[0], tok[1:]
        if c == "W":
            log("warte %s s" % rest); time.sleep(float(rest))
        elif c == "T":
            log("tippe %s" % rest)
            gp.press_button(button=KN[rest]); gp.update(); time.sleep(0.13)
            gp.release_button(button=KN[rest]); gp.update(); time.sleep(0.15)
        elif c == "D":
            log("druecke %s" % rest); gp.press_button(button=KN[rest]); gp.update(); time.sleep(0.05)
        elif c == "U":
            log("lasse los %s" % rest); gp.release_button(button=KN[rest]); gp.update(); time.sleep(0.05)
        elif c == "H":
            k, s = rest.split(":")
            log("halte %s %s s" % (k, s))
            gp.press_button(button=KN[k]); gp.update(); time.sleep(float(s))
            gp.release_button(button=KN[k]); gp.update(); time.sleep(0.1)
        elif c == "S":
            lbsave(os.path.join(args.outdir, rest + ".sav"))
        elif c == "F":
            n, dt, name = rest.split(":")
            for i in range(int(n)):
                t1 = time.monotonic()
                lbsave(os.path.join(args.outdir, "%s_%02d.sav" % (name, i)))
                rest_t = float(dt) - (time.monotonic() - t1)
                if rest_t > 0:
                    time.sleep(rest_t)
        else:
            log("unbekannter Schritt %s" % tok)

    log("grazioeses Schliessen")
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    for _ in range(60):
        time.sleep(0.5)
        if proc.poll() is not None:
            break
    time.sleep(1.5)
    fresh = False
    for _ in range(30):
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_mt:
            fresh = True
            break
        time.sleep(0.5)
    if not fresh:
        log("FEHLER: resume.sav nicht neu geschrieben")
        sys.exit(3)
    dst = os.path.join(args.outdir, args.final + ".sav")
    shutil.copy2(RESUME, dst)
    log("FERTIG -> %s" % dst)


if __name__ == "__main__":
    main()
