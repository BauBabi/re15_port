"""Pruefung der erzeugten Maskendaten.

1. FORMAT: laesst sich jede Sektion mit der Original-Logik lesen? Grenzen eingehalten?
2. DECKUNGSPROBE (die schaerfste): die Masken durch den Atlas auf den Hintergrund
   blitten. Da der Atlas-Inhalt = Hintergrund-Inhalt an der Zielstelle ist, MUSS das
   Bild praktisch unveraendert bleiben. Jede sichtbare Aenderung ist eine falsch
   platzierte Maske — genau der Fehler, der im Spiel als "Hintergrundfetzen ueber
   Leon" auffallen wuerde.
"""
import os, sys, glob, struct
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, bg_rgb, TBL, ROOT

def parse_section(b, off):
    gc, mc = struct.unpack_from("<HH", b, off)
    if gc == 0xFFFF or gc == 0 or mc == 0 or gc > 256: return None
    p = off+4; gn=[]; gdx=[]; gdy=[]
    for i in range(gc):
        n_, base, dx, dy = struct.unpack_from("<HHhh", b, p); gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    out=[]; gi=0; used=0
    for _ in range(sum(gn)):
        sx,sy,dx,dy = b[p],b[p+1],b[p+2],b[p+3]
        dep, size = struct.unpack_from("<HH", b, p+4); p += 8
        if (size & 0xf000) == 0:
            w,h = struct.unpack_from("<HH", b, p); p += 4
        else:
            w = h = (size>>12)*8
        while gi < gc and used >= gn[gi]: gi += 1; used = 0
        ax = gdx[gi] if gi < gc else 0; ay = gdy[gi] if gi < gc else 0
        used += 1
        out.append((sx,sy,((dx+ax+0x8000)&0xffff)-0x8000,((dy+ay+0x8000)&0xffff)-0x8000,w,h,dep))
    return out

bad_fmt = 0; cuts = 0; maxmask = 0
diffs = []; worst = []
for f in sorted(glob.glob("build/masken/*.MSK")):
    name = os.path.basename(f)[:-4]; rid = int(name[4:],16); stage = (rid>>12)&0xF
    b = open(f,'rb').read()
    assert b[:4] == b"R15M"
    ver, nCut = struct.unpack_from("<II", b, 4)
    offs = struct.unpack_from("<%dI"%nCut, b, 12)
    for c in range(nCut):
        if not offs[c]: continue
        ms = parse_section(b, offs[c])
        if ms is None: bad_fmt += 1; continue
        cuts += 1; maxmask = max(maxmask, len(ms))
        if len(ms) > 105: bad_fmt += 1
        A = atlas_rgb(stage, rid, c); B = bg_rgb(stage, rid, c)
        if A is None or B is None: continue
        at, idx = A
        canvas = B.astype(np.int16).copy()
        painted = np.zeros((240,320), bool)
        for (sx,sy,X,Y,w,h,dep) in ms:
            if sy+h > idx.shape[0] or sx+w > idx.shape[1]: bad_fmt += 1; continue
            if X < 0 or Y < 0 or X+w > 320 or Y+h > 240: bad_fmt += 1; continue
            sub = idx[sy:sy+h, sx:sx+w]
            op = sub != 0
            tgt = canvas[Y:Y+h, X:X+w]
            src = at[sy:sy+h, sx:sx+w]
            tgt[op] = src[op]
            painted[Y:Y+h, X:X+w] |= op
        if painted.sum() == 0: continue
        d = np.abs(canvas.astype(int) - B.astype(int)).sum(2)[painted]
        diffs.append(float(np.mean(d)))
        worst.append((float(np.percentile(d,95)), name, c, int(painted.sum())))

print("Cuts geprueft: %d | Formatfehler: %d | groesste Maskenzahl: %d (Grenze 105)"%(cuts,bad_fmt,maxmask))
a = np.array(diffs)
print("DECKUNGSPROBE — Farbabweichung der gemalten Pixel (Summe ueber RGB, 0..765):")
print("   Mittel %.2f | Median %.2f | 90%%-Quantil %.2f"%(a.mean(), np.median(a), np.percentile(a,90)))
worst.sort(reverse=True)
print("   auffaelligste Cuts: "+", ".join("%s#%d p95=%.0f"%(w[1],w[2],w[0]) for w in worst[:5]))
