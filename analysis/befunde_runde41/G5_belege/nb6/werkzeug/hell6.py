# NB6: integrierte Lampenhelligkeit (Summe Grauwert ueber Median, Fenster x 70..100 / y 115..127 in 320x240) fuer die Lampe bei x -6000:
# RE2-VRAM s04 (echte PSX-Ausgabe) | RE2-Aufnahme Bild -42 (Achse -2,0) | Port nativ (RE15_WINDOW_SCALE=1) | Port Fenster 3x (F1581) | NB5-Stand.
import subprocess, numpy as np, sys
from PIL import Image
sys.path.insert(0, ".")
from vram5 import vram, rgb
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb5/lauf"
def summe(img, s):
    g = img.astype(float).mean(axis=2)
    w = g[int(115 * s):int(127 * s), int(70 * s):int(100 * s)]
    return float(np.clip(w - np.median(w), 0, None).sum()) / (s * s), float(w.max())
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % (63.47 + (-42 - 2.0) / 29.9), "-i", SRC, "-frames:v", "1",
                      "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
quellen = [("RE2-VRAM s04 (PSX-Ausgabe, 320x240)", rgb(vram(4)[0:240, 0:320]), 1),
           ("RE2-Aufnahme Bild -42 (640x480)", np.frombuffer(raw[:640*480*3], np.uint8).reshape(480, 640, 3), 2)]
for name, lauf, f, s in (("Port NB6 nativ F1581", "nativ3", 1581, 1), ("Port NB5 nativ F1581", "nativ2", 1581, 1),
                         ("Port NB6 Fenster 3x F1581 (Leon)", "fd6_leon", 1581, 3), ("Port NB6 Fenster 3x F1581 (Elza)", "fd6_elza", 1581, 3),
                         ("Port NB5 Fenster 3x F1581", "fd_leon", 1581, 3)):
    quellen.append((name, np.asarray(Image.open("%s/%s/fd/f%06d.ppm" % (L, lauf, f)).convert("RGB")), s))
ref = summe(quellen[0][1], 1)[0]
print("Quelle | Summe ueber Median | Maximum | / RE2-VRAM")
for name, img, s in quellen:
    sm, mx = summe(img, s)
    print("%-38s %8.0f  %5.0f  %.2f" % (name, sm, mx, sm / ref))

print()
print("Pixelvergleich gegen RE2-VRAM s04 im Fenster x 70..105 / y 112..128 (Grauwert, 612 Pixel):")
a = rgb(vram(4)[0:240, 0:320]).astype(int).mean(axis=2)[112:129, 70:106]
for name, lauf in (("Port NB6 nativ", "nativ3"), ("Port NB5 nativ", "nativ2")):
    p = np.asarray(Image.open("%s/%s/fd/f001581.ppm" % (L, lauf)).convert("RGB")).astype(int).mean(axis=2)[112:129, 70:106]
    d = np.abs(a - p)
    print("  %-16s mittlere |Abw.| %.2f, |Abw.| > 16: %d Pixel, max %d" % (name, d.mean(), int((d > 16).sum()), int(d.max())))
