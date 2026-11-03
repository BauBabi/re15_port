"""NB3 M1 Rueckfall: Satz in Cut 11 (Auge (7626,-3256,-25360) Blick West). Gesucht: Annette ganz zwischen den Balken und frei,
Form 8..40 %, Annette moeglichst mit dem Gesicht zur Kamera (Form seitlich), Sturz in Cut 11 sichtbar, Weg/Stand fern der Feuer."""
from szmass import *
import json, sys
from suche1 import frei
from suche2 import FEUER, feuer_ab, weg_ab
A0 = (8000, -24250)
AUGE11 = (7626, -25360)
erg = []
for D in (3760, 5157):
  for ax in range(-300, 4501, 250):
    for az in range(-24900, -21199, 200):
      if not frei(ax, az, 350) or feuer_ab(ax, az) < 900: continue
      for th in range(100, 261, 10):
        t = math.radians(th); fx = ax + D*math.cos(t); fz = az + D*math.sin(t)
        if not frei(fx, fz, 1300): continue
        gf = blick(fx, fz, ax, az); ga = blick(ax, az, fx, fz)
        # Gesicht zur Kamera: Winkel zwischen Annettes Blick (zur Form) und der Richtung zum Auge
        v1 = (fx-ax, fz-az); v2 = (AUGE11[0]-ax, AUGE11[1]-az)
        cosw = (v1[0]*v2[0]+v1[1]*v2[1]) / (math.hypot(*v1)*math.hypot(*v2))
        buf, al = idpuffer(11, [(FORM,2,0,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
        m = mass(buf, al)
        if not m['frau_ganz'] or m['frau_sichtbar'] < 0.97 or m['form'] > 0.40 or m['form'] < 0.08: continue
        buf, al = idpuffer(11, [(FORM,10,0,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
        ml = mass(buf, al)
        e = dict(D=D, A=(ax,az), F=(int(fx),int(fz)), th=th, cosw=round(cosw,2), fA=int(feuer_ab(ax,az)), fF=int(feuer_ab(fx,fz)),
                 weg=int(weg_ab(*A0,ax,az)), c11=m, land=ml)
        erg.append(e)
        print(D, e['A'], th, e['F'], 'cos %.2f fA %d fF %d | satz11 form %.2f fr %.2f bb %s | land %.2f' % (
            cosw, e['fA'], e['fF'], m['form'], m['frau_sichtbar'], m['frau_bbox'], ml['form']), flush=True)
json.dump(erg, open('suche4.json', 'w'), indent=0)
