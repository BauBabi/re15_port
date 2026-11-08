"""Freigegebene Szene V7 (sfv4/r/V7_szene/debug.log, nur lesen): Kettengelenke je Takt -> Gelenkwinkel, Spitze gegen Boden (y 0)."""
import re, json, numpy as np
D=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/r/V7_szene/debug.log'
S8=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8'
rx=re.compile(r'\[hangar-takt\] h(-?\d+) Schwanz B .*?((?: \d\(-?\d+,-?\d+,-?\d+\)){8})')
W=[];Y=[];H=[]
for l in open(D,errors='replace'):
    m=rx.search(l)
    if not m: continue
    pts=np.array([[int(v) for v in t.split(',')] for t in re.findall(r'\((-?\d+,-?\d+,-?\d+)\)',m.group(2))],float)
    w=[]
    for j in range(1,7):
        a=pts[j]-pts[j-1]; c=pts[j+1]-pts[j]
        w.append(np.degrees(np.arccos(np.clip(a@c/np.linalg.norm(a)/np.linalg.norm(c),-1,1))))
    W.append(w); Y.append(pts[:,1].max()); H.append(int(m.group(1)))
W=np.array(W); Y=np.array(Y)
r=dict(takte=len(W),winkel_median=[round(float(x),1) for x in np.median(W,0)],winkel_max=[round(float(x),1) for x in W.max(0)],
       takte_mit_gelenk_ueber_60=int((W>60).any(1).sum()),tiefstes_gelenk_y_max=round(float(Y.max()),1),takte_unter_boden=int((Y>0).sum()),
       h_bereich=[min(H),max(H)])
print(r); json.dump(r,open(S8+'/m28_szene.json','w'))
