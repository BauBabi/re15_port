"""SF V8 Runde 5 (P5): Luecke zwischen Ruecken und Kopf - Mass, Riegel S16 und Ansichten.

    python luecke_r5.py [EMD ...] [--ansichten alle|kurz] [--varianten 7|1] [--aus JSON] [--jobs N]

Nutzer 2026-11-09 (woertlich): "Das einzige was mir noch auffällt, es gibt immer noch eine Lücke zwischen Rücken und Kopf beim 2 beiner
Birkin. Das ist in der Vorlage so wie es aussieht nicht so." Frueher: "Auch der obere Rücken geht im original screenshot direkt mehr oder
weniger in den Kopf über." und Darstellung.png (weiss) "smooth straight to head without further hills....".
Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Jede Zahl ist PORT-WAHL mit Messung.

Warum der fruehere Schlitz-Riegel (S9b/S9s, r4_gemein.schlitz) die Luecke nicht fasste (gemessen sfv8/r5p5, luecke2.py / schlitz_alle.py):
  1. Er zaehlt nur Loecher DURCH die Silhouette (Bildpunkte, die ohne Rueckseiten-Culling belegt sind und mit nicht). Die Luecke ist eine
     Kerbe AM RAND der Silhouette zwischen zwei Teilen (Mesh 8 Buckel und Mesh 9 Kopf): dort ist auch ohne Culling nichts, also entsteht
     kein Schlitz-Bildpunkt.
  2. Er laeuft nur in den Ansichten von vorn (P1, Szene Takt F336-F372). Die Kerbe liegt in Foto 2 / PV (Kamera seitlich) und in der Szene
     dort, wo der Kopf sich gegen den Rumpf hebt (Clip 11 Bild 29, Takt F310: Kopf gegen Knochen 8 bis 30 Grad gedreht).
Mass (je Ansicht, Maler-Reihenfolge und Rueckseiten-Culling wie im Port, f_rast.zeichne ohne Licht):
  S = Silhouette (alle Knochen), H = Kopf (Knochen 9), R = Ruecken = Bildpunkte von Knochen 8, deren Ort im Knochenraum 8 oberhalb des
  Kopfgelenks liegt (y < Y_RUECKEN; Kopfgelenk relpos Knochen 9 = (111, -971, -133), y < 0 = oben). So zaehlt der Buckel und die
  Schulterkuppe in Kopfhoehe, nicht Kehle/Brust unter dem Kinn (dort grenzt in PV/Foto 2 die (W)-linke Schulter an das Gesicht,
  Knochenraum 8 y -666..-695, z +1021..+1036, gemessen luecke2.py - kein "Ruecken").
  (a) Kerbe: Huelle = konvexe Huelle von H und den Bildpunkten von Knochen 8 im Umkreis NAH px um H; Kerben-Bildpunkte = Huelle ohne S, je
      zusammenhaengendes Stueck, das an H UND an R grenzt (8er-Nachbarschaft).
  (b) Spalt: Bildpunkte, die das Schliessen von S (Quadrat SPALT px) fuellt und die nicht in S liegen, je Stueck, das an R grenzt und
      hoechstens NAH px von H entfernt ist (Risse durch den Buckel neben dem Kopf).
  Luecke = (a) + (b) in Bildpunkten. Riegel S16: in allen Ansichten und allen 7 Halbpixel-Varianten (riegel_v8.S2C_VARIANTEN) = 0.
Ansichten: PV, P1 (Foto 1), P2 (Foto 2), Szene alle 144 Takte F235-F378 (die Birkin-Posen der Szene F200-F1100, r3_gemein.szene_posen),
  rundum: Pose PV und P2 mit Gier + k x 256 (k 1..15, 22,5-Grad-Schritte, gleiche Kamera).
"""
import os, sys, json, argparse
import numpy as np
import cv2
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import r3_gemein as R3                # noqa: E402
import f_rast as FR                   # noqa: E402
import birkin_buehne as BB            # noqa: E402

Y_RUECKEN = -971       # Knochenraum 8: Kopfgelenk y (EMR relpos Knochen 9 = (111, -971, -133)); Ruecken = oberhalb
NAH = 12               # px: Umkreis um den Kopf (184 E. am Koerper in PV bei 65,4 px je 1000 E., Messblatt V8 Kopf; halbe Kopfbreite in
                       # PV 15 px, gemessen sfv8/r5p5 sicht.py)
SPALT = 5              # px: Schliessen mit 5 x 5 fuellt Risse bis 4 px Breite
VARIANTEN = ((0.375, 0, 0), (0.375, 0.5, 0), (0.375, -0.5, 0), (0.375, 0, 0.5), (0.375, 0, -0.5), (0.0, 0, 0), (0.75, 0, 0))  # = S2C_VARIANTEN
K3 = np.ones((3, 3), np.uint8)


