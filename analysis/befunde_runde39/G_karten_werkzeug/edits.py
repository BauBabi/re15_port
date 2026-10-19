"""Kachel-Korrekturen (Runde 39 Spur G) im Modell anwenden + Zustands-Robustheit pruefen."""
import sys, copy; sys.path.insert(0,'.')
from modell import *
from marken_soll import verschiebe
# (seite, rect, c, r, w, h, neu)   neu: 0 durchsichtig, 1 Koerper, 4 Wand
EDITS=[
 (2,0, 7,76,1,10,1), (2,1,35,32,1,16,1), (2,2,16,49,1,8,1), (2,2,16,67,1,11,1), (2,3,51,1,1,34,1),
 (2,4,66,20,1,1,1), (2,7,0,1,1,15,1), (2,7,15,1,1,15,1), (2,8,0,1,1,15,1), (2,8,15,1,1,15,1), (2,9,9,1,1,8,1),
 (3,5,40,7,1,1,0),
 (4,4,0,1,1,38,0),(4,4,1,1,1,38,4),(4,4,8,40,1,38,0),(4,4,9,39,1,39,4),
 (1,3,0,2,1,5,1),(1,4,0,1,1,16,1),(1,5,0,1,1,16,1),
]
KANTEN_WEG={(2,207,89,15,33,0),(2,207,89,15,33,2),(2,207,123,15,16,2),(2,207,123,15,16,1),(2,207,139,15,15,2)}
def rect_liste(seite):
    from schwenk import rects
    return rects(seite)
def wende_an(S, ops, seite, edits=EDITS):
    """ersetzt Kachel-Ops durch Stuecke; Texel-Aenderung je Rechteck (nicht je uv!)."""
    R=rect_liste(seite)
    out=[]
    for o in ops:
        if ist_kachel(o):
            # welches Rechteck? (Geometrie-Ursprung passend zu uv)
            ri=None
            for i,(x,y,w,h,u,v) in enumerate(R):
                if o['u']-u==o['x']-x and o['v']-v==o['y']-y and x<=o['x']<x+w and y<=o['y']<y+h and u<=o['u']<u+w:
                    ri=i; break
            o2=dict(o); o2['rect']=ri; o2['edits']=[e for e in edits if e[0]==seite and e[1]==ri]
            o2['rx'],o2['ry']=(R[ri][0],R[ri][1]) if ri is not None else (o['x'],o['y'])
            out.append(o2)
        elif o['kind']==4 and o['r']==176 and o['g']==176 and o['b']==176 and o['abe']==0:
            # Schema-Kanten: als Seite einer Zelle erkennen
            weg=False
            for (sp,cx,cy,cw,ch,s2) in KANTEN_WEG:
                if sp!=seite: continue
                if s2==0 and o['y']==cy and o['h']==1 and cx<=o['x']<cx+cw: weg=True
                if s2==1 and o['y']==cy+ch-1 and o['h']==1 and cx<=o['x']<cx+cw: weg=True
                if s2==2 and o['x']==cx and o['w']==1 and cy<=o['y']<cy+ch: weg=True
                if s2==3 and o['x']==cx+cw-1 and o['w']==1 and cy<=o['y']<cy+ch: weg=True
            if not weg: out.append(o)
        else: out.append(o)
    return out
def render_e(S, ops, basis):
    """wie Seite.render, aber Kachel-Ops mit Texel-Korrekturen."""
    out=basis.copy(); deck=np.zeros(out.shape,bool)
    for o in reversed(ops):
        if not ist_karte(o): continue
        x,y,w,h=o['x'],o['y'],o['w'],o['h']
        if ist_kachel(o):
            for j in range(h):
                for i in range(w):
                    X,Y=x+i,y+j
                    if not(0<=X<320 and 0<=Y<240): continue
                    t=S.tex[(o['v']+j)&255,(o['u']+i)&255]
                    lc,lr=X-o.get('rx',x),Y-o.get('ry',y)
                    for (sp,ri,c,r,ew,eh,neu) in o.get('edits',[]):
                        if c<=lc<c+ew and r<=lr<r+eh: t=neu
                    if t==0: continue
                    unten=out[Y,X] if deck[Y,X] else KL['P']
                    if t==1:
                        if o['clut']==19: continue
                        if o['clut']==18: c2='C' if unten==KL['P'] else ('CW' if unten==KL['W'] else 'CC')
                        else: c2='B' if unten==KL['P'] else ('X' if unten==KL['W'] else 'BB')
                        out[Y,X]=KL[c2]
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
                    if o['abe']==1 and col in ((32,104,0),(104,8,8)):
                        unten=out[Y,X] if deck[Y,X] else KL['P']
                        if col==(104,8,8): out[Y,X]=KL['C' if unten==KL['P'] else ('CW' if unten==KL['W'] else 'CC')]
                        else: out[Y,X]=KL['B' if unten==KL['P'] else ('X' if unten==KL['W'] else 'BB')]
                    else: out[Y,X]=KL[c]
                    deck[Y,X]=True
    return out
if __name__=='__main__':
    for seite in (2,3,4,1):
        S=Seite(seite); bas=S.basis()
        ops=wende_an(S, verschiebe(S.ops,seite), seite)
        mod=render_e(S,ops,bas)
        d=vergleich(mod,S.soll); ys,xs=np.nonzero(d)
        from collections import Counter
        print(NAMEN[seite],'mit Korrekturen vs SOLL:',d.sum(),Counter((NAME[mod[y,x]],NAME[S.soll[y,x]]) for y,x in zip(ys,xs)))
