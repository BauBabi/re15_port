"""tafeln_r6.py - Tafeln der Runde 6b (P5-neu "Kragen weg, echter Nacken", P4-neu), roh, deterministisch.

09_vorn_kopf.png      von vorn nah am Kopf, Spalten Foto 1 | V7 | V8 r4 | V8 r5d | NEU: P1, Szene Takt F354 und Takt F300 (offline, Spielkamera)
15_ruecken_kopf.png   Ruecken/Kopf: je Pose PV / P2 / P1 Vorlage | V7 | V8 r4 | V8 r5d | NEU (x3), darunter dieselben Bilder mit den
                      Luecken-Bildpunkten (luecke_r5, tuerkis; Titel Summe px / tiefer als 1,5 px / groesste Tiefe) und dem Mass an der Vorlage;
                      dann die Szenen-Takte mit der staerksten Luecke des neuen Stands sowie F311 und F332; unten das Nackenstueck (Flaechen von
                      Mesh 8 ohne Gegenstueck in V8 r4) gruen, ueber Gesichts-Bildpunkten magenta
17_kragen_weg.png     Vorderansichten der ECHTEN Szenenbilder (Framedump 960x720) F352, F366, F372, F278, F306, je V7 | V8 r5d | NEU, gleicher
                      Ausschnitt (320x240 um den Schwerpunkt des V7-Gesichts aus dem Marker-Lauf KO_V7), x2
14b_tonne.png         echte Szenenbilder F315, F318, F321, F324 ("hellrote Tonne mit grauen Reifen"), V7 | V8 r5d | NEU, Ausschnitt = Kasten der
                      Figur aus dem gruenen Marker-Lauf, x2
Framedump-Nummer = Takt + 6 (r4_gemein).
"""
import os, sys, argparse
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import f_rast as FR                   # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import tafeln_v8 as T                 # noqa: E402
import r3_gemein as R3                # noqa: E402
import luecke_r5 as L                 # noqa: E402
import szene_marker as SM             # noqa: E402

