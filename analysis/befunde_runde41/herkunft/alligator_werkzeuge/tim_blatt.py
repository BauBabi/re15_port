# Alle eindeutigen TIMs der Disc-RDTs (+ DO2, EMS) als Kontaktbogen, jede Stage eigener Bogen.
import json,struct,os,hashlib,collections,sys
import numpy as np
from PIL import Image,ImageDraw
R=r'C:/workspace/git/reAi_v2'
T=json.load(open('tim_liste.json'))
def dec(d,o,fl,cl,im):
    p=o+8
    pal=None
    if fl&8:
        bl=struct.unpack_from('<I',d,p)[0]
        cx,cy,cw,ch=cl
        c=np.frombuffer(d,'<u2',cw*ch,p+12).reshape(ch,cw)
        pal=c
        p+=bl
    bl,ix,iy,iw,ih=struct.unpack_from('<I4H',d,p)
    px=d[p+12:p+bl]
    b=fl&7
    if b==1:
        a=np.frombuffer(px,np.uint8).reshape(ih,iw*2)
        c=pal[0][a]
    elif b==0:
        a=np.frombuffer(px,np.uint8).reshape(ih,iw*2)
        x=np.zeros((ih,iw*4),np.uint8); x[:,0::2]=a&15; x[:,1::2]=a>>4
        c=pal[0][x]
    elif b==2:
        c=np.frombuffer(px,'<u2').reshape(ih,iw)
    else:
        return None,0
    rgb=np.stack([(c&31)*8,((c>>5)&31)*8,((c>>10)&31)*8],-1).astype(np.uint8)
    return rgb,len(px)
seen={}
by=collections.defaultdict(list)
for p,o,fl,cl,im in T:
    if not p.endswith(('.RDT','.DO2')): continue
    lp=R+'/Version_40%'+p
    d=open(lp,'rb').read()
    try:
        rgb,n=dec(d,o,fl,cl,im)
    except Exception as ex:
        print('fehler',p,o,ex); continue
    if rgb is None: continue
    h=hashlib.md5(rgb.tobytes()).hexdigest()
    if h in seen:
        seen[h].append(p); continue
    seen[h]=[p]
    st=os.path.basename(os.path.dirname(p))
    by[st].append((os.path.basename(p)[4:8],o,rgb,h))
print({k:len(v) for k,v in by.items()},'eindeutig',len(seen))
for st,L in sorted(by.items()):
    W=120;H=150;cols=10
    rows=(len(L)+cols-1)//cols
    S=Image.new('RGB',(cols*W,rows*H),(20,20,24)); dr=ImageDraw.Draw(S)
    for i,(n,o,rgb,h) in enumerate(L):
        im=Image.fromarray(rgb); im.thumbnail((W-4,H-14))
        S.paste(im,((i%cols)*W+2,(i//cols)*H+12))
        dr.text(((i%cols)*W+2,(i//cols)*H+1),f'{n}@{o:x}',fill=(255,255,0))
    S.save(f'../alligator_belege/tims_{st}.jpg',quality=85)
    print(st,S.size)
