"""E6: Tafeln (roh, kein Gamma ausser wo angeschrieben).
T1_referenz_bild2.png   Hintergrund + Frau: Video | Port roh | Modell em_frei | form_video Bestand | Abstand, dazu Frau x3
T2_referenz_bild1.png   Leon x3: Video | Port roh | Bestand EX farbe.md | EX an Leon; dazu Bild 1 neben den 6 Schnitten von 5080
T3_wesen_bild1.png / T4_wesen_bild2.png   Vorlage | Umkehr e | Albedo (gueltig) | K5e_f1 Spiel (Disc-Licht) | K5e_f1 Kette | K5e_f1 Textur |
                                          Material/Ausschluss, x2
T5_zielfarben.png       Zielfarbe je Material (Bild 1, Bild 2: p10 | Median | p90) neben K5c, K5e_f1, K5f (gleiche Kette)"""
import json
import numpy as np
import cv2
import ek

F = cv2.FONT_HERSHEY_SIMPLEX
NAMEN = ['Haut', 'Gelenk', 'Schwanzspitze', 'Buckel', 'Krallen', 'Kopf', 'Lippe/Unterkiefer', 'Hals', 'F1', 'Sonder']
FARBE_MAT = [(200, 120, 160), (90, 170, 230), (240, 90, 60), (250, 200, 60), (120, 230, 120), (180, 120, 250), (250, 150, 220),
             (60, 220, 220), (255, 255, 255), (150, 150, 150)]


def u8(a):
    return np.clip(np.asarray(a, np.float32) + .5, 0, 255).astype(np.uint8)


