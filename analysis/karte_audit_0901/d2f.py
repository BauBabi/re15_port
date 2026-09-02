# -*- coding: utf-8 -*-
"""Werkzeugkasten fuer das 2F-Audit (Seite 3)."""
import sys, os, math, collections, itertools
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sonde_lib import *

PAGE = 3
P3 = [r for r in range(0x1000, 0x1260, 0x10) if page_of(r) == 3]

def rdt(rid):
    return read_rdt(rid) or read_rdt(rid + 1)

def zonen(rid):
    g = rdt(rid)
    return zones_of(g[0]) if g else []

def zellen_der_zone(rid, bb):
    """SCA-Zellen, die zu dieser Zone gehoeren (Mittelpunkt in der Bbox)."""
    g = rdt(rid)
    out = []
    for (x, z, w, d) in g[0]:
        cx, cz = x + w // 2, z + d // 2
        if bb[0] <= cx <= bb[1] and bb[2] <= cz <= bb[3]:
            out.append((x, z, w, d))
    return out

def raster(cells, n=4):
    """n x n Rasterpunkte je Zelle - wie der Generator (_sca_punkte)."""
    pts = []
    for (x, z, w, d) in cells:
        for i in range(n):
            for j in range(n):
                pts.append((x + w * (2*i+1) // (2*n), z + d * (2*j+1) // (2*n)))
    return pts

def flaeche_union(cells, N=256):
    """Belegter Anteil der Bbox: Rasterung NxN ueber der Bbox, Zellen-Vereinigung."""
    if not cells: return 0.0, (0,0,0,0), 0
    x0 = min(c[0] for c in cells); x1 = max(c[0]+c[2] for c in cells)
    z0 = min(c[1] for c in cells); z1 = max(c[1]+c[3] for c in cells)
    W = x1-x0; D = z1-z0
    if W <= 0 or D <= 0: return 0.0, (x0,x1,z0,z1), 0
    grid = [[0]*N for _ in range(N)]
    for (x,z,w,d) in cells:
        i0 = max(0, (x-x0)*N//W); i1 = min(N, -(-(x+w-x0)*N//W))
        j0 = max(0, (z-z0)*N//D); j1 = min(N, -(-(z+d-z0)*N//D))
        for j in range(j0, j1):
            row = grid[j]
            for i in range(i0, i1): row[i] = 1
    occ = sum(sum(r) for r in grid)
    return occ/float(N*N), (x0,x1,z0,z1), occ

def kachel(pg, ri):
    """Liste der gezeichneten Pixel (k,j) relativ zur Rechteck-Ecke + Kennzahlen."""
    pix = page_pix(pg); R = rects(pg)[ri]; U,V = rect_uv(pg, ri)
    drawn = set(); wand = set()
    for j in range(R[3]):
        for k in range(R[2]):
            if V+j >= 256 or U+k >= 256: continue
            c = pix[V+j][U+k]
            if c != 0:
                drawn.add((k,j))
                if c == 4: wand.add((k,j))
    return R, drawn, wand

def kachel_kennzahl(pg, ri):
    R, drawn, wand = kachel(pg, ri)
    if not drawn: return dict(R=R, n=0)
    xs=[p[0] for p in drawn]; ys=[p[1] for p in drawn]
    bw = max(xs)-min(xs)+1; bh = max(ys)-min(ys)+1
    innen = len(drawn) - len(wand)      # Palettenindex != 4 = Innenflaeche
    return dict(R=R, n=len(drawn), innen=innen, wand=len(wand),
                bx0=min(xs), by0=min(ys), bw=bw, bh=bh,
                solid=len(drawn)/float(bw*bh), asp=bw/float(bh))
