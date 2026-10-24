# vgl_video.py <out.png> <scale> <crop x0,y0,x1,y1 in 320x240-Koordinaten|-> <port_dir> ticks...
# Original: orig4/vf/vNNNN.png (Videobild k = 30 + 2*(Takt-62)), Port: fd_<Takt+93>.ppm
import sys
from PIL import Image, ImageDraw
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/L'
out, sc, crop, pdir = sys.argv[1], float(sys.argv[2]), sys.argv[3], sys.argv[4]
ticks = [int(t) for t in sys.argv[5:]]
box = None if crop == '-' else tuple(int(v) for v in crop.split(','))
w0, h0 = (320, 240) if box is None else (box[2]-box[0], box[3]-box[1])
W, H = int(w0*sc), int(h0*sc)
cols = 2
sheet = Image.new('RGB', (cols*W*2 + 8, H*((len(ticks)+cols-1)//cols)))
d = ImageDraw.Draw(sheet)
for i, t in enumerate(ticks):
    k = 30 + 2*(t-62)
    o = Image.open(S + '/orig4/vf/v%04d.png' % k).convert('RGB').resize((320, 240), Image.BILINEAR)
    p = Image.open(pdir + '/fd_%06d.ppm' % (t+93)).convert('RGB')
    if box: o = o.crop(box); p = p.crop(box)
    o = o.resize((W, H), Image.BILINEAR); p = p.resize((W, H), Image.NEAREST)
    x = (i % cols) * (2*W + 8); y = (i // cols) * H
    sheet.paste(o, (x, y)); sheet.paste(p, (x+W, y))
    d.text((x+2, y+1), 'ORIG Takt %d' % t, fill=(255,255,0)); d.text((x+W+2, y+1), 'PORT F%d' % (t+93), fill=(0,255,255))
sheet.save(out); print(out, sheet.size)
