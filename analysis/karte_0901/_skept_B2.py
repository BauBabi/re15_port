import os, sys, re, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import rects, rect_uv, page_pix
h=collections.Counter()
for pg in range(13):
    pix=page_pix(pg)
    if pix is None: print('keine Seite',pg); continue
    for r,(RX,RY,RW,RH) in enumerate(rects(pg)):
        U,V=rect_uv(pg,r)
        for j in range(RH):
            for i in range(RW):
                if V+j<256 and U+i<256: h[pix[V+j][U+i]]+=1
print('Palettenindex-Histogramm ueber alle Rechtecke aller Seiten:')
for k in sorted(h): print(f'  idx {k:2d} : {h[k]:8d}')
