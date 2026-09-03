"""Q1: Ist der sprite.pri-Vordergrundatlas ein AUSSCHNITT des Hintergrundbildes?

Vergleicht fuer jede Maske eines Cuts den Atlas-Ausschnitt (srcX,srcY,w,h) mit dem
Hintergrund an der Zielstelle (dstX,dstY,w,h). Gegenprobe: derselbe Atlas-Ausschnitt
gegen eine ZUFAELLIGE andere Stelle des Hintergrunds. Ist die Hypothese richtig, muss
der Treffer deutlich besser sein als die Gegenprobe.

Der Hintergrund kommt aus dem MDEC-Dekoder (Java-Extraktor, BMP), der Atlas aus dem
8-bpp-TIM — zwei verschiedene Pipelines, deshalb wird helligkeits-normalisiert
verglichen (mittlere Absolutdifferenz nach Abzug des Mittelwerts je Ausschnitt).
"""
import os, sys, struct, glob, random
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sld import sld_decompress

ROOT = "re15_port/shared_assets/PSX"
BMPT = "re15_port/cmake/build/extracted/shared_assets_extracted/PSX"
TBL  = {1:0x1EAE4, 2:0x1713C, 3:0x1D0F8, 4:0x18590, 5:0x1DF18, 6:0x01E3C}
u16 = lambda b,o: struct.unpack_from("<H", b, o)[0]
s16 = lambda b,o: struct.unpack_from("<h", b, o)[0]
u32 = lambda b,o: struct.unpack_from("<I", b, o)[0]

_stage = {}
def stage_bin(s):
    if s not in _stage:
        _stage[s] = open(os.path.join(ROOT,"BIN","STAGE%d.BIN"%s),'rb').read()
    return _stage[s]

_bss = {}
def chunk(stage, rid, cut):
    k = (stage, rid>>4)
    if k not in _bss:
        p = os.path.join(ROOT,"STAGE%d"%stage,"ROOM%03X.BSS"%(rid>>4))
        _bss[k] = open(p,'rb').read() if os.path.exists(p) else b""
    b = _bss[k]; o = cut*0x10000
    return b[o:o+0x10000] if o+0x10000 <= len(b) else None

def atlas_rgb(stage, rid, cut):
    ch = chunk(stage, rid, cut)
    if ch is None or cut >= 16: return None
    L = u16(stage_bin(stage), TBL[stage] + ((rid>>4)&0xFF)*0x20 + cut*2)
    if L < 8 or L > len(ch) or u32(ch, L-4) == 0: return None
    off = u32(ch, L-8)
    if off+4 > len(ch): return None
    out,_ = sld_decompress(ch, off+4, u32(ch, off))
    flag = u32(out,4); o = 8
    if not (flag & 8): return None
    clen = u32(out,o); clut_raw = out[o+12:o+clen]; o += clen
    plen,px,py,pw,ph = struct.unpack_from("<IHHHH", out, o)
    W, H = pw*2, ph
    idx = np.frombuffer(out[o+12:o+plen], dtype=np.uint8)[:W*H].reshape(H, W)
    clut = np.frombuffer(clut_raw[:512], dtype='<u2')
    r = ((clut & 0x1f) << 3).astype(np.uint8)
    g = (((clut >> 5) & 0x1f) << 3).astype(np.uint8)
    b = (((clut >> 10) & 0x1f) << 3).astype(np.uint8)
    pal = np.stack([r,g,b], 1)
    return pal[idx], idx

PPMT = os.environ.get("RE15_BG_PPM", "build/bg_ppm")

def bg_rgb(stage, rid, cut):
    """Hintergrundbild eines Cuts.

    ZUERST der eigene C-Dekoderabzug (probe_bg_dump, alle 1119 Cuts aller 6 Stages),
    ERST DANN der Java-Extraktionsbaum. Grund: der Extraktionsbaum deckt STAGE6 gar
    nicht und STAGE5 nur teilweise ab — ein Generator, der sich darauf stuetzt, waere
    fuer ein Sechstel des Spiels blind. Der C-Abzug ist ausserdem die richtige
    Referenz, weil er zeigt, was der Port tatsaechlich darstellt (die beiden weichen
    um im Mittel ~4 von 255 ab, Chroma-Rundung)."""
    q = "%s/ROOM%03X%02d.ppm" % (PPMT, rid>>4, cut)
    if os.path.exists(q):
        return np.asarray(Image.open(q).convert("RGB"), dtype=np.int16)
    p = "%s/STAGE%d/ROOM%03X/ROOM%03X%02d.bmp" % (BMPT, stage, rid>>4, rid>>4, cut)
    if not os.path.exists(p): return None
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.int16)

