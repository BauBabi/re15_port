import os,sys
import numpy as np
from scipy import ndimage
from PIL import Image,ImageDraw
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/tools'
sys.path.insert(0,T)
import form_buehne as B
A=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/shared_assets/RE2/'
emd=open(A+'BAHNSTEIG_FORM.EMD','rb').read(); tim=open(A+'BAHNSTEIG_FORM.TIM','rb').read()
BAS=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/build/abn1_spiel/'
aus=sys.argv[1]; faelle=sys.argv[2:]
R=B.Raum('5080'); 
tafeln=[]
for f in faelle:
    name,c,b,g,sch=f.split(':'); c,b,g,sch=int(c),int(b),int(g),int(sch)
    s=R.schnitt(sch); kam=B.Kamera.aus_rid(s['pos'],s['ziel'],s['fov']); licht=R.licht(sch)
    r,m=B.buehne(emd,tim,c,b,(-23500,0,-18200),g,kam,licht,None)
    h=ndimage.binary_fill_holes(m)&~m
    lab,k=ndimage.label(h); gr=ndimage.sum(h,lab,range(1,k+1)) if k else []
    echt=np.asarray(Image.open(BAS+name+'/bild/b_0020.png').convert('RGB'))
    # Fenster 640x480 (Skala 2 -> 640x480 ist das Port-Bild 320x240 x2)
    mark=echt.copy()
    rand=h & ~ndimage.binary_erosion(h,iterations=2)
    mark[rand]=(255,0,255)
    ys,xs=np.nonzero(m|h); y0,y1,x0,x1=max(ys.min()-10,0),min(ys.max()+10,480),max(xs.min()-10,0),min(xs.max()+10,640)
    txt='%s: Clip %d Bild %d Gier %d, ROOM5080 Schnitt %d. Loecher (Buehne, gleiche Kamera): %s px (640x480)'%(name,c,b,g,sch,', '.join(str(int(v)) for v in sorted(gr,reverse=True) if v>=12))
    tafeln.append((txt,np.hstack([echt[y0:y1,x0:x1],mark[y0:y1,x0:x1]])))
Wd=max(t[1].shape[1] for t in tafeln); H=sum(t[1].shape[0]+22 for t in tafeln)
out=Image.new('RGB',(Wd,H),(18,18,22)); d=ImageDraw.Draw(out); y=0
for t,a in tafeln:
    d.text((4,y+3),t,fill=(240,200,120)); out.paste(Image.fromarray(a),(0,y+20)); y+=a.shape[0]+22
out.save(aus); print(aus,out.size)
