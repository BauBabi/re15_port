from szmass import *
import json, sys
from suche1 import frei
FEUER = [(3000,-21000),(5450,-20950),(8450,-22700),(11350,-20650),(12600,-21250),(3600,-25600),(1200,-21100),(2400,-22600),
         (-900,-24600),(-600,-23900),(-900,-23250),(-600,-22750),(9900,-21200),(11250,-21550),(9900,-24500),(4000,-24600),(-1000,-22250)]
def feuer_ab(x, z): return min(math.hypot(x-a, z-b) for a, b in FEUER)
def weg_ab(x0, z0, x1, z1):
    return min(feuer_ab(x0+(x1-x0)*t/20, z0+(z1-z0)*t/20) for t in range(21))
A0 = (8000, -24250)
if __name__ == "__main__":
    D = 5157
    erg = []
    for ax in range(-300, 4001, 250):
      for az in range(-24800, -21099, 250):
        if not frei(ax, az, 350) or feuer_ab(ax, az) < 1200: continue
        for th in range(-30, 31, 5):
          t = math.radians(th); fx = ax + D*math.cos(t); fz = az + D*math.sin(t)
          if not frei(fx, fz, 1500): continue
          gf = blick(fx, fz, ax, az); ga = blick(ax, az, fx, fz)
          buf, al = idpuffer(11, [(FORM,2,0,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
          m11 = mass(buf, al)
          if not m11['frau_ganz'] or m11['frau_sichtbar'] < 0.97 or m11['form'] > 0.40 or m11['form'] < 0.04: continue
          hx = ax + 3760*math.cos(t); hz = az + 3760*math.sin(t)
          buf, al = idpuffer(15, [(FORM,10,40,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
          m15s = mass(buf, al)
          buf, al = idpuffer(15, [(FORM,4,14,(hx,0,hz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
          m15h = mass(buf, al)
          buf, al = idpuffer(11, [(FORM,10,0,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),(ga+2048)&4095,2)])
          m11l = mass(buf, al)
          e = dict(A=(ax,az), F=(int(fx),int(fz)), th=th, feuerA=int(feuer_ab(ax,az)), weg=int(weg_ab(*A0, ax, az)),
                   c11=m11, c15starr=m15s, c15hieb=m15h, c11land=m11l, augeA=int(math.hypot(ax-46, az+24302)))
          erg.append(e)
          print(e['A'], th, e['F'], 'fA %d weg %d | c11 form %.2f fh %d | starr15 %.2f fr %.2f | hieb15 %.2f fr %.2f | land11 %.2f | augeA %d' % (
              e['feuerA'], e['weg'], m11['form'], m11['frau_bbox'][3]-m11['frau_bbox'][1], m15s['form'], m15s['frau_sichtbar'],
              m15h['form'], m15h['frau_sichtbar'], m11l['form'], e['augeA']), flush=True)
    json.dump(erg, open('suche2.json', 'w'), indent=0)
