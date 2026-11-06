"""Spur SF Schwanz V2: Pose (Clip, Bild, Gier, Lage) je Videobild an den KI-Freisteller anpassen (Koerper ohne Schwanz).

Die KI-Freisteller birkinNN sind pixelgenau auf dem Originalbild (SF_LESER_A/B Abschnitt 1, Formblatt V2 Punkt 1), ihre
Maske ist damit der Umriss der Figur im Original. Gesucht wird die Pose des Modells Referenz 2 (Koerper, Knochen 0-15), deren
Umriss in der Spielkamera ROOM50A0 Schnitt 12 am besten deckt (IoU). Die Lage folgt aus der Sohlenmitte der Maske
(Rueckprojektion auf den Boden) plus Feinsuche. Dieselbe Pose gilt dann fuer Ref2, V1, V2 (Modellvergleich bei gleicher Pose).

python anpassen_v2.py <emd> <aus.json> h,h,... seite|vorn [clips] [x_ab]
"""
import sys, os, json
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2
import birkin_buehne as BB


def ki_maske640(h):
    _rgb, m = b2.ki(h)
    return cv2.resize(m.astype(np.uint8), (640, 480), interpolation=cv2.INTER_AREA) > 0


def umriss(mo, pose):
    return b2.umriss_schnell(mo, pose, knochen=range(16))


def iou(a, b):
    u = (a | b).sum()
    return float((a & b).sum()) / u if u else 0.0


def sohle_lage(mo, pose, m_ki):
    """Lage so verschieben, dass die Sohlenmitte des Renders auf die Sohlenmitte der KI-Maske faellt (Boden y 0)."""
    ys, xs = np.nonzero(m_ki)
    yk = ys.max(); xk = xs[ys >= yk - 6].mean()
    m = umriss(mo, pose)
    ys2, xs2 = np.nonzero(m)
    if not len(ys2):
        return pose
    yr = ys2.max(); xr = xs2[ys2 >= yr - 6].mean()
    a = np.array(BB.boden_aus_video('50A0', 12, xk, yk), float)
    b = np.array(BB.boden_aus_video('50A0', 12, xr, yr), float)
    d = a - b
    return dict(pose, x=int(pose['x'] + d[0]), z=int(pose['z'] + d[2]))


def blick_gier(pose_xz, art):
    """Gier-Mitte fuer die Blickart: 'vorn' = Vorderseite (+X) zur Kamera, 'seite' = rechte Flanke (W) zur Kamera und
    Blick nach Bild-rechts (Formblatt V2 Abschnitt 0, Seitenpruefung: Original h83-h97 zeigt die rechte Flanke (W))."""
    r, kam, M, li = b2.raum()
    g0 = BB.gier_zur_kamera((pose_xz['x'], 0, pose_xz['z']), kam)
    if art == 'vorn':
        return g0
    for g in ((g0 + 1024) & 0xFFF, (g0 - 1024) & 0xFFF):
        p = dict(clip=0, bild=0, gier=g, x=pose_xz['x'], z=pose_xz['z'])
        a, b = b2.projiziere(p, [[0, -1500, 0], [1000, -1500, 0]])
        if b[0] > a[0]:
            return g
    raise RuntimeError('keine Seitengier')


def maske_ab(m, x_ab):
    if x_ab:
        m = m.copy(); m[:, :x_ab] = False
    return m


def suche(mo, h, clips, art, x_ab=0, fenster=448):
    m_ki = maske_ab(ki_maske640(h), x_ab)
    best = (-1, None)
    base = dict(x=-8488, z=-21651)
    gm = blick_gier(base, art)
    for clip in clips:
        n = mo.clips[clip][1]
        for bild in range(0, n, 3):
            for dg in range(-fenster, fenster + 1, 64):
                p = dict(clip=clip, bild=bild, gier=(gm + dg) & 0xFFF, x=base['x'], z=base['z'])
                p = sohle_lage(mo, p, m_ki)
                s = iou(maske_ab(umriss(mo, p), x_ab), m_ki)
                if s > best[0]:
                    best = (s, p)
    s, p = best
    for schritt in (32, 16, 8):
        besser = True
        while besser:
            besser = False
            for k, d in (('gier', schritt), ('gier', -schritt), ('bild', 1), ('bild', -1),
                         ('x', 4 * schritt), ('x', -4 * schritt), ('z', 4 * schritt), ('z', -4 * schritt)):
                q = dict(p); q[k] = q[k] + d
                if k == 'bild':
                    q['bild'] %= mo.clips[q['clip']][1]
                if k == 'gier':
                    q['gier'] %= 4096
                    dgm = ((q['gier'] - gm + 2048) & 0xFFF) - 2048
                    if abs(dgm) > fenster:
                        continue
                    q = sohle_lage(mo, q, m_ki)
                t = iou(maske_ab(umriss(mo, q), x_ab), m_ki)
                if t > s + 1e-4:
                    s, p, besser = t, q, True
    return s, p


def main():
    emd, aus, hs = sys.argv[1], sys.argv[2], [int(x) for x in sys.argv[3].split(',')]
    art = sys.argv[4]                       # 'seite' oder 'vorn'
    clips = [int(x, 0) for x in sys.argv[5].split(',')] if len(sys.argv) > 5 else [0, 1]
    x_ab = int(sys.argv[6]) if len(sys.argv) > 6 else 0     # Seite: Spalten links davon (Schwanzbogen der KI) ausblenden
    fenster = int(sys.argv[7]) if len(sys.argv) > 7 else 448   # Gier-Fenster um die Blickart (Q12)
    mo = b2.modell(emd)
    R = json.load(open(aus)) if os.path.exists(aus) else {}
    for h in hs:
        s, p = suche(mo, h, clips, art, x_ab, fenster)
        p['art'], p['x_ab'], p['fenster'] = art, x_ab, fenster
        p['iou'] = round(s, 4)
        R[str(h)] = p
        print('h%d' % h, p, flush=True)
        json.dump(R, open(aus, 'w'), indent=1)


if __name__ == '__main__':
    main()
