# bogen.py <out.png> <cols> <scale> <label-prefix> file1 [file2 ...]  — Kontaktbogen aus PPM/PNG-Bildern (mit Beschriftung)
import sys
from PIL import Image, ImageDraw
out, cols, scale = sys.argv[1], int(sys.argv[2]), float(sys.argv[3])
pref = sys.argv[4]
files = sys.argv[5:]
ims = [Image.open(f).convert('RGB') for f in files]
w, h = ims[0].size
W, H = int(w*scale), int(h*scale)
rows = (len(ims)+cols-1)//cols
sheet = Image.new('RGB', (cols*W, rows*H), (0,0,0))
d = ImageDraw.Draw(sheet)
import os, re
for i,(im,f) in enumerate(zip(ims,files)):
    x, y = (i%cols)*W, (i//cols)*H
    sheet.paste(im.resize((W,H), Image.NEAREST), (x,y))
    m = re.search(r'(\d+)\.(ppm|png)$', f)
    lab = pref + (m.group(1).lstrip('0') or '0') if m else os.path.basename(f)
    d.rectangle([x, y, x+len(lab)*6+4, y+11], fill=(0,0,0))
    d.text((x+2,y), lab, fill=(255,255,0))
sheet.save(out)
print(out, sheet.size)
