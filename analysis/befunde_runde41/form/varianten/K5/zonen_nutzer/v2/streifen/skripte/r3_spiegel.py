"""Runde 3: Spiegel-Gegenstuecke (z -> -z im Knochenraum von Kopf 22) der 10 Kopfflaechen aus Zone 30: Texturzeile, Rotanteil, Material."""
import json
from s_basis import *  # noqa
import s_hals as SH
import material_lesen as ML
import r3_lage as RL
k = kand(ASSET); m = k.mo.meshes[22]; frot = SH.flaechen_rot(k); mat = ML.material_je_flaeche()
mitte = {}
for q, faces, verts in ((0, m.tf, m.tv), (1, m.qf, m.qv)):
    for i, g in enumerate(faces):
        mitte['22|%d|%d' % (q, i)] = np.array([verts[j][:3] for j in g[1::2]], float).mean(0)
erg = {}
for f in [x for x in RL.Z30 if x.startswith('22|')]:
    c = mitte[f] * np.array([1, 1, -1])
    g, d = min(((h, np.linalg.norm(v - c)) for h, v in mitte.items()), key=lambda t: t[1])
    erg[f] = dict(mitte=mitte[f].round().tolist(), zeile=frot[f][1], rot=round(frot[f][0], 3), spiegel=g, spiegel_abstand=round(d, 1),
                  spiegel_zeile=frot[g][1], spiegel_rot=round(frot[g][0], 3), spiegel_material=mat.get(g, '?'), spiegel_in_zone30=g in RL.Z30)
json.dump(erg, open(ZIEL + '/messung/r3_spiegel.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
for f, e in erg.items():
    print(f, e)
