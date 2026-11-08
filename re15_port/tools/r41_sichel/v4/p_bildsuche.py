"""Pruefer V4: Bildsuche der Vorlagebilder im Video 4. Dekodiert Bilder 1300..1549 (0-basierter
ffmpeg-Index i) als RGB, vergleicht mit Beschreibung{1,2}_Vorlage.png (mittlere abs. Abweichung).
Speichert die Bilder i-2..i+2 als PNG (Name = 1-basiertes n = i+1) und frames.npy.
Aufruf: python p_bildsuche.py OUTDIR"""
import sys, subprocess, json, numpy as np
from PIL import Image
OUT = sys.argv[1]
VID = r"C:/workspace/git/reAi_v2/Version_80%/Resident Evil 1.5 Complete Disc Video 4 (480p Upscale) (480p).mp4"
I0, N = 1300, 250
cmd = ["ffmpeg", "-v", "error", "-i", VID, "-vf", f"select=between(n\,{I0}\,{I0+N-1})", "-vsync", "0",
       "-f", "rawvideo", "-pix_fmt", "rgb24", "-"]
raw = subprocess.run(cmd, capture_output=True, check=True).stdout
fr = np.frombuffer(raw, np.uint8).reshape(-1, 480, 640, 3)
np.save(OUT + "/frames1300.npy", fr)
res = {}
for k in (1, 2):
    t = np.asarray(Image.open(rf"C:/workspace/git/reAi_v2/Version_80%/Beschreibung{k}_Vorlage.png").convert("RGB")).astype(np.int16)
    d = np.abs(fr.astype(np.int16) - t).mean(axis=(1, 2, 3))
    o = np.argsort(d)[:4]
    i = int(o[0]) + I0
    res[k] = {"index0": i, "n1": i + 1, "h": i + 1 - 1350, "abw_best": round(float(d[o[0]]), 3),
              "naechste": [(int(j) + I0, round(float(d[j]), 3)) for j in o[1:]]}
    for j in range(i - 2, i + 3):
        Image.fromarray(fr[j - I0]).save(f"{OUT}/v{k}_n{j+1}.png")
    print(k, res[k])
json.dump(res, open(OUT + "/P_bildsuche.json", "w"), indent=1)
