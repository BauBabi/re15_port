"""NB4 N5: gibt es in Wagen C (Schnitte 13/14/15 schauen nach Osten, 11/12 nach Westen) eine Lage Annette A / Form F (Abstand
D = 3760 + 1397 wie der Hieb), bei der man beim Satz Annettes Gesicht sieht (Blickrichtung zur Form hoechstens 60 Grad von der
Richtung A -> Auge), die Form hoechstens 40 % des Bandes deckt und Annette ganz und >= 95 % frei im Band steht?"""
import sys, math, json
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\analysis\befunde_runde41\AN_belege\werkzeug\n3')
from szmass import *
from suche1 import frei
from suche2 import feuer_ab
D = 5157
erg = []; n = 0
for cut in (11, 12, 13, 14, 15):
    s = RAUM.schnitt(cut); E = s['pos']
    for ax in range(-4500, 20001, 500):
        for az in range(-25300, -21399, 300):
            if not frei(ax, az, 350): continue
            for th in range(0, 360, 15):
                t = math.radians(th); fx = ax + D*math.cos(t); fz = az + D*math.sin(t)
                if not frei(fx, fz, 1300): continue
                ex, ez = E[0]-ax, E[2]-az; vx, vz = fx-ax, fz-az
                w = math.degrees(math.acos((ex*vx+ez*vz)/(math.hypot(ex,ez)*math.hypot(vx,vz))))
                if w > 60: continue
                n += 1
                gf = blick(fx, fz, ax, az); ga = blick(ax, az, fx, fz)
                buf, al = idpuffer(cut, [(FORM, 2, 0, (fx, 0, fz), gf, 1), (FRAU, 1, 0, (ax, 0, az), ga, 2)])
                m = mass(buf, al)
                if m['form'] <= 0.40 and m.get('frau_ganz') and m.get('frau_sichtbar', 0) >= 0.95:
                    e = dict(cut=cut, A=(ax, az), F=(int(fx), int(fz)), th=th, w=int(w), form=round(m['form'], 3),
                             frei=round(m['frau_sichtbar'], 3), bbox=m['frau_bbox'], fA=int(feuer_ab(ax, az)))
                    erg.append(e); print(e, flush=True)
print('geprueft', n, 'Treffer', len(erg))
json.dump(erg, open(r'suche5.json', 'w'), indent=0)
