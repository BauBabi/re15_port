import os,sys,json
import numpy as np
from scipy import ndimage
from PIL import Image,ImageDraw
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/tools'
sys.path.insert(0,T)
import form_buehne as B
A=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/shared_assets/RE2/'
emd=open(A+'BAHNSTEIG_FORM.EMD','rb').read(); tim=open(A+'BAHNSTEIG_FORM.TIM','rb').read()
raum,schnitt,lage=sys.argv[1],int(sys.argv[2]),tuple(int(v) for v in sys.argv[3].split(','))
R=B.Raum(raum); s=R.schnitt(schnitt); kam=B.Kamera.aus_rid(s['pos'],s['ziel'],s['fov']); licht=R.licht(schnitt)
faelle=[tuple(int(v) for v in f.split(':')) for f in sys.argv[4].split(',')]
hg=None
if len(sys.argv)>6 and sys.argv[6]!='-':
    hg=np.asarray(Image.open(sys.argv[6]).convert('RGB').resize((640,480)))
tafeln=[]
for c,b,g in faelle:
    r,m=B.buehne(emd,tim,c,b,lage,g,kam,licht,hg)
    h=ndimage.binary_fill_holes(m)&~m
    r=r.copy(); r2=r.copy(); r2[h]=(255,0,255)
    ys,xs=np.nonzero(m|h); y0,y1,x0,x1=max(ys.min()-10,0),min(ys.max()+10,480),max(xs.min()-10,0),min(xs.max()+10,640)
    tafeln.append(('Clip %d Bild %d Gier %d: Loch %d px'%(c,b,g,int(h.sum())),np.hstack([r[y0:y1,x0:x1],r2[y0:y1,x0:x1]])))
W=max(t[1].shape[1] for t in tafeln); H=sum(t[1].shape[0]+22 for t in tafeln)
out=Image.new('RGB',(W,H),(18,18,22)); d=ImageDraw.Draw(out); y=0
for t,a in tafeln:
    d.text((4,y+3),t,fill=(240,200,120)); out.paste(Image.fromarray(a),(0,y+20)); y+=a.shape[0]+22
out.save(sys.argv[5]); print(sys.argv[5],out.size)
