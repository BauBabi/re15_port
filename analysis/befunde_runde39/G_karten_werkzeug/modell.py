import numpy as np, copy, sys
from PIL import Image
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G39k')
from komp import lade_ops, lade_pix, farbe_klasse, KL, NAME, G
NAMEN={2:'map1F',3:'map2F',4:'map3F',1:'mapB2'}
def ist_kachel(o): return o['kind']==0 and o['page']==5 and o['clut'] in (17,18,19)
def ist_karte(o): return ist_kachel(o) or o['kind'] in (4,6)
class Seite:
    def __init__(s, seite, opsdatei=None):
        s.seite=seite
        s.ops=lade_ops(opsdatei or f'{G}/ops_{seite}.txt')
        s.tex=lade_pix(seite).astype(np.int16)
        s.ist=farbe_klasse(np.array(Image.open(f'{G}/ist_{NAMEN[seite]}.png'))) if seite in NAMEN else None
        s.soll=farbe_klasse(np.array(Image.open(f'{G}/soll_{NAMEN[seite]}.png'))) if seite in NAMEN else None
        if s.ist is not None: s._tilgen()
    def _tilgen(s):
        # oberste Op je Pixel
        top=s.oberste(s.ops)
        for (Y,X),(idx,u,v) in top.items():
            if s.tex[v,u] not in (0,1) and s.ist[Y,X]==KL["B"]: s.tex[v,u]=1
    def oberste(s, ops):
        top={}
        for idx,o in enumerate(ops):
            if not ist_karte(o): continue
            for j in range(o['h']):
                for i in range(o['w']):
                    X,Y=o['x']+i,o['y']+j
                    if not(0<=X<320 and 0<=Y<240) or (Y,X) in top: continue
                    if ist_kachel(o):
                        u,v=(o['u']+i)&255,(o['v']+j)&255
                        if s.tex[v,u]==0: continue
                        top[(Y,X)]=(idx,u,v)
                    else:
                        top[(Y,X)]=(idx,-1,-1)
        return top
    def render(s, ops, basis):
        out=basis.copy(); deck=np.zeros(out.shape,bool)
        for o in reversed(ops):
            if not ist_karte(o): continue
            x,y,w,h=o['x'],o['y'],o['w'],o['h']
            if ist_kachel(o):
                for j in range(h):
                    for i in range(w):
                        X,Y=x+i,y+j
                        if not(0<=X<320 and 0<=Y<240): continue
                        t=s.tex[(o['v']+j)&255,(o['u']+i)&255]
                        if t==0: continue
                        unten=out[Y,X] if deck[Y,X] else KL['P']
                        if t==1:
                            if o['clut']==19: continue
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
                        if not(0<=X<320 and 0<=Y<240): continue
                        if o['abe']==1 and col==(32,104,0):
                            unten=out[Y,X] if deck[Y,X] else KL['P']
                            out[Y,X]=KL['B' if unten==KL['P'] else ('X' if unten==KL['W'] else 'BB')]
                        else: out[Y,X]=KL[c]
                        deck[Y,X]=True
        return out, deck
    def basis(s):
        """Hintergrund: IST-Klassen ausserhalb gedeckter Kartenpixel, P darunter."""
        b=s.ist.copy()
        top=s.oberste(s.ops)
        for (Y,X) in top: b[Y,X]=KL['P']
        return b
BOX=(slice(50,200),slice(90,240))
def vergleich(a,b):
    d=(a!=b); m=np.zeros(a.shape,bool); m[BOX]=True
    return (d&m)
if __name__=='__main__':
    for seite in (2,3,4,1):
        S=Seite(seite)
        bas=S.basis()
        mod,_=S.render(S.ops,bas)
        d=vergleich(mod,S.ist)
        d2=vergleich(mod,S.soll)
        # C-Klasse im IST (aktueller Raum) gleich behandeln
        print(NAMEN[seite],'modell vs IST:',d.sum(),' modell vs SOLL:',d2.sum())
