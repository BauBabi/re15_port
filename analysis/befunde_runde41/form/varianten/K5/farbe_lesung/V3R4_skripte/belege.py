"""Belegbilder V3R4_*.png (Nachtrag Rot). Nichts aufgehellt: Original- und Kettenbilder roh; Klassenkarten als beschriftete Falschfarbe."""
from basis import *
import render as R

AUS = K5 + '/farbe_lesung'
E = json.load(open('mess.json', encoding='utf-8'))
H_ROT = (-30.0, 70.0)
F_ROT = (235, 70, 30)       # Falschfarbe: Rotklasse eng (a* >= A0, Farbton -30..70 Grad)
F_MAUVE = (190, 80, 230)    # Falschfarbe: a* >= A0, Farbton < -30 Grad (mauve/violett)
F_GROSS = (255, 230, 0)     # Umriss der groessten zusammenhaengenden Rotflaeche (eng) der Figur
F_LEUCHT = (0, 200, 220)    # Leuchtstelle (nicht gemessen, bleibt wie Runde 3)


def schrift(img, t, x, y, s=0.5, c=(255, 255, 255)):
    cv2.putText(img, t, (x, y), cv2.FONT_HERSHEY_SIMPLEX, s, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(img, t, (x, y), cv2.FONT_HERSHEY_SIMPLEX, s, c, 1, cv2.LINE_AA)


def klassenkarte(L, basis, reg, leucht, A0):
    """Grau = L* der Haut (L* x 2,55, unveraendert), Rot/Mauve = Klassen; Regionsgrenzen weiss; Leuchtstelle tuerkis umrandet."""
    g = np.clip(L[..., 0] * 2.55, 0, 255).astype(np.uint8)
    img = np.stack([g, g, g], -1)
    img[~basis] = (0, 0, 0)
    h = np.degrees(np.arctan2(L[..., 2], L[..., 1]))
    r = basis & (L[..., 1] >= A0)
    eng = r & (h >= H_ROT[0]) & (h < H_ROT[1])
    img[eng] = F_ROT
    img[r & (h < H_ROT[0])] = F_MAUVE
    n, cc, st, _c = cv2.connectedComponentsWithStats(eng.astype(np.uint8), connectivity=8)
    if n > 1:
        i = 1 + int(np.argmax(st[1:, cv2.CC_STAT_AREA]))
        k = (cc == i).astype(np.uint8)
        rand = k.astype(bool) & ~cv2.erode(k, np.ones((3, 3), np.uint8)).astype(bool)
        img[rand] = F_GROSS
    gr = np.zeros(reg.shape, bool)
    for a in (0, 1):
        d = np.diff(reg, axis=a) != 0
        if a == 0:
            gr[1:] |= d
        else:
            gr[:, 1:] |= d
    img[gr & basis] = (255, 255, 255)
    lr = leucht & ~cv2.erode(leucht.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    img[lr] = F_LEUCHT
    return img, 100.0 * eng[basis].mean()


def kachel(img, box, f, titel, zeile2=None):
    x0, y0, x1, y1 = box
    k = cv2.resize(np.ascontiguousarray(img[y0:y1, x0:x1]), ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST)
    k = k[..., ::-1].copy()
    schrift(k, titel, 8, 22, 0.6)
    if zeile2:
        schrift(k, zeile2, 8, 44, 0.5, (200, 230, 255))
    return k


def legende(breite, text):
    b = np.full((96, breite, 3), 24, np.uint8)
    x, y = 10, 10
    for farbe, t in ((F_ROT, 'Rot eng (a* >= A0, Farbton -30..70)'), (F_MAUVE, 'mauve (a* >= A0, Farbton < -30)'), (F_GROSS, 'groesste Rotflaeche'),
                     (F_LEUCHT, 'Leuchtstelle (nicht gemessen)'), ((255, 255, 255), 'Regionsgrenze'), ((0, 0, 0), 'nicht gemessen')):
        if x + 24 + 9 * len(t) > breite:
            x, y = 10, y + 26
        cv2.rectangle(b, (x, y), (x + 18, y + 18), farbe[::-1], -1); cv2.rectangle(b, (x, y), (x + 18, y + 18), (128, 128, 128), 1)
        schrift(b, t, x + 24, y + 15, 0.45); x += 24 + 9 * len(t) + 20
    schrift(b, text, 10, 88, 0.45, (230, 230, 120))
    return b


def szene(n, f=3):
    d = np.load('szene%d.npz' % n)
    basis, reg, leucht, figu = d['basis'], d['reg'], d['leucht'], d['figu']
    A0 = float(d['A0T'])
    ys, xs = np.nonzero(figu | basis)
    box = (max(xs.min() - 6, 0), max(ys.min() - 6, 0), min(xs.max() + 7, 640), min(ys.max() + 7, 480))
    LO = lab(d['To']); LV = lab(d['v3T'])
    ko, ro = klassenkarte(LO, basis, reg, leucht, A0)
    kv, rv = klassenkarte(LV, basis, reg, leucht, A0)
    z = E['bilder'][str(n)]['zonen']['Figur']
    oben = np.hstack([kachel(d['V'], box, f, 'Bild %d Original (Video), roh' % n),
                      kachel(d['v3vid'], box, f, 'V3 r3 durch form_video (Saat 15), roh')])
    unten = np.hstack([kachel(ko, box, f, 'Original, rueckgerechnet T: Klassen (Falschfarbe)',
                              'Rot eng %.1f %%, Farbton %.1f Grad, gr. Flaeche %.1f %%' % (z['O_T']['rot_eng'], z['O_T']['h_rot'], z['O_T']['gross_eng_p'])),
                       kachel(kv, box, f, 'V3 r3 Kette T: Klassen (Falschfarbe)',
                              'Rot eng %.1f %%, Farbton %.1f Grad, gr. Flaeche %.1f %%' % (z['V3_T15']['rot_eng'], z['V3_T15']['h_rot'], z['V3_T15']['gross_eng_p']))])
    kopf = np.full((34, oben.shape[1], 3), 24, np.uint8)
    schrift(kopf, 'V3R4 Bild %d: Vorlage gegen V3 r3 in derselben Kette, %dfach, Ausschnitt x %d-%d, y %d-%d. Schwelle a* >= %d (Neutral-Probe p99). Figur ohne Leuchtstelle.'
            % (n, f, box[0], box[2], box[1], box[3], A0), 8, 23, 0.55)
    bild = np.vstack([kopf, oben, unten, legende(oben.shape[1], 'Grau = L* der Haut (L* x 2,55), nicht aufgehellt. Regionen: Knochen + lokale Normale (K5c-Geometrie, Leser B fid%d).' % n)])
    cv2.imwrite(AUS + '/V3R4_%d_bild%d_rot_x%d.png' % (n, n, f), bild)
    print('Bild', n, box, bild.shape, 'eng O/V3', round(ro, 1), round(rv, 1))


def regionskarte_neutral():
    """G4 und V3 r3 neutral (Tafel-03-Licht), drei Winkel: oben Licht roh, Mitte Klassen der Texel (Falschfarbe), unten Regionen (Falschfarbe)."""
    A0 = E['neutral']['A0']
    RF = [(70, 110, 200), (90, 170, 220), (200, 200, 90), (100, 190, 100), (40, 120, 60), (180, 120, 60), (220, 150, 200), (150, 90, 160)]
    zeilen = []
    for nm in ('G4', 'V3'):
        r1, r2, r3 = [], [], []
        for wt, az, el in R.WINKEL:
            d = np.load('neutral_%s_%d.npz' % (nm, az))
            img, ia, msk, rg = d['img'], d['ia'], d['msk'], d['reg']
            L = lab(ia.astype(np.float32))
            k, _ = klassenkarte(L, msk & (rg >= 0), rg, np.zeros_like(msk), A0)
            k[msk & (rg < 0)] = (60, 60, 60)
            rk = np.full(img.shape, 0, np.uint8)
            for i, c in enumerate(RF):
                rk[rg == i] = c
            rk[msk & (rg < 0)] = (60, 60, 60)
            box = (0, 0, img.shape[1], img.shape[0])
            t = 'RE2-G4 (Clip 0)' if nm == 'G4' else 'V3 r3 (Clip 2)'
            r1.append(kachel(img, box, 1, '%s %s' % (t, wt), 'Licht 0,55+0,45 N.L, roh'))
            r2.append(kachel(k, box, 1, 'Texel: Klassen (Falschfarbe)', 'a* >= %d' % A0))
            r3.append(kachel(rk, box, 1, 'Regionen (Falschfarbe)'))
        zeilen += [np.hstack(r1), np.hstack(r2), np.hstack(r3)]
    t = np.vstack(zeilen)
    leg = np.full((40, t.shape[1], 3), 24, np.uint8)
    x = 10
    for i, rn in enumerate(REGIONEN):
        cv2.rectangle(leg, (x, 10), (x + 18, 28), RF[i][::-1], -1)
        schrift(leg, rn, x + 24, 25, 0.45); x += 24 + 10 * len(rn) + 16
    leg = np.vstack([leg, np.full((26, t.shape[1], 3), 24, np.uint8)])
    schrift(leg, 'grau = ohne Region (G4 kleine Arme 20-25)', 10, 56, 0.45)
    kopf = np.full((34, t.shape[1], 3), 24, np.uint8)
    schrift(kopf, 'V3R4 neutral: RE2-G4 gegen V3 r3, gleicher Renderer (render.blick), gleiches Licht, Umkugel 0,46, roh. Klassen auf Texeln (Licht herausgerechnet).', 8, 23, 0.55)
    cv2.imwrite(AUS + '/V3R4_3_neutral_g4_v3.png', np.vstack([kopf, t, leg, legende(t.shape[1], 'Gleiche Klassenfarben wie V3R4_1/2.')]))
    print('neutral', t.shape)


if __name__ == '__main__':
    szene(1); szene(2); regionskarte_neutral()