def masks(d, po):
    gc = u16(d,po); mc = u16(d,po+2)
    if gc == 0xFFFF or gc == 0 or mc == 0 or gc > 256: return []
    gn=[];gdx=[];gdy=[]; p = po+4
    for i in range(gc):
        gn.append(u16(d,p)); gdx.append(s16(d,p+4)); gdy.append(s16(d,p+6)); p += 8
    out=[]; gi=0; used=0
    for _ in range(sum(gn)):
        if p+8 > len(d): break
        sx,sy,dx,dy = d[p],d[p+1],d[p+2],d[p+3]
        depth = u16(d,p+4); size = u16(d,p+6); p += 8
        if (size & 0xf000) == 0:
            w = u16(d,p); h = u16(d,p+2); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]: gi += 1; used = 0
        ax = gdx[gi] if gi < gc else 0; ay = gdy[gi] if gi < gc else 0
        used += 1
        X = ((dx+ax+0x8000) & 0xffff) - 0x8000
        Y = ((dy+ay+0x8000) & 0xffff) - 0x8000
        out.append((sx,sy,X,Y,w,h,depth))
    return out

def norm_mad(a, b):
    """Mittlere Absolutdifferenz nach Abzug des jeweiligen Mittelwerts (helligkeitsfrei)."""
    a = a.astype(np.float32); b = b.astype(np.float32)
    return float(np.abs((a - a.mean()) - (b - b.mean())).mean())

if __name__ == '__main__':
    rng = random.Random(12345)
    hit, miss, skipped = [], [], 0
    percut = []
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
            if A is None or B is None: skipped += 1; continue
            at, idx = A
            ch_, cm_ = [], []
            for (sx,sy,X,Y,w,h,dep) in ms:
                if w <= 0 or h <= 0: continue
                if sy+h > at.shape[0] or sx+w > at.shape[1]: continue
                if X < 0 or Y < 0 or Y+h > 240 or X+w > 320: continue
                if idx[sy:sy+h, sx:sx+w].min() == 0: continue      # transparente Pixel -> unfair
                a = at[sy:sy+h, sx:sx+w]
                b = B[Y:Y+h, X:X+w]
                ch_.append(norm_mad(a,b))
                rx = rng.randrange(0, max(1,320-w)); ry = rng.randrange(0, max(1,240-h))
                cm_.append(norm_mad(a, B[ry:ry+h, rx:rx+w]))
            if ch_:
                hit += ch_; miss += cm_
                percut.append((name, c, len(ch_), float(np.mean(ch_)), float(np.mean(cm_))))

    print("verglichene Masken: %d   (Cuts: %d, uebersprungen: %d)" % (len(hit), len(percut), skipped))
    print("Treffer  (Atlas vs. BG an der Zielstelle): MAD Mittel %.2f  Median %.2f" % (np.mean(hit), np.median(hit)))
    print("Gegenprobe (zufaellige BG-Stelle)        : MAD Mittel %.2f  Median %.2f" % (np.mean(miss), np.median(miss)))
    h = np.array(hit); m = np.array(miss)
    print("Masken mit MAD < 6 : Treffer %.1f%%  Gegenprobe %.1f%%" % (100*(h<6).mean(), 100*(m<6).mean()))
    print("Treffer besser als eigene Gegenprobe: %.1f%%" % (100*(h<m).mean()))
    percut.sort(key=lambda r: r[3])
    print("\nbeste Cuts:  " + ", ".join("%s#%d %.1f"%(r[0],r[1],r[3]) for r in percut[:6]))
    print("schlechteste: " + ", ".join("%s#%d %.1f"%(r[0],r[1],r[3]) for r in percut[-6:]))
