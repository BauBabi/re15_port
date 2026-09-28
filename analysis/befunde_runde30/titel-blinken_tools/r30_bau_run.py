#!/usr/bin/env python3
"""r30_bau_run.py - Runde 30 / Thema D, Bau-Agent.
Startet eine exe fuer <sek> Sekunden mit den angegebenen Umgebungsvariablen und beendet
NUR die eigene PID (nie taskkill /IM - parallel laufen fremde re15_pc.exe).
Aufruf: r30_bau_run.py <exe> <sek> <arbeitsverzeichnis> [VAR=wert ...]
Beispiel (Abnahme Periode):
  r30_bau_run.py re15_pc.exe 22 lauf RE15_NO_INTRO=1 RE15_TITLE_PULSE_LOG=puls.txt
  r30_pulse_frame_stats.py puls.txt
ohne VSync zusaetzlich RE15_SOFTWARE_RENDER=1 (RE15_WINDOW_SCALE=1 / 6 aendert die Bildrate)."""
import os, subprocess, sys, time
exe, secs, cwd = sys.argv[1], float(sys.argv[2]), sys.argv[3]
env = dict(os.environ)
for kv in sys.argv[4:]:
    k, v = kv.split("=", 1)
    env[k] = v
os.makedirs(cwd, exist_ok=True)
t0 = time.time()
p = subprocess.Popen([exe], cwd=cwd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
print("pid", p.pid)
try:
    p.wait(timeout=secs)
    print("exe endete selbst, rc=%s nach %.1f s" % (p.returncode, time.time() - t0))
except subprocess.TimeoutExpired:
    p.terminate()
    p.wait()
    print("beendet (eigene PID) nach %.1f s" % (time.time() - t0))
