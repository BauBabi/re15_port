#!/usr/bin/env python3
"""Runde 41 Spur J: Original-Szenen ROOM3060 (Marvin-Ende) / ROOM3061 (Sherry-Schacht) in DuckStation.

Vorlage: Spur-E-Treiber abn0_orig_kurbel.py (Abnahme E 0 §3.6). Ablauf: Quick-Load eines GEPATCHTEN Debug-Menue-Stands
(re15_ss_patch.py: Spielermodell work_vars[0x10] = DAT_800b0ff0, Flags, JUMP-Satz), Debug-JUMP Stage/Index mit
geschlossenem Regelkreis (Menue-Ist aus Slot-1-Save: Zeile 0x800bbe5d, Stage 0x800bbe5e, Index 0x800bbe5f+Stage),
dann entweder Video (ToggleMediaCapture voruebergehend auf SDL-0/LeftShoulder; Aufnahme laeuft ab VOR dem Laden) oder
Slot-1-Saves zu festen Zeiten nach dem Laden (--lbzeiten). settings.ini wird im finally BYTEGLEICH zurueckgeschrieben.
Close des EIGENEN Prozesses per taskkill /PID ohne /F (nie /IM).
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
SKRIPTE = "C:/workspace/git/reAi_v2/.claude/skills/re15-savestate-ghidra/scripts"
B = vg.XUSB_BUTTON
T0 = time.monotonic()
def log(m): print("[%6.1f] %s" % (time.monotonic() - T0, m), flush=True)
def lebt(pid):
    r = subprocess.run(["tasklist", "/FI", "PID eq %d" % pid, "/NH"], capture_output=True)
    return (" %d " % pid) in r.stdout.decode("mbcs", "replace")

ap = argparse.ArgumentParser()
ap.add_argument("--state", required=True)
ap.add_argument("--triangle", type=int, default=2)
ap.add_argument("--right", type=int, default=6)
ap.add_argument("--load", type=float, default=16.0)
ap.add_argument("--video", action="store_true")
ap.add_argument("--lbzeiten", default="")     # Sekunden nach dem Laden, z.B. "18,20,22"
ap.add_argument("--seq", default="")          # nach dem Laden: Knopf:Halten:Pause,...
ap.add_argument("--tail", type=float, default=12.0)
ap.add_argument("--out", required=True)
ap.add_argument("--select", action="store_true")   # Zustand ohne offenes Debug-Menue: erst SELECT (Back) = Menue auf
ap.add_argument("--video_nach", type=float, default=-1.0)   # Video erst N s NACH dem Laden einschalten (Aufnahme bricht beim
                                                            # Stage-Wechsel ab, gemessen o3060_v1: nur 8,9 s Menue-Bilder)
args = ap.parse_args()
BTN = {"S": B.XUSB_GAMEPAD_X, "X": B.XUSB_GAMEPAD_A, "U": B.XUSB_GAMEPAD_DPAD_UP, "D": B.XUSB_GAMEPAD_DPAD_DOWN,
       "L": B.XUSB_GAMEPAD_DPAD_LEFT, "R": B.XUSB_GAMEPAD_DPAD_RIGHT, "T": B.XUSB_GAMEPAD_START,
       "Y": B.XUSB_GAMEPAD_Y, "M": B.XUSB_GAMEPAD_RIGHT_SHOULDER, "W": None}
ini = open(INI, "rb").read(); bak = ini; ini2 = ini
if args.video:
    pat = rb"(?m)^ToggleMediaCapture = [^\r\n]*"
    if not re.search(pat, ini2): log("WARN: ToggleMediaCapture fehlt"); sys.exit(2)
    ini2 = re.sub(pat, b"ToggleMediaCapture = SDL-0/Guide", ini2)
changed = ini2 != ini
if changed: open(INI, "wb").write(ini2); log("settings.ini voruebergehend: ToggleMediaCapture = SDL-0/Guide (eigener Knopf, LB bleibt Slot-1-Save)")
vids_before = set(glob.glob(os.path.join(VIDEOS, "*")))
proc = None
sys.path.insert(0, SKRIPTE)
from re15_ss import Ram
try:
    gp = vg.VX360Gamepad(); time.sleep(3.0); log("vgamepad up")
    def tap(btn, hold=0.13, gap=0.6):
        if btn is not None:
            gp.press_button(button=btn); gp.update(); time.sleep(hold)
            gp.release_button(button=btn); gp.update()
        time.sleep(gap)
    def lb_save(name):
        m0 = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        tap(B.XUSB_GAMEPAD_LEFT_SHOULDER, hold=0.20, gap=0.2)
        for _ in range(30):
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > m0: break
            time.sleep(0.2)
        time.sleep(0.4)
        if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > m0:
            dst = os.path.splitext(args.out)[0] + "_%s.sav" % name
            shutil.copyfile(SLOT1, dst); log("Slot 1 -> %s" % dst); return dst
        log("WARN: Slot 1 nicht neu (%s)" % name); return None
    # Start ueber PowerShell Start-Process: ein direkt per CreateProcess (subprocess.Popen) gestartetes DuckStation
    # beendet sich in dieser Umgebung nach 1-2 s mit rc 0 (gemessen 2026-10-24, probe_ds2.py: plain/nohandles/newcon/
    # detached/breakaway), per Start-Process laeuft es (probe_ds3.py).
    ps = ("$p = Start-Process -FilePath '%s' -ArgumentList '-batch','-statefile','\"%s\"','\"%s\"' -PassThru; $p.Id"
          % (DUCK, os.path.abspath(args.state), CUE))
    r0 = subprocess.run(["powershell", "-NoProfile", "-Command", ps], capture_output=True)
    proc_pid = int(r0.stdout.decode("mbcs", "replace").strip().splitlines()[-1])
    proc = proc_pid
    log("DuckStation pid=%d, Zustand laden %.0fs" % (proc_pid, args.load)); time.sleep(args.load)
    if args.select:
        log("SELECT -> Debug-Menue"); tap(B.XUSB_GAMEPAD_BACK, hold=0.13, gap=1.5)
    for _ in range(2): tap(B.XUSB_GAMEPAD_DPAD_UP, hold=0.10, gap=0.40)
    tap(B.XUSB_GAMEPAD_DPAD_DOWN, hold=0.10, gap=0.60)
    for i in range(args.triangle): tap(B.XUSB_GAMEPAD_Y, hold=0.10, gap=0.60)
    for i in range(args.right): tap(B.XUSB_GAMEPAD_DPAD_RIGHT, hold=0.10, gap=0.60)
    lb_count = 0
    for versuch in range(16):
        p = lb_save("menue"); lb_count += 1
        if not p: continue
        r = Ram(p); row, st = r.u8(0x800bbe5d), r.u8(0x800bbe5e); idx = r.u8(0x800bbe5f + st)
        log("Menue-Ist: Zeile %d Stage %d Index %d (Soll 1/%d/%d)" % (row, st, idx, args.triangle, args.right))
        if row == 1 and st == args.triangle and idx == args.right: break
        if row != 1: tap(B.XUSB_GAMEPAD_DPAD_DOWN if row < 1 else B.XUSB_GAMEPAD_DPAD_UP, hold=0.10, gap=0.60); continue
        if st != args.triangle: tap(B.XUSB_GAMEPAD_Y, hold=0.10, gap=0.60); continue
        tap(B.XUSB_GAMEPAD_DPAD_RIGHT if idx < args.right else B.XUSB_GAMEPAD_DPAD_LEFT, hold=0.10, gap=0.60)
    else:
        log("FEHLER: Menue nicht eingestellt"); raise SystemExit(5)
    # Im Videolauf ist LB zugleich ToggleMediaCapture: nach ungerader Zahl LB laeuft die Aufnahme bereits.
    if args.video and args.video_nach < 0:
        log("GUIDE -> Aufnahme AN (vor dem Laden)"); tap(B.XUSB_GAMEPAD_GUIDE, hold=0.25, gap=1.0)
    time.sleep(0.6)
    log("Quadrat -> LADEN"); t_load = time.monotonic(); tap(B.XUSB_GAMEPAD_X, hold=0.20, gap=0.1)
    if args.video and args.video_nach >= 0:
        w = t_load + args.video_nach - time.monotonic()
        if w > 0: time.sleep(w)
        log("GUIDE -> Aufnahme AN (%.1f s nach dem Laden)" % args.video_nach)
        tap(B.XUSB_GAMEPAD_GUIDE, hold=0.25, gap=0.1)
    for tok in [t for t in args.seq.split(",") if t.strip()]:
        k, h, g = tok.split(":"); log("SEQ %s %.2f/%.2f" % (k, float(h), float(g))); tap(BTN[k], hold=float(h), gap=float(g))
    for i, t in enumerate([float(x) for x in args.lbzeiten.split(",") if x.strip()]):
        w = t_load + t - time.monotonic()
        if w > 0: time.sleep(w)
        lb_save("t%02d_%.1fs" % (i, t))
    time.sleep(args.tail)
    if args.video:
        log("GUIDE -> Aufnahme AUS"); tap(B.XUSB_GAMEPAD_GUIDE, hold=0.25, gap=3.0)
    resume_m = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    log("Close pid %d (taskkill /PID ohne /F)" % proc_pid)
    subprocess.run(["taskkill", "/PID", str(proc_pid)], capture_output=True)
    for _ in range(60):
        time.sleep(0.5)
        if not lebt(proc_pid): break
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
