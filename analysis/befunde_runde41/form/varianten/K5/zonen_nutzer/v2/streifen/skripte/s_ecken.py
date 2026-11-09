"""Bild-2-Pose (Asset Clip 12 Bild 0): die Ecken der Keil-Flaechen von Kopf 22 einzeln (Index im Teil, Lage zur Brust-/Hals-Oberflaeche).
Und: welche Kopf-/Kieferflaechen sind ueber alle Clips (jedes 10. Bild) aus 10 Richtungen im Tiefenpuffer je sichtbar?"""
import json
import math
from collections import Counter, defaultdict
from s_basis import *  # noqa: F401,F403
import s_geo
from s_hals import kamera_blick

k = kand(ASSET)
lage, gier, clip, bild = k.stand(2)
kf = k.kf(clip, bild)
R, T = k.mo.pose(kf)


def teil_ecken(b):
    m = k.mo.meshes[b]
    W = R[b].astype(np.int64); t = T[b].astype(np.int64)
    out = {}
    for viereck, verts, faces in ((0, m.tv, m.tf), (1, m.qv, m.qf)):
        if not verts:
            continue
        v = np.array([x[:3] for x in verts], np.int64)
        out[viereck] = (((W @ v.T) >> 12).T + t).astype(float), faces
    return out


E22 = teil_ecken(22)
flaechen = defaultdict(list)
for b in (10, 21):
    for viereck, (V, faces) in teil_ecken(b).items():
        for fi, g in enumerate(faces):
            ie = g[1::2]
            tri = ((0, 1, 3), (0, 3, 2)) if viereck else ((0, 1, 2),)
            for a, bb, cc in tri:
                flaechen['V'].append(V[[ie[a], ie[bb], ie[cc]]]); flaechen['tag'].append('%d|%d|%d' % (b, viereck, fi))
TB = np.array(flaechen['V'])
erg = json.load(open(AUS + '/schnitt_bild2.json', encoding='utf-8'))
keil_fl = []
for e in erg['keil_flaechen']:
    t = e['flaeche'].split(',')[1].strip().split()  # 'Viereck 4 Haelfte 0'
    keil_fl.append((1 if t[0] == 'Viereck' else 0, int(t[1])))
keil_fl = sorted(set(keil_fl))
aus = []
for viereck, fi in keil_fl:
    V, faces = E22[viereck]
    ie = faces[fi][1::2]
    P = V[list(ie)]
    d2, X, innen = s_geo.naechster_punkt(P, TB[:, 0], TB[:, 1], TB[:, 2])
    j = d2.argmin(1); d = np.sqrt(d2[np.arange(len(P)), j]); Xj = X[np.arange(len(P)), j]
    n = -np.cross(TB[:, 1] - TB[:, 0], TB[:, 2] - TB[:, 0])[j]
    n /= np.maximum(np.linalg.norm(n, axis=1, keepdims=True), 1e-9)
    sd = ((P - Xj) * n).sum(1)
    aus.append(dict(flaeche='22|%d|%d' % (viereck, fi), ecken=[int(x) for x in ie],
                    lage=['innen %.0f' % d[i] if (sd[i] < 0 and innen[i, j[i]]) else ('aussen %.0f' % d[i] if sd[i] >= 0 else 'unklar %.0f' % d[i])
                          for i in range(len(P))],
                    naechste_flaeche=[flaechen['tag'][int(x)] for x in j]))
for a in aus:
    print(a)
# Sichtbarkeit ueber Clips und Richtungen (Tiefenpuffer, nur Teile 10/21/22/23 + alle anderen als Verdecker)
sicht = Counter(); gesamt = 0
richt = [(az, el) for az in (0, 72, 144, 216, 288) for el in (-35, 35)]
for c, (f0, nb) in enumerate(k.mo.clips):
    for b in range(0, nb, 10):
        for az, el in richt:
            kam = kamera_blick((0, -1200, 0), az, el)
            tris = dreiecke_3d(k.mo, k.kf(c, b), (0, 0, 0), 0, kam)
            Rr = rastern_beide(tris, k.tex, 240, 320, np.eye(3))
            for i in np.unique(Rr['puffer_i'][Rr['puffer_i'] >= 0]):
                tg = tris[i][4]
                if tg[0] == 22 and (tg[2], tg[1]) in keil_fl:
                    sicht['22|%d|%d' % (tg[2], tg[1])] += int((Rr['puffer_i'] == i).sum())
            gesamt += 1
print('Ansichten', gesamt)
res = dict(ecken=aus, sichtbar_px_tiefenpuffer=dict(sicht), ansichten=gesamt, richtungen=richt, bilder='jedes 10. Bild aller 28 Clips')
json.dump(res, open(AUS + '/ecken_keil.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
print(json.dumps(res['sichtbar_px_tiefenpuffer'], indent=1))
