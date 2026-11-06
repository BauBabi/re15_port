"""Riegel V3 der Sichel-Form (Messblatt V3 Abschnitt 8, R1-R32) - Exit 1, wenn eine Zeile rot ist.

    python riegel_v3.py <EMD> [--lauf debug.log] [--json aus.json] [--bilder DIR]

Messwege (Messblatt V3 Abschnitt 8, deterministisch, keine Handgrenzen ausser den genannten Fenstern):
  Kopf R1-R14: Messansicht "Kopf vorn nah" (birkin_formen.nah_raster / EA.render wie P_port_kopf.nah: Pose Clip 1 Bild 0,
    Mitte T[9] + (150,-150,0), Blick von vorn, neutrales Licht, Farbe x 1,35, 0,9 px/E., H = 0,9 x 3865 = 3479 px). Die Leinwand
    ist 1200 statt 960 px breit (Mitte x 720 statt 480): das Schulterauge liegt 0,117 H = 407 px links vom Kopfauge, mit 960 px
    laege das Messfenster ausserhalb des Bildes. Massstab und Lage zur Figur sind gleich.
    Augen (R1): auf Mesh 9 oberhalb des Mauls dunkle Flecken L* < 0,6 x Median-L* der Kopfhaut dort, Flecken naeher als
    0,008 H zusammengefasst (ein Ringauge ist EIN Auge), Flaeche >= (0,004 H)^2. Kopfauge = groesster Fleck.
    R3-R9: P_kopf.auge auf der auf H 569 verkleinerten Ansicht (INTER_AREA, Faktor 569/3479), Fenster +-12 px um das Kopfauge
    bzw. um die erwartete Lage des Schulterauges (Kopfauge + (-0,117, +0,055) H, Messblatt K12).
    R10-R14: Maul an der vollen Ansicht, Fenster x = Kopfbreite (Mesh 9), y = Augenmitte + 0,025 H .. + 0,10 H; dunkle Flaeche
    = Komponente L* < 0,5 x Fenster-Median mit dem dunkelsten Punkt (Gauss 3), Loecher gefuellt (P_port_kopf.maul_port);
    Zaehne = L* > Median + 15 im 4-px-Saum ausserhalb der dunklen Flaeche, 8er-Komponenten, oben/unten nach Bbox-Mitte.
  Schwanz R15-R27: P_port.messen (Spielkamera 50A0/12, RDT-Licht, Posen sfv2/posen_seite.json h85 und h92 - beide muessen bestehen).
  Machart R28-R32: machart_v3.pruefe (Referenz 2 = Spiel-Asset EM30_HANGAR.EMD).
  Bewegung (R25b/R26b, nur mit --lauf): Schwanzgelenke je Szenentakt aus debug.log ("[hangar-takt] hN Schwanz B y ..."),
    in die Spielkamera projiziert (b2.projiziere); Gelenkwinkel im Bild je Takt h84-h132 (Gang) und h16-h70 (Halten):
    hoechstens 3 Gelenke > 20 Grad, keines > 36 Grad (Gegenstueck zu R25/R26 bei 7 starren Gliedern: ein Glied = 0,13 H > 0,05 H).
"""
import os, sys, json, re, argparse
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
sys.path.insert(0, os.path.dirname(os.path.dirname(HIER)))
import emd_ansichtsblatt as EA
import birkin_formen as BF
import b2
from P_farbe import lab, r1
from P_kopf import auge
import P_port
import machart_v3

SFV2 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2'
NAH = BF.KOPF_V3['nah']
HP = NAH['H_px']
H569 = 569


def nah_bild(mo):
    r = BF.nah_raster(mo, NAH)
    V, N, F = r['V'], r['N'], r['F']
    col, alp = EA.render(mo, V - r['mitte'], N, F, np.array([0, 0, 1.0]), np.array([0.0, 1.0, 0.0]), np.array([1.0, 0, 0]),
                         NAH['breite'], NAH['hoehe'], NAH['massstab'], NAH['cx'], NAH['cy'])
    return (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8), alp, r


