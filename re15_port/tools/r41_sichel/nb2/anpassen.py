"""Runde 41 SF NB2: Haltungs-Parameter (birkin_formen.ducken) gegen die Video-Umrisse bewerten / suchen.

    python re15_port/tools/r41_sichel/nb2/anpassen.py <basis.emd> <debug.log> bewerten '{"becken":..}'
    python re15_port/tools/r41_sichel/nb2/anpassen.py <basis.emd> <debug.log> suchen

Mass: je Videobild (SF_belege/nb2/umrisse_video.json) IoU des Port-Umrisses (schnell.py, Knochen 0..15 ohne Schwanz)
mit Pose/Gier aus der Messzeile; Lage = Szene (Messzeile) und zusaetzlich beste Verschiebung +-40 Bildpunkte (Form allein).
Basis = EM30_HANGAR vor NB2; ducken() wird auf deren EMR-Bank 0 angewendet (Knochen 0-15). GESTALTUNG, keine RE-Werte.
"""
import json, os, sys
import numpy as np
from scipy.signal import fftconvolve
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(HIER, '..', '..'))
import schnell as SN, portmaske as PM, messung as MS   # noqa: E402
import birkin_formen as BF                              # noqa: E402
import emd_ansichtsblatt as EA                          # noqa: E402

NS = [1350, 1390, 1414, 1450, 1466, 1474]


def mit_haltung(blob, p):
    d = EA.emd_dir(blob)
    emr = BF.ducken(blob[d[2]:d[3]], p)
    return blob[:d[2]] + emr + blob[d[3]:]


def iou_verschoben(v, m, r=40):
    I = fftconvolve(v.astype(np.float32), m[::-1, ::-1].astype(np.float32), mode='same')
    cy, cx = v.shape[0] // 2, v.shape[1] // 2
    I = I[cy - r:cy + r + 1, cx - r:cx + r + 1]
    U = v.sum() + m.sum() - I
    J = I / np.maximum(U, 1)
    k = np.unravel_index(np.argmax(J), J.shape)
    return float(J[k]), int(k[1] - r), int(k[0] - r)


_V = {}


def bewerten(blob, P, ns=NS, knochen=range(16)):
    sc = SN.Schablone(blob, set(knochen))
    out = []
    for n in ns:
        if n not in _V:
            v = MS.video_maske(n); v[:MS.FENSTER.get(n, 0)] = False; _V[n] = v
        v = _V[n]
        c, b, g, x, z = P[n - 1350]
        m = sc.maske(c, b, g, x, z); m[:MS.FENSTER.get(n, 0)] = False
        i0 = MS.iou(v, m)
        iv, dx, dy = iou_verschoben(v, m)
        hv, bv = MS.kasten(v); hp, bp = MS.kasten(m)
        out.append(dict(n=n, iou=i0, iouv=iv, dx=dx, dy=dy, v=(hv, bv), p=(hp, bp)))
    return out


def zeigen(r, titel=''):
    print(titel, 'IoU %.3f  verschoben %.3f  | ' % (np.mean([e['iou'] for e in r]), np.mean([e['iouv'] for e in r])) +
          ' '.join('h%d:%.2f/%.2f(%d,%d) H/B %.2f|%.2f' % (e['n'] - 1350, e['iou'], e['iouv'], e['dx'], e['dy'],
                                                          e['v'][0] / e['v'][1], e['p'][0] / e['p'][1]) for e in r))


if __name__ == '__main__':
    blob = open(sys.argv[1], 'rb').read(); P = PM.posen(sys.argv[2])
    if sys.argv[3] == 'bewerten':
        p = json.loads(sys.argv[4]) if len(sys.argv) > 4 else None
        zeigen(bewerten(mit_haltung(blob, p) if p else blob, P), json.dumps(p))


