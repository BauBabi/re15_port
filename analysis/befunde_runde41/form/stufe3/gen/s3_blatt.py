"""Stufe 3: Kontaktblaetter je Clip aus einem fertigen EMD (Offline-Buehne form_buehne, Spielart: Textur, Gouraud ohne Raumlicht).

    python s3_blatt.py EMD TIM --clip N [--schritt K] [--ansichten 0,1024,3072] --aus blatt.png
    python s3_blatt.py EMD TIM --alle --aus-dir DIR

Jede Zeile = eine Ansicht, jede Spalte = ein Bild des Clips (jedes K-te, letztes immer). Die Wurzelspur wird mitgefuehrt
(Aktor laeuft um die Spur weiter, wie bb_schieben), damit Gleiten der Fuesse sichtbar wird; ein Bodenstrich bei y = 0.
"""
import os, sys, argparse, math
import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import form_emd as FE       # noqa: E402
import form_buehne as FB    # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402

LICHT_AUS = None


def boden_linie(bild, kam, lage, gier, laenge=6000):
    """Bodenraster bei y = 0 um die Lage, im Port-Bild abgebildet (nur Linien, Hilfe fuer das Auge)."""
    d = ImageDraw.Draw(bild)
    pts = []
    bx = int(round(lage[0] / 1000.0)) * 1000
    for x0 in range(-laenge, laenge + 1, 1000):
        for (a, b) in (((x0 + bx, 0, -laenge), (x0 + bx, 0, laenge)), ((-laenge + bx, 0, x0), (laenge + bx, 0, x0))):
            q = []
            for p in (a, b):
                v = (kam.V @ np.array([p[0], 0, p[2]], np.int64)) >> 12
                v = v + kam.t
                if v[2] < 200:
                    q = None
                    break
                sx, sy = FB._gte(np.array([v[0]]), np.array([v[1]]), np.array([v[2]]), kam.h)
                x, y = FB.abbilden(float(sx[0]) + 0.375, float(sy[0]) + 0.375, kam.abbildung)
                q.append((x, y))
            if q:
                d.line(q, fill=(70, 74, 80), width=1)


def render(mo, tex, kf, lage, gier, kam, breite=640, hoehe=480):
    img = Image.new('RGB', (breite, hoehe), (40, 42, 46))
    boden_linie(img, kam, lage, gier)
    ziel = np.array(img)
    m = FB.rastern(FB.dreiecke(mo, kf, lage, gier, kam, LICHT_AUS), tex, ziel, kam.abbildung)
    return ziel, m


def clip_blatt(emd, tim, clip, ansichten, schritt, aus, titel='', mitlaufen=True, max_spalten=12, ab_bild=0, bis_bild=None):
    mo = FE.modell(emd, tim)
    tex = FB.Textur(tim)
    f0, n = mo.clips[clip]
    bis = n if bis_bild is None else min(n, bis_bild)
    bilder = list(range(ab_bild, bis, schritt))
    if bilder[-1] != bis - 1:
        bilder.append(bis - 1)
    while len(bilder) > max_spalten:
        bilder = bilder[::2] + ([bis - 1] if bilder[::2][-1] != bis - 1 else [])
    # Spur kumuliert -> Aktorlage (gier 0: Vorderseite +X)
    spur = []
    for i in range(n):
        kf = mo.frames[f0 + i] & 0xFFF
        o = kf * mo.skel.kf_size
        spur.append((EA.s16(mo.skel.kf_data, o + 6), EA.s16(mo.skel.kf_data, o + 10)))
    lage_x = [0.0] * n
    acc = 0.0
    for i in range(1, n):
        acc += spur[i][0] - spur[i - 1][0]
        lage_x[i] = acc
    mitte = (max(lage_x) + min(lage_x)) / 2 if mitlaufen else 0
    tafeln = []
    for a in ansichten:
        zeile = []
        for i in bilder:
            kam = S.kamera_seite(a, ziel_x=int(lage_x[i]) if mitlaufen else 0)
            kf = mo.frames[f0 + i] & 0xFFF
            lage = (int(lage_x[i]) if mitlaufen else 0, 0, 0)
            r, m = render(mo, tex, kf, lage, 0, kam)
            wort = mo.frames[f0 + i]
            mark = ('T' if wort & FE.BILD_TREFFER else '') + ('S%d' % (wort >> 28) if wort & FE.BILD_TON else '')
            zeile.append(('Bild %d %s' % (i, mark), r, m))
        tafeln.append(zeile)
    # gemeinsamer Ausschnitt ueber alle Tafeln
    m = np.zeros(tafeln[0][0][2].shape, bool)
    for z in tafeln:
        for _t, _a, mm in z:
            m |= mm
    ys, xs = np.nonzero(m)
    if len(xs):
        y0, y1 = max(ys.min() - 24, 0), min(ys.max() + 25, m.shape[0])
        x0, x1 = max(xs.min() - 24, 0), min(xs.max() + 25, m.shape[1])
    else:
        y0, y1, x0, x1 = 0, m.shape[0], 0, m.shape[1]
    w, h = x1 - x0, y1 - y0
    pad, zl = 6, 18
    spalten = len(bilder)
    B = Image.new('RGB', (pad + spalten * (w + pad), 34 + len(tafeln) * (h + zl + pad)), (18, 19, 22))
    d = ImageDraw.Draw(B)
    d.text((pad, 8), titel or ('Clip %d, %d Bilder' % (clip, n)), font=EA.font(16), fill=(200, 203, 210))
    for zi, z in enumerate(tafeln):
        for si, (t, a, _m) in enumerate(z):
            x, y = pad + si * (w + pad), 34 + zi * (h + zl + pad)
            B.paste(Image.fromarray(a[y0:y1, x0:x1]), (x, y))
            d.text((x + 2, y + h + 1), t, font=EA.font(12), fill=(214, 138, 74))
    os.makedirs(os.path.dirname(os.path.abspath(aus)), exist_ok=True)
    B.save(aus)
    return aus


def main():
    a = argparse.ArgumentParser()
    a.add_argument('emd')
    a.add_argument('tim')
    a.add_argument('--clip', type=int)
    a.add_argument('--schritt', type=int, default=0)
    a.add_argument('--ansichten', default='0,1024,3072')
    a.add_argument('--aus')
    a.add_argument('--titel', default='')
    a.add_argument('--stand', action='store_true', help='Aktor bleibt stehen (Spur nicht mitfuehren)')
    a.add_argument('--ab', type=int, default=0)
    a.add_argument('--bis', type=int, default=None)
    g = a.parse_args()
    emd, tim = open(g.emd, 'rb').read(), open(g.tim, 'rb').read()
    mo = FE.modell(emd, tim)
    n = mo.clips[g.clip][1]
    schritt = g.schritt or max(1, math.ceil(n / 12))
    print(clip_blatt(emd, tim, g.clip, [int(x) for x in g.ansichten.split(',')], schritt, g.aus, g.titel,
                     mitlaufen=not g.stand, ab_bild=g.ab, bis_bild=g.bis))


if __name__ == '__main__':
    main()