def maul_messen(rgb, fenster):
    x0, y0, x1, y1 = fenster
    A = lab(rgb); L = A[..., 0]
    sub = L[y0:y1, x0:x1]; Asub = A[y0:y1, x0:x1]
    Lh = float(np.median(sub))
    iy, ix = np.unravel_index(np.argmin(nd.gaussian_filter(sub, 3)), sub.shape)
    lb, _ = nd.label(sub < 0.5 * Lh)
    m = nd.binary_fill_holes(lb == lb[iy, ix])
    z = (sub > Lh + 15) & nd.binary_dilation(m, iterations=4) & ~m
    zl, zn = nd.label(z, structure=np.ones((3, 3)))
    mm = m | z
    ys, xs = np.nonzero(mm)
    mitte_y = (ys.min() + ys.max()) / 2.0
    teile = []
    for i in range(1, zn + 1):
        yy, xx = np.nonzero(zl == i)
        teile.append(dict(b_px=int(xx.max() - xx.min() + 1), y=float(yy.mean())))
    oben = sum(1 for t in teile if t['y'] < mitte_y); unten = zn - oben
    return dict(bbox=[int(xs.min() + x0), int(ys.min() + y0), int(xs.max() + x0), int(ys.max() + y0)],
                b_H=round((xs.max() - xs.min() + 1) / HP, 4), h_H=round((ys.max() - ys.min() + 1) / HP, 4),
                bh=round((xs.max() - xs.min() + 1) / (ys.max() - ys.min() + 1), 2),
                mitte=[float((xs.min() + xs.max()) / 2 + x0), float(mitte_y + y0)],
                lab_innen=r1(Asub[m & (sub < 0.5 * Lh)].mean(0)), lab_zahn=(r1(Asub[z].mean(0)) if z.any() else None),
                zahn_teile=int(zn), oben=int(oben), unten=int(unten),
                zahn_b_max_H=round(max([t['b_px'] for t in teile], default=0) / HP, 4), L_median=round(Lh, 1)), mm


