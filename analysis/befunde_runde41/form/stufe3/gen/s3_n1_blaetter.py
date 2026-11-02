"""Nachbesserung 1, Asset-Spur: Vorher/Nachher-Blaetter (Offline-Buehne wie s3_blatt, Textur, Gouraud ohne Raumlicht).

    python s3_n1_blaetter.py spruenge|sturz|taumeln [--vorher EMD --vorher-tim TIM] [--aus PNG]

vorher = Spiel-Asset der Abnahme 1 (J; stufe3/roh/n1_vorher = stufe3/asset aus Commit d8eab158b), nachher = stufe3/asset. Kein RE-Wert."""
import os, sys, math, argparse
import numpy as np
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import s3_blatt as B        # noqa: E402
import form_emd as FE       # noqa: E402
import form_buehne as FB    # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402

N1 = os.path.join(HIER, '..', 'nachbesserung1')
NACH = os.path.join(HIER, '..', 'asset')
VOR = os.path.join(HIER, '..', 'roh', 'n1_vorher')

# (Clip, Bild a, Bild b, Gier der Kamera, Text): Folgebilder, an denen die Abnahme bzw. die Messung einen Sprung fand
SPRUENGE = [(0, 15, 16, 2048, 'Rennen 15->16 Fuss L'), (0, 10, 11, 2048, 'Rennen 10->11 Unterarm L (Bild 11 = Pose Bild 1)'),
            (10, 30, 31, 0, 'Drohen 30->31 Bein R'), (10, 44, 45, 2048, 'Drohen 44->45 Bein L (Bild 45 = Pose Bild 2)'),
            (9, 20, 21, 1536, 'Nachhieb 20->21 Krallen'), (4, 14, 15, 2048, 'Hieb links 14->15 Krallen'),
            (27, 42, 43, 2048, 'Griff 42->43 Unterarm L'), (11, 36, 37, 2048, 'Flucht 36->37 Bein L')]


def drehung(r, f, c, a, b):
    kfa = f['pool'][f['clips'][c]['bilder'][a]['kf']]
    kfb = f['pool'][f['clips'][c]['bilder'][b]['kf']]
    Ra, _ = r.fk(r.aus_keyframe(kfa))
    Rb, _ = r.fk(r.aus_keyframe(kfb))
    best = (0.0, -1)
    for j in range(32):
        d = math.degrees(math.acos(max(-1, min(1, (np.trace(Ra[j].T @ Rb[j]) - 1) / 2))))
        if d > best[0]:
            best = (d, j)
    return best


def kachel(mo, tex, c, i, gier_kam):
    f0, n = mo.clips[c]
    kf = mo.frames[f0 + i] & 0xFFF
    kam = S.kamera_seite(gier_kam, abstand=11000)
    r, m = B.render(mo, tex, kf, (0, 0, 0), 0, kam)
    return r, m


def spruenge(aus, vor_emd, vor_tim):
    r = S.Rig()
    paare = []
    for name, (emd_p, tim_p) in (('vorher (J, Abnahme 1)', (vor_emd, vor_tim)), ('nachher (K3, Nachbesserung 1)',
                                 (os.path.join(NACH, 'BAHNSTEIG_FORM.EMD'), os.path.join(NACH, 'BAHNSTEIG_FORM.TIM')))):
        emd, tim = open(emd_p, 'rb').read(), open(tim_p, 'rb').read()
        paare.append((name, FE.modell(emd, tim), FB.Textur(tim), FE.lese_emd(emd)))
    tafeln = []
    for (c, a, b, g, txt) in SPRUENGE:
        reihe = []
        for name, mo, tex, f in paare:
            d, j = drehung(r, f, c, a, b)
            ra, ma = kachel(mo, tex, c, a, g)
            rb, mb = kachel(mo, tex, c, b, g)
            reihe.append((name, d, j, ra, ma, rb, mb))
        m = np.zeros(reihe[0][4].shape, bool)
        for e in reihe:
            m |= e[4] | e[6]
        ys, xs = np.nonzero(m)
        box = (max(ys.min() - 10, 0), min(ys.max() + 10, m.shape[0]), max(xs.min() - 10, 0), min(xs.max() + 10, m.shape[1]))
        tafeln.append((c, a, b, txt, reihe, box))
    pad, k = 6, 0.5
    Wt = int(max(t[5][3] - t[5][2] for t in tafeln) * k)
    W = pad + 4 * (Wt + pad)
    H = 30 + sum(int((t[5][1] - t[5][0]) * k) + 40 for t in tafeln)
    out = Image.new('RGB', (W, H), (18, 19, 22))
    d = ImageDraw.Draw(out)
    d.text((pad, 8), 'Nachbesserung 1, M8e: Gelenkspruenge (wahre Knochendrehung zwischen Folgebildern). Links vorher, rechts nachher; je Bild a | Bild b',
           font=EA.font(14), fill=(200, 203, 210))
    y = 30
    for (c, a, b, txt, reihe, (y0, y1, x0, x1)) in tafeln:
        x = pad
        for name, dd, j, ra, ma, rb, mb in reihe:
            d.text((x + 2, y + 2), '%s - %s: max %.0f Grad (Kn %d %s)' % (txt, name.split(' ')[0], dd, j, S.NAMEN[j]), font=EA.font(11),
                   fill=(240, 200, 120) if name.startswith('nachher') else (220, 150, 150))
            for (i, img) in ((a, ra), (b, rb)):
                kk = Image.fromarray(img[y0:y1, x0:x1]).resize((int((x1 - x0) * k), int((y1 - y0) * k)))
                out.paste(kk, (x, y + 18))
                d.text((x + 2, y + 18 + kk.size[1] + 1), 'Bild %d' % i, font=EA.font(11), fill=(214, 138, 74))
                x += Wt + pad
        y += int((y1 - y0) * k) + 40
    out.save(aus)
    return aus


