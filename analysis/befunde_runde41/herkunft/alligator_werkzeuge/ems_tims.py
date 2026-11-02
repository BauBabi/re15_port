# Kontaktbogen aller 25 Disc-EMS-TIMs (CDEMD0), je CLUT-Zeile 0.
import sys,struct
sys.path.insert(0,'.')
from emd_tab import *
import numpy as np
from PIL import Image,ImageDraw
ORD=[0x10,0x11,0x12,0x13,0x16,0x18,0x1A,0x20,0x21,0x24,0x25,0x26,0x27,0x29,0x2B,0x2D,0x30,0x36,0x40,0x42,0x45,0x47,0x49,0x4B,0x4D]
B=blocks(R+'/Version_40%/PSX/EMD/CDEMD0.EMS')
W=200;H=210;cols=7
S=Image.new('RGB',(cols*W,5*H),(20,20,24)); dr=ImageDraw.Draw(S)
for i,(c,b) in enumerate(B):
    D=E.emd_dir(b); t=b[D[8]:]
    px,pal,cy=E.tim_decode(t)
    rgb=pal[0][px]
    im=Image.fromarray(rgb.astype('uint8')); im.thumbnail((W-4,H-14))
    S.paste(im,((i%cols)*W+2,(i//cols)*H+12))
    dr.text(((i%cols)*W+2,(i//cols)*H+1),f'Typ {ORD[i]:02X} {px.shape[1]}x{px.shape[0]}',fill=(255,255,0))
S.save('../alligator_belege/disc_ems_tims.jpg',quality=88)
print(S.size)
