"""Belegbilder B_1..B_5, B_7, B_8 (B_6 aus lauf_render.py). Keine Aufhellung der Renders; Original und Rueckrechnung in echter Helligkeit."""
import json
import numpy as np, cv2
import fl, zonen as Z, texel
import k5blatt_hilfe as H

BOX = {1: (150, 160, 470, 480), 2: (110, 130, 510, 480)}
SK = {1: 2, 2: 1.6}
REGF = [(0, 255, 0), (255, 160, 0), (0, 200, 255), (255, 0, 255), (255, 255, 0)]   # BGR je Region


def crop(a, n):
    x0, y0, x1, y1 = BOX[n]
    c = np.ascontiguousarray(a[y0:y1, x0:x1])
    return cv2.resize(c, None, fx=SK[n], fy=SK[n], interpolation=cv2.INTER_NEAREST)


def umrisse(a, n, regs, mit_text=True):
    x0, y0 = BOX[n][:2]
    s = SK[n]
    for i, (nm, m) in enumerate(regs):
        cs, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cs = [np.round((c - np.array([x0, y0])) * s).astype(np.int32) for c in cs]
        cv2.drawContours(a, cs, -1, REGF[i % 5], 1, cv2.LINE_AA)
    return a


def legende(breite, regs):
    zeilen = []
    a = np.full((22 * (len(regs) + 2), breite, 3), 24, np.uint8)
    for i, (nm, _) in enumerate(regs):
        cv2.rectangle(a, (8, 6 + 22 * i), (24, 20 + 22 * i), REGF[i % 5], -1)
        H.schrift(a, nm, 32, 19 + 22 * i, 0.45)
    y = 22 * len(regs) + 6
    x = 8
    for i, k in enumerate(Z.KL):
        c = Z.KL_FARBE[i]
        cv2.rectangle(a, (x, y), (x + 14, y + 14), (c[2], c[1], c[0]), -1)
        H.schrift(a, k, x + 18, y + 13, 0.4)
        x += 18 + 9 * len(k) + 6
    return a


def zonen_blatt(n):
    v = cv2.imread(fl.FORM + '/bild%d.png' % n)
    T = np.load(fl.S + '/T_orig%d.npy' % n)
    clip = np.load(fl.S + '/clip_orig%d.npy' % n)
    fig = np.load(fl.S + '/fig_orig%d.npy' % n)
    regs = Z.regionen(n)
    k1 = crop(v, n)
    H.schrift(k1, 'Bild %d Original (Video)' % n, 6, 18, 0.5)
    k2 = crop(np.clip(T, 0, 255).astype(np.uint8)[:, :, ::-1], n)
    H.schrift(k2, 'Rueckrechnung: Texturschaetzung T = umkehr / Licht', 6, 18, 0.45)
    H.schrift(k2, 'echte Helligkeit, keine Aufhellung', 6, 36, 0.42, (200, 230, 255))
    kb = Z.klassenbild(T, clip, fig)[:, :, ::-1]
    k3 = umrisse(crop(kb, n), n, regs)
    H.schrift(k3, 'Materialklassen je Pixel + Zonen (Nutzer-Teile)', 6, 18, 0.45)
    r = H.reihe([k1, k2, k3])
    kopf = H.kopfzeile('B_%d: Farbzonen Bild %d, Original. Masken = Nutzer-Teile ohne Boden (Pruefer) und Logo. Klassen siehe FARBE_LESER_B.md Abschnitt 1.' % (n, n), r.shape[1], g=0.5)
    cv2.imwrite(fl.OUT + '/B_%d_zonen_bild%d.png' % (n, n), H.stapel([kopf, r, legende(r.shape[1], regs)]))


def balken(kl_anteile, kl_farben, breite=300, hoehe=26):
    a = np.full((hoehe, breite, 3), 40, np.uint8)
    x = 0
    for i, k in enumerate(Z.KL):
        ant = kl_anteile.get(k[0], 0)
        w = int(round(ant * breite))
        if w <= 0:
            continue
        f = kl_farben.get(k[0])
        c = f['T'] if f else Z.KL_FARBE[i]
        a[:, x:x + w] = (c[2], c[1], c[0])
        x += w
    return a


