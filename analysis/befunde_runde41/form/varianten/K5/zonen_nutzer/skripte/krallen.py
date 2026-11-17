"""Krallen: Nutzer-Krallen (Dreiecke/Spitzen) gegen Asset (K5e_f1, eingezogen) und K5c (vor dem Einziehen) in derselben Pose."""
import sys, os, json
import numpy as np, cv2
from scipy import ndimage as ndi
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/V4F_skripte')
import v4f
from zonen_spec import lade, VS, AB
from zonen_bau import loecher, endpunkt
HIER = os.path.dirname(os.path.abspath(__file__))
K5C = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/stufe3/basis_k5c'
KRALLEN = {'l': (14, 15, 28, 30), 'r': (19, 20, 29, 31)}


def nach_v(pts, lf, n):
    v = VS[lf]; a = AB[str(n)]['aehnlich']; s = a['massstab']; tx, ty = a['versatz']
    return [[round(s * (x - v['dx']) + tx, 1), round(s * (y - v['dy']) + ty, 1)] for x, y in pts]


def maske_v(m, lf, n):
    v = VS[lf]; a = AB[str(n)]['aehnlich']; s = a['massstab']; tx, ty = a['versatz']
    M = np.array([[s, 0, tx - s * v['dx']], [0, s, ty - s * v['dy']]], np.float32)
    return cv2.warpAffine(m.astype(np.float32), M, (640, 480), flags=cv2.INTER_AREA) >= 0.3


def spitze(mask_v):
    """Spitze eines Krallendreiecks: Punkt mit groesstem Abstand vom Schwerpunkt."""
    ys, xs = np.nonzero(mask_v)
    c = np.array([xs.mean(), ys.mean()]); d = np.hypot(xs - c[0], ys - c[1]); i = d.argmax()
    return [int(xs[i]), int(ys[i])]


def main():
    erg = {}
    ken = {}
    for name, ordner in (('asset', HIER + '/asset'), ('k5c', K5C)):
        for n in (1, 2):
            s, K = v4f.szene_id(ordner, n)
            ken[(name, n)] = K
            if name == 'k5c':
                np.savez_compressed(HIER + '/kennung_k5c_%d.npz' % n, mesh=K['mesh'], maske=K['maske'])
    out = {'sichtbar': {}}
    for (name, n), K in ken.items():
        m = K['mesh']
        out['sichtbar']['%s_bild%d' % (name, n)] = {str(c): int((m == c).sum()) for side in KRALLEN.values() for c in side}
    # Vorderbein links (Bild 2): 4 Dreiecke
    lf = 'Vorderes_Linkes_Bein_mit_fuß.png'
    M, fig, (H, W) = lade(lf)
    s = cv2.dilate((M['hellblau'] & (np.indices(M['hellblau'].shape)[0] >= 438)).astype(np.uint8), np.ones((2, 2), np.uint8)) > 0
    f = ndi.binary_fill_holes(s) & ~s
    lab, nl = ndi.label(f)
    drei = []
    for i in range(1, nl + 1):
        q = lab == i
        if q.sum() < 15:
            continue
        qd = cv2.dilate(q.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
        mv = maske_v(qd, lf, 2)
        ys, xs = np.nonzero(q)
        e = dict(bbox_beschriftet=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())], px_vorlage=int(mv.sum()), spitze_vorlage=spitze(mv))
        for name in ('asset', 'k5c'):
            K = ken[(name, 2)]; mm = K['mesh'][mv & K['maske']]
            e['meshes_' + name] = {str(int(a)): int((mm == a).sum()) for a in np.unique(mm)}
        drei.append(e)
    drei.sort(key=lambda e: e['spitze_vorlage'][0])
    out['vorn_links_nutzer'] = dict(text='Exakt 4 Krallen', dreiecke=drei, anzahl=len(drei))
    # Vorderbein rechts (Bild 2): Spitzen = freie Enden der hellblauen Linie (ohne Hinweislinie) + geschlossene Dreiecke
    lf = 'vorne_rechte_Hand_und_Kralle.png'
    q, info = loecher(lf, 'hellblau', (296, 505, 462, 620))
    enden = [endpunkt(lf, 'hellblau', p, 20) for p in ((301, 613), (442, 511), (448, 583))]
    M, fig, _ = lade(lf)
    lab, nl = ndi.label(q)
    dre = []
    for i in range(1, nl + 1):
        qq = lab == i; mv = maske_v(qq, lf, 2)
        ys, xs = np.nonzero(qq)
        e = dict(bbox_beschriftet=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())], px_vorlage=int(mv.sum()))
        for name in ('asset', 'k5c'):
            K = ken[(name, 2)]; mm = K['mesh'][mv & K['maske']]
            e['meshes_' + name] = {str(int(a)): int((mm == a).sum()) for a in np.unique(mm)}
        dre.append(e)
    # Krallenzone unter der Grundlinie (aus zonen_vorlage)
    zv = dict(np.load(HIER + '/zonen_vorlage.npz'))
    kz = zv['krallen_vorn_rechts']
    kz_m = {}
    for name in ('asset', 'k5c'):
        K = ken[(name, 2)]; mm = K['mesh'][kz & K['maske']]
        kz_m[name] = {str(int(a)): int((mm == a).sum()) for a in np.unique(mm)}
    spitzen_v = nach_v(enden, lf, 2)
    sp = []
    for p, pv in zip(enden, spitzen_v):
        e = dict(beschriftet=p, vorlage=pv)
        for name in ('asset', 'k5c'):
            K = ken[(name, 2)]; x, y = int(round(pv[0])), int(round(pv[1]))
            win = K['mesh'][max(y - 3, 0):y + 4, max(x - 3, 0):x + 4]
            e['meshes_7x7_' + name] = {str(int(a)): int((win == a).sum()) for a in np.unique(win) if a >= 0}
        sp.append(e)
    out['vorn_rechts_nutzer'] = dict(text='Krallen (Zahl nicht genannt)', freie_linienenden=sp, geschlossene_dreiecke=dre,
                                     anzahl_spitzen=len(sp), krallenzone_meshes=kz_m)
    # Hinterfuss links (Bild 1): zwei rote Krallenmarken
    lf = 'linker_rechter_Fuß_hinten.png'
    M, fig, _ = lade(lf)
    marken = []
    for bx in ((512, 378, 540, 398), (512, 418, 542, 458)):
        x0, y0, x1, y1 = bx; q = np.zeros_like(M['rot']); q[y0:y1 + 1, x0:x1 + 1] = M['rot'][y0:y1 + 1, x0:x1 + 1]
        q = cv2.dilate(q.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
        mv = maske_v(q, lf, 1)
        ys, xs = np.nonzero(q)
        e = dict(bbox_beschriftet=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())], px_vorlage=int(mv.sum()))
        for name in ('asset', 'k5c'):
            K = ken[(name, 1)]; mm = K['mesh'][mv & K['maske']]
            e['meshes_' + name] = {str(int(a)): int((mm == a).sum()) for a in np.unique(mm)}
        marken.append(e)
    out['hinten_links_nutzer'] = dict(text='Kaum sichtbar, aber wahrscheinlich 3 Krallen. Rechts waere es sicher genauso', marken=marken, anzahl_marken=len(marken))
    json.dump(out, open(HIER + '/krallen.json', 'w'), indent=1, ensure_ascii=False)
    print(json.dumps(out, indent=1, ensure_ascii=False)[:6000])


if __name__ == '__main__':
    main()
