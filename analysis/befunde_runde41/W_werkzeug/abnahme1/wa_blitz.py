"""wa_blitz.py - Abnahme W1: Blink-Analyse einer Aufnahme (capture.mkv, Fenstergroesse aus ffprobe, 30 Bilder/s).
Liest die Bilder ab <von_s>, Graustufen. Je Bildpunkt Median ueber die Folge; ein BLITZ ist ein Lauf von 1..12 Bildern,
in dem der Punkt > SCHWELLE (25) ueber seinem Median liegt und danach wieder darunter faellt, an einem Punkt, der in
>= 80 % der Bilder innerhalb 12 vom Median liegt (ruhender Hintergrund). Blitz-Bildpunkte je Bild werden zu Flecken
(8er-Nachbarschaft) zusammengefasst; Fleck >= 3 Bildpunkte zaehlt. Ausserdem: Bildwechsel mit Aenderung > 8 (wie w_flacker).
Aufruf: wa_blitz.py <mkv> <out_prefix> [von_s] [--fenster x0 x1 y0 y1]
Schreibt <out_prefix>_karte.png (Blitzhaeufigkeit rot ueber dem Medianbild) und <out_prefix>.txt."""
import subprocess, sys, numpy as np
from PIL import Image
from scipy import ndimage
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
mkv, pre = sys.argv[1], sys.argv[2]
von = float(sys.argv[3]) if len(sys.argv) > 3 and not sys.argv[3].startswith("--") else 0.0
fen = None
if "--fenster" in sys.argv:
    k = sys.argv.index("--fenster"); fen = tuple(int(v) for v in sys.argv[k + 1:k + 5])
pr = subprocess.run([FF.replace("ffmpeg.exe", "ffprobe.exe"), "-v", "error", "-show_entries", "stream=width,height", "-of", "csv=p=0", mkv],
                    capture_output=True, text=True).stdout.strip().split(",")
W, H = int(pr[0]), int(pr[1])
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", str(von), "-i", mkv, "-f", "rawvideo", "-pix_fmt", "rgb24", "-"],
                     capture_output=True).stdout
n = len(raw) // (W * H * 3)
rgb = np.frombuffer(raw[: n * W * H * 3], np.uint8).reshape(n, H, W, 3)
g = rgb.astype(np.int16).mean(axis=3).astype(np.int16)
med = np.median(g, axis=0).astype(np.int16)
dev = g - med[None]
ruhig = (np.abs(dev) <= 12).mean(axis=0) >= 0.80
hoch = (dev > 25) & ruhig[None]
# Laeufe 1..12 Bilder lang (beidseitig begrenzt)
blitz = np.zeros_like(hoch)
lauf = np.zeros(hoch.shape[1:], np.int16)
start = np.zeros(hoch.shape[1:], np.int16)
for t in range(n):
    h = hoch[t]
    neu = h & (lauf == 0)
    start[neu] = t
    lauf[h] += 1
    ende = (~h) & (lauf > 0)
    if ende.any():
        ys, xs = np.nonzero(ende & (lauf <= 12) & (start > 0))
        for y, x in zip(ys, xs):
            blitz[start[y, x]:t, y, x] = True
    lauf[~h] = 0
ereig = []
for t in range(n):
    if not blitz[t].any(): continue
    lab, k = ndimage.label(blitz[t], structure=np.ones((3, 3)))
    for j, sl in enumerate(ndimage.find_objects(lab), 1):
        area = int((lab[sl] == j).sum())
        if area < 3: continue
        y0, y1, x0, x1 = sl[0].start, sl[0].stop - 1, sl[1].start, sl[1].stop - 1
        if fen and not (fen[0] <= (x0 + x1) / 2 <= fen[1] and fen[2] <= (y0 + y1) / 2 <= fen[3]): continue
        dmax = int(dev[t][sl][lab[sl] == j].max())
        ereig.append((t, x0, x1, y0, y1, area, dmax))
# Blitz-Ereignisse = zusammenhaengende Bildfolgen desselben Ortes
ev = []
for e in ereig:
    for v in ev:
        if e[0] - v["t1"] <= 1 and not (e[2] < v["x0"] - 4 or e[1] > v["x1"] + 4 or e[4] < v["y0"] - 4 or e[3] > v["y1"] + 4):
            v["t1"] = e[0]; v["x0"] = min(v["x0"], e[1]); v["x1"] = max(v["x1"], e[2]); v["y0"] = min(v["y0"], e[3]); v["y1"] = max(v["y1"], e[4]); v["dmax"] = max(v["dmax"], e[6]); v["px"] = max(v["px"], e[5]); break
    else:
        ev.append({"t0": e[0], "t1": e[0], "x0": e[1], "x1": e[2], "y0": e[3], "y1": e[4], "dmax": e[6], "px": e[5]})
d8 = np.abs(np.diff(g, axis=0)) > 8
wechsel = int((d8.reshape(n - 1, -1).any(axis=1)).sum()) if n > 1 else 0
karte = blitz.sum(axis=0)
img = np.stack([med] * 3, axis=2).clip(0, 255).astype(np.uint8)
m = karte > 0
img[m] = [255, 0, 0]
Image.fromarray(img).save(pre + "_karte.png")
with open(pre + ".txt", "w") as f:
    f.write("bilder=%d ab=%.1fs wechsel_gt8=%d von %d blitz_ereignisse=%d blitz_bildpunkte=%d\n" % (n, von, wechsel, n - 1, len(ev), int(m.sum())))
    for v in ev:
        f.write("  Blitz Bild %d..%d (%d Bilder) x%d..%d y%d..%d max+%d px%d\n" % (v["t0"], v["t1"], v["t1"] - v["t0"] + 1, v["x0"], v["x1"], v["y0"], v["y1"], v["dmax"], v["px"]))
print(open(pre + ".txt").read().splitlines()[0])