def farbfelder():
    o = json.load(open(fl.S + '/zonen_orig.json'))
    v = json.load(open(fl.S + '/zonen_var_K5c_K4_heute.json'))
    zeilen = []
    kopf = np.full((30, 1300, 3), 24, np.uint8)
    H.schrift(kopf, 'B_3: Farbfelder je Zone, Balkenbreite = Klassenanteil, Feldfarbe = mittlere Texturfarbe T der Klasse in der Zone (keine Aufhellung)', 8, 20, 0.5, (230, 230, 120))
    zeilen.append(kopf)
    sp = np.full((26, 1300, 3), 24, np.uint8)
    for x, t in ((330, 'Original (Rueckrechnung)'), (650, 'K5c Albedo (Textur am Pixel)'), (970, 'K5c Kette (Render+Filter+Umkehr)')):
        H.schrift(sp, t, x, 18, 0.45)
    zeilen.append(sp)
    for n in (1, 2):
        for zo, za, zk in zip(o['bild%d' % n], v['K5c_bild%d' % n]['albedo'], v['K5c_bild%d' % n]['kette']):
            if zo['zone'] == 'Figur gesamt':
                continue
            a = np.full((30, 1300, 3), 24, np.uint8)
            H.schrift(a, 'B%d %s' % (n, zo['zone'][:40]), 6, 20, 0.4)
            for x, z in ((330, zo), (650, za), (970, zk)):
                if z.get('px'):
                    a[2:28, x:x + 300] = balken(z['klassen'], z.get('klassen_farbe', {}))
            zeilen.append(a)
    # Kiefer Bild 2: Bezuege
    T2 = np.load(fl.S + '/T_orig2.npy')
    a = np.full((60, 1300, 3), 24, np.uint8)
    fel = [('Band UK Original', (184, 108, 104)), ('Zahnflecken UK Original', (202, 193, 178)), ('Mantel Frau Original', (204, 195, 184)),
           ('K5c Kiefer-Textur', (172, 149, 129)), ('Band bei Zahn=neutral 235', (214, 132, 137))]
    x = 6
    for t, c in fel:
        cv2.rectangle(a, (x, 4), (x + 60, 34), (c[2], c[1], c[0]), -1)
        H.schrift(a, t, x, 52, 0.38)
        H.schrift(a, str(c), x + 64, 24, 0.38)
        x += 255
    zeilen.append(a)
    cv2.imwrite(fl.OUT + '/B_3_farbfelder_zonen.png', H.stapel(zeilen))


def k5c_blatt(n):
    v = cv2.imread(fl.FORM + '/bild%d.png' % n)
    sauber = np.load(fl.S + '/K5c_sauber%d.npy' % n)
    vid = np.load(fl.S + '/K5c_vid%d.npy' % n)
    T = np.load(fl.S + '/K5c_T%d.npy' % n)
    clip = np.load(fl.S + '/K5c_clip%d.npy' % n)
    fig = np.load(fl.S + '/K5c_fig%d.npy' % n)
    To = np.load(fl.S + '/T_orig%d.npy' % n)
    co = np.load(fl.S + '/clip_orig%d.npy' % n)
    fo = np.load(fl.S + '/fig_orig%d.npy' % n)
    regs = Z.regionen(n)
    al = np.zeros((480, 640), bool)
    for _, m in regs:
        al |= m
    k1 = crop(v, n)
    H.schrift(k1, 'Bild %d Original' % n, 6, 18, 0.5)
    k2 = crop(sauber[:, :, ::-1], n)
    H.schrift(k2, 'K5c, Spiellicht (mess), sauber', 6, 18, 0.45)
    H.schrift(k2, 'ohne Aufhellung', 6, 36, 0.42, (200, 230, 255))
    k3 = crop(vid[:, :, ::-1], n)
    H.schrift(k3, 'K5c durch video_filter', 6, 18, 0.45)
    k4 = umrisse(crop(Z.klassenbild(To, co, fo)[:, :, ::-1], n), n, regs)
    H.schrift(k4, 'Klassen Original', 6, 18, 0.45)
    k5 = umrisse(crop(Z.klassenbild(T, clip, fig & al)[:, :, ::-1], n), n, regs)
    H.schrift(k5, 'Klassen K5c (gleiche Kette)', 6, 18, 0.45)
    r1 = H.reihe([k1, k2, k3])
    r2 = H.reihe([k4, k5])
    kopf = H.kopfzeile('B_%d: K5c in der Kamera von Bild %d mit Szenenlicht neben dem Screenshot. Kein Gamma, keine Aufhellung.' % (n + 3, n), r1.shape[1], g=0.5)
    cv2.imwrite(fl.OUT + '/B_%d_k5c_bild%d.png' % (n + 3, n), H.stapel([kopf, r1, r2, legende(r1.shape[1], regs)]))


