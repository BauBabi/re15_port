"""Q2: Laesst sich ein Atlas-Teil allein durch Bildabgleich im Hintergrund WIEDERFINDEN?

Pruefmenge: die Cuts, in denen die Kuenstler sowohl Atlas als auch Maskenrechtecke
hinterlassen haben — dort ist die richtige Zielposition bekannt. Gesucht wird per
normalisierter Kreuzkorrelation ueber das ganze 320x240-Bild; gemessen wird, wie oft
das Maximum auf der wahren Position liegt.

Faellt das gut aus, sind die 238 Cuts "Atlas vorhanden, Geometrie fehlt" bis auf die
Tiefe geloest — die Rechtecke lassen sich aus dem Atlas zurueckgewinnen.
"""
import os, sys, struct, glob
import numpy as np
from scipy.signal import fftconvolve
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, bg_rgb, masks, TBL, u32, ROOT

def ncc_best(tpl, img):
    """Normalisierte Kreuzkorrelation, Rueckgabe (x, y, score) des Maximums."""
    t = tpl.astype(np.float64); t = t - t.mean()
    tn = np.sqrt((t*t).sum())
    if tn < 1e-6: return None
    I = img.astype(np.float64)
    th, tw = t.shape
    ones = np.ones((th, tw))
    num = fftconvolve(I, t[::-1, ::-1], mode='valid')
    s1  = fftconvolve(I, ones, mode='valid')
    s2  = fftconvolve(I*I, ones, mode='valid')
    n   = th*tw
    var = s2 - s1*s1/n
    var[var < 1e-6] = 1e-6
    sc  = (num - s1*t.mean()*0) / (np.sqrt(var) * tn)   # t ist bereits mittelwertfrei
    y, x = np.unravel_index(np.argmax(sc), sc.shape)
    return int(x), int(y), float(sc[y, x])

lum = lambda a: (a[...,0]*0.299 + a[...,1]*0.587 + a[...,2]*0.114)

exact = near = far = 0
scores_ok, scores_bad = [], []
tested_cuts = 0
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name = os.path.basename(rdt)[:-4]; rid = int(name[4:],16)
    if rid & 1: continue
    stage = (rid>>12) & 0xF
    if stage not in TBL: continue
    d = open(rdt,'rb').read()
    if len(d) < 0x60: continue
    nCut = d[1]; cam = u32(d,0x24)
    if not cam or cam+32*nCut > len(d): continue
    for c in range(nCut):
        ms = masks(d, u32(d, cam+c*32+0x1C))
        if not ms: continue
        A = atlas_rgb(stage, rid, c); B = bg_rgb(stage, rid, c)
        if A is None or B is None: continue
        at, idx = A
        atl = lum(at); bgl = lum(B)
        used = 0
        for (sx,sy,X,Y,w,h,dep) in ms:
            if w < 8 or h < 8: continue                       # zu klein fuer eine eindeutige Korrelation
            if sy+h > atl.shape[0] or sx+w > atl.shape[1]: continue
            if X < 0 or Y < 0 or Y+h > 240 or X+w > 320: continue
            if idx[sy:sy+h, sx:sx+w].min() == 0: continue
            r = ncc_best(atl[sy:sy+h, sx:sx+w], bgl)
            if r is None: continue
            bx, by, sc = r
            dd = max(abs(bx-X), abs(by-Y))
            if dd == 0: exact += 1; scores_ok.append(sc)
            elif dd <= 2: near += 1; scores_ok.append(sc)
            else: far += 1; scores_bad.append(sc)
            used += 1
        if used: tested_cuts += 1

tot = exact+near+far
print("gesuchte Maskenteile: %d in %d Cuts" % (tot, tested_cuts))
print("  exakt getroffen (0 px) : %5d  %.1f%%" % (exact, 100*exact/max(1,tot)))
print("  bis 2 px daneben       : %5d  %.1f%%" % (near, 100*near/max(1,tot)))
print("  verfehlt (>2 px)       : %5d  %.1f%%" % (far,  100*far /max(1,tot)))
if scores_ok: print("  NCC-Wert Treffer  : Median %.3f" % np.median(scores_ok))
if scores_bad: print("  NCC-Wert Fehlschlag: Median %.3f" % np.median(scores_bad))
