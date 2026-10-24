#!/usr/bin/env python3
"""Runde 41 Spur J: Original-Szene aus einem Raum-Zustand (kein Debug-Menue) in DuckStation messen.

Ablauf: einen (ggf. per re15_ss_patch.py gepatchten) Savestate direkt laden (-statefile), nach --load Sekunden die
Aufnahme starten (--video: ToggleMediaCapture voruebergehend auf SDL-0/Guide), dann im Takt --takt abwechselnd die
Knoepfe aus --knoepfe druecken (Aktion/Nachricht weiter), Slot-1-Saves (LB = SaveSelectedSaveState) zu den Zeiten
--lbzeiten (Sekunden nach dem Laden) ziehen, nach --dauer Sekunden Aufnahme aus und den EIGENEN Prozess per
taskkill /PID ohne /F schliessen. settings.ini wird im finally BYTEGLEICH zurueckgeschrieben. Vorlage: j41_orig.py.
"""
import time, sys, subprocess, os, shutil, argparse, glob, re
import vgamepad as vg
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
SSDIR = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates"
RESUME = os.path.join(SSDIR, "HASH-957757946319438E_resume.sav")
SLOT1 = os.path.join(SSDIR, "HASH-957757946319438E_1.sav")
VIDEOS = r"C:\Users\mjoedicke\AppData\Local\DuckStation\videos"
INI = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
B = vg.XUSB_BUTTON
T0 = time.monotonic()
def log(m): print("[%6.1f] %s" % (time.monotonic() - T0, m), flush=True)
def lebt(pid):
    r = subprocess.run(["tasklist", "/FI", "PID eq %d" % pid, "/NH"], capture_output=True)
    return (" %d " % pid) in r.stdout.decode("mbcs", "replace")

ap = argparse.ArgumentParser()
ap.add_argument("--state", required=True)
ap.add_argument("--load", type=float, default=16.0)
ap.add_argument("--video", action="store_true")
ap.add_argument("--knoepfe", default="X,O")       # X = Kreuz (SDL A), O = Kreis (SDL B), abwechselnd
ap.add_argument("--takt", type=float, default=1.5)
ap.add_argument("--druck_bis", type=float, default=60.0)   # Sekunden nach dem Laden, bis dahin wird gedrueckt
ap.add_argument("--lbzeiten", default="")
ap.add_argument("--dauer", type=float, default=60.0)
ap.add_argument("--out", required=True)
args = ap.parse_args()
BTN = {"X": B.XUSB_GAMEPAD_A, "O": B.XUSB_GAMEPAD_B, "S": B.XUSB_GAMEPAD_X, "U": B.XUSB_GAMEPAD_DPAD_UP}
# Laeuft schon ein DuckStation (andere Spur)? Dann NICHT starten (Slot-1/Resume/Pad wuerden kollidieren).
r = subprocess.run(["tasklist", "/FI", "IMAGENAME eq duckstation-qt-x64-ReleaseLTCG.exe", "/NH"], capture_output=True)
if b"duckstation" in r.stdout.lower():
    log("ABBRUCH: ein anderes DuckStation laeuft bereits"); sys.exit(3)
ini = open(INI, "rb").read(); bak = ini; ini2 = ini
if args.video:
    pat = rb"(?m)^ToggleMediaCapture = [^\r\n]*"
    if not re.search(pat, ini2): log("WARN: ToggleMediaCapture fehlt"); sys.exit(2)
    ini2 = re.sub(pat, b"ToggleMediaCapture = SDL-0/Guide", ini2)
changed = ini2 != ini
if changed: open(INI, "wb").write(ini2); log("settings.ini voruebergehend: ToggleMediaCapture = SDL-0/Guide")
vids_before = set(glob.glob(os.path.join(VIDEOS, "*")))
proc = None
try:
    gp = vg.VX360Gamepad(); time.sleep(3.0); log("vgamepad up")
    def tap(btn, hold=0.13, gap=0.0):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update()
        if gap > 0: time.sleep(gap)
    def lb_save(name):
        m0 = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        tap(B.XUSB_GAMEPAD_LEFT_SHOULDER, hold=0.20)
        for _ in range(30):
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > m0: break
            time.sleep(0.1)
        time.sleep(0.3)
        if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > m0:
            dst = os.path.splitext(args.out)[0] + "_%s.sav" % name
            shutil.copyfile(SLOT1, dst); log("Slot 1 -> %s" % dst); return dst
        log("WARN: Slot 1 nicht neu (%s)" % name); return None
    ps = ("$p = Start-Process -FilePath '%s' -ArgumentList '-batch','-statefile','\"%s\"','\"%s\"' -PassThru; $p.Id"
          % (DUCK, os.path.abspath(args.state), CUE))
    r0 = subprocess.run(["powershell", "-NoProfile", "-Command", ps], capture_output=True)
    proc = int(r0.stdout.decode("mbcs", "replace").strip().splitlines()[-1])
    log("DuckStation pid=%d, Zustand laden %.0fs" % (proc, args.load)); time.sleep(args.load)
    t_load = time.monotonic()
    if args.video:
        log("GUIDE -> Aufnahme AN"); tap(B.XUSB_GAMEPAD_GUIDE, hold=0.25)
    knoepfe = [BTN[k] for k in args.knoepfe.split(",") if k.strip()]
    lbz = [float(x) for x in args.lbzeiten.split(",") if x.strip()]
    naechster_druck = 0.3; ki = 0; li = 0
    while True:
        jetzt = time.monotonic() - t_load
        if jetzt >= args.dauer: break
        if li < len(lbz) and jetzt >= lbz[li]:
            lb_save("t%02d_%.1fs" % (li, lbz[li])); li += 1; continue
        if knoepfe and jetzt >= naechster_druck and jetzt < args.druck_bis:
            tap(knoepfe[ki % len(knoepfe)], hold=0.12); ki += 1; naechster_druck += args.takt; continue
        time.sleep(0.02)
    if args.video:
        log("GUIDE -> Aufnahme AUS"); tap(B.XUSB_GAMEPAD_GUIDE, hold=0.25, gap=3.0)
    resume_m = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    log("Close pid %d (taskkill /PID ohne /F)" % proc)
    subprocess.run(["taskkill", "/PID", str(proc)], capture_output=True)
    for _ in range(60):
        time.sleep(0.5)
        if not lebt(proc): break
    time.sleep(2.0)
    if os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_m:
        shutil.copyfile(RESUME, args.out); log("Resume -> %s" % args.out)
    new = sorted(set(glob.glob(os.path.join(VIDEOS, "*"))) - vids_before, key=os.path.getmtime)
    for v in new: log("VIDEO %s (%d B)" % (v, os.path.getsize(v)))
    if new:
        dst = os.path.splitext(args.out)[0] + os.path.splitext(new[-1])[1]
        shutil.copyfile(new[-1], dst); log("Video -> %s" % dst)
finally:
    if changed: open(INI, "wb").write(bak); log("settings.ini wiederhergestellt (bytegleich)")
    if proc is not None and lebt(proc):
        log("Prozess lebt noch (pid %d) -> taskkill /PID ohne /F" % proc)
        subprocess.run(["taskkill", "/PID", str(proc)], capture_output=True)
        for _ in range(60):
            time.sleep(0.5)
            if not lebt(proc): break
