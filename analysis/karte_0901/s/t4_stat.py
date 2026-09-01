# -*- coding: utf-8 -*-
import sys, collections, statistics, math, json
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as L
G=L.G; s16=G['s16']; u16=G['u16']; BASES=G['BASES']; page_of=G['page_of']

def zeile(rid):
    idx=BASES[(rid>>12)-1]+((rid>>4)&0xff); a=0x800768b0+idx*8
    return idx,a,(s16(a),s16(a+2),u16(a+4),u16(a+6))

rooms=sorted(set(k[0] for k in ZONES))
echt=[]; stub=[]
for r in rooms:
    idx,a,(ox,oy,sx,sy)=zeile(r)
    (echt if (sx>1 and sy>1) else stub).append((r,idx,a,ox,oy,sx,sy))
print('Header-Raeume %d  echt %d  stub %d'%(len(rooms),len(echt),len(stub)))

# SCA-Punkte
def sca_pts(rid):
    sca=RDT[rid][0]; pts=[]
    for (x,zz,w,dep) in sca:
        for i in range(4):
            for j in range(4):
                pts.append((x+w*(2*i+1)//8, zz+dep*(2*j+1)//8))
    return pts

rows=[]
for (r,idx,a,ox,oy,sx,sy) in echt:
    # Zone mit Zeile bzw. Zone 0
    zs=[ZONES[k] for k in sorted(ZONES) if k[0]==r]
    z=None
    for zz in zs:
        if zz['sx']>1: z=zz; break
    if z is None: z=zs[0]
    R=G['rects'](z['page'])[z['rect']]
    pts=sca_pts(r)
    P=[( ((((wx+32000)*10*sx)>>20)+5)//10+ox, -((((wz+32000)*10*sy)>>20)+5)//10+oy ) for wx,wz in pts]
    px0=min(p[0] for p in P); px1=max(p[0] for p in P)
    py0=min(p[1] for p in P); py1=max(p[1] for p in P)
    # Welt-Bbox der ZONE (nicht des ganzen Raums)
    wx0,wx1,wz0,wz1=z['x0'],z['x1'],z['z0'],z['z1']
    rows.append(dict(room=r,idx=idx,addr=a,ox=ox,oy=oy,sx=sx,sy=sy,page=z['page'],rect=z['rect'],
        R=R, projw=px1-px0+1, projh=py1-py0+1, wW=wx1-wx0, wD=wz1-wz0,
        upx=(1<<20)/sx, upy=(1<<20)/sy, hdr=(z['sx']>1)))
json.dump(rows,open(r'analysis\karte_0901\s\echt.json','w'))

print()
print('%-5s %-4s %5s %5s %6s %6s %8s %8s %3s/%-2s %4sx%-4s %5sx%-5s %6sx%-6s'%(
 'Raum','idx','ox','oy','sx','sy','E/px_x','E/px_y','pg','rc','rW','rH','projW','projH','weltW','weltD'))
for d in rows:
    print('%04X %4d %5d %5d %6d %6d %8.1f %8.1f %3d/%-2d %4dx%-4d %5dx%-5d %6dx%-6d'%(
      d['room'],d['idx'],d['ox'],d['oy'],d['sx'],d['sy'],d['upx'],d['upy'],
      d['page'],d['rect'],d['R'][2],d['R'][3],d['projw'],d['projh'],d['wW'],d['wD']))

print()
print('=== Streuung Welteinheiten je Pixel, je Stage ===')
print('%-6s %3s %8s %8s %8s %8s %8s'%('Stage','n','min','median','max','stdev','max/min'))
for st in range(1,7):
    v=[d['upx'] for d in rows if d['room']>>12==st]+[d['upy'] for d in rows if d['room']>>12==st]
    if not v: continue
    print('%-6d %3d %8.1f %8.1f %8.1f %8.2f %8.3f'%(st,len(v),min(v),statistics.median(v),max(v),
        statistics.pstdev(v),max(v)/min(v)))
v=[d['upx'] for d in rows]+[d['upy'] for d in rows]
print('%-6s %3d %8.1f %8.1f %8.1f %8.2f %8.3f'%('ALLE',len(v),min(v),statistics.median(v),max(v),statistics.pstdev(v),max(v)/min(v)))
print()
print('RE2-Referenz 450 E/px: Abweichung Median je Stage')
for st in range(1,7):
    v=[d['upx'] for d in rows if d['room']>>12==st]+[d['upy'] for d in rows if d['room']>>12==st]
    if not v: continue
    m=statistics.median(v)
    print('  Stage %d: Median %7.1f  = %+6.1f %%  gegen RE2 450'%(st,m,(m/450-1)*100))
print()
print('anisotrop (sx != sy):', sum(1 for d in rows if d['sx']!=d['sy']),'von',len(rows))
r=[d['sy']/d['sx'] for d in rows]
print('sy/sx  min %.4f  median %.4f  max %.4f'%(min(r),statistics.median(r),max(r)))