ROH = os.path.join(V.BAUM.replace('r41_sichel_v8', 'r41_sichel'), 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle')   # nur gelesen


def breit(zs):
    w = max(z.shape[1] for z in zs)
    return np.vstack([np.hstack([z, np.zeros((z.shape[0], w - z.shape[1], 3), np.uint8)]) for z in zs])


def tafel09(st, aus):
    f1 = T.bild(V.V80 + 'Beschreibung1_Vorlage.png')
    bx = (380, 130, 530, 260)
    zs = [T.reihe([T.kachel(f1, bx, 3, 'Foto 1 (n1485)')] + [T.kachel(T.render(b, T.P1), bx, 3, n + ' P1') for n, b in st])]
    sp = R3.szene_posen()
    for tk in (354, 300):
        A = FR.mit_bg(V.modell(st[0][1]), sp[tk])
        ys, xs = np.nonzero(A['bone'][:480] == 9)
        cx, cy = int(xs.mean()), int(ys.mean())
        x0, y0 = min(max(cx - 75, 0), 490), min(max(cy - 60, 0), 350)
        b2 = (x0, y0, x0 + 150, y0 + 130)
        zs.append(T.reihe([np.zeros((390, 450, 3), np.uint8)] + [T.kachel(T.render(b, sp[tk]), b2, 3, '%s Takt F%d' % (n, tk)) for n, b in st]))
    T.speichern(T.kopf(breit(zs), 'SF V8 09: von vorn nah am Kopf - Foto 1 | V7 | V8 r4 | V8 r5d | NEU: P1, Szene Takt F354 und F300 offline (roh, x3)'),
                os.path.join(aus, '09_vorn_kopf.png'))


def tafel15(st, aus):
    import vorlage_luecke_r5 as VL
    mos = [(n, V.modell(b)) for n, b in st]
    mo_neu = mos[-1][1]

    def bx_kopf(A, w=110, h=90):
        ys, xs = np.nonzero(A['bone'][:480] == 9)
        cx, cy = int(xs.mean()), int(ys.mean())
        x0, y0 = min(max(cx - w // 2, 0), 640 - w), min(max(cy - h // 2 - 10, 0), 480 - h)
        return (x0, y0, x0 + w, y0 + h)

    def paar(m_, pose, titel, bx):
        A = FR.mit_bg(m_, pose)
        B = FR.zeichne(m_, pose, None, licht=False)
        r, G = L.luecke(B)
        nt, mt = L.tiefe(B['bone'][:480], G)
        im = A['img'][:480]
        g = im.copy(); g[G] = (0, 255, 255)
        return T.kachel(im, bx, 3, titel), T.kachel(g, bx, 3, '%d px / %d tief / %.1f' % (r['kerbe'] + r['spalt'], nt, mt)), r['kerbe'] + r['spalt']

    zs = []
    for nm, pose, n in (('PV', V.PV, 1439), ('P2', R3.P2, 1435), ('P1', R3.P1, 1485)):
        bx = bx_kopf(FR.mit_bg(mo_neu, pose))
        vb = T.bild(os.path.join(ROH, 'f%04d.png' % (n + 1)))
        if nm == 'P1':
            mk = np.zeros(((bx[3] - bx[1]) * 3, (bx[2] - bx[0]) * 3, 3), np.uint8)
            T.schrift(mk, 'n1485: Maske nicht brauchbar', 6, 22, 0.5)
        else:
            H = FR.zeichne(mo_neu, pose, None, licht=False)['bone'][:480] == 9
            S_ = VL.de_bild(n) > 9
            s_, Gm = VL.kerben(S_, VL.kasten(H), H)
            vm = vb[:480].copy()
            vm[S_ & ~(cv2.erode(S_.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0)] = (255, 255, 0)
            vm[Gm] = (0, 255, 255)
            am = [s for s in s_ if s['kopf'] <= VL.AM_KOPF]
            mk = T.kachel(vm, bx, 3, 'Mass: ' + (', '.join('%d/%d/%.1f' % (s['px'], s['tief'], s['max']) for s in am[:2]) or '-'))
        a, b = [T.kachel(vb, bx, 3, 'Vorlage n%d (%s)' % (n, nm))], [mk]
        for name, m_ in mos:
            k1, k2, _s = paar(m_, pose, '%s %s' % (name, nm), bx)
            a.append(k1); b.append(k2)
        zs += [T.reihe(a), T.reihe(b)]
    sp = R3.szene_posen()
    rang = []
    for k in sorted(sp):
        r, _G = L.luecke(FR.zeichne(mo_neu, sp[k], None, licht=False))
        rang.append((r['kerbe'] + r['spalt'], k))
    takte = sorted(set(k for _w, k in sorted(rang, reverse=True)[:3]) | {311, 332})
    for k in takte:
        bx = bx_kopf(FR.mit_bg(mo_neu, sp[k]))
        ks = []
        for name, m_ in mos:
            _k1, k2, _s = paar(m_, sp[k], '', bx)
            T.schrift(k2, '%s Takt F%d' % (name, k), 6, 44, 0.5)
            ks.append(k2)
        ks.append(T.kachel(FR.mit_bg(mo_neu, sp[k])['img'][:480], bx, 3, 'NEU Takt F%d ohne Mass' % k))
        zs.append(T.reihe(ks))
    m4, m8 = KB.teile(st[1][1])[2][8], KB.teile(st[-1][1])[2][8]
    namen = (['d%d' % i for i in range(len(m4['tuv']), len(m8['tuv']))] + ['v%d' % i for i in range(len(m4['quv']), len(m8['quv']))])
    if namen:
        z6 = []
        for nm, pose in (('PV', V.PV), ('P2', R3.P2), ('P1', R3.P1), ('Takt F354', sp[354]), ('Takt F300', sp[300])):
            A = FR.mit_bg(mo_neu, pose)
            ein = (A['bone'][:480] == 8) & np.isin(A['flaeche'][:480], namen)
            ges = (A['bone'][:480] == 9) & (A['lok'][:480][..., 0] >= L.GESICHT_X)
            bx = bx_kopf(A)
            g = A['img'][:480].copy(); g[ein] = (0, 255, 0); g[ein & ges] = (255, 0, 255)
            z6.append(T.kachel(g, bx, 3, 'Nackenstueck %s: %d px' % (nm, int(ein.sum()))))
        zs.append(T.reihe(z6))
    T.speichern(T.kopf(breit(zs), 'SF V8 15 Ruecken/Kopf: Vorlage|V7|r4|r5d|NEU (PV, P2, P1; Luecke tuerkis); Szene-Takte V7|r4|r5d|NEU|NEU roh; '
                       'unten Nackenstueck gruen'),
                os.path.join(aus, '15_ruecken_kopf.png'))


def fd(d, f):
    return T.bild(os.path.join(d, 'fd', 'f%06d.ppm' % f))


def tafel17(laeufe, ko_v7, aus):
    zs = []
    for f in (352, 366, 372, 278, 306):
        m = SM.mag(SM.bild(os.path.join(ko_v7, 'fd', 'f%06d.ppm' % f)))
        ys, xs = np.nonzero(m)
        cx, cy = int(xs.mean()), int(ys.mean())
        x0, y0 = min(max(cx - 160, 0), 960 - 320), min(max(cy - 130, 0), 720 - 240)
        zs.append(T.reihe([T.kachel(fd(d, f), (x0, y0, x0 + 320, y0 + 240), 2, '%s F%d' % (n, f)) for n, d in laeufe]))
    T.speichern(T.kopf(breit(zs), 'SF V8 17: Kragen weg? Vorderansichten der echten Szenenbilder F352, F366, F372, F278, F306 - V7 | V8 r5d | NEU, '
                       'gleicher Ausschnitt (Framedump 960x720, roh, x2)'), os.path.join(aus, '17_kragen_weg.png'))


def tafel_tonne(laeufe, gr, aus):
    zs = []
    for f in (315, 318, 321, 324):
        g = SM.gr(SM.bild(os.path.join(gr, 'fd', 'f%06d.ppm' % f)))
        ys, xs = np.nonzero(g)
        x0, y0, x1, y1 = max(int(xs.min()) - 10, 0), max(int(ys.min()) - 10, 0), min(int(xs.max()) + 10, 960), min(int(ys.max()) + 10, 720)
        zs.append(T.reihe([T.kachel(fd(d, f), (x0, y0, x1, y1), 2, '%s F%d' % (n, f)) for n, d in laeufe]))
    T.speichern(T.kopf(breit(zs), 'SF V8 14b: "hellrote Tonne mit grauen Reifen" F315-F324 - echte Szenenbilder V7 | V8 r5d | NEU (Framedump, roh, x2)'),
                os.path.join(aus, '14b_tonne.png'))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--v7', required=True); ap.add_argument('--r4', required=True); ap.add_argument('--r5d', required=True)
    ap.add_argument('--neu', default=V.ASSET)
    ap.add_argument('--fd-v7'); ap.add_argument('--fd-r5d'); ap.add_argument('--fd-neu'); ap.add_argument('--ko-v7'); ap.add_argument('--gr')
    ap.add_argument('--aus', default=os.path.join(V.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v8'))
    ap.add_argument('--nur', default='09,15,17,14b')
    a = ap.parse_args()
    neu = open(a.neu, 'rb').read()
    st = [('V7', open(a.v7, 'rb').read()), ('V8 r4', open(a.r4, 'rb').read()), ('V8 r5d', open(a.r5d, 'rb').read()), ('NEU ' + V.sha(neu)[:8], neu)]
    nur = a.nur.split(',')
    if '09' in nur:
        tafel09(st, a.aus)
    if '15' in nur:
        tafel15(st, a.aus)
    laeufe = [('V7', a.fd_v7), ('V8 r5d', a.fd_r5d), ('NEU ' + V.sha(neu)[:8], a.fd_neu)]
    if '17' in nur and a.fd_neu and a.ko_v7:
        tafel17(laeufe, a.ko_v7, a.aus)
    if '14b' in nur and a.fd_neu and a.gr:
        tafel_tonne(laeufe, a.gr, a.aus)


if __name__ == '__main__':
    main()
