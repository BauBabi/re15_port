# crop.py <dir> <out.png> <frames> <x0,y0,x1,y1> [zoom] [cols]
import sys, os
from PIL import Image, ImageDraw
d, out, frames, box = sys.argv[1], sys.argv[2], [int(x) for x in sys.argv[3].split(',')], [int(v) for v in sys.argv[4].split(',')]
z = float(sys.argv[5]) if len(sys.argv) > 5 else 1.0
cols = int(sys.argv[6]) if len(sys.argv) > 6 else 6
ims=[]
for f in frames:
    p=os.path.join(d,'f_%06d.ppm'%f)
    if not os.path.exists(p): continue
    im=Image.open(p).convert('RGB').crop(box)
    im=im.resize((int(im.width*z),int(im.height*z)), Image.NEAREST)
    ImageDraw.Draw(im).text((2,2),'F%d'%f,fill=(255,255,0))
    ims.append(im)
cols=min(cols,len(ims)); rows=(len(ims)+cols-1)//cols
W,H=ims[0].size
s=Image.new('RGB',(cols*W,rows*H))
for i,im in enumerate(ims): s.paste(im,((i%cols)*W,(i//cols)*H))
s.save(out); print(out,s.size)
