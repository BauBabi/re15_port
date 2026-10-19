import sys
G=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G39k'
import numpy as np
from PIL import Image, ImageDraw
n=sys.argv[1]; x0,y0,x1,y1=map(int,sys.argv[2:6]); Z=int(sys.argv[6]) if len(sys.argv)>6 else 5
S=np.array(Image.open(f'{G}/soll_{n}.png')).astype(int); L=np.array(Image.open(f'{G}/ist_{n}.png')).astype(int)
if len(sys.argv)>7: L=np.array(Image.open(sys.argv[7]).convert('RGB')).astype(int)
c=lambda a: a[y0:y1, x0:x1]
s=c(S); l=c(L)
d=(np.abs(s-l).sum(axis=2)>0)
ov=l.copy(); ov[d]=[255,0,255]
W=(x1-x0)*Z; H=(y1-y0)*Z
out=Image.new('RGB',(W*3+20, H+20),(255,255,255))
for i,a in enumerate((l,s,ov)):
    im=Image.fromarray(a.astype(np.uint8)).resize((W,H),Image.NEAREST)
    out.paste(im,(i*(W+10),20))
dr=ImageDraw.Draw(out)
dr.text((2,2),f'IST {n} x{x0}..{x1} y{y0}..{y1}',fill=(0,0,0)); dr.text((W+12,2),'SOLL',fill=(0,0,0)); dr.text((2*W+22,2),'DIFF',fill=(0,0,0))
# grid ticks every 10 px
for i in range(3):
    for gx in range(((x0+9)//10)*10, x1, 10):
        X=i*(W+10)+(gx-x0)*Z; dr.line([(X,20),(X,24)],fill=(255,0,0))
    for gy in range(((y0+9)//10)*10, y1, 10):
        Y=20+(gy-y0)*Z; dr.line([(i*(W+10),Y),(i*(W+10)+4,Y)],fill=(255,0,0))
out.save(f'{G}/vis_{n}_{x0}_{y0}.png')
print(f'{G}/vis_{n}_{x0}_{y0}.png', d.sum())
