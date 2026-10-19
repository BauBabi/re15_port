import sys; sys.path.insert(0,'.')
from loeser import loese
from modell import NAMEN
from collections import defaultdict
def zu_rechtecken(punkte):
    """Menge (c,r) -> Liste maximaler senkrechter/waagerechter Laeufe als Rechtecke (c0,r0,w,h)."""
    p=set(punkte); out=[]
    # senkrechte Laeufe je Spalte
    cols=defaultdict(list)
    for c,r in p: cols[c].append(r)
    for c in sorted(cols):
        rs=sorted(cols[c]); s=rs[0]; prev=s
        for r in rs[1:]+[None]:
            if r is not None and r==prev+1: prev=r; continue
            out.append((c,s,1,prev-s+1))
            if r is not None: s=prev=r
    # waagerecht zusammenfassen, wenn mehrere Spalten gleiche Laeufe haben
    merged=[]
    for c,s,w,h in sorted(out, key=lambda t:(t[1],t[3],t[0])):
        if merged and merged[-1][1]==s and merged[-1][3]==h and merged[-1][0]+merged[-1][2]==c:
            m=merged[-1]; merged[-1]=(m[0],m[1],m[2]+1,m[3])
        else: merged.append((c,s,w,h))
    return merged
if __name__=='__main__':
    for seite in map(int,sys.argv[1:]):
        S,ops,aend,ung=loese(seite)
        print('==',NAMEN[seite], 'Seite',seite)
        for (i,art,wert),pk in sorted(aend.items()):
            o=ops[i]
            if art=='texel':
                lok=[(p[0]-o['u'] if p[0]>=o['u'] else p[0]+256-o['u'], p[1]-o['v']) for p in pk]
                print(f" Kachel op{i} ({o['x']},{o['y']},{o['w']},{o['h']}) uv({o['u']},{o['v']}) -> texel {wert}: {zu_rechtecken(lok)}  (Spalte,Zeile,w,h lokal)")
            else:
                print(f" FILL op{i} ({o['x']},{o['y']},{o['w']},{o['h']}) weg: {sorted(pk)[:3]}.. n={len(pk)}")
