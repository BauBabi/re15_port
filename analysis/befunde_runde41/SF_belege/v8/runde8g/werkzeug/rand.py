"""Messung im Spielbild (Szene 50A0, exe d8ef12963, 960x720, F317-F343 = h74-h100 Bogen, "wenn er sich eindreht"): woher kommen dunkle/braune
Randstreifen? Je Bildpunkt des Schwanzes:
  Klasse  aus dem Klassen-Markerlauf (marker.py klasse): Bn Bauch, Bf Bauch-Fuge, Fn Flanke/Ruecken, Ff Flanke/Ruecken-Fuge, N Nadel, W Wurzel
  Glied   aus dem Glied-Markerlauf (marker.py glied)
  Naht    Bildpunkt aendert sich im Naht-Markerlauf des Stands (Texel u 54-55 v 46-60 gruen) = er zeigt ein Naht-Texel
  Farbe   Lab des normalen Laufs des Stands
Schwanzmaske = Bildpunkte, in denen der Klassen-Markerlauf vom Lauf ohne Schwanz abweicht (Markerfarben sind voll gesaettigt).

    python rand.py <L-Ordner> <stand> [<stand> ...]     (Laeufe <L>/n_<stand>, <L>/naht_<stand>, <L>/s, <L>/mk_klasse, <L>/mk_glied)
"""
import sys, json, os
import numpy as np
from PIL import Image
from scipy import ndimage as ndi
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
sys.path[:0] = [T + '/v4']
import v4_gemein as G      # noqa: E402

F0, F1 = 317, 343
KL = ['Bn', 'Bf', 'Fn', 'Ff', 'N', 'W']
MUSTER6 = {(1, 0, 0): 0, (1, 0, 1): 1, (0, 1, 0): 2, (0, 1, 1): 3, (0, 0, 1): 4, (1, 1, 0): 5}
MUSTER7 = {(1, 0, 0): 16, (0, 1, 0): 17, (0, 0, 1): 18, (1, 1, 0): 19, (1, 0, 1): 20, (0, 1, 1): 21, (1, 1, 1): 22}


def bild(d, f):
    return np.asarray(Image.open(os.path.join(d, 'fd', 'f%06d.ppm' % f)).convert('RGB'), dtype=int)


def muster(img, tab, maske):
    ch = (img > 24).astype(int)                    # Kanal an / aus (Markerfarbe x Licht; aus = 0..24 aus Kanten-Mischung)
    out = np.full(img.shape[:2], -1, int)
    for k, v in tab.items():
        out[(ch[..., 0] == k[0]) & (ch[..., 1] == k[1]) & (ch[..., 2] == k[2]) & maske] = v
    return out


