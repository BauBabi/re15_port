"""Stufe 3: Uebersichtsblatt, je Clip ein bezeichnendes Bild (schraeg vorn links), dazu die zwei Screenshot-Posen.

    python s3_uebersicht.py      -> stufe3/kontakt/00_uebersicht.png
"""
import os, sys
import numpy as np
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import s3_blatt as B        # noqa: E402
import form_emd as FE       # noqa: E402
import form_buehne as FB    # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402

ASSET = os.path.join(HIER, '..', 'asset')
WAHL = [(2, 0, 'Stehen (Ruhe J)'), (5, 30, 'Gehen'), (0, 4, 'Rennen'), (0, 11, 'Rennen Bild 11 = Pose Bild 1'),
        (10, 30, 'Drohen: Gebruell'), (10, 48, 'Drohen Bild 48 = Pose Bild 2'), (10, 84, 'Drohen: Aufbaeumen'),
        (3, 16, 'Hieb rechts: Ausholen'), (3, 29, 'Hieb rechts: Treffer'), (4, 16, 'Hieb links: Treffer'),
        (9, 17, 'Nachhieb: Arme hoch'), (9, 36, 'Nachhieb: Bruellen'), (26, 14, 'Biss: Maul auf'), (26, 21, 'Biss: Schuetteln'),
        (27, 8, 'Griff: packt (Bild 8)'), (27, 16, 'Griff: Schleudern'), (8, 9, 'Zucken'), (11, 6, 'Flucht: Gebruell'),
        (11, 30, 'Flucht: Galopp'), (11, 46, 'Flucht: Sprung'), (12, 0, 'Sturz Bild 0 = Pose Bild 2'), (12, 26, 'Sturz: kippt'),
        (12, 44, 'Sturz: faellt')]


def main():
    emd = open(os.path.join(ASSET, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim = open(os.path.join(ASSET, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    mo = FE.modell(emd, tim)
    tex = FB.Textur(tim)
    kam = S.kamera_seite(1536, abstand=12000, ziel_y=-1300)
    bilder = []
    for c, f, t in WAHL:
        f0, n = mo.clips[c]
        kf = mo.frames[f0 + min(f, n - 1)] & 0xFFF
        r, m = B.render(mo, tex, kf, (0, 0, 0), 0, kam)
        bilder.append(('Clip %d Bild %d: %s' % (c, f, t), r, m))
    m = np.zeros(bilder[0][2].shape, bool)
    for _t, _a, mm in bilder:
        m |= mm
    ys, xs = np.nonzero(m)
    y0, y1 = max(ys.min() - 20, 0), min(ys.max() + 21, m.shape[0])
    x0, x1 = max(xs.min() - 20, 0), min(xs.max() + 21, m.shape[1])
    w, h = x1 - x0, y1 - y0
    sp = 6
    zeilen = (len(bilder) + sp - 1) // sp
    out = Image.new('RGB', (8 + sp * (w + 8), 36 + zeilen * (h + 24)), (18, 19, 22))
    d = ImageDraw.Draw(out)
    d.text((8, 8), 'Bahnsteig-Form J, Stufe 3: bezeichnende Bilder je Clip (Offline-Buehne, schraeg vorn links, Textur ohne Raumlicht)',
           font=EA.font(16), fill=(200, 203, 210))
    for i, (t, a, _m) in enumerate(bilder):
        x, y = 8 + (i % sp) * (w + 8), 36 + (i // sp) * (h + 24)
        out.paste(Image.fromarray(a[y0:y1, x0:x1]), (x, y))
        d.text((x + 2, y + h + 3), t, font=EA.font(13), fill=(214, 138, 74))
    ziel = os.path.join(HIER, '..', 'kontakt', '00_uebersicht.png')
    out.save(ziel)
    print(ziel, out.size)


if __name__ == '__main__':
    main()
