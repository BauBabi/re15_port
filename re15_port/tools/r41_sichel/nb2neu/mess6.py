"""NB6 HSF: alle Messwerte der Abnahme 4 fuer einen Lauf am GLEICHEN Bild (Video h = n1350+h, Port h ueber die Messzeile).
Masken und Kaesten unveraendert aus SF_belege/abnahme4/werkzeuge (sm4, smfarbe, halten4, mess4, umriss4, rotkante, norm4);
neu: Videoschwarz (c6.vsw) fuer alle Helligkeitswerte des Videos ('Vk' = Video korrigiert), Bodenschatten des Spider-Man.
python mess6.py <lauf> [teile]   teile: a,b1,b2,b3,b4,b5,c (Standard alle) -> <NB6_RUNS>/m6_<lauf>.json"""
import sys, json, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from c6 import *
run = sys.argv[1]
TEILE = sys.argv[2].split(',') if len(sys.argv) > 2 else ['a', 'b1', 'b2', 'b3', 'b4', 'b5', 'c']
R = {}
Pr = lambda h: port(run, h)
Pw = lambda h: cv2.GaussianBlur(port(run, h), (0, 0), 1.35)
k3 = np.ones((3, 3), np.uint8)
def bgm(src):
    return np.median(np.stack([src(h) for h in range(100, 121, 2)]), axis=0).astype(np.float32)
VB = bgm(vid); PB = bgm(Pr)
def dmask(im, B, box):
    x0, x1, y0, y1 = box
    d = np.abs(im.astype(np.float32) - B).max(axis=2); m = (d > 45).astype(np.uint8)
    mm = np.zeros_like(m); mm[y0:y1, x0:x1] = m[y0:y1, x0:x1]
    return cv2.morphologyEx(mm, cv2.MORPH_OPEN, k3) > 0
def klassen(im, m):
    """smfarbe.py: schwarz Luma<25, dunkelbraun/rot 25..70 & r>b+8, hell gelb/orange Luma>=70 & r>b+40 & g>b+20"""
    c = im[m].astype(int); b, g, r = c[:, 0], c[:, 1], c[:, 2]; L = 0.299 * r + 0.587 * g + 0.114 * b
    if len(L) < 20: return None
    sw = L < 25; db = (L >= 25) & (L < 70) & (r > b + 8); hg = (L >= 70) & (r > b + 40) & (g > b + 20)
    return [len(L), 100 * sw.mean(), 100 * db.mean(), 100 * hg.mean(), int(hg.sum())] + (
        [int(np.median(r[hg])), int(np.median(g[hg])), int(np.median(b[hg]))] if hg.sum() > 20 else [0, 0, 0])
def gelb3(im):
    b, g, r = [im[..., i].astype(int) for i in range(3)]
    return (r > 70) & (g > 45) & (r - b > 35) & (g - b > 15) & (r < 1.9 * g)
def stat_maske(m):
    ys, xs = np.nonzero(m)
    if len(xs) < 50: return None
    return dict(A=int(len(xs)), top=int(np.percentile(ys, 5)), bot=int(np.percentile(ys, 95)), x05=int(np.percentile(xs, 5)),
                x95=int(np.percentile(xs, 95)), cx=int(xs.mean()), cy=int(ys.mean()))

