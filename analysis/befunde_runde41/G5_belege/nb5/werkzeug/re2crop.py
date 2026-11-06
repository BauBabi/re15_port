import subprocess, numpy as np, sys
from PIL import Image, ImageDraw
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
def re2_t(n): return 63.47 + n / 29.9
ns = [int(a) for a in sys.argv[2].split(",")]; x0, y0, x1, y1 = [int(a) for a in sys.argv[3].split(",")]; z = int(sys.argv[4]); cols = int(sys.argv[5])
W, H = (x1-x0)*z, (y1-y0)*z
out = Image.new("RGB", (cols*(W+4), ((len(ns)+cols-1)//cols)*(H+14))); dr = ImageDraw.Draw(out)
for i, n in enumerate(ns):
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % re2_t(n - 2.0), "-i", SRC, "-frames:v", "1",
                          "-vf", "scale=320:240", "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
    a = Image.fromarray(np.frombuffer(raw[:320*240*3], np.uint8).reshape(240, 320, 3))
    x = (i % cols)*(W+4); y = (i//cols)*(H+14)
    dr.text((x+2, y), "RE2 %d" % n, fill=(255,255,0)); out.paste(a.crop((x0,y0,x1,y1)).resize((W,H), Image.NEAREST), (x, y+14))
out.save(sys.argv[1]); print(out.size)
