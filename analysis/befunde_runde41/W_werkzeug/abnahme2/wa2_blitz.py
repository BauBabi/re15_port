"""wa2_blitz.py - Abnahme W2: Blink-Analyse einer Fenster-Aufnahme (capture.mkv, 30 Bilder/s).
Bilder ab <ab_s> werden per ffmpeg auf 320x240 (area) gebracht = PSX-Bildpunkte, Graustufen.
(1) BLITZ wie abnahme1/wa_blitz.py: Lauf von 1..12 Bildern > 25 ueber dem Median des Punktes, der in >= 80 % der
    Bilder ruhig ist (|d| <= 12); Fleck >= 3 Bildpunkte (8er-Nachbarschaft); Ereignisse = gleiche Stelle in Folgebildern.
(2) ZWEITE, UNABHAENGIGE SICHT: je Bildpunkt Zahl der Bildwechsel mit |d| > 16 ("Wechselzahl"). Ein Punkt mit
    Wechselzahl >= 6 ausserhalb der Figur-Kaesten gilt als unruhig; ausgegeben werden die Flecken (>= 2 Bildpunkte).
(3) Oberer Bildrand y 0..3: Wechselzahl je Spalte (Tropfen-Fleck (83,0..1) aus Dossier W 5.1).
Figur-Kaesten aus befund.log-Zeilen 'kasten=x..,y..' (erweitert x +-12, y -45..+8 wie Abnahme 1).
Aufruf: wa2_blitz.py <mkv> <out_prefix> <ab_s> [<befund_lauf.log>]"""
import re, subprocess, sys, numpy as np
from PIL import Image
from scipy import ndimage
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
mkv, pre, ab = sys.argv[1], sys.argv[2], float(sys.argv[3])
bef = open(sys.argv[4], encoding="utf-8", errors="replace").read() if len(sys.argv) > 4 else ""
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", str(ab), "-i", mkv, "-vf", "scale=320:240:flags=area",
                      "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
W, H = 320, 240
n = len(raw) // (W * H * 3)
rgb = np.frombuffer(raw[: n * W * H * 3], np.uint8).reshape(n, H, W, 3)
g = rgb.astype(np.int16).mean(axis=3).astype(np.int16)
med = np.median(g, axis=0).astype(np.int16)
dev = g - med[None]
ruhig = (np.abs(dev) <= 12).mean(axis=0) >= 0.80
hoch = (dev > 25) & ruhig[None]
blitz = np.zeros_like(hoch); lauf = np.zeros((H, W), np.int16); start = np.zeros((H, W), np.int16)
for t in range(n):
    h = hoch[t]; neu = h & (lauf == 0); start[neu] = t; lauf[h] += 1
    ende = (~h) & (lauf > 0)
    if ende.any():
        ys, xs = np.nonzero(ende & (lauf <= 12) & (start > 0))
        for y, x in zip(ys, xs): blitz[start[y, x]:t, y, x] = True
    lauf[~h] = 0
maske = np.zeros((H, W), bool)
k = [tuple(map(int, m.groups())) for m in re.finditer(r"kasten=x(-?\d+)\.\.(-?\d+),y(-?\d+)\.\.(-?\d+)", bef)]
for x0, x1, y0, y1 in k:
    maske[max(0, y0 - 45):max(0, min(H, y1 + 9)), max(0, x0 - 12):max(0, min(W, x1 + 13))] = True
ev = []
for t in range(n):
    if not blitz[t].any(): continue
    lab, kk = ndimage.label(blitz[t], structure=np.ones((3, 3)))
    for j, sl in enumerate(ndimage.find_objects(lab), 1):
        a = int((lab[sl] == j).sum())
        if a < 3: continue
        y0, y1, x0, x1 = sl[0].start, sl[0].stop - 1, sl[1].start, sl[1].stop - 1
        fig = bool(maske[(y0 + y1) // 2, (x0 + x1) // 2])
        for v in ev:
            if t - v["t1"] <= 1 and not (x1 < v["x0"] - 4 or x0 > v["x1"] + 4 or y1 < v["y0"] - 4 or y0 > v["y1"] + 4):
                v.update(t1=t, x0=min(v["x0"], x0), x1=max(v["x1"], x1), y0=min(v["y0"], y0), y1=max(v["y1"], y1)); break
        else:
            ev.append(dict(t0=t, t1=t, x0=x0, x1=x1, y0=y0, y1=y1, fig=fig))
wz = (np.abs(np.diff(g, axis=0)) > 16).sum(axis=0) if n > 1 else np.zeros((H, W), int)
unruhig = (wz >= 6) & ~maske
lab, kk = ndimage.label(unruhig, structure=np.ones((3, 3)))
fl = []
for j, sl in enumerate(ndimage.find_objects(lab), 1):
    a = int((lab[sl] == j).sum())
    if a < 2: continue
    fl.append((a, sl[1].start, sl[1].stop - 1, sl[0].start, sl[0].stop - 1, int(wz[sl][lab[sl] == j].max())))
fl.sort(reverse=True)
oben = wz[0:4].max(axis=0)
d8 = (np.abs(np.diff(g, axis=0)) > 8)
d8a = (d8 & ~maske[None]).reshape(n - 1, -1).any(axis=1) if n > 1 else []
img = np.stack([med] * 3, axis=2).clip(0, 255).astype(np.uint8)
img[blitz.any(axis=0)] = [255, 0, 0]; img[unruhig & ~blitz.any(axis=0)] = [255, 255, 0]
Image.fromarray(img).resize((640, 480), Image.NEAREST).save(pre + "_karte.png")
aus = [e for e in ev if not e["fig"]]
with open(pre + ".txt", "w") as f:
    f.write("bilder=%d (%.1f s) ab=%.1fs | Blitz-Ereignisse ausserhalb Figur %d (an Figur %d) | unruhige Flecken ausserhalb Figur %d | "
            "Wechsel>8 ausserhalb Figur %d von %d | Rand oben y0..3: Spalten mit Wechselzahl>=6: %s | Kaesten %d\n" % (
        n, n / 30.0, ab, len(aus), len(ev) - len(aus), len(fl), int(np.sum(d8a)), n - 1,
        ",".join("%d(%d)" % (x, oben[x]) for x in np.nonzero(oben >= 6)[0][:20]) or "-", len(k)))
    for e in aus[:60]:
        f.write("  Blitz Bild %d..%d (%d) x%d..%d y%d..%d\n" % (e["t0"], e["t1"], e["t1"] - e["t0"] + 1, e["x0"], e["x1"], e["y0"], e["y1"]))
    for a, x0, x1, y0, y1, m in fl[:30]:
        f.write("  unruhig %d px x%d..%d y%d..%d Wechselzahl max %d\n" % (a, x0, x1, y0, y1, m))
np.save(pre + "_wz.npy", wz.astype(np.int16))
print(open(pre + ".txt").read().splitlines()[0])
