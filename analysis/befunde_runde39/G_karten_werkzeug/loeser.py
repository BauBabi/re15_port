"""Kachel-/Kanten-Aenderungen, die das Soll erzeugen (je Pixel minimale Aenderung der Lagen)."""
import sys, itertools; sys.path.insert(0,'.')
from modell import *
from marken_soll import verschiebe
from collections import Counter, defaultdict
def lagen(S, ops, X, Y):
    """Lagen an (X,Y) von oben nach unten: ('k', opidx, u, v, texel) bzw. ('f', opidx, farbe, abe)."""
    out=[]
    for idx,o in enumerate(ops):
        if not ist_karte(o): continue
        if not(o['x']<=X<o['x']+o['w'] and o['y']<=Y<o['y']+o['h']): continue
        if ist_kachel(o):
            u,v=(o['u']+X-o['x'])&255,(o['v']+Y-o['y'])&255
            t=int(S.tex[v,u])
            if t==0: out.append(('k',idx,u,v,0)); continue
            out.append(('k',idx,u,v,t))
        else:
            out.append(('f',idx,(o['r'],o['g'],o['b']),o['abe']))
    return out
def klasse(stack, clut_of):
    # von unten nach oben mischen
    cur='P'
    for l in reversed(stack):
        if l[0]=='k':
            t=l[4]
            if t==0: continue
            if t==1:
                if clut_of(l[1])==19: continue
                cur={'P':'B','W':'X'}.get(cur,'BB') if clut_of(l[1])!=18 else {'P':'C','W':'CW'}.get(cur,'CC')
            elif t==4: cur='W'
            else: cur='T'
        else:
            if l[3]==1 and l[2]==(32,104,0): cur={'P':'B','W':'X'}.get(cur,'BB')
            elif l[2]==(176,176,176): cur='W'
            elif l[2]==(224,168,40): cur='D'
            elif l[2]==(240,240,216): cur='S'
            else: cur='O'
    return cur
def loese(seite):
    S=Seite(seite); bas=S.basis(); ops=verschiebe(S.ops,seite)
    mod,_=S.render(ops,bas)
    d=vergleich(mod,S.soll)
    clut_of=lambda i: ops[i]['clut']
    aend=defaultdict(set)   # (opidx, art) -> set of (u,v) bzw. (X,Y)
    ungeloest=[]
    for Y,X in zip(*np.nonzero(d)):
        ziel=NAME[S.soll[Y,X]]
        if ziel=='O': continue
        st=lagen(S,ops,X,Y)
        best=None
        optionen=[]
        for l in st:
            if l[0]=='k': optionen.append([l[4]]+[t for t in (0,1,4) if t!=l[4]])
            else: optionen.append(['keep','weg'])
        for combo in itertools.product(*optionen):
            n=sum(1 for c,o in zip(combo,optionen) if c!=o[0])
            if best and n>=best[0]: continue
            st2=[]
            for c,l in zip(combo,st):
                if l[0]=='k': st2.append(l[:4]+(c,))
                elif c=='keep': st2.append(l)
            if klasse(st2,clut_of)==ziel: best=(n,combo)
        if not best: ungeloest.append((X,Y,ziel,st)); continue
        for c,o,l in zip(best[1],optionen,st):
            if c==o[0]: continue
            if l[0]=='k': aend[(l[1],'texel',c)].add((l[2],l[3],X,Y))
            else: aend[(l[1],'fill_weg',None)].add((X,Y))
    return S,ops,aend,ungeloest
if __name__=='__main__':
    for seite in map(int,sys.argv[1:]):
        S,ops,aend,ung=loese(seite)
        print('==',NAMEN[seite],'ungeloest',len(ung))
        for (i,art,wert),pk in sorted(aend.items()):
            o=ops[i]
            if art=='texel':
                us=[p[0] for p in pk]; vs=[p[1] for p in pk]; xs=[p[2] for p in pk]; ys=[p[3] for p in pk]
                print(f" op{i} Kachel ({o['x']},{o['y']},{o['w']},{o['h']}) uv({o['u']},{o['v']}) clut{o['clut']}: texel->{wert} n={len(pk)} u{min(us)}..{max(us)} v{min(vs)}..{max(vs)} | Schirm x{min(xs)}..{max(xs)} y{min(ys)}..{max(ys)}")
            else:
                xs=[p[0] for p in pk]; ys=[p[1] for p in pk]
                print(f" op{i} FILL ({o['x']},{o['y']},{o['w']},{o['h']}) rgb({o['r']},{o['g']},{o['b']}): weg n={len(pk)} x{min(xs)}..{max(xs)} y{min(ys)}..{max(ys)}")
        for u in ung[:10]: print('  UNGELOEST',u)
