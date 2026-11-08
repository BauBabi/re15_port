# nb4_kollision.py <exe> <arbeitsordner> <versatz_s> [getrennt]
# Startet test_r41_gatorfix nb4 (Leon) und nach <versatz_s> Sekunden nb4_elza, beide G2_NB4_TEIL=K.
# Ohne 'getrennt': beide im SELBEN Arbeitsordner (wie ctest: WORKING_DIRECTORY build/tests/unit) -> gemeinsame gator_boss.log.
# Mit 'getrennt': je ein eigener Unterordner.
import os, subprocess, sys, time
exe, wd, versatz = sys.argv[1], sys.argv[2], float(sys.argv[3])
getrennt = len(sys.argv) > 4 and sys.argv[4] == "getrennt"
os.makedirs(wd, exist_ok=True)
env = dict(os.environ, G2_NB4_TEIL="K")
def start(teil):
    d = os.path.join(wd, teil) if getrennt else wd
    os.makedirs(d, exist_ok=True)
    out = open(os.path.join(wd, teil + "_out.txt"), "w")
    return subprocess.Popen([exe, teil], cwd=d, env=env, stdout=out, stderr=subprocess.STDOUT), out
t0 = time.time()
pl, ol = start("nb4")
time.sleep(versatz)
pe, oe = start("nb4_elza")
rl = pl.wait(); tl = time.time() - t0
re_ = pe.wait(); te = time.time() - t0
ol.close(); oe.close()
print("nb4 rc=%d (%.1f s)  nb4_elza rc=%d (%.1f s)  versatz %.1f s  %s" % (rl, tl, re_, te, versatz, "getrennt" if getrennt else "gemeinsamer Ordner"))
for teil in ("nb4", "nb4_elza"):
    for z in open(os.path.join(wd, teil + "_out.txt"), errors="replace"):
        if "FEHL" in z or "Fehler" in z or "K1: 0 Zeilen" in z:
            print("  %s: %s" % (teil, z.rstrip()))
