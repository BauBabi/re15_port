# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Nahblaetter Kopf, Hand, Fuss, Rumpf im Ton und mit Textur, Ruhe. RE2-Modelle als Eichmass Handwerk. Dazu Spielgroesse aller Modelle."""
from PIL import Image
from u_biege import *
from u_biege_blatt import filtern


def blick_dir(az, el):
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    return np.stack([rechts, unten, vor])


def rahmen(form, R, T, teile):
    W = welt(form, R, T)
    P = np.vstack([W[i] for i in teile if len(W[i])])
    return (P.max(0) + P.min(0)) / 2, float(np.linalg.norm(P - (P.max(0) + P.min(0)) / 2, axis=1).max())


def nah(k, clip, bl, teile, sichten, ton=True, gross=340, nur=None, g=None):
    form = lade(k)
    mo, tex = modell(k)
    R, T = pose(form, clip, bl)
    mi, rad = rahmen(form, R, T, teile)
    s = gross * 0.47 / rad
    aus = []
    q = filtern(mo, set(nur)) if nur else mo
    for t, az, el in sichten:
        a = tafel(q, tex, R, T, blick_dir(az, el), mi, s, gross, ton=ton, grund=58)
        if g:
            m = (a != 58).any(axis=2)
            a[m] = np.clip(np.power(a[m] / 255.0, g) * 255, 0, 255).astype(np.uint8)
        aus.append(a)
    return aus


def blatt_teil(ziel, kopf, reihen, sichten, ton=True, g=None):
    t = []
    for titel, k, clip, bl, teile in reihen:
        for (s_, az, el), a in zip(sichten, nah(k, clip, bl, teile, sichten, ton=ton, nur=teile, g=g)):
            t.append(('%s %s' % (titel, s_), a))
    FB.montage(t, len(sichten), ziel, kopf)


KOPF = [I['hals'], I['kopf'], I['kiefer']]
HAND = [I[x] for x in ('hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4')]
FUSS = [I['fuss_l'], I['unterschenkel_l']]

if __name__ == '__main__':
    S3 = (('seite', 180, 6), ('3/4', 130, 12), ('vorn', 90, 6), ('oben', 110, 45))
    k_re = [('%s Kopf' % k, k, 2, 0, KOPF) for k in ROHBAU]
    blatt_teil(AUS + '/nah_kopf_ton.png', 'Kopf, Hals, Kiefer: Ton', k_re + [('K34 Kopf', 'K34', 5, 40, [26]), ('K28 Kopf', 'K28', 0, 0, [2, 3, 4]), ('K22 Kopf', 'K22', 0, 0, [7, 8, 9])], S3)
    blatt_teil(AUS + '/nah_kopf_tex.png', 'Kopf, Hals, Kiefer: Textur, Gamma 0,6', k_re + [('K34 Kopf', 'K34', 5, 40, [26]), ('K28 Kopf', 'K28', 0, 0, [2, 3, 4])], S3, ton=False, g=0.6)
    S4 = (('aussen', 0, 10), ('innen', 180, 10), ('vorn', 90, 15), ('oben', 90, 60))
    h_re = [('%s Hand r' % k, k, 2, 0, HAND) for k in ROHBAU]
    blatt_teil(AUS + '/nah_hand_ton.png', 'Rechte Hand mit Klauen: Ton', h_re + [('K34 Hand', 'K34', 5, 40, [13, 14]), ('K33 Hand', 'K33', 0, 0, [13, 14, 15, 16]), ('K28 Hand', 'K28', 0, 0, [11, 12])], S4)
    f_re = [('%s Fuss l' % k, k, 2, 0, [I['fuss_l']]) for k in ROHBAU]
    blatt_teil(AUS + '/nah_fuss_ton.png', 'Linker Fuss: Ton', f_re + [('K34 Fuss', 'K34', 5, 40, [8, 4]), ('K33 Fuss', 'K33', 0, 0, [25])], S4)
    # Spielgroesse: Figur etwa 100 Bildpunkte hoch, wie im Fenster 320 x 240, x2 hart, ohne Gamma
    t = []
    for k, cl, bl in (('F', 2, 0), ('K34', 5, 40), ('K28', 0, 0)):
        mo, tex = modell(k)
        kf = kf_von(mo, cl, bl)
        for s_, az, el in (('vorn', 90, 14), ('3/4', 45, 14), ('Seite', 180, 14), ('hinten', -90, 14), ('3/4 hinten', -135, 14)):
            a, _ = bild(mo, tex, kf, az, el, 170, grund=60)
            b = np.array(Image.fromarray(a).resize((340, 340), Image.NEAREST))
            t.append(('%s %s' % (k, s_), b))
    FB.montage(t, 5, AUS + '/klein_alle.png', 'Spielgroesse: Figur rund 100 Bildpunkte hoch, x2 hart vergroessert, ohne Gamma')