def stapel(aus, teile, titel):
    ims = [Image.open(p).convert('RGB') for p in teile]
    W = max(i.size[0] for i in ims)
    H = sum(i.size[1] for i in ims) + 30
    out = Image.new('RGB', (W, H), (18, 19, 22))
    ImageDraw.Draw(out).text((6, 8), titel, font=EA.font(14), fill=(240, 200, 120))
    y = 30
    for i in ims:
        out.paste(i, (0, y))
        y += i.size[1]
    out.save(aus)
    for p in teile:
        os.remove(p)
    return aus


def main():
    a = argparse.ArgumentParser()
    a.add_argument('was', choices=('spruenge', 'sturz', 'taumeln'))
    a.add_argument('--vorher', default=os.path.join(VOR, 'BAHNSTEIG_FORM.EMD'))
    a.add_argument('--vorher-tim', default=os.path.join(VOR, 'BAHNSTEIG_FORM.TIM'))
    a.add_argument('--aus')
    g = a.parse_args()
    os.makedirs(N1, exist_ok=True)
    ne, nt = open(os.path.join(NACH, 'BAHNSTEIG_FORM.EMD'), 'rb').read(), open(os.path.join(NACH, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    if g.was == 'spruenge':
        print(spruenge(g.aus or os.path.join(N1, 'spruenge_vorher_nachher.png'), g.vorher, g.vorher_tim))
    elif g.was == 'sturz':
        ve, vt = open(g.vorher, 'rb').read(), open(g.vorher_tim, 'rb').read()
        t1, t2 = os.path.join(N1, '_s1.png'), os.path.join(N1, '_s2.png')
        B.clip_blatt(ve, vt, 12, [0, 1536], 3, t1, 'vorher (J): Clip 12 Bild 0-30, feste Kamera, Bodenraster steht in der Welt - Fuesse gleiten (bis 150 je Bild)',
                     max_spalten=12, bis_bild=31, kamera_folgt=False)
        B.clip_blatt(ne, nt, 12, [0, 1536], 3, t2, 'nachher (K3): Clip 12 Bild 0-30 - Hinterfuesse stehen in der Welt und machen Schritte (Gleiten 4)',
                     max_spalten=12, bis_bild=31, kamera_folgt=False)
        print(stapel(g.aus or os.path.join(N1, 'sturz_clip12_vorher_nachher.png'), [t1, t2],
                     'Nachbesserung 1, M1: Sturz im Wagen (Clip 12, Bild 0 = Pose Bild 2), Spur bis Bild 30 = -3270'))
    else:
        t1 = os.path.join(N1, '_t1.png')
        B.clip_blatt(ne, nt, 13, [0, 1536, 3584], 3, t1, 'Clip 13 Taumeln (neu, PORT-WAHL): 40 Bilder, Spur -80 je Bild rueckwaerts, feste Kamera, '
                     'Bodenraster steht in der Welt: alle vier Fuesse stehen und treten zurueck', max_spalten=14, kamera_folgt=False)
        print(stapel(g.aus or os.path.join(N1, 'taumeln_clip13.png'), [t1], 'Nachbesserung 1, M5: Rueckstoss-Taumeln statt Gleiten auf Clip 8'))


if __name__ == '__main__':
    main()
