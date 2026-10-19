import sys; sys.path.insert(0,'.')
from komp import farbe_klasse, KL, NAME
import numpy as np
from PIL import Image
from collections import Counter
NAMEN={2:'map1F',3:'map2F',4:'map3F',1:'mapB2'}
sd=sys.argv[1]
gesamt=0
for seite,name in NAMEN.items():
    a=farbe_klasse(np.array(Image.open(f'{sd}/s{seite:02d}.bmp').convert('RGB')))
    s=farbe_klasse(np.array(Image.open(f'soll_{name}.png')))
    # aktuell (C) == Koerper (B) fuer den Vergleich (Zustand des Nutzers: 1090 oben aktuell)
    a2=a.copy(); s2=s.copy()
    for k in (a2,s2):
        k[k==KL['C']]=KL['B']; k[k==KL['CW']]=KL['X']
    m=np.zeros(a.shape,bool); m[50:200,90:240]=True
    d=(a2!=s2)&m
    # Spieler-Marker-Ring der Vorlage (O) ausnehmen
    d&=(s2!=KL['O'])&(a2!=KL['O'])
    ys,xs=np.nonzero(d)
    print(name,'Abweichungen gegen Soll:',int(d.sum()), Counter((NAME[a2[y,x]],NAME[s2[y,x]]) for y,x in zip(ys,xs)), list(zip(xs.tolist(),ys.tolist()))[:12])
    gesamt+=int(d.sum())
print('GESAMT',gesamt)
