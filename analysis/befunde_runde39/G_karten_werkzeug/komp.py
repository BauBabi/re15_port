"""Klassen-Kompositor der Kartenseite aus der Op-Liste (probe_r39_karten ops)."""
import sys, numpy as np
from PIL import Image
G=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G39k'
PSX=r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten/re15_port/shared_assets/PSX/DATA'
KL={'P':0,'B':1,'W':2,'X':3,'D':4,'S':5,'C':6,'O':7,'BB':8,'T':9,'CW':10,'CC':11}
NAME={v:k for k,v in KL.items()}
def farbe_klasse(img):
    a=img.astype(int); h,w,_=a.shape
    k=np.full((h,w),KL['O'],np.int16)
    def m(rgb): return (a[:,:,0]==rgb[0])&(a[:,:,1]==rgb[1])&(a[:,:,2]==rgb[2])
    for rgb,c in (((0,16,88),'P'),((0,16,120),'P'),((16,56,40),'B'),((16,56,56),'B'),((176,176,176),'W'),
                  ((104,136,88),'X'),((224,168,40),'D'),((240,240,216),'S'),((48,8,48),'C'),((48,8,64),'C')):
        k[m(rgb)]=KL[c]
    return k
def lade_ops(pfad):
    ops=[]
    for ln in open(pfad):
        if ln.startswith('op '):
            f=list(map(int,ln.split()[1:]))
            ops.append(dict(i=f[0],kind=f[1],page=f[2],clut=f[3],abe=f[4],x=f[5],y=f[6],w=f[7],h=f[8],u=f[9],v=f[10],r=f[11],g=f[12],b=f[13]))
    return ops
def lade_pix(seite):
    d=open(f'{PSX}/MAP0{seite+1:X}.PIX','rb').read()
    t=np.zeros((256,256),np.uint8)
    b=np.frombuffer(d[:128*256],np.uint8).reshape(256,128)
    t[:,0::2]=b&15; t[:,1::2]=b>>4
    return t
def ist_karte(o): return (o['kind']==0 and o['page']==5) or o['kind'] in (4,6)
def komp(ops, seite, basis):
    """basis = Klassenbild (240x320) fuer alles, was keine Karten-Op deckt."""
    tex=lade_pix(seite)
    out=basis.copy()
    deck=np.zeros(out.shape,bool)
    for o in reversed(ops):      # von HINTEN: spaeter eingereiht = unten
        if not ist_karte(o): continue
        x,y,w,h=o['x'],o['y'],o['w'],o['h']
        if o['kind']==0:
            for j in range(h):
                for i in range(w):
                    X,Y=x+i,y+j
                    if not(0<=X<320 and 0<=Y<240): continue
                    t=tex[(o['v']+j)&255,(o['u']+i)&255]
                    if t==0: continue
                    unten=out[Y,X] if deck[Y,X] else KL['P']
                    if t==1:
                        if o['clut']==19: continue   # 0x0000 durchsichtig
                        if o['clut']==18: c='C' if unten==KL['P'] else ('CW' if unten==KL['W'] else 'CC')
                        else: c='B' if unten==KL['P'] else ('X' if unten==KL['W'] else 'BB')
                        out[Y,X]=KL[c]
                    elif t==4: out[Y,X]=KL['W']
                    else: out[Y,X]=KL['T']
                    deck[Y,X]=True
        else:
            col=(o['r'],o['g'],o['b'])
            c={(224,168,40):'D',(240,240,216):'S',(176,176,176):'W'}.get(col,'O')
            for j in range(h):
                for i in range(w):
                    X,Y=x+i,y+j
                    if 0<=X<320 and 0<=Y<240:
                        if o['kind']==6:
                            pass
                        if o['abe']==1 and col==(32,104,0):
                            unten=out[Y,X] if deck[Y,X] else KL['P']
                            cc='B' if unten==KL['P'] else ('X' if unten==KL['W'] else 'BB')
                            out[Y,X]=KL[cc]; deck[Y,X]=True; continue
                        out[Y,X]=KL[c]; deck[Y,X]=True
    return out, deck
if __name__=='__main__':
    seite=int(sys.argv[1]); name=sys.argv[2]
    ops=lade_ops(f'{G}/ops_{seite}.txt')
    ist=farbe_klasse(np.array(Image.open(f'{G}/ist_{name}.png')))
    basis=ist.copy()
    mod,deck=komp(ops,seite,basis)
    # Karte-Bereich: deck; Vergleich nur wo gedeckt oder ist != P in Kartenbox
    box=np.zeros(ist.shape,bool); box[50:200,90:240]=True
    diff=(mod!=ist)&box
    ys,xs=np.nonzero(diff)
    print('Abweichungen', diff.sum())
    from collections import Counter
    print(Counter((NAME[ist[y,x]],NAME[mod[y,x]]) for y,x in zip(ys,xs)).most_common(10))
    print(list(zip(xs,ys))[:30])
