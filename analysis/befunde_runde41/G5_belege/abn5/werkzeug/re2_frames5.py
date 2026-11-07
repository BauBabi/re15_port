# Abnahme 5: RE2-Aufnahme 61,30..67,20 s als RGB 320x240 (alle Videobilder), Index 0 = erstes Bild ab 61,30 s
import subprocess, numpy as np
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
B = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5"
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "61.300", "-i", B + "/re2/re2_lauf1.mp4", "-t", "5.900",
                      "-vf", "scale=320:240", "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
n = len(raw) // (320 * 240 * 3)
v = np.frombuffer(raw[:n * 320 * 240 * 3], np.uint8).reshape(n, 240, 320, 3)
np.save(B + "/re2/re2_6130_rgb.npy", v)
g = v.astype(np.int16).mean(axis=3)
# Schnittkante = groesste Bilddifferenz in den ersten 30 Bildern
d = [np.abs(g[i] - g[i - 1]).mean() for i in range(1, 30)]
k = int(np.argmax(d)) + 1
print('Bilder', n, 'Schnittbild Index', k, 'Diff %.2f' % d[k - 1])
open(B + "/re2/re2_schnitt_index.txt", "w").write(str(k))
