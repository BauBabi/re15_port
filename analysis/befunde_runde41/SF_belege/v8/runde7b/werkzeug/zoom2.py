import sys, os
from PIL import Image, ImageDraw
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
VID = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/roh/alle'
QU = []
for a in sys.argv[3:]:
    k, p = a.split('=', 1); QU.append((k, p))
Fs = [int(x) for x in sys.argv[1].split(',')]
x0, y0, x1, y1 = [int(v) for v in sys.argv[2].split(',')]   # Framedump-Koordinaten 960x720
S = float(os.environ.get('S', 2))
W, H = int((x1 - x0) * S), int((y1 - y0) * S)
rows = []
for F in Fs:
    tiles = []
    pv = os.path.join(VID, 'f%04d.png' % (F + 1108))
    if os.path.exists(pv):
        im = Image.open(pv).convert('RGB').resize((960, 720), Image.BILINEAR)
        tiles.append(('Video f%d' % (F + 1108), im.crop((x0, y0, x1, y1)).resize((W, H), Image.NEAREST)))
    for k, d in QU:
        p = os.path.join(d, 'f%06d.ppm' % F)
        im = Image.open(p).convert('RGB')
        tiles.append(('%s F%d' % (k, F), im.crop((x0, y0, x1, y1)).resize((W, H), Image.NEAREST)))
    row = Image.new('RGB', (W * len(tiles), H))
    for i, (t, im) in enumerate(tiles):
        row.paste(im, (W * i, 0)); ImageDraw.Draw(row).text((W * i + 4, 4), t, fill=(255, 255, 0))
    rows.append(row)
out = Image.new('RGB', (max(r.width for r in rows), H * len(rows)))
for i, r in enumerate(rows): out.paste(r, (0, H * i))
out.save(os.environ.get('OUT', 'zoom2.png'))
