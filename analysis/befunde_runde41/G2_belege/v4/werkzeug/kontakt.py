import sys, subprocess, os, tempfile
from PIL import Image, ImageDraw
vid, aus, t0, t1, dt = sys.argv[1], sys.argv[2], float(sys.argv[3]), float(sys.argv[4]), float(sys.argv[5])
FF = "C:/ProgramData/chocolatey/bin/ffmpeg"; td = tempfile.mkdtemp(); ims = []
t = t0
while t <= t1:
    p = os.path.join(td, "%.1f.png" % t)
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.2f" % t, "-i", vid, "-frames:v", "1", "-vf", "scale=240:180", "-y", p])
    im = Image.open(p).convert("RGB") if os.path.exists(p) else Image.new("RGB", (240, 180))
    ImageDraw.Draw(im).text((3, 3), "%.0f" % t, fill=(255, 255, 0)); ims.append(im); t += dt
c = 6; r = (len(ims) + c - 1) // c
S_ = Image.new("RGB", (c * 242, r * 182))
for i, im in enumerate(ims): S_.paste(im, ((i % c) * 242, (i // c) * 182))
S_.save(aus, quality=80)