def kopf(emd, bilder=None):
    mo = BF.modell_aus_blob(open(emd, 'rb').read())
    b, alp, r = nah_bild(mo)
    A = lab(b.astype(float)); L = A[..., 0]
    k9 = r['knochen'] == 9
    ys9, xs9 = np.nonzero(k9)
    # Maul grob (fuer "oberhalb des Mauls"): groesste dunkle Komponente auf Mesh 9
    d9 = k9 & (L < 0.5 * np.median(L[k9]))
    lb, n = nd.label(d9)
    gr = nd.sum(d9, lb, range(1, n + 1))
    mk = lb == 1 + int(np.argmax(gr))
    maul_oben = int(np.nonzero(mk.any(1))[0].min())
    # R1 Augen auf dem Kopf
    oberhalb = k9.copy(); oberhalb[maul_oben - int(0.01 * HP):] = False
    Lk = float(np.median(L[oberhalb]))
    dunkel = oberhalb & (L < 0.6 * Lk)
    dunkel = nd.binary_opening(dunkel, iterations=2)
    grp, ng = nd.label(nd.binary_dilation(dunkel, iterations=int(round(0.004 * HP))))
    flecken = []
    for i in range(1, ng + 1):
        f = dunkel & (grp == i)
        if f.sum() >= (0.004 * HP) ** 2:
            # nur ein Fleck, der gegen seine Umgebung dunkel ist (Ring 0,004-0,012 H auf Mesh 9): Schattenseiten des Kopfs
            # (Flaechen vom Licht weg) sind grossflaechig dunkel, aber kein Auge
            ring = nd.binary_dilation(f, iterations=int(0.012 * HP)) & ~nd.binary_dilation(f, iterations=int(0.004 * HP)) & k9
            Lr = float(np.median(L[ring])) if ring.any() else 0.0
            yy, xx = np.nonzero(f)
            if float(L[f].mean()) < 0.6 * Lr:
                flecken.append(dict(n=int(f.sum()), mitte=[float(xx.mean()), float(yy.mean())], L=round(float(L[f].mean()), 1),
                                    L_umgebung=round(Lr, 1)))
    flecken.sort(key=lambda f: -f['n'])
    erg = dict(R1=dict(kopfaugen=len(flecken), flecken=flecken[:4]))
    if not flecken:
        return erg
    ax, ay = flecken[0]['mitte']
    # verkleinert auf H 569
    s = H569 / HP
    W569, H569b = int(round(b.shape[1] * s)), int(round(b.shape[0] * s))
    b5 = cv2.resize(b, (W569, H569b), interpolation=cv2.INTER_AREA).astype(float)
    a5 = (cv2.resize(alp.astype(np.float32), (W569, H569b), interpolation=cv2.INTER_AREA) * 255).astype(np.uint8)
    cx, cy = int(round(ax * s)), int(round(ay * s))
    ka, kam = auge(b5, a5, (cx - 12, cy - 12, cx + 13, cy + 13), H569)
    sx, sy = int(round((ax - 0.117 * HP) * s)), int(round((ay + 0.055 * HP) * s))
    sa, sam = auge(b5, a5, (sx - 12, sy - 12, sx + 13, sy + 13), H569)
    erg['kopfauge'] = ka; erg['schulterauge'] = sa
    erg['R2'] = dict(dx_H=round((sa['mitte'][0] - ka['mitte'][0]) / H569, 4), dy_H=round((sa['mitte'][1] - ka['mitte'][1]) / H569, 4),
                     L_dunkel_haut=sa['L_dunkel_haut'])
    # Maul
    x0, x1 = int(xs9.min()), int(xs9.max()) + 1
    fen = (x0, int(ay + 0.025 * HP), x1, int(ay + 0.10 * HP))
    mu, mum = maul_messen(b.astype(float), fen)
    erg['maul'] = mu
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    rr = np.hypot(xx - ax, yy - ay)
    ring = (rr >= 0.0105 * HP) & (rr <= 0.0158 * HP) & k9
    lab_haut = A[ring].mean(0)
    erg['haut_auge'] = r1(lab_haut)
    erg['R12'] = dict(unter_auge_H=round((mu['mitte'][1] - ay) / HP, 4), versatz_H=round(abs(mu['mitte'][0] - ax) / HP, 4))
    erg['R13'] = dict(L_innen_haut=round(mu['lab_innen'][0] / lab_haut[0], 3), da=round(mu['lab_innen'][1] - lab_haut[1], 1))
    erg['auge_px'] = [ax, ay]
    if bilder:
        vis = b.copy()
        for msk, col in ((mum, (255, 0, 255)),):
            c, _ = cv2.findContours(msk.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(vis[fen[1]:fen[3], fen[0]:fen[2]], c, -1, col, 2)
        cv2.circle(vis, (int(ax), int(ay)), int(0.02 * HP), (0, 255, 0), 2)
        cv2.circle(vis, (int((sa['mitte'][0]) / s), int((sa['mitte'][1]) / s)), int(0.02 * HP), (0, 255, 255), 2)
        cv2.rectangle(vis, (fen[0], fen[1]), (fen[2], fen[3]), (255, 255, 0), 1)
        os.makedirs(bilder, exist_ok=True)
        cv2.imwrite(os.path.join(bilder, 'riegel_kopf_%s.png' % os.path.basename(emd)[:-4]), cv2.cvtColor(vis, cv2.COLOR_RGB2BGR))
        cv2.imwrite(os.path.join(bilder, 'nah_%s.png' % os.path.basename(emd)[:-4]), cv2.cvtColor(b, cv2.COLOR_RGB2BGR))
    return erg


def schwanz(emd, bilder=None):
    P = json.load(open(os.path.join(SFV2, 'posen_seite.json')))
    out = {}
    for h in ('85', '92'):
        d, vis = P_port.messen(emd, int(h), P[h])
        out[h] = d
        if bilder:
            cv2.imwrite(os.path.join(bilder, 'riegel_schwanz_%s_h%s.png' % (os.path.basename(emd)[:-4], h)), cv2.cvtColor(vis, cv2.COLOR_RGB2BGR))
    return out


def lauf_lesen(pfad):
    g = {}
    for z in open(pfad, errors='replace'):
        m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', z)
        if m:
            g[int(m.group(1))] = np.array([tuple(map(int, t)) for t in re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(3))], float)
    return g


