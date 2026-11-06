"""mixleiste.py <aus.jpg> <titel> <spalte>... -- F1 F2 ...   Spalte = "<kopf>=fd:<laufordner>" (RE15_FRAMEDUMP-Serie fd_NNNNNN.ppm, Bild F exakt bzw. naechstes >= F)
oder "<kopf>=vid:<video.mp4>:<versatz_s>:<fps>" (gdigrab-Video: t = versatz + F/fps, Naeherung +-1 s). Zeilen = Spielbild F. 320 x 240 je Bild."""
import sys, os, glob, re, subprocess, tempfile
from PIL import Image, ImageDraw
aus, titel = sys.argv[1], sys.argv[2]
i = sys.argv.index("--"); spalten = [a.split("=", 1) for a in sys.argv[3:i]]; Fs = [int(f) for f in sys.argv[i + 1:]]
FF = "C:/ProgramData/chocolatey/bin/ffmpeg"; td = tempfile.mkdtemp()
W, H, KO, RA = 320, 240, 22, 66; N = len(spalten)
img = Image.new("RGB", (RA + N * W + 6 * (N - 1) + 6, KO + len(Fs) * (H + 4)), (20, 20, 20)); d = ImageDraw.Draw(img)
farbe = [(255, 255, 0), (0, 200, 255), (0, 255, 0), (255, 140, 0)]
for k, (kopf, _) in enumerate(spalten): d.text((RA + k * (W + 6), 4), kopf, fill=farbe[k % 4])
d.text((2, 4), titel[:10], fill=(200, 200, 200))
vorrat = {}
for k, (_, q) in enumerate(spalten):
    if q.startswith("fd:"):
        vorrat[k] = {int(re.search(r"fd_(\d+)\.ppm$", p).group(1)): p for p in glob.glob(os.path.join(q[3:], "fd_*.ppm"))}
for r, F in enumerate(Fs):
    y = KO + r * (H + 4)
    for k, (_, q) in enumerate(spalten):
        im = None; marke = ""
        if q.startswith("fd:"):
            fs = vorrat[k]; p = fs.get(F); fx = F
            if p is None and fs:
                kand = [f for f in fs if f >= F]
                if kand: fx = min(kand); p = fs[fx]; marke = "F%d" % fx
            if p: im = Image.open(p).convert("RGB").resize((W, H), Image.BILINEAR)
        else:
            vid, off, fps = q.split(":", 1)[1].rsplit(":", 2); t = float(off) + F / float(fps)
            p = os.path.join(td, "v%d_%d.png" % (k, F))
            subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.2f" % t, "-i", vid, "-frames:v", "1", "-vf", "scale=%d:%d" % (W, H), "-y", p], check=False)
            if os.path.exists(p): im = Image.open(p).convert("RGB"); marke = "t %.1f s" % t
        if im is None: im = Image.new("RGB", (W, H), (40, 0, 0))
        img.paste(im, (RA + k * (W + 6), y))
        if marke: d.text((RA + k * (W + 6) + 4, y + 4), marke, fill=(255, 160, 120))
    d.text((4, y + H // 2), "F%d" % F, fill=(255, 255, 255))
img.save(aus, quality=88); print("geschrieben", aus, img.size)
