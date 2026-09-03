"""Vergleicht das Tiefenmodell DIREKT mit den Kuenstler-Tiefen.

Bisher wurde der Tiefenfaktor ueber ein Ersatzmass geeicht ("Uebereinstimmung" einer
nachgebauten Verdeckung). Hier wird die Groesse verglichen, um die es geht: der
depth-Wert je Maskenrechteck. Region = die ECHTE Kuenstler-Silhouette, damit nur der
Tiefenrechner gemessen wird und nicht die Auswahl.
"""
import os, sys, struct
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, original as ORIG
from geom import load_rdt, cut_view, vz_at_floor
from scipy import ndimage

CD = "re15_port/shared_assets/PSX"

def modell_tiefe(rdt, cam, cut, region, faktor):
    v = cut_view(rdt, cam, cut)
    if not v: return None
    R,t,H = v
    if H <= 0: return None
    lab, n = ndimage.label(region, structure=np.ones((3,3)))
    dep = np.zeros((240,320), np.int32)
    for k in range(1, n+1):
        m = lab == k
        for x in np.where(m.any(0))[0]:
            rows = np.where(m[:,x])[0]
            z = vz_at_floor(R,t,H, x+0.5, float(min(int(rows.max()),239)))
            if not z: continue
            dep[rows,x] = max(1, min(1023, int(z*faktor/64.0)))
    return dep

def main():
    import glob
    rooms = sorted({os.path.basename(p)[:-4] for p in glob.glob("build/bg_ppm/ROOM*.ppm")})
    rooms = sorted({r[:8] for r in rooms})
    faktoren = [0.80, 0.90, 1.00, 1.10, 1.20, 1.30]
    stat = {f: [] for f in faktoren}
    nrect = 0
    for room in rooms:
        rdt, _ = load_rdt(CD, room)
        if not rdt: continue
        try:
            cam = struct.unpack_from("<I", rdt, 0x24)[0]
            at = ORIG.atlas(room)
        except Exception:
            continue
        for cut in range(rdt[1]):
            ms = ORIG.artist_rects(rdt, cam, cut)
            if not ms or cut not in at: continue
            idx = at[cut][0]
            reg = np.zeros((240,320), bool)
            felder = []
            for (sx,sy,X,Y,w,h,dep,gi) in ms:
                x0,x1=max(0,X),min(320,X+w); y0,y1=max(0,Y),min(240,Y+h)
                if x1<=x0 or y1<=y0: continue
                sub = idx[sy+(y0-Y):sy+(y1-Y), sx+(x0-X):sx+(x1-X)]
                if sub.shape != (y1-y0,x1-x0): continue
                op = sub != 0
                reg[y0:y1,x0:x1] |= op
                felder.append((x0,y0,x1,y1,dep,op))
            if not felder: continue
            for f in faktoren:
                d = modell_tiefe(rdt, cam, cut, reg, f)
                if d is None: continue
                for (x0,y0,x1,y1,dep,op) in felder:
                    win = d[y0:y1,x0:x1][op]
                    win = win[win>0]
                    if len(win)==0 or dep==0: continue
                    stat[f].append(int(np.median(win)) - dep)
            nrect += len(felder)
    print("Rechtecke:", nrect)
    for f in faktoren:
        a = np.array(stat[f])
        if not len(a): continue
        print("Faktor %.2f: Median %+6.1f  Mittel %+7.1f  |Fehler|<8: %5.1f %%  "
              "zu NAH (verdeckt zu viel): %5.1f %%  zu FERN: %5.1f %%"
              % (f, np.median(a), a.mean(), 100*(np.abs(a)<8).mean(),
                 100*(a < -8).mean(), 100*(a > 8).mean()))
main()