def atlas_blatt():
    import stil
    zeilen = []
    for nm, mo in (('EM30_HANGAR', fl.lade_em30()), ('K5c', fl.lade_var(fl.K5 + '/K5c'))):
        per, tri, feld = texel.sammeln(mo)
        a, m = texel.atlasbild(mo, per)
        L = fl.lab(a[m])
        kb = np.zeros_like(a)
        kb[m] = np.array(Z.KL_FARBE, np.uint8)[Z.klassen(L, np.zeros(len(L), bool))]
        s = 2
        A = cv2.resize(a[:, :, ::-1], None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
        K = cv2.resize(kb[:, :, ::-1], None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
        H.schrift(A, '%s Atlas (Texel mit der CLUT-Zeile der Flaeche), 2-fach' % nm, 6, 18, 0.5)
        H.schrift(K, '%s Materialklassen je Texel' % nm, 6, 18, 0.5)
        zeilen.append(H.reihe([A, K]))
    st = json.load(open(fl.S + '/stil.json'))
    t = np.full((70, max(z.shape[1] for z in zeilen), 3), 24, np.uint8)
    e, k = st['EM30_haut'], st['K5c_gesamt']
    H.schrift(t, 'EM30 Haut: Chroma %.1f+-%.1f, Rot %.1f%%, Farben/Teil %.1f, HF %.3f, L*>85 %.2f%%' % (e['T6_chroma_mittel'], e['T6_chroma_streu'], e['T7_rot_anteil_proz'], e['T2_farben_je_teil'], e['T5_hf'], e['T4_glanz_L85_proz']), 8, 24, 0.5)
    H.schrift(t, 'K5c gesamt: Chroma %.1f+-%.1f, Rot %.1f%%, Farben/Teil %.1f, HF %.3f, L*>85 %.2f%%' % (k['T6_chroma_mittel'], k['T6_chroma_streu'], k['T7_rot_anteil_proz'], k['T2_farben_je_teil'], k['T5_hf'], k['T4_glanz_L85_proz']), 8, 52, 0.5)
    kopf = H.kopfzeile('B_7: Atlas EM30_HANGAR (gelobter Stand, Capcom-D0-Basis) gegen K5c, gleicher Massstab, Klassen wie B_1/B_2', t.shape[1], g=0.5)
    cv2.imwrite(fl.OUT + '/B_7_atlas_em30_k5c.png', H.stapel([kopf, t] + zeilen))


def kiefer_blatt():
    v = cv2.imread(fl.FORM + '/bild2.png')
    box = (200, 290, 360, 420)
    s = 3
    def c(a):
        x0, y0, x1, y1 = box
        return cv2.resize(np.ascontiguousarray(a[y0:y1, x0:x1]), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
    T = np.load(fl.S + '/T_orig2.npy')
    sauber = np.load(fl.S + '/K5c_sauber2.npy')
    vid = np.load(fl.S + '/K5c_vid2.npy')
    alb = np.load(fl.S + '/K5c_alb2.npy')
    ks = [c(v), c(np.clip(T, 0, 255).astype(np.uint8)[:, :, ::-1]), c(alb[:, :, ::-1]), c(sauber[:, :, ::-1]), c(vid[:, :, ::-1])]
    for k, t in zip(ks, ('Original', 'Original rueckgerechnet T', 'K5c Albedo (Textur)', 'K5c Spiellicht sauber', 'K5c gefiltert')):
        uk = Z.mk('unterkiefer')
        cs, _ = cv2.findContours(uk.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cs = [np.round((cc - np.array(box[:2])) * s).astype(np.int32) for cc in cs]
        cv2.drawContours(k, cs, -1, (255, 120, 255), 1, cv2.LINE_AA)
        H.schrift(k, t, 6, 18, 0.5)
    r = H.reihe(ks)
    kopf = H.kopfzeile('B_8: Unterkiefer Bild 2 (rosa Umriss = Nutzer-Teil Unterkiefer), x 200-360, y 290-420, 3-fach, ohne Aufhellung', r.shape[1], g=0.5)
    cv2.imwrite(fl.OUT + '/B_8_kiefer_bild2.png', H.stapel([kopf, r]))


if __name__ == '__main__':
    zonen_blatt(1)
    zonen_blatt(2)
    farbfelder()
    k5c_blatt(1)
    k5c_blatt(2)
    atlas_blatt()
    kiefer_blatt()
    print('ok')