def ansichten(art='alle'):
    sp = R3.szene_posen()
    out = [('PV', V.PV), ('P1', R3.P1), ('P2', R3.P2)]
    if art == 'kurz':
        return out + [('F%d' % k, sp[k]) for k in (262, 290, 310, 312, 328)]
    out += [('F%d' % k, sp[k]) for k in sorted(sp)]
    for nm, p in (('PV', V.PV), ('P2', R3.P2)):
        for k in range(1, 16):
            q = dict(p)
            q['gier'] = (int(p['gier']) + 256 * k) & 0xFFF
            out.append(('%s+%d' % (nm, k * 256), q))
    return out


def zeichne(mo, pose, var=None):
    if var is None:
        return FR.zeichne(mo, pose, None, licht=False)
    off, dx, dy = var
    orig = BB.abbilden

    def ab(x, y, M):
        X, Y_ = orig(np.asarray(x) - BB.PHASE + off, np.asarray(y) - BB.PHASE + off, M)
        return X + dx, Y_ + dy
    BB.abbilden = ab
    try:
        return FR.zeichne(mo, pose, None, licht=False)
    finally:
        BB.abbilden = orig


def ruecken_maske(A):
    """Bildpunkte von Knochen 8 mit Ort (Knochenraum 8, A['lok']) y < Y_RUECKEN."""
    b = A['bone'][:480]
    m8 = b == 8
    out = np.zeros_like(m8)
    out[m8] = A['lok'][:480][m8][:, 1] < Y_RUECKEN
    return out


def luecke(A):
    """-> (dict(kerbe, spalt, stuecke), Maske der Luecken-Bildpunkte)."""
    return luecke_aus(A['bone'][:480], ruecken_maske(A))


def luecke_aus(b, R):
    """Kern des Masses: b = Knochen je Bildpunkt (-1 = Hintergrund), R = Ruecken-Bildpunkte (Knochen 8, y < Y_RUECKEN)."""
    H = (b == 9).astype(np.uint8)
    S = b >= 0
    if not H.any():
        return dict(kerbe=0, spalt=0, stuecke=[]), np.zeros_like(S)
    Hn = cv2.dilate(H, np.ones((2 * NAH + 1, 2 * NAH + 1), np.uint8)) > 0
    Hd = cv2.dilate(H, K3) > 0
    Bn = (b == 8) & Hn
    pts = np.argwhere(H.astype(bool) | Bn)[:, ::-1].astype(np.int32)
    hull = np.zeros(S.shape, np.uint8)
    cv2.fillConvexPoly(hull, cv2.convexHull(pts), 1)
    lab, n = ndimage.label((hull > 0) & ~S, np.ones((3, 3)))
    G = np.zeros_like(S)
    stuecke = []
    for i in range(1, n + 1):
        c = lab == i
        cd = cv2.dilate(c.astype(np.uint8), K3) > 0
        if (cd & Hd).any() and (cd & R).any():
            G |= c
            ys, xs = np.nonzero(c)
            stuecke.append(('kerbe', int(c.sum()), int(xs.mean()), int(ys.mean())))
    kerbe = int(G.sum())
    zu = cv2.morphologyEx(S.astype(np.uint8), cv2.MORPH_CLOSE, np.ones((SPALT, SPALT), np.uint8)) > 0
    lab, n = ndimage.label(zu & ~S & ~G, np.ones((3, 3)))
    spalt = 0
    for i in range(1, n + 1):
        c = lab == i
        cd = cv2.dilate(c.astype(np.uint8), K3) > 0
        if (cd & R).any() and (c & Hn).any():
            G |= c
            spalt += int(c.sum())
            ys, xs = np.nonzero(c)
            stuecke.append(('spalt', int(c.sum()), int(xs.mean()), int(ys.mean())))
    return dict(kerbe=kerbe, spalt=spalt, stuecke=stuecke), G


DYN = (8, 16)            # Knochen, deren Mesh die Suche aendert (Mesh 8 Stufe r, Mesh 16 Ring 1 Stufe w); der Rest wird je Ansicht gemerkt


GESICHT_X = 150          # Mesh 9 lok x >= 150 = vordere Haelfte des Kopfes (Mesh 9 x -156..353; Gesicht vorn, sfv8/r5p5 gesicht.py)


def _vorbereiten(tris):
    """Dreiecke (f_rast.dreiecke) -> [(Tiefe, Ecken in 1/16 px, Knochen, Code)]; Code 1 = Ruecken (Knochen 8, Schwerpunkt y < Y_RUECKEN),
    2 = Gesicht (Knochen 9, Schwerpunkt x >= GESICHT_X)."""
    r, kam, M, li = V.b2.raum()
    if not tris:
        return []
    P = np.array([[q[0] + BB.PHASE, q[1] + BB.PHASE] for t in tris for q in t[1]], float)
    x, y = BB.abbilden(P[:, 0], P[:, 1], M)
    pts = np.round(np.stack([x - 0.5, y - 0.5], 1) * 16).astype(np.int32).reshape(-1, 3, 2)
    out = []
    for i, t in enumerate(tris):
        bb, PL = t[5], t[7]
        code = 1 if (bb == 8 and float(np.mean(PL[:, 1])) < Y_RUECKEN) else (2 if (bb == 9 and float(np.mean(PL[:, 0])) >= GESICHT_X) else 0)
        out.append((t[0], pts[i], int(bb), code))
    return out


