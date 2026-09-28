#!/usr/bin/env python3
"""r30_bau_ingame_cmp.py - Runde 30 / Thema D, Bau-Agent.
Vorher/Nachher: komponierte SPIEL-Bilder (RE15_FRAMEDUMP, echter Renderpfad) der
Ausgangs-exe und der neuen exe byte-weise vergleichen.
Aufruf: r30_bau_ingame_cmp.py <exe_vorher> <exe_nachher> <ausgabe-verzeichnis> [sekunden]"""
import os, subprocess, sys, hashlib, time

SECS = float(sys.argv[4]) if len(sys.argv) > 4 else 25.0

def run(exe, wd):
    os.makedirs(wd, exist_ok=True)
    for f in os.listdir(wd):
        if f.endswith(".ppm"): os.remove(os.path.join(wd, f))
    env = dict(os.environ)
    env.update({"RE15_NO_INTRO": "1", "RE15_TITLE_SHOT": "title.bmp", "RE15_TITLE_SHOT_AF": "4",
                "RE15_FRAMEDUMP": "30-450/30:" + wd.replace("\\", "/") + "/f_"})
    t0 = time.time()
    p = subprocess.Popen([exe], cwd=wd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        p.wait(timeout=SECS)
    except subprocess.TimeoutExpired:
        p.terminate(); p.wait()
    print("  %s: beendet nach %.1f s" % (os.path.basename(exe), time.time() - t0))
    out = {}
    for f in sorted(os.listdir(wd)):
        if f.endswith(".ppm"):
            d = open(os.path.join(wd, f), "rb").read()
            out[f] = (hashlib.sha256(d).hexdigest(), len(set(d[-320 * 240 * 3:])))
    return out

a = run(sys.argv[1], os.path.join(sys.argv[3], "vorher"))
b = run(sys.argv[2], os.path.join(sys.argv[3], "nachher"))
names = sorted(set(a) & set(b))
same = 0
for n in names:
    eq = a[n][0] == b[n][0]
    same += eq
    print("  %-16s vorher %s  nachher %s  (%3d verschiedene Bytewerte)  %s" % (
        n, a[n][0][:16], b[n][0][:16], a[n][1], "GLEICH" if eq else "VERSCHIEDEN"))
print("Spielbilder: %d von %d byte-gleich; verschiedene Bilder darunter: %d" % (
    same, len(names), len(set(a[n][0] for n in names))))
sys.exit(0 if same == len(names) and names else 1)
