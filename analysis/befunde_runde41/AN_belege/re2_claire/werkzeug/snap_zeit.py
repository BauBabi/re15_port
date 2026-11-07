"""snap_zeit.py LAUFDIR VIDEO PRAEFIX VERSATZ_BILDER - Videozeit jedes Savestates eines Laufs.
Jedes Savestate-Vorschaubild (PRAEFIXnn.png, 256x192) wird gegen alle Bilder des Laufvideos verglichen (64x48 grau, MSE);
aus den eindeutigen Treffern (bestes Bild deutlich besser als das zweitbeste ausserhalb +-3 Bilder) wird der Versatz
Videozeit - Treiberlogzeit bestimmt (Median) und auf alle Staende angewandt. VERSATZ_BILDER rechnet auf die Zeitachse von
Lauf 1 um (Lauf 2 = Lauf 1 + 44 Bilder, Lauf 3 = Lauf 1; an allen 10/12 Schnittkanten bildgenau gemessen)."""
import sys, re, glob, os, subprocess, numpy as np
from PIL import Image
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
FPS = 49.74682
d, v, pre, vb = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4])
p = subprocess.run([FF, "-v", "error", "-i", v, "-vf", "scale=64:48,format=gray", "-f", "rawvideo", "-"], capture_output=True)
V = np.frombuffer(p.stdout, np.uint8).reshape(-1, 48, 64).astype(np.float32)
log = open(os.path.join(d, "log.txt")).read()
T = {m.group(2): float(m.group(1)) for m in re.finditer(r"\[\s*([\d.]+)\] snap -> .*?/(%s\d\d)\.sav" % pre, log)}
rows = []
for f in sorted(glob.glob(os.path.join(d, pre + "*.png"))):
    n = os.path.basename(f)[:-4]
    im = np.asarray(Image.open(f).convert("L").resize((64, 48), Image.BILINEAR), np.float32)
    e = ((V - im) ** 2).mean(axis=(1, 2))
    k = int(np.argmin(e))
    mask = np.ones(len(e), bool); mask[max(0, k - 3):k + 4] = False
    eind = e[mask].min() > 2.0 * e[k] + 5
    rows.append((n, T.get(n), k, e[k], eind))
offs = [k / FPS - t for n, t, k, ek, ei in rows if ei and t is not None]
off = float(np.median(offs))
print("# Versatz Videozeit-Logzeit %.3f s (Median aus %d eindeutigen Treffern, Streuung %.3f s)" % (off, len(offs), float(np.std(offs))))
for n, t, k, ek, ei in rows:
    vt = t + off
    print("%s log %7.3f  Video %7.3f s (Bild %5d)  Lauf-1-Zeit %7.3f s  Bildtreffer %5d %s" % (
        n, t, vt, round(vt * FPS), vt - vb / FPS, k, "eindeutig" if ei else ""))
