from szmass import *
import json, sys
from suche1 import frei
from suche2 import FEUER, feuer_ab, weg_ab
A0 = (8000, -24250)
D = int(sys.argv[1])
erg = []
for ax in range(2500, 8001, 250):
  for az in range(-24900, -21299, 200):
    if not frei(ax, az, 350) or feuer_ab(ax, az) < 1000 or weg_ab(*A0, ax, az) < 700: continue
    for th in range(120, 241, 6):            # Form westlich/nordwestlich/suedwestlich
      t = math.radians(th); fx = ax + D*math.cos(t); fz = az + D*math.sin(t)
      if not frei(fx, fz, 1300): continue
      gf = blick(fx, fz, ax, az); ga = blick(ax, az, fx, fz)
      buf, al = idpuffer(15, [(FORM,2,0,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
      ms = mass(buf, al)
      if not ms['frau_ganz'] or ms['frau_sichtbar'] < 0.95 or ms['form'] > 0.40 or ms['form'] < 0.05: continue
      buf, al = idpuffer(15, [(FORM,10,0,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
      ml = mass(buf, al)
      buf, al = idpuffer(11, [(FORM,10,40,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
      m11 = mass(buf, al)
      e = dict(A=(ax,az), F=(int(fx),int(fz)), th=th, fA=int(feuer_ab(ax,az)), fF=int(feuer_ab(fx,fz)), weg=int(weg_ab(*A0,ax,az)),
               c15=ms, c15land=ml, c11=m11)
      erg.append(e)
      print(e['A'], th, e['F'], 'fA %d fF %d weg %d | satz15 form %.2f fr %.2f bb %s | land15 %.2f fr %.2f | c11 form %.2f fr %.2f' % (
          e['fA'], e['fF'], e['weg'], ms['form'], ms['frau_sichtbar'], ms['frau_bbox'], ml['form'], ml['frau_sichtbar'], m11['form'], m11['frau_sichtbar']), flush=True)
json.dump(erg, open('suche3_%d.json' % D, 'w'), indent=0)