def bewegung(pfad):
    g = lauf_lesen(pfad)
    welt = dict(gier=0, x=0, z=0, y=0)
    je = {}
    for h, Pw in sorted(g.items()):
        q = b2.projiziere(welt, Pw)
        d = np.diff(q, axis=0)
        a = np.unwrap(np.arctan2(d[:, 1], d[:, 0]))
        w = np.abs(np.degrees(np.diff(a)))
        # 3D-Gelenkwinkel zum Vergleich
        d3 = np.diff(Pw, axis=0); d3 /= np.linalg.norm(d3, axis=1, keepdims=True)
        w3 = np.degrees(np.arccos(np.clip((d3[:-1] * d3[1:]).sum(1), -1, 1)))
        je[h] = dict(bild=[round(float(x), 1) for x in w], raum=[round(float(x), 1) for x in w3])
    def zus(lo, hi):
        hs = [h for h in je if lo <= h <= hi]
        if not hs:
            return None
        mx = max(max(je[h]['bild']) for h in hs)
        n20 = max(sum(1 for x in je[h]['bild'] if x > 20) for h in hs)
        m3 = max(max(je[h]['raum']) for h in hs)
        return dict(takte=len(hs), max_bild_grad=round(mx, 1), max_knicke_je_takt=n20, max_raum_grad=round(m3, 1),
                    ok=mx <= 36 and n20 <= 3)
    return dict(gang_h84_h132=zus(84, 132), halten_h16_h70=zus(16, 70), je_takt=je)