# ── Anpassung mit Lage aus dem Video (Fusspunkt per boden_aus_video) und freien Stoergroessen je Bild ──────────────
# Fusspunkte (Mitte der Sohlen, 640x480) von Hand abgelesen (Lupe/spur.py), Weltpunkt = Rueckprojektion auf y = 0.
FUSS_VIDEO = {1350: (387.5, 428), 1390: (369, 414), 1414: (360, 417), 1450: (388, 425), 1466: (394, 427), 1474: (402, 440)}


def lage_video(n):
    import birkin_buehne as BB
    x, _y, z = BB.boden_aus_video('50A0', 12, *FUSS_VIDEO[n])
    return x, z


def stoer_suchen(blob, P, ns=NS, gier_spanne=(-240, -120, 0, 120, 240), schritt=2, r=16):
    """Je Bild: Clip-Bild (gleicher Clip wie die Szene) und Gier frei, Lage aus dem Video, Verschiebung +-r."""
    sc = SN.Schablone(blob, set(range(16)))
    aus = {}
    for n in ns:
        v = MS.video_maske(n); v[:MS.FENSTER.get(n, 0)] = False
        c, _b, g, _x, _z = P[n - 1350]
        x, z = lage_video(n)
        nf = sc.mo.clips[c][1]
        bereich = range(10, 38, schritt) if c == 11 else range(0, nf, schritt)   # Halten: Szene nutzt B10..37
        best = None
        for b in bereich:
            for dg in gier_spanne:
                m = sc.maske(c, b, (g + dg) & 0xfff, x, z); m[:MS.FENSTER.get(n, 0)] = False
                i, dx, dy = iou_verschoben(v, m, r)
                if best is None or i > best[0]:
                    best = (i, b, (g + dg) & 0xfff, dx, dy)
        aus[n] = dict(clip=c, bild=best[1], gier=best[2], x=x, z=z, iou=best[0], dx=best[3], dy=best[4])
    return aus


def bewerten_fest(blob, stoer, ns=NS, r=16, knochen=range(16)):
    sc = SN.Schablone(blob, set(knochen))
    out = []
    for n in ns:
        v = MS.video_maske(n); v[:MS.FENSTER.get(n, 0)] = False
        s = stoer[n]
        m = sc.maske(s['clip'], s['bild'], s['gier'], s['x'], s['z']); m[:MS.FENSTER.get(n, 0)] = False
        iv, dx, dy = iou_verschoben(v, m, r)
        hv, bv = MS.kasten(v); hp, bp = MS.kasten(m)
        out.append(dict(n=n, iou=MS.iou(v, m), iouv=iv, dx=dx, dy=dy, v=(hv, bv), p=(hp, bp)))
    return out


def suchen(blob, stoer, start, schritte, runden=2, ns=NS):
    """Koordinatensuche ueber die ducken-Parameter bei festen Stoergroessen (Mittel der IoU nach Verschiebung)."""
    p = dict(start)
    def wert(q):
        r = bewerten_fest(mit_haltung(blob, q), stoer, ns)
        return float(np.mean([e['iouv'] for e in r])), r
    best, _ = wert(p)
    for _ in range(runden):
        for k, st in schritte.items():
            for richt in (1, -1):
                while True:
                    q = dict(p); q[k] = p[k] + richt * st
                    w, _r = wert(q)
                    if w > best + 1e-4:
                        best, p = w, q
                    else:
                        break
        schritte = {k: v / 2 for k, v in schritte.items()}
    return p, best


def form_variante(aend):
    """FORMEN['hangar'] mit Aenderungen bauen (birkin_formen.baue, wie der Generator). aend: dict mit
    'ducken' (Teilwerte), 'skal' {b: (sx,sy,sz)}, 'rel' {b: (dx,dy,dz)}."""
    import copy
    f = copy.deepcopy(BF.FORMEN['hangar'])
    for k, v in aend.items():
        if k == 'ducken':
            f['ducken'].update(v)
        elif k in ('skal', 'rel'):
            f[k].update(v)
        else:
            f[k] = v
    return BF.baue(BF.lade_em30(), f)
