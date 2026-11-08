"""SF Messblatt Fotovorlage, Schritt 6: Klauenhand (W)-links (Knochen 12, Mesh 12; V5r4 = Referenz 2 bytegleich).

Modell: Krallen = Zusammenhangskomponenten der Flaechen, deren Ecken alle y >= Y_FINGER liegen (Knochenraum, Hand laeuft
entlang +y vom Handgelenk 0 bis 473); je Komponente Spitze (max y), Wurzel (min y), Laenge; projiziert in Pose 1 (px und
in H_port). Y_FINGER = 330: unterhalb liegen nur 6 Ecken je Ring der Handflaeche (Ecken-Histogramm im Ausdruck).
Foto: n1485 (Vorlage 1) + n1476/n1479 (gleicher Ort, weniger abgeblendet): rote Zone (a* >= 8) und helle Krallenstreifen
(L >= 24 innerhalb der Handbox x 476-500, y 344-366) je Zeile; Laenge in H1 = 302.
"""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
from f_analyse import welt
S = os.path.join(G.SFV4, 'foto')
Y_FINGER = 330
V = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')


def krallen(mo):
    m = mo.meshes[12]
    comps = []
    for verts, faces, vi in ((m.tv, m.tf, 0), (m.qv, m.qf, 1)):
        P = np.array([x[:3] for x in verts], float)
        fs = [list(g[1::2]) for g in faces]
        fs = [f for f in fs if all(P[i, 1] >= Y_FINGER for i in f)]
        # Union-Find ueber gemeinsame Ecken (gleiche Koordinate)
        key = {i: tuple(P[i].astype(int)) for f in fs for i in f}
        par = {}
        def fnd(a):
            while par.setdefault(a, a) != a: a = par[a]
            return a
        for f in fs:
            ks = [key[i] for i in f]
            for k in ks[1:]: par[fnd(k)] = fnd(ks[0])
        grp = {}
        for k in key.values(): grp.setdefault(fnd(k), set()).add(k)
        for g in grp.values():
            Q = np.array(sorted(g), float)
            comps.append(Q)
    # Komponenten aus Dreieck- und Viereckliste mit gemeinsamen Ecken zusammenlegen
    merged = []
    for Q in comps:
        s = set(map(tuple, Q.astype(int)))
        for M in merged:
            if s & M: M |= s; break
        else:
            merged.append(s)
    again = True
    while again:
        again = False
        for i in range(len(merged)):
            for j in range(i + 1, len(merged)):
                if merged[i] & merged[j]:
                    merged[i] |= merged.pop(j); again = True; break
            if again: break
    return [np.array(sorted(M), float) for M in merged]