if 'a' in TEILE:
    # --- A Farbklassen (smfarbe.py), Video roh und schwarzkorrigiert
    PH = {'Ansprung': (range(2, 10), (0, 300, 45, 435)), 'Halten': (range(24, 60, 4), (60, 330, 45, 260)),
          'Flucht': (range(80, 86), (380, 640, 45, 435))}
    F = {}
    for name, (hs, box) in PH.items():
        acc = {'V': [], 'Vk': [], 'P': [], 'Pw': []}
        for h in hs:
            v = vid(h); mv = dmask(v, VB, box); p = Pr(h); mp = dmask(p, PB, box)
            acc['V'].append(klassen(v, mv)); acc['Vk'].append(klassen(vsw(v), mv)); acc['P'].append(klassen(p, mp))
            acc['Pw'].append(klassen(cv2.GaussianBlur(p, (0, 0), 1.35), mp))
        F[name] = {}
        for t, L in acc.items():
            L = [x for x in L if x]
            a = np.array([x[:5] for x in L], float); meds = [x[5:] for x in L if x[4] > 20]
            F[name][t] = dict(maske=int(a[:, 0].mean()), schwarz=round(a[:, 1].mean(), 1), dunkelbraun_rot=round(a[:, 2].mean(), 1),
                              hell_proz=round(a[:, 3].mean(), 1), hell_px=int(a[:, 4].mean()),
                              hell_median=np.median(np.array(meds), axis=0).astype(int).tolist() if meds else None)
            print('A %-8s %-2s Maske %6d schwarz %4.1f%% dunkelbraun/rot %4.1f%% hell %4.1f%% (%5d px) Median hell %s' % (
                name, t, F[name][t]['maske'], F[name][t]['schwarz'], F[name][t]['dunkelbraun_rot'], F[name][t]['hell_proz'],
                F[name][t]['hell_px'], F[name][t]['hell_median']))
    R['A_farbklassen'] = F
    # --- A Lage Ansprung (sm4.py Differenzmaske x<300) und Halten (halten4.py Kasten x 60-320 y 45-240)
    L = {}
    for h in range(0, 17, 2):
        L[h] = dict(V=stat_maske(dmask(vid(h), VB, (0, 300, 45, 435))), P=stat_maske(dmask(Pr(h), PB, (0, 300, 45, 435))))
    R['A_ansprung_diffmaske'] = L
    for h in (4, 8, 10, 12):
        v, p = L[h]['V'], L[h]['P']
        print('A Ansprung h%-2d V A%5d y%3d-%3d x%3d-%3d c(%3d,%3d) | P A%5d y%3d-%3d x%3d-%3d c(%3d,%3d)' % (
            h, v['A'], v['top'], v['bot'], v['x05'], v['x95'], v['cx'], v['cy'], p['A'], p['top'], p['bot'], p['x05'], p['x95'], p['cx'], p['cy']))
    H = {}
    for h in (20, 28, 40, 53, 64):
        H[h] = dict(V=stat_maske(dmask(vid(h), VB, (60, 320, 45, 240))), P=stat_maske(dmask(Pr(h), PB, (60, 320, 45, 240))))
        v, p = H[h]['V'], H[h]['P']
        print('A Halten   h%-2d V A%5d y%3d-%3d x%3d-%3d c(%3d,%3d) | P A%5d y%3d-%3d x%3d-%3d c(%3d,%3d)' % (
            h, v['A'], v['top'], v['bot'], v['x05'], v['x95'], v['cx'], v['cy'], p['A'], p['top'], p['bot'], p['x05'], p['x95'], p['cx'], p['cy']))
    R['A_halten_diffmaske_x60-320_y45-240'] = H
    # Gelb-Maske der Abnahme 3 (Lage), ortsfestes Gelb aus h100-h110 heraus
    sv = np.zeros((480, 640), bool); sp = np.zeros((480, 640), bool)
    for h in range(100, 111): sv |= gelb3(vid(h)); sp |= gelb3(Pw(h))
    sv = cv2.dilate(sv.astype(np.uint8), np.ones((7, 7), np.uint8)).astype(bool)
    sp = cv2.dilate(sp.astype(np.uint8), np.ones((7, 7), np.uint8)).astype(bool)
    def smg(im, st, xmax=640, ymax=436):
        m = gelb3(im) & ~st; m[:45] = 0; m[ymax:] = 0; m[:, xmax:] = 0; ys, xs = np.nonzero(m)
        if len(xs) <= 40: return None
        return [int(xs.mean()), int(ys.mean()), int(len(xs)), int(np.percentile(ys, 5)), int(np.percentile(ys, 95)), int(xs.min()), int(xs.max())]
    G = {}
    for name, hs, xm, ym in (('ansprung', range(2, 15, 2), 330, 436), ('halten', (20, 28, 40, 53, 64), 460, 300),
                             ('wurf', (68, 70, 72, 74, 76), 640, 436), ('flucht', range(79, 88), 640, 436)):
        G[name] = {h: dict(V=smg(vid(h), sv, xm, ym), P=smg(Pw(h), sp, xm, ym)) for h in hs}
        for h in hs: print('gelb3 %-8s h%-2d V %s P %s' % (name, h, G[name][h]['V'], G[name][h]['P']))
    R['gelb_abn3maske_[cx,cy,n,y5,y95,xmin,xmax]'] = G

