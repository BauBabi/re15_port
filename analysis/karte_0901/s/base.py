# -*- coding: utf-8 -*-
"""Gemeinsame Daten-Basis: Zonen (aus dem generierten Header) + SCA-Zellen JE ZONE."""
import sys, collections, math, itertools, json, statistics
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *          # ZONES, RDT, doors_of, zone_at, proj_cal, proj_bbox, rects, rect_uv, page_pix, page_of, GLYPHEN, gly_of
import sonde_lib as L
G = L.G
s16=G['s16']; u16=G['u16']; BASES=G['BASES']; page_of=G['page_of']
rects=G['rects']; rect_uv=G['rect_uv']; GAP=G['GAP']; MIN_FRAC=G['MIN_FRAC']

def zeile(rid):
    idx=BASES[(rid>>12)-1]+((rid>>4)&0xff); a=0x800768b0+idx*8
    return idx,a,(s16(a),s16(a+2),u16(a+4),u16(a+6))

def zones_cells(sca):
    """wie zones_of, aber liefert (bbox, cells) je Komponente in derselben Reihenfolge"""
    n=len(sca)
    if not n: return []
    parent=list(range(n))
    def find(a):
        while parent[a]!=a: parent[a]=parent[parent[a]]; a=parent[a]
        return a
    for i in range(n):
        xi,zi,wi,di=sca[i]
        for j in range(i+1,n):
            xj,zj,wj,dj=sca[j]
            if (xi<xj+wj+GAP and xj<xi+wi+GAP and zi<zj+dj+GAP and zj<zi+di+GAP):
                ra,rb=find(i),find(j)
                if ra!=rb: parent[rb]=ra
    grp=collections.defaultdict(list)
    for i in range(n): grp[find(i)].append(sca[i])
    out=[]
    for g in grp.values():
        xs=[c[0] for c in g]; zs=[c[1] for c in g]
        xe=[c[0]+c[2] for c in g]; ze=[c[1]+c[3] for c in g]
        bb=(min(xs),max(xe),min(zs),max(ze))
        out.append((bb,(bb[1]-bb[0])*(bb[3]-bb[2]),g))
    tot=sum(o[1] for o in out) or 1
    out=[o for o in out if o[1]>=MIN_FRAC*tot]
    out.sort(key=lambda o:-o[1])
    return [(o[0],o[2]) for o in out]

ZCELLS={}       # (room,zi) -> [cells]
for b in sorted(RDT):
    for i,(bb,cells) in enumerate(zones_cells(RDT[b][0])):
        ZCELLS[(b,i)]=cells

def pts_of(key, n=4):
    """Rasterpunkte der Zone: n x n je SCA-Zelle (identisch zum Generator-Raster bei n=4)"""
    out=[]
    for (x,zz,w,dep) in ZCELLS.get(key,()):
        for i in range(n):
            for j in range(n):
                out.append((x+w*(2*i+1)//(2*n), zz+dep*(2*j+1)//(2*n)))
    return out

def proj(wx,wz,ox,oy,sx,sy):
    return (((((wx+32000)*10*sx)>>20)+5)//10+ox, -((((wz+32000)*10*sy)>>20)+5)//10+oy)

def klemm_fuell(key, ox,oy,sx,sy):
    """Klemmrate (Anteil Rasterpunkte AUSSERHALB des Rechtecks) und Fuellgrad."""
    z=ZONES[key]; R=rects(z['page'])[z['rect']]
    P=[proj(a,c,ox,oy,sx,sy) for a,c in pts_of(key)]
    if not P: return None
    x0,y0,w,h=R
    inn=sum(1 for p in P if x0<=p[0]<=x0+w-1 and y0<=p[1]<=y0+h-1)
    grid=set((p[0],p[1]) for p in P if x0<=p[0]<=x0+w-1 and y0<=p[1]<=y0+h-1)
    return dict(n=len(P), klemm=1.0-inn/len(P), fuell=len(grid)/float(w*h),
                bb=(min(p[0] for p in P),max(p[0] for p in P),min(p[1] for p in P),max(p[1] for p in P)))

ROOMS=sorted(set(k[0] for k in ZONES))
ECHT=[r for r in ROOMS if zeile(r)[2][2]>1 and zeile(r)[2][3]>1]
STUB=[r for r in ROOMS if r not in ECHT]
def zonen(r): return [k for k in sorted(ZONES) if k[0]==r]