def main():
    A_ = json.load(open(os.path.join(S, 'analyse.json')))
    pose = A_['1']['pose']; Hp = A_['1']['H_port']
    mo = b2.modell(G.EMD_V4)
    P12 = np.array([x[:3] for x in mo.meshes[12].tv] + [x[:3] for x in mo.meshes[12].qv], float)
    hist = np.histogram(P12[:, 1], bins=[0, 100, 200, 300, 330, 400, 500])
    K = krallen(mo)
    rows = []
    for Q in K:
        tip = Q[np.argmax(Q[:, 1])]; root = Q[np.argmin(Q[:, 1])]
        pp = welt(mo, pose, 12, [root, tip])
        rows.append(dict(ecken=len(Q), spitze_lok=tip.astype(int).tolist(), wurzel_lok=root.astype(int).tolist(),
                         laenge_lok=round(float(np.linalg.norm(tip - root)), 1), spitze_bild=G.r1(pp[1]), wurzel_bild=G.r1(pp[0]),
                         laenge_bild_px=round(float(np.linalg.norm(pp[1] - pp[0])), 1),
                         laenge_bild_H=round(float(np.linalg.norm(pp[1] - pp[0])) / Hp, 3)))
    hand = welt(mo, pose, 12, [[0, 0, 0], [0, 473, 0], [0, Y_FINGER, 0]])
    ids = np.load(os.path.join(S, 'ids_1.npy'))
    ys, xs = np.nonzero(ids == 12)
    modell = dict(krallen_zahl=len(K), krallen=rows, ecken_histogramm_y=dict(zip([str(int(b)) for b in hist[1][:-1]], hist[0].tolist())),
                  hand_gelenk_bild=G.r1(hand[0]), hand_spitze_bild=G.r1(hand[1]),
                  hand_laenge_px=round(float(np.linalg.norm(hand[1] - hand[0])), 1),
                  hand_laenge_H=round(float(np.linalg.norm(hand[1] - hand[0])) / Hp, 3),
                  fingeransatz_bild=G.r1(hand[2]),
                  sichtbar_px=int(len(xs)), sichtbar_box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None,
                  sichtbar_hoehe_H=round((ys.max() - ys.min() + 1) / Hp, 3) if len(xs) else None,
                  sichtbar_breite_H=round((xs.max() - xs.min() + 1) / Hp, 3) if len(xs) else None, H_port=Hp)
    # Foto
    foto = {}
    for n in (1476, 1479, 1485):
        im = G.vorlage(1) if n == 1485 else cv2.cvtColor(cv2.imread(V % n), cv2.COLOR_BGR2RGB).astype(float)
        Lab = G.lab(im)
        box = np.zeros(Lab.shape[:2], bool); box[325:372, 470:512] = True
        rot = box & (Lab[..., 1] >= 8)
        nn, lb, st, _ = cv2.connectedComponentsWithStats(rot.astype(np.uint8), 8)
        k = 1 + int(np.argmax(st[1:, 4])) if nn > 1 else 0
        rm = lb == k
        ys, xs = np.nonzero(rm)
        kb = np.zeros_like(box); kb[344:368, 476:500] = True
        hell = kb & (Lab[..., 0] >= 24) & (Lab[..., 1] < 8)
        zeilen = {}
        for y in range(344, 368):
            xx = np.nonzero(hell[y])[0]
            if len(xx):
                laeufe = np.split(xx, np.nonzero(np.diff(xx) > 1)[0] + 1)
                zeilen[y] = [[int(l[0]), int(l[-1])] for l in laeufe]
        yh = sorted(zeilen)
        foto[n] = dict(rot_box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None,
                       rot_px=int(rm.sum()), rot_hoehe_H=round((ys.max() - ys.min() + 1) / G.H_ORIG[1], 3) if len(xs) else None,
                       rot_breite_H=round((xs.max() - xs.min() + 1) / G.H_ORIG[1], 3) if len(xs) else None,
                       hell_zeilen=zeilen, hell_y=[yh[0], yh[-1]] if yh else None,
                       hand_unterkante=int(max([ys.max()] + yh)) if len(xs) else None,
                       laeufe_max=max((len(v) for v in zeilen.values()), default=0))
    # Krallenflaeche im Bild: Modell = Pixel von Knochen 12 mit Knochenraum-y >= Y_FINGER (f_rast), Videoraum-Lab;
    # Foto n1476 = helle Keilflaeche (L >= 24, a* < 8) als 8er-Komponente um (487,355), Fenster x 480-495, y 346-364
    import f_rast as FR
    A = FR.mit_bg(mo, pose)
    bo = A['bone'][:480]; lk = A['lok'][:480]
    km = (bo == 12) & (lk[..., 1] >= Y_FINGER)
    Lm = G.lab(G.videoraum(A['img'][:480]))
    ys, xs = np.nonzero(km)
    modell['krallen_bild'] = dict(px=int(km.sum()), box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None,
                                  lab=G.r1(Lm[km].mean(0)) if km.any() else None,
                                  hand_lab=G.r1(Lm[(bo == 12) & ~km].mean(0)))
    np.save(os.path.join(S, 'krallen_modell.npy'), km)
    im = cv2.cvtColor(cv2.imread(V % 1476), cv2.COLOR_BGR2RGB).astype(float); Lb = G.lab(im)
    w = np.zeros(Lb.shape[:2], bool); w[346:365, 480:496] = True
    hk = w & (Lb[..., 0] >= 24) & (Lb[..., 1] < 8)
    nn, lb, st, _ = cv2.connectedComponentsWithStats(hk.astype(np.uint8), 8)
    k = lb[355, 487]
    if k:
        mk = lb == k; ys, xs = np.nonzero(mk)
        foto['keil_1476'] = dict(px=int(mk.sum()), box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                                 breite_H=round((xs.max() - xs.min() + 1) / G.H_ORIG[1], 3), hoehe_H=round((ys.max() - ys.min() + 1) / G.H_ORIG[1], 3),
                                 lab=G.r1(Lb[mk].mean(0)), je_zeile={int(y): [int(xs[ys == y].min()), int(xs[ys == y].max())] for y in np.unique(ys)})
        np.save(os.path.join(S, 'krallen_foto.npy'), mk)
    out = dict(modell=modell, foto=foto)
    json.dump(out, open(os.path.join(S, 'krallen.json'), 'w'), indent=1)
    print(json.dumps(out)[:4000])


if __name__ == '__main__':
    main()