if 'b1' in TEILE:
    # --- B1 Bodenschatten des Spider-Man: Boden x 0-320, y 350-430; Bildpunkt dunkler als 0,6 x eigener Hintergrund
    # (Median h100-h120; Video schwarzkorrigiert, Port roh) -> Anzahl, x-Spanne, y-Spanne
    VBk = bgm(lambda h: vsw(vid(h))).astype(np.float32); VBkY = Yg(VBk.astype(np.uint8)); PBY = Yg(PB.astype(np.uint8))
    S = {}
    for h in (0, 2, 4, 6, 8, 10, 12, 16, 20, 30, 40, 50, 60, 70):
        r = {}
        for t, im, BY in (('Vk', vsw(vid(h)), VBkY), ('P', Pr(h), PBY)):
            Y = Yg(im); m = (Y < 0.6 * BY) & (BY > 30); mm = np.zeros_like(m); mm[350:430, 0:320] = m[350:430, 0:320]
            mm = cv2.morphologyEx(mm.astype(np.uint8), cv2.MORPH_OPEN, k3) > 0
            ys, xs = np.nonzero(mm)
            r[t] = [int(len(xs))] + ([int(np.percentile(xs, 3)), int(np.percentile(xs, 97)), int(np.percentile(ys, 3)), int(np.percentile(ys, 97))] if len(xs) > 30 else [])
        S[h] = r; print('B1 Schatten h%-2d Vk %s P %s' % (h, r['Vk'], r['P']))
    R['B1_schatten_x0-320_y350-430_[n,x3,x97,y3,y97]'] = S

if 'b2' in TEILE:
    # --- B2 Flucht-Start: Differenzmaske im ganzen Bild unten links/mittig + Gelb (s. a)
    D = {}
    for h in range(78, 88):
        D[h] = dict(V=stat_maske(dmask(vid(h), VB, (0, 640, 45, 435)) & ~dmask(vid(100), VB, (0, 640, 45, 435))),
                    P=stat_maske(dmask(Pr(h), PB, (0, 640, 45, 435)) & ~dmask(Pr(100), PB, (0, 640, 45, 435))))
        print('B2 Flucht h%-2d V %s | P %s' % (h, D[h]['V'], D[h]['P']))
    R['B2_flucht_diffmaske_ohne_birkin_h100'] = D