def schrift(img, t, xy=(4, 14), s=.42):
    cv2.putText(img, t, xy, F, s, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(img, t, xy, F, s, (255, 255, 120), 1, cv2.LINE_AA)
    return img


def kachel(a, titel, box=None, f=1.0):
    a = u8(a)
    if box:
        y0, y1, x0, x1 = box
        a = a[y0:y1, x0:x1]
    if f != 1.0:
        a = cv2.resize(a, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    a = a.copy()
    return schrift(a, titel)


def reihe(ks, pad=6):
    h = max(k.shape[0] for k in ks)
    out = []
    for k in ks:
        out.append(np.pad(k, ((0, h - k.shape[0]), (0, pad), (0, 0)), constant_values=24))
    return np.hstack(out)


def stapel(rs, pad=6):
    w = max(r.shape[1] for r in rs)
    return np.vstack([np.pad(r, ((0, pad), (0, w - r.shape[1]), (0, 0)), constant_values=24) for r in rs])


def kopf(img, t):
    k = np.full((26, img.shape[1], 3), 24, np.uint8)
    schrift(k, t, (6, 18), .5)
    return np.vstack([k, img])


def speichern(name, img):
    cv2.imwrite(ek.TAFELN + '/' + name, img[:, :, ::-1])
    print('Tafel', name, img.shape)


def t1():
    V = ek.video(2)
    z3 = np.load(ek.SCR + '/e3_b2.npz')
    z4 = np.load(ek.SCR + '/e4_frau.npz')
    import em
    th = np.array(json.load(open(ek.DATEN + '/e4b_modell_bild2.json'))['alle']['frei'])
    comp = z4['comp'].astype(np.float32)
    P = em.vorwaerts(comp, th)
    B = ek.FV.video_filter(u8(comp), bild=2, entzerren=False, rauschen=False).astype(np.float32)
    M = z3['M']
    d = ek.de76(P, V)
    dk = np.where(M[..., None], np.repeat(np.clip(d * 4, 0, 255)[..., None], 3, 2), 30)
    r1 = reihe([kachel(V, 'Video Dia 145 (Mittel 24 Bilder)', f=.6), kachel(comp, 'Port roh: 5090/11 + Frau (Disc-Licht)', f=.6),
                kachel(P, 'Modell em_frei (BG + Frau)', f=.6), kachel(B, 'form_video Bestand', f=.6),
                kachel(dk, 'dE76 em_frei x4, nur Fitmaske', f=.6)])
    box = (165, 315, 120, 230)
    r2 = reihe([kachel(V, 'Video', box, 3), kachel(comp, 'Port roh', box, 3), kachel(P, 'em_frei', box, 3), kachel(B, 'Bestand', box, 3)])
    speichern('T1_referenz_bild2.png', kopf(stapel([r1, r2]), 'T1 Referenzpaare Bild 2: Hintergrund 5090 Schnitt 11 und Frau (Typ 0x4D), vorher (roh) / nachher (Modell)'))


def t2():
    V = ek.video(1)
    z = np.load(ek.SCR + '/e4_leon.npz')
    box = (185, 385, 60, 180)
    r1 = reihe([kachel(V, 'Video Dia 143', box, 3), kachel(z['comp'], 'Port roh (Umkehr + Leon)', box, 3),
                kachel(z['P0'], 'Bestand EX farbe.md', box, 3), kachel(z['P'], 'EX an Leon', box, 3)])
    bg = [cv2.resize(ek.lade_rgb(ek.WT + '/re15_port/build/form_bg/ROOM5080%d.ppm' % i), (256, 192), interpolation=cv2.INTER_AREA)
          for i in range(6)]
    r2 = reihe([kachel(cv2.resize(u8(V), (256, 192), interpolation=cv2.INTER_AREA), 'Bild 1 (Video)')] +
               [schrift(b.copy(), '5080 Schnitt %d' % i) for i, b in enumerate(bg)])
    speichern('T2_referenz_bild1.png', kopf(stapel([r1, r2]), 'T2 Bild 1: Leon x3 vorher/nachher; unten Bild 1 gegen alle 6 Schnitte von 5080 (keiner zeigt diese Ansicht)'))


def t_wesen(n):
    V = ek.video(n)
    z = np.load(ek.SCR + '/e5_b%d.npz' % n)
    k = np.load(ek.SCR + '/e5_b%d_K5e_f1.npz' % n)
    m = z['m']
    ys, xs = np.nonzero(m)
    box = (max(ys.min() - 10, 0), min(ys.max() + 10, 480), max(xs.min() - 10, 0), min(xs.max() + 10, 640))
    g = z['gueltig']
    alb = np.where(g[..., None], z['alb'], 0)
    albS = np.where(g[..., None], k['albS'], 0)
    mat = np.full(V.shape, 30, np.uint8)
    for i, c in enumerate(FARBE_MAT):
        mat[(z['mat'] == i) & z['kern']] = c
    mat[z['clip'] & z['m']] = (255, 0, 0)
    mat[z['feuer'] & z['m'] & ~z['clip']] = (255, 140, 0)
    mat[z['streu'] & z['m'] & ~z['clip'] & ~z['feuer']] = (255, 255, 0)
    mat[z['kern'] & ~g & ~(z['clip'] | z['feuer'] | z['streu'])] = (90, 90, 90)
    f = 2
    r = reihe([kachel(V, 'Vorlage (Dia-Mittel)', box, f), kachel(z['eR'], 'Umkehr: Spielfarbe e', box, f),
               kachel(alb, 'Albedo (Textur 0..255), gueltig', box, f), kachel(k['s'], 'K5e_f1 im Spiel (Disc-Licht)', box, f),
               kachel(k['alb'], 'K5e_f1 Textur', box, f), kachel(albS, 'K5e_f1 durch dieselbe Kette', box, f),
               kachel(mat, 'Material / rot=Clip orange=Feuer gelb=Streu', box, f)])
    leg = np.full((22, r.shape[1], 3), 24, np.uint8)
    x = 6
    for nm, c in zip(NAMEN, FARBE_MAT):
        cv2.rectangle(leg, (x, 5), (x + 12, 17), c, -1)
        cv2.putText(leg, nm, (x + 16, 16), F, .4, (220, 220, 220), 1, cv2.LINE_AA)
        x += 30 + 8 * len(nm)
    titel = {1: 'T3 Bild 1 (Rueckseite, 5080, Modell form_video EX an Leon)', 2: 'T4 Bild 2 (Vorderseite, 5090/11, Modell form_video Bestand)'}[n]
    speichern('T%d_wesen_bild%d.png' % (n + 2, n), kopf(np.vstack([r, leg]), titel + ': Wesen zurueckgerechnet neben Vorlage und K5e_f1'))


def t5():
    j = json.load(open(ek.DATEN + '/e7_ziel.json'))
    S = 46
    zeilen = []
    for nm in NAMEN[:-1]:
        z = j['materialien'].get(nm, {})
        felder = []
        for quelle in ('bild1', 'bild2'):
            q = z.get(quelle)
            blk = np.full((S + 20, 3 * S + 6, 3), 24, np.uint8)
            if q and 'median' in q:
                for i, key in enumerate(('p10', 'median', 'p90')):
                    blk[0:S, i * (S + 3):i * (S + 3) + S] = u8(q[key])
                schrift(blk, '%s n=%d L%.0f' % (quelle, q['pixel'], q['lab_median'][0]), (2, S + 14), .36)
            else:
                schrift(blk, '%s: nicht messbar' % quelle, (2, S + 14), .36)
            felder.append(blk)
        for st in ('K5c', 'K5e_f1', 'K5f'):
            q = z.get('staende', {}).get(st, {})
            blk = np.full((S + 20, S + 70, 3), 24, np.uint8)
            if q.get('median'):
                blk[0:S, 0:S] = u8(q['median'])
                schrift(blk, '%s dE%.0f' % (st, q.get('dE76_zum_ziel', -1)), (2, S + 14), .36)
            felder.append(blk)
        lab = np.full((S + 20, 170, 3), 24, np.uint8)
        schrift(lab, nm, (4, 20), .5)
        if z.get('ziel'):
            schrift(lab, 'Ziel L%.0f a%.0f b%.0f' % tuple(z['ziel']['lab']), (4, 40), .4)
        zeilen.append(reihe([lab] + felder))
    speichern('T5_zielfarben.png', kopf(stapel(zeilen), 'T5 Zielfarben je Material (Textur-Einheiten 0..255): Bild 1 / Bild 2 je p10|Median|p90, daneben K5c, K5e_f1, K5f durch dieselbe Kette'))


if __name__ == '__main__':
    import sys
    was = sys.argv[1:] or ['1', '2', 'w1', 'w2', '5']
    for w in was:
        {'1': t1, '2': t2, 'w1': lambda: t_wesen(1), 'w2': lambda: t_wesen(2), '5': t5}[w]()
