"""Kalibriert die Nachbearbeitung gegen die ECHTE Kuenstler-Silhouette.

Wahrheit = die undurchsichtigen Pixel des Original-SLD-Atlas an ihrer Zielstelle
(nicht die Rechteck-Vereinigung — die ist deutlich groeber als die Maske selbst).
Die Auswahl wird dabei als PERFEKT angenommen (alle Superpixel, die zu >=50 % in der
Wahrheit liegen), damit ausschliesslich die Nachbearbeitung gemessen wird.
"""
import sys, os, itertools
import numpy as np
from scipy import ndimage
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
from geom import load_bg, load_rdt
from blatt import segment
import original as ORIG
import struct

CD = "re15_port/shared_assets/PSX"

def wahrheit(room, cut, at, rdt, cam):
    ms = ORIG.artist_rects(rdt, cam, cut)
    if not ms: return None
    idx = at.get(cut, (None, None))[0]
    if idx is None: return None
    w = np.zeros((240,320), bool)
    for (sx,sy,X,Y,ww,hh,dep,gi) in ms:
        x0,x1=max(0,X),min(320,X+ww); y0,y1=max(0,Y),min(240,Y+hh)
        if x1<=x0 or y1<=y0: continue
        sub = idx[sy+(y0-Y):sy+(y1-Y), sx+(x0-X):sx+(x1-X)]
        if sub.shape != (y1-y0, x1-x0): continue
        w[y0:y1,x0:x1] |= (sub != 0)
    return w

def nach(r, oben, grow, fuellen):
    r = r.copy()
    if oben:
        o = r.copy()
        for k in range(1, oben+1): o[:-k] |= r[k:]
        r = o
    if grow: r = ndimage.binary_dilation(r, iterations=grow)
    if fuellen == "global":
        for x in range(320):
            ys = np.nonzero(r[:,x])[0]
            if len(ys): r[ys.min():ys.max()+1, x] = True
    elif fuellen == "komponente":
        lab, n = ndimage.label(r, structure=np.ones((3,3)))
        for k in range(1, n+1):
            m = lab == k
            cols = np.where(m.any(0))[0]
            for x in cols:
                ys = np.nonzero(m[:,x])[0]
                r[ys.min():ys.max()+1, x] = True
    return r

FAELLE = [("ROOM1150",1),("ROOM1150",2),("ROOM1150",3),("ROOM1170",4),("ROOM1170",5),
          ("ROOM1210",4),("ROOM1090",8),("ROOM1220",1)]

def main():
    daten = []
    for room, cut in FAELLE:
        rdt, _ = load_rdt(CD, room)
        if not rdt: continue
        cam = struct.unpack_from("<I", rdt, 0x24)[0]
        at = ORIG.atlas(room)
        w = wahrheit(room, cut, at, rdt, cam)
        bg = load_bg("build/bg_ppm", int(room[4:],16), cut)
        if w is None or bg is None or w.sum() < 500: continue
        daten.append((room, cut, bg, w))
    print("Faelle:", [(r,c) for r,c,_,_ in daten])
    for ns in (90, 150, 220, 320):
        segs = [segment(bg, ns) for _,_,bg,_ in daten]
        for oben, grow, fuellen in itertools.product((0,2,4,6),(0,1),("aus","komponente","global")):
            P=[]; A=[]
            for (room,cut,bg,w), seg in zip(daten, segs):
                # perfekte Auswahl: Superpixel mit >=50 % Wahrheitsanteil
                ids=[s for s in np.unique(seg) if w[seg==s].mean()>=0.5]
                r = nach(np.isin(seg, ids), oben, grow, fuellen)
                hit=(r&w).sum()
                P.append(hit/max(1,r.sum())); A.append(hit/max(1,w.sum()))
            print("seg %3d  oben %d grow %d fuell %-11s  Praezision %5.1f %%  Ausbeute %5.1f %%"
                  % (ns, oben, grow, fuellen, 100*np.mean(P), 100*np.mean(A)))
main()

def saum():
    """Misst den Saum an der OBERKANTE je Bildspalte: wie viele Pixel liegt unsere
    Regionsoberkante UNTER der Oberkante der Wahrheit (positiv = Luecke = Fuss schaut durch)."""
    import struct as st
    for room, cut in FAELLE:
        rdt,_ = load_rdt(CD, room)
        if not rdt: continue
        cam = st.unpack_from("<I", rdt, 0x24)[0]
        at = ORIG.atlas(room)
        w = wahrheit(room, cut, at, rdt, cam)
        bg = load_bg("build/bg_ppm", int(room[4:],16), cut)
        if w is None or bg is None or w.sum()<500: continue
        for ns in (220, 320):
            seg = segment(bg, ns)
            ids=[s for s in np.unique(seg) if w[seg==s].mean()>=0.5]
            roh = np.isin(seg, ids)
            for oben in (0,2,4,6):
                r = nach(roh, oben, 1, "komponente")
                d=[]
                for x in range(320):
                    tw=np.nonzero(w[:,x])[0]; tr=np.nonzero(r[:,x])[0]
                    if len(tw)==0: continue
                    if len(tr)==0: d.append(240); continue
                    d.append(tr.min()-tw.min())
                d=np.array(d)
                print("%s c%d seg%3d oben%d: Saum Median %+d  90%%-Quantil %+d  schlimmste %+d  Spalten mit Luecke>0: %d/%d"
                      % (room, cut, ns, oben, int(np.median(d)), int(np.percentile(d,90)), int(d.max()), int((d>0).sum()), len(d)))
saum()
