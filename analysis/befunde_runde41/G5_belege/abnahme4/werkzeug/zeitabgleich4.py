# Abnahme 4: Zeitachse RE2 <-> Port per Bildabgleich pruefen. Fuer Port-Szenenbild n (Framedump) das RE2-Videobild mit der kleinsten
# mittleren Differenz (160x120, Graustufen) im Fenster +-20 Spielbilder um re2_t(n) suchen -> Versatz in Spielbildern.
import subprocess, sys, os, numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2zeit4 import re2_t
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
src = sys.argv[1]; fd = sys.argv[2]; ns = [int(x) for x in sys.argv[3].split(",")]
def video(t0, t1):
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % (t1 - t0), "-i", src,
                          "-vf", "scale=160:120,format=gray", "-f", "rawvideo", "-"], capture_output=True).stdout
    return np.frombuffer(raw, np.uint8).reshape(-1, 120, 160).astype(np.float32)
for n in ns:
    t0 = re2_t(n) - 20 / 29.9; t1 = re2_t(n) + 20 / 29.9
    v = video(t0, t1)
    p = np.asarray(Image.open(fd % (1613 + n)).convert("L").resize((160, 120), Image.BILINEAR), dtype=np.float32)
    d = np.abs(v - p[None]).mean(axis=(1, 2))
    k = int(np.argmin(d)); t = t0 + k / VFPS
    print("Port Bild %4d: bestes RE2-Videobild %.3f s (re2_t %.3f) Versatz %+.1f Spielbilder, Diff %.1f (Median %.1f)" % (n, t, re2_t(n), (t - re2_t(n)) * 29.9, d[k], np.median(d)))