def main():
    L = sys.argv[1]
    staende = sys.argv[2:]
    erg = {s: dict(naht=[], kante_minus=[], kante_plus=[], bauch_mitte=[], flanke=[], rand_braun=[], rand=[]) for s in staende}
    unklar = 0; gesamt = 0
    tab = {s: {} for s in staende}
    for f in range(F0, F1 + 1):
        s_ = bild(os.path.join(L, 's'), f)
        mk = bild(os.path.join(L, 'mk_klasse'), f)
        mg = bild(os.path.join(L, 'mk_glied'), f)
        T_ = np.abs(mk - s_).sum(2) > 0
        kl = muster(mk, MUSTER6, T_)
        gl = muster(mg, MUSTER7, T_)
        gesamt += int(T_.sum()); unklar += int(((kl < 0) & T_).sum())
        bauch = (kl == 0) | (kl == 1)
        flanke = (kl == 2) | (kl == 3)
        # Abstand zur Grenze Bauch/Flanke (im Bild) und zum Schwanzumriss
        d_fl = ndi.distance_transform_edt(~flanke)     # Abstand zum naechsten Flanken-Bildpunkt
        d_rand = ndi.distance_transform_edt(T_)        # Abstand zum Umriss (innen)
        kante = bauch & (d_fl <= 2.0)
        mitte = bauch & (d_fl >= 5.0)
        rand = T_ & (d_rand <= 2.0)
        for s in staende:
            n = bild(os.path.join(L, 'n_' + s), f)
            nh = bild(os.path.join(L, 'naht_' + s), f)
            naht = T_ & (np.abs(nh - n).sum(2) > 0)
            lab = G.lab(n.astype(float))
            e = erg[s]
            e['naht'].append(int(naht.sum()))
            # Kante getrennt nach der Seite: die Naht liegt nur auf -theta_b (sq 0); Seite aus dem Naht-Lauf von 8c/8d nicht ableitbar -> beide
            # Kanten zusammen, dazu der Anteil der Naht-Bildpunkte an der Kante
            e['kante_minus'].append(int((kante & naht).sum()))
            e['kante_plus'].append(int(kante.sum()))
            e['bauch_mitte'].append(lab[mitte])
            e['flanke'].append(lab[flanke])
            e.setdefault('kante_lab', []).append(lab[kante])
            e.setdefault('naht_lab', []).append(lab[naht])
            e['rand'].append(int(rand.sum()))
            for g in range(16, 23):
                for k in (0, 1, 2, 3):
                    m = (gl == g) & (kl == k)
                    if m.any():
                        tab[s].setdefault((g, KL[k]), []).append(lab[m])
    print('Schwanz-Bildpunkte F%d-F%d: %d, davon ohne erkannte Klasse %d (%.2f %%)' % (F0, F1, gesamt, unklar, 100.0 * unklar / max(gesamt, 1)))
    aus = {}
    for s in staende:
        e = erg[s]
        mit = np.concatenate(e['bauch_mitte']); ka = np.concatenate(e['kante_lab']); fl = np.concatenate(e['flanke'])
        nl = np.concatenate(e['naht_lab']) if sum(e['naht']) else np.zeros((0, 3))
        L5 = float(np.percentile(mit[:, 0], 5))
        dunkel_kante = float((ka[:, 0] < L5).mean() * 100)

        def lch(x):
            if not len(x):
                return '-'
            C = np.hypot(x[:, 1], x[:, 2]); h = np.degrees(np.arctan2(x[:, 2], x[:, 1])) % 360
            return 'L* %.1f a* %.1f b* %.1f C* %.1f h %.0f' % (np.median(x[:, 0]), np.median(x[:, 1]), np.median(x[:, 2]), np.median(C), np.median(h))
        print('== %s' % s)
        print('  Naht-Bildpunkte (Texel u 54-55) je Bild: Summe %d, max %d, Bilder mit > 0: %d; davon an der Bauchkante (<= 2 px zur Flanke) %d; Farbe %s'
              % (sum(e['naht']), max(e['naht']), sum(1 for x in e['naht'] if x), sum(e['kante_minus']), lch(nl)))
        print('  Bauchmitte (>= 5 px von der Flanke) %s; 5. Perzentil L* %.1f' % (lch(mit), L5))
        print('  Bauchkante (<= 2 px zur Flanke) %s; Anteil dunkler als das 5. Perzentil der Mitte %.1f %%' % (lch(ka), dunkel_kante))
        print('  Flanke/Ruecken %s' % lch(fl))
        for g in range(16, 23):
            z = []
            for k in ('Fn', 'Ff', 'Bn', 'Bf'):
                x = tab[s].get((g, k))
                if x:
                    x = np.concatenate(x)
                    z.append('%s %d px %s' % (k, len(x), lch(x)))
            print('    Glied %d: %s' % (g, ' | '.join(z)))
        aus[s] = dict(naht=e['naht'], naht_kante=e['kante_minus'], kante=e['kante_plus'], dunkel_kante_prozent=round(dunkel_kante, 2),
                      L5_mitte=round(L5, 2), bauch_mitte=lch(mit), bauchkante=lch(ka), flanke=lch(fl), naht_farbe=lch(nl))
    json.dump(aus, open(os.path.join(L, 'rand.json'), 'w'), indent=1)


if __name__ == '__main__':
    main()