if 'b3' in TEILE:
    def gruen(im):
        b, g, r = [im[..., i].astype(int) for i in range(3)]; return (g > r + 14) & (g > b + 6) & (g > 45)
    gv = np.zeros((480, 640), bool); gp = np.zeros((480, 640), bool)
    for h in range(100, 121, 2): gv |= gruen(vid(h)); gp |= gruen(Pw(h))
    gv = cv2.dilate(gv.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool); gp = cv2.dilate(gp.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
    G = {}; sv_ = sp_ = 0
    for h in range(18, 93):
        mv = gruen(vid(h)) & ~gv; mp = gruen(Pw(h)) & ~gp; mv[:45] = mv[436:] = 0; mp[:45] = mp[436:] = 0
        sv_ += int(mv.sum()); sp_ += int(mp.sum())
        if h in (28, 34, 40, 48, 53, 56, 58, 64, 70, 84, 86, 88, 90):
            def info(im, m, imk=None):
                ys, xs = np.nonzero(m)
                if len(xs) < 20: return [int(len(xs))]
                c = (imk if imk is not None else im)[m]
                return [int(len(xs)), int(np.percentile(xs, 5)), int(np.percentile(xs, 95)), int(np.percentile(ys, 5)), int(np.percentile(ys, 95)),
                        [int(np.median(c[:, 2])), int(np.median(c[:, 1])), int(np.median(c[:, 0]))]]
            G[h] = dict(V=info(vid(h), mv), Vk=info(vid(h), mv, vsw(vid(h))), P=info(Pw(h), mp))
            print('B3 gruen h%-2d V %s Vk-Farbe %s | P %s' % (h, G[h]['V'], G[h]['Vk'][-1] if len(G[h]['Vk']) > 1 else '-', G[h]['P']))
    # Saeule h53-h58 im Kasten x 320-365 y 95-150 (Video-Lage der Abnahme)
    sa = {h: [int((gruen(vid(h)) & ~gv)[95:150, 320:365].sum()), int((gruen(Pw(h)) & ~gp)[95:150, 320:365].sum())] for h in (53, 56, 58)}
    print('B3 Saeule x320-365 y95-150 V/P', sa)
    R['B3_gruen_[n,x5,x95,y5,y95,rgb]'] = G; R['B3_gruen_summe_h18_h92_V_P'] = [sv_, sp_]; R['B3_saeule_x320-365_y95-150'] = sa
    print('B3 gruen Summe h18-h92 V %d P %d' % (sv_, sp_))

if 'b4' in TEILE or 'c' in TEILE:
    # rotkante.py: Haut-Oberkante (helligkeitsnormiert, Gauss 1,35)
    B0 = Yg(bg())
    def norm(im):
        g = Yg(im); s = (slice(60, 420), slice(10, 140))
        A = np.vstack([B0[s].ravel(), np.ones(B0[s].size)]).T
        (a, b), *_ = np.linalg.lstsq(A, g[s].ravel(), rcond=None)
        return np.clip((im.astype(np.float32) - b) / max(a, 0.05), 0, 255).astype(np.uint8), a
    def haut(im):
        n, a = norm(cv2.GaussianBlur(im, (0, 0), 1.35))
        c = n.astype(int); b, g, r = c[..., 0], c[..., 1], c[..., 2]
        m = (r > g + 18) & (r > b + 10) & (r > 45); m[:60] = 0; m[436:] = 0; m[:, :230] = 0
        m = cv2.morphologyEx(m.astype(np.uint8), cv2.MORPH_OPEN, k3) > 0
        rows = np.nonzero(m.sum(axis=1) >= 6)[0]; ys, xs = np.nonzero(m)
        if len(xs) < 100: return None
        return [int(rows.min()) if len(rows) else None, int(np.percentile(xs, 2)), int(np.percentile(xs, 98)), int(len(xs))]
    HK = {}
    for h in (84, 88, 92, 96, 98, 100, 102, 124, 132, 140, 146, 148, 152, 156):
        HK[h] = dict(V=haut(vid(h)), P=haut(Pr(h)))
        print('B4/C Hautkante h%-3d V [oben,x2,x98,A] %s | P %s' % (h, HK[h]['V'], HK[h]['P']))
    R['B4_hautkante_[oben,x2,x98,A]'] = HK

if 'b5' in TEILE:
    B0 = Yg(bg())
    def norm(im):
        g = Yg(im); s = (slice(60, 420), slice(10, 140))
        A = np.vstack([B0[s].ravel(), np.ones(B0[s].size)]).T
        (a, b), *_ = np.linalg.lstsq(A, g[s].ravel(), rcond=None)
        return np.clip((im.astype(np.float32) - b) / max(a, 0.05), 0, 255).astype(np.uint8), a
    T = {}
    for h in (100, 112, 124, 132, 140, 146, 152, 158):
        def q(im):
            n, a = norm(im); c = n[180:290, 340:460].reshape(-1, 3).astype(int); L = 0.299 * c[:, 2] + 0.587 * c[:, 1] + 0.114 * c[:, 0]
            return [round(float(a), 2), int(np.percentile(L, 10)), int(np.percentile(L, 50)), int(np.percentile(L, 90))]
        T[h] = dict(V=q(vid(h)), Vk=q(vsw(vid(h))), P=q(Pw(h)))
        print('B5 Rumpf x340-460 y180-290 normiert h%-3d [a,L10,L50,L90] V %s Vk %s | P %s' % (h, T[h]['V'], T[h]['Vk'], T[h]['P']))
    R['B5_rumpf_normiert'] = T
    S = {}
    for h in (0, 100, 116, 124, 132):
        def q(im, kor=False):
            n, a = norm(im); L = Yg(n)[180:245, 320:460]
            return [int(np.percentile(L, 25)), int(np.percentile(L, 50)), int(np.percentile(L, 90)), round(float((L > 110).mean() * 100), 1)]
        S[h] = dict(V=q(vid(h)), Vk=q(vsw(vid(h))), P=q(Pw(h)))
        print('B5 Schulter x320-460 y180-245 normiert h%-3d [p25,p50,p90,%%>110] V %s Vk %s | P %s' % (h, S[h]['V'], S[h]['Vk'], S[h]['P']))
    R['B5_schulter_normiert'] = S

if 'c' in TEILE:
    R['C_kappe_x340-380_y160-185'] = {h: [round(100 * rot(vid(h), 340, 380, 160, 185) / 1000, 1), round(100 * rot(Pw(h), 340, 380, 160, 185) / 1000, 1)] for h in (0, 3)}
    R['C_rot_oben_x395-435_y270-335'] = {h: [rot(vid(h), 395, 435, 270, 335), rot(Pw(h), 395, 435, 270, 335)] for h in (60, 62, 64, 70)}
    R['C_streifen_schwach_x395-475_y260-420'] = {h: [rotw(vid(h), 395, 475, 260, 420), rotw(Pw(h), 395, 475, 260, 420)] for h in (36, 44, 52, 56)}
    def dn(a, b, x0=280, x1=520, y0=60, y1=440): return int((np.abs(a.astype(int) - b.astype(int)).max(2)[y0:y1, x0:x1] > 20).sum())
    R['C_bewegung_birkinkasten'] = {h: [dn(vid(h), vid(h - 1)), dn(Pr(h), Pr(h - 1))] for h in (70, 71, 72, 96, 97, 98)}
    for k in ('C_kappe_x340-380_y160-185', 'C_rot_oben_x395-435_y270-335', 'C_streifen_schwach_x395-475_y260-420', 'C_bewegung_birkinkasten'):
        print(k, R[k])

ziel = os.path.join(RUNS, 'm6_%s_%s.json' % (run, '-'.join(TEILE)))
json.dump(R, open(ziel, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
print('->', ziel)