def statisch(mo, pose):
    return _vorbereiten(FR.dreiecke(mo, pose, False, knochen=[k for k in range(mo.skel.bone_count) if k not in DYN]))


def tief(b, G, ab=1.5):
    """Luecken-Bildpunkte mit Abstand >= ab px zum freien Hintergrund (ohne Silhouette und ohne Luecke): eine 1 px schmale Sichel an der
    Huellkante zaehlt nicht, eine Kerbe/ein Spalt, in den man hineinsieht, schon (Suche ruecken_r5: Gewicht W_TIEF)."""
    if not G.any():
        return 0
    frei = (b < 0) & ~G
    d = ndimage.distance_transform_edt(~frei)
    return int((d[G] >= ab).sum())


def schnell(mo, pose, fest=None, gesicht=False):
    """Nur fuer die Suche (ruecken_r5.py): Knochen-Bild mit Maler-Reihenfolge und Culling wie f_rast, aber ganze Dreiecke per
    cv2.fillConvexPoly (1/16 px) und 'oberhalb des Kopfgelenks' / 'Gesicht' je Dreieck (Schwerpunkt) statt je Bildpunkt; ohne
    Texel-Transparenz. fest = statisch(mo0, pose) eines Modells mit denselben Knochen und Meshes ausser DYN (spart die unveraenderten
    Dreiecke). Der Riegel S16 misst mit luecke() auf dem vollen Raster (gemessen sfv8/r5p5 vgl_schnell.py: Korrelation 0,98 ueber 177
    Ansichten). -> (Knochen-Bild, Ruecken-Maske[, Gesichts-Bildpunkte])"""
    b = np.full((480, 640), -1, np.int16)
    C = np.zeros((480, 640), np.uint8)
    tris = (fest + _vorbereiten(FR.dreiecke(mo, pose, False, knochen=list(DYN)))) if fest is not None else         _vorbereiten(FR.dreiecke(mo, pose, False))
    for _t, pts, bb, code in sorted(tris, key=lambda t: -t[0]):
        cv2.fillConvexPoly(b, pts, bb, lineType=cv2.LINE_8, shift=4)
        cv2.fillConvexPoly(C, pts, code, lineType=cv2.LINE_8, shift=4)
    if gesicht:
        return b, C == 1, int((C == 2).sum())
    return b, C == 1


def messen(mo, liste, varianten=VARIANTEN):
    out = []
    for nm, pose in liste:
        w, st = [], []
        for var in varianten:
            r, _G = luecke(zeichne(mo, pose, var))
            w.append(r['kerbe'] + r['spalt'])
            if not st and r['stuecke']:
                st = r['stuecke']
        out.append(dict(ansicht=nm, max=max(w), varianten=w, stuecke=st))
    return out


def _arbeit(args):
    pfad, art, nv, i0, i1 = args
    mo = V.modell(open(pfad, 'rb').read())
    return messen(mo, ansichten(art)[i0:i1], VARIANTEN[:nv])


def messen_parallel(pfad, art='alle', nv=7, jobs=8):
    from multiprocessing import Pool
    n = len(ansichten(art))
    k = (n + jobs - 1) // jobs
    teile = [(pfad, art, nv, i, min(i + k, n)) for i in range(0, n, k)]
    with Pool(len(teile)) as p:
        res = p.map(_arbeit, teile)
    return [r for t in res for r in t]


def zusammen(res):
    return dict(summe=int(sum(r['max'] for r in res)), ansichten_mit=[(r['ansicht'], r['max']) for r in res if r['max']],
                ok=all(r['max'] == 0 for r in res))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd', nargs='*', default=[V.ASSET])
    ap.add_argument('--ansichten', default='alle')
    ap.add_argument('--varianten', type=int, default=7)
    ap.add_argument('--jobs', type=int, default=8)
    ap.add_argument('--aus')
    a = ap.parse_args()
    alles = {}
    for p in a.emd:
        res = messen_parallel(p, a.ansichten, a.varianten, a.jobs)
        z = zusammen(res)
        alles[p] = dict(sha256=V.sha(open(p, 'rb').read()), **z, je_ansicht=res)
        print('%s sha %s: Luecke Summe %d in %d Ansichten -> %s' % (os.path.basename(p), alles[p]['sha256'][:8], z['summe'],
              len(z['ansichten_mit']), 'GRUEN' if z['ok'] else 'ROT'))
        print('   ', z['ansichten_mit'])
    if a.aus:
        json.dump(alles, open(a.aus, 'w'), indent=1)


if __name__ == '__main__':
    main()
