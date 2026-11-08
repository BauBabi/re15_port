# Leser B v3: Hintergrund des Hangar-Schnitts als "dichteste Zeitgruppe" je Pixel (Modus statt Median),
# weil die Figur in x 640-930 mehr als die Haelfte der Bilder belegt (Median zeigt dort einen Geist).
# Je Pixel: Grauwert-Histogramm ueber jedes 2. Bild n1351..n1481 (66 Bilder), Fenster 3 Klassen a 6 Grauwerte,
# dichtestes Fenster -> Mittel der BGR-Werte der Bilder darin.
# Aufruf: python b_hintergrund.py <sfv3-dir> <ki|orig> -> sfv3/tmp/bg_modus_<quelle>.png
import sys, cv2, numpy as np
S,Q=sys.argv[1],sys.argv[2]
fr=np.stack([cv2.imread(f"{S}/{Q}/n{n}.png") for n in range(1351,1483,2)]).astype(np.float32)
g=fr.mean(3)
B=6; nb=256//B+1
idx=(g//B).astype(np.int32)
H=np.zeros((nb,)+g.shape[1:],np.int32)
for k in range(fr.shape[0]):
    np.add.at(H,(idx[k],)+tuple(np.indices(g.shape[1:])),1) if False else None
# schneller: one-hot Summe
for c in range(nb): H[c]=(idx==c).sum(0)
W=H.copy(); W[1:]+=H[:-1]; W[:-1]+=H[1:]
best=W.argmax(0)
sel=(np.abs(idx-best[None])<=1)
cnt=sel.sum(0)
bg=(fr*sel[...,None]).sum(0)/np.maximum(cnt,1)[...,None]
cv2.imwrite(f"{S}/tmp/bg_modus_{Q}.png",bg.astype(np.uint8))
cv2.imwrite(f"{S}/tmp/bg_modus_{Q}_anteil.png",(cnt*255//fr.shape[0]).astype(np.uint8))
print("Anteil Bilder im Modusfenster: min",cnt.min(),"median",np.median(cnt))
