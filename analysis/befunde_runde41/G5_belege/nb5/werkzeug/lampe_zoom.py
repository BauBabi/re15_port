import subprocess, numpy as np, sys
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb5/lauf"
def re2_t(n): return 63.47 + n / 29.9
ns = [-44, -43, -42, -41]
out = Image.new("RGB", (4 * 2 * 240 + 30, 2 * 120 + 10))
for i, n in enumerate(ns):
    t = re2_t(n - 2.0)
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t, "-i", SRC, "-frames:v", "1",
                          "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
    a = np.frombuffer(raw[:640*480*3], np.uint8).reshape(480, 640, 3)
    p = np.asarray(Image.open("%s/fd_leon/fd/f%06d.ppm" % (L, 1613 + n)).convert("RGB"))   # 960x720
    # Ausschnitt um x 70..130, y 100..140 (320er) -> RE2 x2, Port x3
    ra = a[200:280, 140:260]; pa = p[300:420, 210:390]
    print("Bild %d: RE2 max %s Mittel>100 %d | Port max %s Mittel>100 %d" % (n, ra.reshape(-1,3).max(0), (ra.mean(2) > 100).sum() // 4,
          pa.reshape(-1,3).max(0), (pa.mean(2) > 100).sum() // 9))
    out.paste(Image.fromarray(ra).resize((240, 160), Image.NEAREST).crop((0,0,240,120)) if False else Image.fromarray(ra).resize((240, 160), Image.NEAREST), (i * 490, 0))
    out.paste(Image.fromarray(pa).resize((240, 160), Image.NEAREST), (i * 490 + 245, 0))
out.save("lampe_zoom.png")