def zeilen(k, s, m, bw=None):
    """Riegel-Tabelle: (Name, Soll, Ist, ok)."""
    Z = []
    def z(n, soll, ist, ok):
        Z.append((n, soll, ist, bool(ok)))
    ka, sa, mu = k.get('kopfauge'), k.get('schulterauge'), k.get('maul')
    z('R1 Kopfaugen', 'genau 1', str(k['R1']['kopfaugen']), k['R1']['kopfaugen'] == 1)
    if ka:
        r2 = k['R2']
        z('R2 Schulterauge', '-0,10..-0,13 / +0,04..+0,07 H, dunkel', '%.3f / %.3f H, L* dunkel/Haut %.2f' % (r2['dx_H'], r2['dy_H'], r2['L_dunkel_haut']),
          -0.13 <= r2['dx_H'] <= -0.10 and 0.04 <= r2['dy_H'] <= 0.07 and r2['L_dunkel_haut'] < 0.6)
        z('R3 Kopfauge Groesse', 'B 0,017-0,025 x H 0,008-0,014 H', '%.4f x %.4f' % (ka['b_H'], ka['h_H']),
          0.017 <= ka['b_H'] <= 0.025 and 0.008 <= ka['h_H'] <= 0.014)
        z('R4 Kopfauge Form', 'B/H 1,5-2,5', '%.2f' % ka['bh'], 1.5 <= ka['bh'] <= 2.5)
        z('R5 Kopfauge Helligkeit', 'L* Auge/Haut 0,35-0,55', '%.3f' % ka['L_auge_haut'], 0.35 <= ka['L_auge_haut'] <= 0.55)
        z('R6 kein heller Ring', 'lid_hell <= 0,10; L Lid/Haut 0,70-1,00', '%.3f; %.3f' % (ka['lid_hell'], ka['L_lid_haut']),
          ka['lid_hell'] <= 0.10 and 0.70 <= ka['L_lid_haut'] <= 1.00)
        g = ka.get('glanz_rel_box')
        z('R7 keine Sclera, ein Glanzkern', 'L80 <= 0,05; hell 0,05-0,25; Kern x 0,4-0,7 y 0,55-0,8',
          '%.3f; %.3f; %s' % (ka['anteil_L80'], ka['anteil_heller_haut'], g),
          ka['anteil_L80'] <= 0.05 and 0.05 <= ka['anteil_heller_haut'] <= 0.25 and g is not None and 0.4 <= g[0] <= 0.7 and 0.55 <= g[1] <= 0.8)
        z('R8 Pupille', 'L* Pupille/Haut <= 0,12', '%.3f' % ka['L_pup_haut'], ka['L_pup_haut'] <= 0.12)
        q = sa['aequiv_H'] / ka['aequiv_H'] if ka['aequiv_H'] else 0
        z('R9 Schulterauge', 'L* 0,30-0,50; hell <= 0,05; aequiv 0,8-1,3 x Kopfauge',
          '%.3f; %.3f; %.2f' % (sa['L_auge_haut'], sa['anteil_heller_haut'], q),
          0.30 <= sa['L_auge_haut'] <= 0.50 and sa['anteil_heller_haut'] <= 0.05 and 0.8 <= q <= 1.3)
        z('R10 Maul Groesse', 'B 0,075-0,095 x H 0,038-0,055 H', '%.4f x %.4f' % (mu['b_H'], mu['h_H']),
          0.075 <= mu['b_H'] <= 0.095 and 0.038 <= mu['h_H'] <= 0.055)
        z('R11 Maul Form', 'B/H 1,5-2,3', '%.2f' % mu['bh'], 1.5 <= mu['bh'] <= 2.3)
        r12 = k['R12']
        z('R12 Maul Lage', '0,04-0,07 H unter Auge; Versatz <= 0,012 H', '%.4f; %.4f' % (r12['unter_auge_H'], r12['versatz_H']),
          0.04 <= r12['unter_auge_H'] <= 0.07 and r12['versatz_H'] <= 0.012)
        r13 = k['R13']
        z('R13 Maul innen', 'L innen/Haut 0,20-0,40; da >= +4', '%.3f; %+.1f' % (r13['L_innen_haut'], r13['da']),
          0.20 <= r13['L_innen_haut'] <= 0.40 and r13['da'] >= 4)
        lz = mu['lab_zahn']; Lha = k['haut_auge'][0]
        z('R14 Zaehne', 'oben >= 5, unten >= 3; Teil <= 0,005 H; L* <= Haut+20; a* 12-20',
          '%d / %d; %.4f; L %.1f (Haut %.1f); a %.1f' % (mu['oben'], mu['unten'], mu['zahn_b_max_H'], lz[0] if lz else -1, Lha, lz[1] if lz else -1),
          lz is not None and mu['oben'] >= 5 and mu['unten'] >= 3 and mu['zahn_b_max_H'] <= 0.005 and lz[0] <= Lha + 20 and 12 <= lz[1] <= 20)
    def beide(f):
        return [f(s[h]) for h in ('85', '92')]
    def fmt(v, n=3):
        return ' / '.join(('%.' + str(n) + 'f') % x for x in v)
    v = beide(lambda d: d['farbe']['L_verh']); z('R15 Schwanz Helligkeit', 'L*/Haut 0,50-0,72 (h85 / h92)', fmt(v), all(0.50 <= x <= 0.72 for x in v))
    v = beide(lambda d: d['farbe']['C_verh']); z('R16 Schwanz Buntheit', 'C*/Haut 0,45-0,90', fmt(v, 2), all(0.45 <= x <= 0.90 for x in v))
    v = beide(lambda d: d['farbe']['dh']); z('R17 Schwanz Farbton', '|dh| <= 10 Grad', fmt(v, 1), all(abs(x) <= 10 for x in v))
    v = beide(lambda d: (d['farbe']['rot_anteil_schwanz'], d['farbe']['rot_anteil_haut']))
    z('R18 Schwanz Rot', 'Rot Schwanz <= 0,8 x Haut', ' / '.join('%.3f (Haut %.3f)' % x for x in v), all(a <= 0.8 * b for a, b in v))
    v = beide(lambda d: d['farbe']['de00_haut']); z('R19 Schwanz dE00', '6-11', fmt(v, 1), all(6 <= x <= 11 for x in v))
    v = beide(lambda d: (d['muster']['periode_a_ak'], d['muster']['rest_std_a']))
    z('R20 Muster', 'Periode <= 0,25; Rest-Std a* <= 2,0', ' / '.join('%.2f; %.2f' % x for x in v), all(a <= 0.25 and b <= 2.0 for a, b in v))
    for nm, key, lo, hi in (('R21 Dicke s 0,2', 0.2, 0.15, 0.20), ('R22 Dicke s 0,5', 0.5, 0.07, 0.10), ('R23 Dicke s 0,9', 0.9, 0.028, 0.045)):
        v = beide(lambda d: d['form']['dicke_H'][key])
        z(nm, '%.3f-%.3f H' % (lo, hi), fmt(v), all(lo <= x <= hi for x in v))
    def verlauf(d):
        br = d['form']['breiten_005_095']            # s = 0,05 .. 0,95 in 0,05
        teil = br[3:14]                               # s 0,20 .. 0,70
        return max(0.0, max(teil[i + 1] - teil[i] for i in range(len(teil) - 1)))
    v = beide(verlauf); z('R24 Dickenverlauf', 'kein Anstieg > 0,01 H (s 0,2-0,7)', fmt(v), all(x <= 0.01 for x in v))
    v = beide(lambda d: d['form']['max_dwinkel_005H']); z('R25 Glaette', '<= 36 Grad je 0,05 H', fmt(v, 1), all(x <= 36 for x in v))
    v = beide(lambda d: d['form']['knicke']); z('R26 Knicke', '<= 3 (> 20 Grad)', ' / '.join(str(x) for x in v), all(x <= 3 for x in v))
    v = beide(lambda d: d['austritt_unter_OK_H']); z('R27 Austritt', '0,10-0,16 H unter OK', fmt(v), all(0.10 <= x <= 0.16 for x in v))
    z('R28 Koerper unveraendert', 'Meshes 0-15/Skelett/EDD = Ref2 (ausser Mesh 9 UV, <= 1 Viereck Mesh 8)',
      'abweichend %s, Skelett %s, Keyframes %s, EDD %s, Mesh 8 +%d Viereck' % (m['R28']['meshes_abweichend'], m['R28']['skelett_0_15_gleich'],
                                                                  m['R28']['keyframes_0_15_gleich'], m['R28']['edd_gleich'], m.get('mesh8_neue_vierecke', 0)),
      m['R28']['ok'] and m.get('mesh8_neue_vierecke', 0) <= 1)
    z('R29 Dreiecke Schwanz', '<= 212', str(m['R29']['dreiecke_schwanz']), m['R29']['ok'])
    z('R30 Dreiecke Kopf', 'Mesh 9 <= 118; Schulterauge <= 2', '%d; %d' % (m['R30']['dreiecke_kopf'], m['R30']['dreiecke_schulterauge']), m['R30']['ok'])
    z('R31 Texeldichte', 'Schwanz 0,045-0,090; Kopf 0,05-0,091', '%s; %.3f' % (m['R31']['schwanz'], m['R31']['kopf']), m['R31']['ok'])
    z('R32 Farbtiefe/Palette', '8 bpp; Haut-CLUT; C* Schwanz <= C* Haut',
      '8bpp %s; Zeilen %s; C* %.1f <= %.1f' % (m['R32']['bpp8'], m['R32']['schwanz_clut_zeilen'], m['R32']['C_max_schwanz'], m['R32']['C_max_haut']), m['R32']['ok'])
    if bw:
        for nm, key in (('R25b/R26b Gang h84-h132 (Spiellauf)', 'gang_h84_h132'), ('R25b/R26b Halten h16-h70 (Spiellauf)', 'halten_h16_h70')):
            x = bw[key]
            if x:
                z(nm, 'Bild-Gelenkwinkel <= 36 Grad, <= 3 ueber 20 Grad', 'max %.1f Grad, %d ueber 20 (Raum max %.1f, %d Takte)' % (
                    x['max_bild_grad'], x['max_knicke_je_takt'], x['max_raum_grad'], x['takte']), x['ok'])
    return Z


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd')
    ap.add_argument('--lauf')
    ap.add_argument('--json')
    ap.add_argument('--bilder')
    a = ap.parse_args()
    k = kopf(a.emd, a.bilder)
    s = schwanz(a.emd, a.bilder)
    m = machart_v3.pruefe(a.emd)
    bw = bewegung(a.lauf) if a.lauf else None
    Z = zeilen(k, s, m, bw)
    rot = [z for z in Z if not z[3]]
    print('Riegel V3 - %s' % a.emd)
    for n, soll, ist, ok in Z:
        print('%-4s %-38s Soll %-62s Ist %s' % ('ok' if ok else 'ROT', n, soll, ist))
    print('ROT: %d von %d' % (len(rot), len(Z)))
    if a.json:
        json.dump(dict(kopf=k, schwanz=s, machart=m, bewegung=({kk: v for kk, v in bw.items() if kk != 'je_takt'} if bw else None),
                       zeilen=[dict(mass=n, soll=soll, ist=ist, ok=ok) for n, soll, ist, ok in Z]), open(a.json, 'w'), indent=1, default=str)
    sys.exit(1 if rot else 0)


if __name__ == '__main__':
    main()
