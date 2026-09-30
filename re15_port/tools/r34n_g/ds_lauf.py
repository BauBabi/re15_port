#!/usr/bin/env python3
"""Spur G2 (Runde 34 Nacht) - EIN DuckStation-Lauf auf einem Savestate, OHNE Eingabe, mit
Sicherung der Nutzer-Staende.

Warum ohne Eingabe: die Pad-Bindungen in settings.ini sind "Keyboard/X & SDL-0/Y" = AKKORD
(beide gleichzeitig); ein virtueller Pad allein loest nichts aus (gemessen 2026-09-30 08:42:
15x Links + Quadrat im Debug-Menue, Menue blieb auf "JUMP 124 OPENING"). Die Einstellungen des
Nutzers werden NICHT geaendert - stattdessen wird der Savestate vorher per RAM-Patch in den
gewuenschten Zustand gebracht (re15_ss_patch.py) und hier nur abgespielt.

Ablauf:
  1. tasklist: laeuft DuckStation, wird abgebrochen (nie parallel starten).
  2. HASH-957757946319438E_resume.sav, _1.sav, _1.bak nach <sicherung> kopieren (copy2).
  3. DuckStation -batch -statefile <stand> <cue>; nach --warte s optional Aufnahme (ffmpeg
     gdigrab, Desktop-Ausschnitt --bereich x,y,w,h, --fps) ueber --dauer s und/oder ein
     Desktop-Standbild (--standbild).
  4. graziles Schliessen (taskkill OHNE /F -> SaveStateOnExit schreibt resume.sav), warten,
     resume.sav nach <aus.sav> kopieren (nur wenn frisch geschrieben).
  5. Sicherungen ZURUECKLEGEN und per SHA-256 pruefen (auch im Fehlerfall, finally).

  C:/Python310/python.exe re15_port/tools/r34n_g/ds_lauf.py <stand.sav> <aus.sav> <sicherung-ordner>
      [--warte 14] [--dauer 0] [--aufnahme rec.mkv] [--bereich 0,48,600,900] [--fps 60]
      [--standbild shot.png] [--nachlauf 0]
"""
import argparse
import hashlib
import os
import shutil
import subprocess
import sys
import time

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
SS = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates"
NUTZER = ["HASH-957757946319438E_resume.sav", "HASH-957757946319438E_1.sav",
          "HASH-957757946319438E_1.bak"]
FFMPEG = "C:/ProgramData/chocolatey/bin/ffmpeg"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest() if os.path.exists(p) else None


def laeuft():
    r = subprocess.run(["tasklist"], capture_output=True, text=True)
    return "duckstation" in r.stdout.lower()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("stand")
    ap.add_argument("aus")
    ap.add_argument("sicherung")
    ap.add_argument("--warte", type=float, default=14.0)
    ap.add_argument("--dauer", type=float, default=0.0)
    ap.add_argument("--aufnahme", default="")
    ap.add_argument("--bereich", default="0,48,600,900")
    ap.add_argument("--fps", type=int, default=60)
    ap.add_argument("--standbild", default="")
    ap.add_argument("--nachlauf", type=float, default=0.0)
    a = ap.parse_args()
    # DuckStation oeffnet den Stand NICHT ueber einen 8.3-Kurzpfad ("MJOEDI~1"): gemessen
    # 2026-09-30 08:49/08:51 - zwei Laeufe ohne Fenster und ohne resume.sav. Deshalb Langform.
    a.stand = os.path.realpath(a.stand)
    if laeuft():
        print("ABBRUCH: DuckStation laeuft bereits - warten statt parallel starten")
        return 3
    os.makedirs(a.sicherung, exist_ok=True)
    vorher = {}
    for n in NUTZER:
        q = os.path.join(SS, n)
        if os.path.exists(q):
            shutil.copy2(q, os.path.join(a.sicherung, n))
            vorher[n] = sha(q)
    print("gesichert:", {k: v[:12] for k, v in vorher.items()})
    resume = os.path.join(SS, NUTZER[0])
    t_resume = os.path.getmtime(resume) if os.path.exists(resume) else 0
    p = None
    try:
        p = subprocess.Popen([DUCK, "-batch", "-statefile", a.stand, CUE])
        time.sleep(a.warte)
        if a.standbild:
            subprocess.run([FFMPEG, "-hide_banner", "-loglevel", "error", "-y", "-f", "gdigrab",
                            "-framerate", "1", "-i", "desktop", "-frames:v", "1", a.standbild])
        if a.aufnahme and a.dauer > 0:
            x, y, w, h = [int(v) for v in a.bereich.split(",")]
            subprocess.run([FFMPEG, "-hide_banner", "-loglevel", "error", "-y", "-f", "gdigrab",
                            "-framerate", str(a.fps), "-offset_x", str(x), "-offset_y", str(y),
                            "-video_size", "%dx%d" % (w, h), "-i", "desktop", "-t", str(a.dauer),
                            "-c:v", "libx264rgb", "-qp", "0", "-preset", "ultrafast", a.aufnahme])
        elif a.dauer > 0:
            time.sleep(a.dauer)
        if a.nachlauf > 0:
            time.sleep(a.nachlauf)
        subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
        for _ in range(60):
            time.sleep(0.5)
            if p.poll() is not None:
                break
        print("DuckStation exit", p.poll())
        time.sleep(1.0)
        if os.path.exists(resume) and os.path.getmtime(resume) > t_resume:
            shutil.copy2(resume, a.aus)
            print("Endstand ->", a.aus)
        else:
            print("WARNUNG: resume.sav nicht neu geschrieben - kein Endstand")
    finally:
        if p is not None and p.poll() is None:
            subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
            time.sleep(3)
        for n, h in vorher.items():
            shutil.copy2(os.path.join(a.sicherung, n), os.path.join(SS, n))
            ok = sha(os.path.join(SS, n)) == h
            print("zurueckgelegt %s: %s" % (n, "OK" if ok else "FEHLER"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
