"""Riegel V4 der Sichel-Form (analysis/befunde_runde41/SF_MESSBLATT_V4.md Abschnitt 7) - Exit 1, wenn eine Zeile rot ist.

    python riegel_v4.py <EMD> [--lauf debug.log] [--fd LAUFORDNER] [--json aus.json] [--md aus.md] [--kein-exit]

Render fuer alle Riegel: b2.zeichne (Spielkamera 50A0 Schnitt 12, RDT-Licht), 640 x 480, Hintergrund = BSS 50A0/12 (v4_gemein), Posen
posen_v4.json "135" (vorn, Vorlage 1) und "85" (seitlich, Vorlage 2). Keyframe ohne Feder (Ruhe = Keyframe, Messblatt Abschnitt 4).
H_port = Sohle - Oberkante der Koerper-Ids ohne 16-22 auf der nach unten verlaengerten Leinwand (in Pose 135 liegt die Sohle unter
Zeile 479). Kopfbox = Bbox der Pixel mit Id 9. Farben: Vergleichsregel (Gauss sigma 1,5, V' = a + b x P, Lab D65, dE00 skimage).
Messfenster der Id-Masken 2 px erodiert (Regel "2 px erodiertes Fenster" der Lesung).
Abweichungen vom Riegeltext (im Ausdruck vermerkt):
  T1: Id 16 liegt in V4 ganz im Buckel (versenkt) - gemessen wird der unterste Schwanz-Pixel (Ids 16-22) an der Grenze zum Rumpf (7/8).
  F10: die Klaue (B)-rechts = (W)-links ist Id 12 (EM30-Knochen 10-12 = Arm links); Id 15 ist die dunkle Sichel (B)-links. Gemessen
       wird Id 12 (Lage wie das Originalfenster x 482-507), Id 15 wird mit ausgegeben.
"""
import os, sys, json, argparse, hashlib
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import emd_ansichtsblatt as EA
import birkin_formen as BF
from P_schwanz import mittellinie, dicke

SCHW = G.SCHWANZ
ZIEL = dict(  # Messblatt V4 Abschnitt 5 (Ziele, Videoraum) und Abschnitt 7
    auge_v=(46.9, 7.2, -1.1), haar_v=(13.3, 7.4, 1.5), nase=(18.2, 7.1, 1.3), kinn=(9.9, 9.9, 3.8), kuppe_v=(22.6, 15.4, 5.2),
    kv5=(18.9, 7.1, 0.6), auge_s=(48.4, 4.0, 0.5), haar_s=(19.1, 1.2, 1.1), kuppe_s=(27.6, 15.3, 3.3),
    F6=(16.6, 17.3, 6.2), F7=(33.3, 5.5, 2.7), F8=(16.4, 8.3, 1.5), F9=(12.7, -0.9, -1.5), F10=(11.2, 14.0, 2.5),
    F14=(60.0, 11.0, 1.2), F19=(18.4, 0.5, -3.5), F15=(9.3, 11.4, 1.5), F16=(8.7, 8.4, 2.2), F17=(26.8, -0.1, 2.4),
    F18=(15.2, 3.2, 1.6), F0=(64.7, 0.3, -2.1))
FENSTER = dict(KV1=(0.40, 0.02, 0.80, 0.32), KV2=(0.52, 0.25, 0.85, 0.75), KV3=(0.44, 0.66, 0.62, 0.81),
               KV4=(0.21, 0.88, 0.62, 1.05), KV5=(0.05, 0.62, 0.35, 0.85), KS1=(0.11, 0.04, 0.53, 0.29),
               KS2=(0.84, 0.52, 0.96, 0.80))


def er(m, k=2):
    return nd.binary_erosion(m, iterations=k)


def lab1(rgb):
    return G.lab(np.asarray(rgb, float).reshape(1, 3))[0]


def chroma(L):
    return np.hypot(L[..., 1], L[..., 2])


def zeile(name, soll, ist, ok, note=''):
    return dict(mass=name, soll=soll, ist=ist, ok=bool(ok), note=note)


# ---------------------------------------------------------------------------------------------------- Kopf vorn / seitlich
def kopf_blob(V, Lab, maske, box):
    """p_blob in der Kopfbox (nur Kopf-Pixel): hellstes 3x3 (L*), Schwelle = Mitte aus Extrem und Median 21x21, Komponenten."""
    x0, y0, x1, y1 = box
    L = Lab[..., 0]
    win = cv2.blur(L[y0:y1 + 1, x0:x1 + 1].astype(np.float32), (3, 3))
    km = maske[y0:y1 + 1, x0:x1 + 1]
    win = np.where(er(km, 1), win, -1e9)
    iy, ix = np.unravel_index(np.argmax(win), win.shape)
    px, py = ix + x0, iy + y0
    ext = float(win[iy, ix]); umg = float(np.median(L[py - 10:py + 11, px - 10:px + 11]))
    thr = 0.5 * (ext + umg)
    hell = (L > thr) & maske
    lb, k = nd.label(hell)                           # 4er wie p_blob
    groessen = nd.sum(hell, lb, range(1, k + 1)) if k else []
    n_komp = int(sum(1 for g in groessen if g >= 4))
    mk = lb == lb[py, px]
    ys, xs = np.nonzero(mk)
    return dict(maske=mk, n=n_komp, ext=ext, umg=umg, thr=thr, Lmax=float(L[mk].max()), kontrast=float(L[mk].max() - umg),
                bbox=(int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())), wh=(int(xs.ptp() + 1), int(ys.ptp() + 1)),
                sp=(float(xs.mean()), float(ys.mean())), rgb=V[mk].mean(0), px=int(mk.sum()))


def fenster_lab(V, box, anteile):
    bx = G.fenster(box, anteile)
    return lab1(G.mittel(V, bx)), bx


def kopf_vorn(mo, P):
    img, ids, iv = G.zeichne(mo, P['135'], voll=True)
    ok, so, H = G.h_port(iv)
    V = G.videoraum(img); Lab = G.lab(V)
    kb = G.kopfbox(ids); x0, y0, x1, y1 = kb
    kopf = ids == 9
    b = kopf_blob(V, Lab, kopf, kb)
    la = lab1(b['rgb'])
    kv = {k: fenster_lab(V, kb, FENSTER[k]) for k in ('KV1', 'KV2', 'KV3', 'KV4', 'KV5')}
    # K6: Fleckbox an der Kopfmitte gespiegelt
    xm = 0.5 * (x0 + x1)
    bx0, by0, bx1, by1 = b['bbox']
    sp_box = (int(round(2 * xm - bx1)), by0, int(round(2 * xm - bx0)), by1)
    L_sp = lab1(G.mittel(V, sp_box))[0]
    # K8 Anteil L* > 35 im Fenster KV4
    kx = G.fenster(kb, FENSTER['KV4'])
    sub = Lab[kx[1]:kx[3] + 1, kx[0]:kx[2] + 1, 0]
    hellanteil = float((sub > 35).mean())
    Z = []
    Z.append(zeile('K1 Augen vorn', '1 Komponente; Kontrast >= 20 (Original 29,5)', '%d Komp.; %.1f' % (b['n'], b['kontrast']),
                   b['n'] == 1 and b['kontrast'] >= 20))
    d2 = G.de00(la, ZIEL['auge_v'])
    Z.append(zeile('K2 Augenfleck Farbe', '46,9/7,2/-1,1 dE00 <= 6; L* >= KV5 + 20', '%s dE00 %.1f; L* %.1f vs KV5 %.1f + 20' % (
        '/'.join('%.1f' % v for v in la), d2, la[0], kv['KV5'][0][0]), d2 <= 6 and la[0] >= kv['KV5'][0][0] + 20))
    w, h = b['wh'][0] / H, b['wh'][1] / H
    Z.append(zeile('K3 Augenfleck Groesse', 'B 0,016-0,036 / H 0,023-0,043 H', '%.3f x %.3f H (%d x %d px)' % (w, h, *b['wh']),
                   0.016 <= w <= 0.036 and 0.023 <= h <= 0.043))
    dxk = (xm - b['sp'][0]) / H; dyk = (b['sp'][1] - y0) / H
    Z.append(zeile('K4 Augenlage vorn', '(B)-links 0,035-0,065 H; 0,07-0,12 H unter OK', '%.3f H links; %.3f H unter OK' % (dxk, dyk),
                   0.035 <= dxk <= 0.065 and 0.07 <= dyk <= 0.12))
    d5 = G.de00(kv['KV2'][0], ZIEL['haar_v'])
    Z.append(zeile('K5 Haarfeld vorn', '13,3/7,4/1,5 dE00 <= 6; L* <= Auge - 25', '%s dE00 %.1f; L* %.1f vs %.1f' % (
        '/'.join('%.1f' % v for v in kv['KV2'][0]), d5, kv['KV2'][0][0], la[0] - 25), d5 <= 6 and kv['KV2'][0][0] <= la[0] - 25))
    Z.append(zeile('K6 zweites Auge verdeckt', 'L* <= 18 (Original 12,9)', 'L* %.1f (Box %s)' % (L_sp, sp_box), L_sp <= 18))
    d7 = G.de00(kv['KV3'][0], ZIEL['nase'])
    Z.append(zeile('K7 Nase', '18,2/7,1/1,3 dE00 <= 6; L* zwischen Haar und Auge', '%s dE00 %.1f' % (
        '/'.join('%.1f' % v for v in kv['KV3'][0]), d7), d7 <= 6 and kv['KV2'][0][0] <= kv['KV3'][0][0] <= la[0]))
    d8 = G.de00(kv['KV4'][0], ZIEL['kinn'])
    Z.append(zeile('K8 Kinn/Maulzone', '9,9/9,9/3,8 dE00 <= 6; Anteil L*>35 <= 0,05', '%s dE00 %.1f; Anteil %.3f' % (
        '/'.join('%.1f' % v for v in kv['KV4'][0]), d8, hellanteil), d8 <= 6 and hellanteil <= 0.05))
    d9 = G.de00(kv['KV1'][0], ZIEL['kuppe_v'])
    Z.append(zeile('K9 Kopf-Oberseite rot vorn', '22,6/15,4/5,2 dE00 <= 6; a* >= 11', '%s dE00 %.1f' % (
        '/'.join('%.1f' % v for v in kv['KV1'][0]), d9), d9 <= 6 and kv['KV1'][0][1] >= 11))
    kw, kh = (x1 - x0 + 1) / H, (y1 - y0 + 1) / H
    Z.append(zeile('K10 Kopfbox vorn', '0,14-0,18 x 0,15-0,20 H', '%.3f x %.3f H' % (kw, kh), 0.14 <= kw <= 0.18 and 0.15 <= kh <= 0.20))
    info = dict(H=H, ok=ok, sohle=so, kopfbox=kb, blob=dict((k, v) for k, v in b.items() if k != 'maske'),
                fenster={k: (v[1], [round(float(x), 1) for x in v[0]]) for k, v in kv.items()})
    return Z, info, (img, ids, V, b)


def kopf_seite(mo, P):
    img, ids, iv = G.zeichne(mo, P['85'], voll=True)
    ok, so, H = G.h_port(iv)
    V = G.videoraum(img); Lab = G.lab(V)
    kb = G.kopfbox(ids); x0, y0, x1, y1 = kb
    b = kopf_blob(V, Lab, ids == 9, kb)
    la = lab1(b['rgb'])
    d1 = G.de00(la, ZIEL['auge_s'])
    w, h = b['wh'][0] / H, b['wh'][1] / H
    Z = []
    Z.append(zeile('S1 Auge seitlich', '48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H', '%s dE00 %.1f; %.3f x %.3f H; %d Komp.' % (
        '/'.join('%.1f' % v for v in la), d1, w, h, b['n']), d1 <= 6 and 0.028 <= w <= 0.052 and 0.040 <= h <= 0.070))
    hv = (x1 - b['sp'][0]) / H; ue = (so - b['sp'][1]) / H
    Z.append(zeile('S2 Augenlage seitlich', '0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle',
                   '%.3f H; %.3f H' % (hv, ue), 0.03 <= hv <= 0.07 and 0.76 <= ue <= 0.82))
    l3, bx3 = fenster_lab(V, kb, FENSTER['KS2'])
    bb = b['bbox']
    dx = max(0, bx3[0] - bb[2] - 1, bb[0] - bx3[2] - 1); dy = max(0, bx3[1] - bb[3] - 1, bb[1] - bx3[3] - 1)
    ab = float(np.hypot(dx, dy)) / H
    d3 = G.de00(l3, ZIEL['haar_s'])
    Z.append(zeile('S3 Haarkante seitlich', 'L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H',
                   '%s dE00 %.1f; Abstand %.3f H' % ('/'.join('%.1f' % v for v in l3), d3, ab), l3[0] <= 22 and d3 <= 6 and ab <= 0.01))
    l4, _bx4 = fenster_lab(V, kb, FENSTER['KS1'])
    d4 = G.de00(l4, ZIEL['kuppe_s'])
    Z.append(zeile('S4 Kopf-Oberseite rot seitlich', '27,6/15,3/3,3 dE00 <= 6; a* >= 11', '%s dE00 %.1f' % (
        '/'.join('%.1f' % v for v in l4), d4), d4 <= 6 and l4[1] >= 11))
    info = dict(H=H, kopfbox=kb, blob=dict((k, v) for k, v in b.items() if k != 'maske'))
    return Z, info, (img, ids, V, b)


# ---------------------------------------------------------------------------------------------------- Schwanz
def schwanz_mass(img, ids):
    """T1-T6 an einem Render (Bild, ids). Rueckgabe dict mit Punkten (px) und Massen (H)."""
    body = (ids >= 0) & ~np.isin(ids, SCHW)
    ys = np.nonzero(body.any(1))[0]
    ok, so = int(ys.min()), int(ys.max())
    return schwanz_mass_h(img, ids, ok, so)


def schwanz_mass_h(img, ids, ok, so):
    H = float(so - ok)
    body = (ids >= 0) & ~np.isin(ids, SCHW)
    m = np.isin(ids, SCHW)
    lb, k = nd.label(m)
    if k == 0:
        return None
    sz = nd.sum(m, lb, range(1, k + 1)); m = lb == 1 + int(np.argmax(sz))
    xs_top = np.nonzero(body[ok])[0]
    scheitel = float(xs_top.mean())
    rumpf = np.isin(ids, (7, 8))
    grenze = m & nd.binary_dilation(rumpf, structure=np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]]))
    if not grenze.any():
        grenze = m & nd.binary_dilation(body)
    gy, gx = np.nonzero(grenze)
    i = int(np.argmax(gy + 1e-3 * gx))
    beg = np.array([float(gx[i]), float(gy[i])])
    Q = m & nd.binary_dilation(body)
    C, _d = mittellinie(m, Q, H)
    n = len(C)
    w = max(1, int(round(0.01 * H)))
    th = np.zeros(n)
    for j in range(n):
        a, b = max(j - w, 0), min(j + w, n - 1)
        th[j] = np.degrees(np.arctan2(C[b, 1] - C[a, 1], C[b, 0] - C[a, 0]))
    th = np.degrees(np.unwrap(np.radians(th)))
    hw = int(round(0.05 * H))
    dth = np.zeros(n)
    for j in range(hw, n - hw):
        dth[j] = th[j + hw] - th[j - hw]
    rand = int(round(0.03 * H))
    jj = np.arange(rand, n - rand)
    jb = int(jj[np.argmax(np.abs(dth[jj]))])
    breite = np.array([dicke(m, C, j)[0] for j in range(n)])
    je = int(np.argmax(C[:, 1]))
    # Knick: schmalste Stelle zwischen Biegung (+0,03 H) und Ende (-0,03 H)
    a0, a1 = jb + rand, max(jb + rand + 1, je - rand)
    jk = int(a0 + np.argmin(breite[a0:a1])) if a1 > a0 else je
    P = lambda j: C[j]
    rel = lambda p: ((p[0] - beg[0]) / H, (p[1] - beg[1]) / H)
    # T5: Breite bei f der Achse Knick -> Beginn (Index jk -> 0)
    f_br = {}
    for f in np.round(np.arange(0.05, 0.96, 0.05), 2):
        f_br[float(f)] = float(breite[int(round(jk * (1 - f)))] / H)
    # T6 Endstueck
    end_b = breite[jk:je + 1]
    end_mittel = float(np.median(end_b[int(0.15 * len(end_b)):max(int(0.85 * len(end_b)), int(0.15 * len(end_b)) + 1)]) / H) \
        if len(end_b) > 3 else float('nan')
    end_len = float(np.sum(np.hypot(*np.diff(C[jk:je + 1], axis=0).T)) / H) if je > jk else 0.0
    ch = C[je] - C[jk]
    end_dir = float(np.degrees(np.arctan2(ch[1], abs(ch[0]) if ch[0] != 0 else 1e-9)))
    vor = ch[0]          # + = nach vorn (Bild-rechts)
    taille = float(breite[jk] / H)
    schmal_kette = float(np.min(breite[rand:max(rand + 1, jk + 1)]) / H)
    return dict(H=H, ok=ok, sohle=so, scheitel=scheitel, beginn=beg, T1=((scheitel - beg[0]) / H, (so - beg[1]) / H),
                biegung=P(jb), T2=rel(P(jb)), drehung=float(abs(dth[jb])), knick=P(jk), T3=rel(P(jk)), ende=P(je), T4=rel(P(je)),
                ende_ueber_sohle=(so - C[je][1]) / H, T5=f_br, taille=taille, schmal_vor_knick=schmal_kette, end_breite=end_mittel,
                end_laenge=end_len, end_richtung=end_dir, end_vor=float(vor), C=C, breite=breite, jb=jb, jk=jk, je=je, maske=m)


def schwanz(mo, P):
    img, ids, iv = G.zeichne(mo, P['85'], voll=True)
    ok, so, H = G.h_port(iv)
    s = schwanz_mass_h(img, ids, ok, so)
    Z = []
    t1 = s['T1']
    Z.append(zeile('T1 Schwanz Beginn', '0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03)', '%.3f H hinter, %.3f H ueber' % t1,
                   abs(t1[0] - 0.15) <= 0.03 and abs(t1[1] - 0.767) <= 0.03, 'unterster Schwanz-Pixel an Id 7/8 (Id 16 versenkt)'))
    t2 = s['T2']
    Z.append(zeile('T2 Biegung', 'dx -0,245 dy +0,036 (+-0,03); Drehung 60-100 Grad auf 0,10 H', 'dx %.3f dy %.3f; %.0f Grad' % (
        t2[0], t2[1], s['drehung']), abs(t2[0] + 0.245) <= 0.03 and abs(t2[1] - 0.036) <= 0.03 and 60 <= s['drehung'] <= 100))
    t3 = s['T3']
    Z.append(zeile('T3 Knick (Taille)', 'dx -0,265 dy +0,257 (+-0,03)', 'dx %.3f dy %.3f' % t3,
                   abs(t3[0] + 0.265) <= 0.03 and abs(t3[1] - 0.257) <= 0.03))
    t4 = s['T4']
    Z.append(zeile('T4 Ende', 'dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle', 'dx %.3f dy %.3f; %.3f H ueber Sohle' % (
        t4[0], t4[1], s['ende_ueber_sohle']), abs(t4[0] + 0.245) <= 0.03 and abs(t4[1] - 0.397) <= 0.03))
    f = s['T5']
    soll = {0.1: (0.03, 0.015), 0.3: (0.065, 0.02), 0.5: (0.125, 0.025), 0.7: (0.14, 0.025)}
    ok5 = all(abs(f[k] - v) <= t for k, (v, t) in soll.items())
    reihe = [f[k] for k in sorted(f) if 0.1 <= k <= 0.7 + 1e-9]
    abfall = max([reihe[i] - reihe[i + 1] for i in range(len(reihe) - 1)] + [0.0])
    Z.append(zeile('T5 Breite nach oben', 'f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01', '%.3f/%.3f/%.3f/%.3f H; Abfall %.3f' % (
        f[0.1], f[0.3], f[0.5], f[0.7], abfall), ok5 and abfall <= 0.01))
    ok6 = (0.025 <= s['end_breite'] <= 0.05 and 0.12 <= s['end_laenge'] <= 0.17 and 75 <= s['end_richtung'] <= 95 and
           s['taille'] <= 0.035 and s['taille'] <= s['schmal_vor_knick'] + 1e-9)
    Z.append(zeile('T6 Endstueck', 'B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste',
                   'B %.3f; L %.3f H; %.0f Grad (%s); Taille %.3f, schmalste davor %.3f' % (
                       s['end_breite'], s['end_laenge'], s['end_richtung'], 'vorn' if s['end_vor'] > 0 else 'hinten', s['taille'],
                       s['schmal_vor_knick']), ok6))
    # T8 Schwanzfarbe (Vergleichsregel)
    V = G.videoraum(img); Lab = G.lab(V)
    yy, xx = np.mgrid[:480, :640]
    m = np.isin(ids, SCHW)
    me = er(m, 1)
    C = chroma(Lab)
    b = s['beginn']; bi = s['biegung']
    r15 = me & (np.hypot(xx - (b[0] - 12), yy - (b[1] - 5)) <= 12) & (C >= 6)
    r16 = me & (np.hypot(xx - (bi[0] + 10), yy - bi[1]) <= 10) & (C >= 6)
    r17 = me & (C < 6) & (yy <= bi[1])
    r18 = me & (yy > s['knick'][1]) & (yy <= s['ende'][1])
    fr = {}
    for k, r in (('F15', r15), ('F16', r16), ('F17', r17), ('F18', r18)):
        fr[k] = (lab1(V[r].mean(0)) if r.sum() >= 3 else np.full(3, np.nan), int(r.sum()))
    d = {k: G.de00(v[0], ZIEL[k]) if v[1] >= 3 else 99.0 for k, v in fr.items()}
    ok8 = all(x <= 6 for x in d.values()) and fr['F15'][0][1] >= 7 and fr['F18'][0][1] <= 6
    Z.append(zeile('T8 Schwanzfarbe', 'F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6',
                   '; '.join('%s %s dE00 %.1f (%d px)' % (k, '/'.join('%.1f' % x for x in fr[k][0]), d[k], fr[k][1]) for k in fr), ok8))
    return Z, s, (img, ids, V, fr, dict(F15=r15, F16=r16, F17=r17, F18=r18))


# ---------------------------------------------------------------------------------------------------- Koerperfarben
def koerper_farben(mo, P):
    img, ids = G.zeichne(mo, P['135'])
    V = G.videoraum(img); Lab = G.lab(V)
    rg = (V[..., 0] - V[..., 1]) >= 15
    reg = {'F6': er(ids == 8) & rg, 'F7': er(ids == 13), 'F8': er(ids == 10), 'F9': er(np.isin(ids, (0, 1, 4))) & (Lab[..., 2] < 0),
           'F10': er(ids == 12, 1) & rg, 'F10_id15': er(ids == 15, 1) & rg}
    img2, ids2 = G.zeichne(mo, P['85'])
    V2 = G.videoraum(img2); Lab2 = G.lab(V2)
    b8 = er(ids2 == 8)
    L2 = Lab2[..., 0]
    reg2 = {'F14': b8 & (L2 >= np.percentile(L2[b8], 70)) if b8.any() else b8,
            'F19': er(np.isin(ids2, (0, 1, 4))) & (Lab2[..., 2] < 0)}
    erg = {}
    for k, r in reg.items():
        erg[k] = (lab1(V[r].mean(0)) if r.sum() else np.full(3, np.nan), int(r.sum()))
    for k, r in reg2.items():
        erg[k] = (lab1(V2[r].mean(0)) if r.sum() else np.full(3, np.nan), int(r.sum()))
    Z, des = [], []
    for k in ('F6', 'F7', 'F8', 'F9', 'F10', 'F14', 'F19'):
        la, n = erg[k]
        dd = G.de00(la, ZIEL[k]) if n else 99.0
        dl = la[0] - ZIEL[k][0] if n else 99.0
        des.append(dd)
        Z.append(zeile('F %s' % k, '%s dE00 <= 5, |dL| <= 4' % '/'.join('%.1f' % v for v in ZIEL[k]),
                       '%s dE00 %.1f dL %+.1f (%d px)' % ('/'.join('%.1f' % v for v in la), dd, dl, n), dd <= 5 and abs(dl) <= 4,
                       'Id 12 (Klaue (B)-rechts); Id 15: %s (%d px)' % ('/'.join('%.1f' % v for v in erg['F10_id15'][0]),
                                                                       erg['F10_id15'][1]) if k == 'F10' else ''))
    mt = float(np.mean(des))
    Z.append(zeile('F Mittel', 'Mittel dE00 <= 4', '%.2f' % mt, mt <= 4))
    # F0 Gueltigkeit: Wand links im Render (BSS-Hintergrund)
    w = lab1(G.mittel(V, (40, 150, 119, 299)))
    d0 = G.de00(w, ZIEL['F0'])
    Z.append(zeile('F0 Gueltigkeit Farbvergleich', 'Wand links 64,7/0,3/-2,1 dE00 <= 2', '%s dE00 %.2f' % ('/'.join('%.1f' % v for v in w), d0),
                   d0 <= 2))
    return Z, erg, (img, ids, V, img2, ids2, V2, reg, reg2)


def lichtprobe(lauf):
    """Lichtgegenprobe (Messblatt Abschnitt 7): b2-Render des Laufmodells in der Laufpose h118 gegen RE15_FRAMEDUMP desselben Bildes,
    Rumpf-Mittel (Id 7/8, 2 px erodiert) in der Vergleichsregel; dE00 <= 3."""
    import re
    d = os.path.dirname(lauf)
    sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
    os.environ['NB6_RUNS'] = os.path.dirname(d)
    import c6
    import riegel_v3 as R3
    PO = R3.lauf_posen(lauf)
    emd = os.path.join(d, 'cd', 'EMD', 'EM30_HANGAR.EMD')
    if not os.path.exists(emd):
        emd = G.REF2
    mo = b2.modell(emd)
    erg = {}
    for h in (118, 135):
        if h not in PO:
            continue
        img, ids = G.zeichne(mo, PO[h])
        fd = c6.port(os.path.basename(d), h)
        if fd is None:
            continue
        fd = cv2.cvtColor(fd, cv2.COLOR_BGR2RGB)
        r = er(np.isin(ids, (7, 8)))
        a = lab1(G.videoraum(img)[r].mean(0)); b = lab1(G.videoraum(fd)[r].mean(0))
        erg[h] = (a, b, G.de00(a, b), int(r.sum()))
    return erg


# ---------------------------------------------------------------------------------------------------- Machart / Koerper = Ref2
def machart(pfad):
    import machart_v3 as MV
    blob = open(pfad, 'rb').read(); rb = open(G.REF2, 'rb').read()
    mo, mr = BF.modell_aus_blob(blob), BF.modell_aus_blob(rb)
    d, dr = EA.emd_dir(blob), EA.emd_dir(rb)
    abw = [b for b in list(range(9)) + list(range(10, 16)) if MV.mesh_bytes(mo.meshes[b]) != MV.mesh_bytes(mr.meshes[b])]
    sk, skr = mo.skel, mr.skel
    skel = all(tuple(sk.rel[b]) == tuple(skr.rel[b]) and sk.parent[b] == skr.parent[b] for b in range(16))
    kf = sk.kf_count == skr.kf_count and all(
        all(tuple(EA.kf_angles(sk, k, b)) == tuple(EA.kf_angles(skr, k, b)) for b in range(16)) and
        tuple(EA.kf_pos(sk, k)) == tuple(EA.kf_pos(skr, k)) for k in range(sk.kf_count))
    edd = blob[d[1]:d[2]] == rb[dr[1]:dr[2]]
    sonst = all(blob[d[i]:d[i + 1]] == rb[dr[i]:dr[i + 1]] for i in (0, 3, 4, 5, 6))
    Z = []
    Z.append(zeile('G1 Koerper-Geometrie = Ref2', 'Meshes 0-8, 10-15 byte-gleich; Skelett/Keyframes 0-15, EDD gleich',
                   'abweichend %s; Skelett %s; Keyframes %s; EDD %s; uebrige Sektionen %s; Ref2 sha256 %s' % (
                       abw or 'keine', skel, kf, edd, sonst, hashlib.sha256(rb).hexdigest()[:8]), not abw and skel and kf and edd))
    ds = sum(MV.dreiecke(mo.meshes[b]) for b in SCHW)
    Z.append(zeile('M1 Dreiecke Schwanz', '<= 212', '%d (%s)' % (ds, [MV.dreiecke(mo.meshes[b]) for b in SCHW]), ds <= 212))
    dk = MV.dreiecke(mo.meshes[9])
    Z.append(zeile('M2 Dreiecke Kopf', '<= 118', '%d' % dk, dk <= 118))
    tim = blob[d[8]:]
    bpp8 = (EA.u32(tim, 4) & 7) == 1
    zeilen9 = sorted({uv[2] >> 6 for uv in list(mo.meshes[9].tuv) + list(mo.meshes[9].quv)})
    zeilen_s = sorted({uv[2] >> 6 for b in SCHW for uv in list(mo.meshes[b].tuv) + list(mo.meshes[b].quv)})
    clut_h = mo.pal.shape[0]
    ok3 = bpp8 and all(480 <= z < 480 + clut_h for z in zeilen9 + zeilen_s)
    Z.append(zeile('M3 Farbtiefe', '8-bpp-TIM, Texel aus der CLUT', '8 bpp %s; CLUT-Zeilen Kopf %s, Schwanz %s (TIM %d Zeilen)' % (
        bpp8, zeilen9, zeilen_s, clut_h), ok3))
    dsch = [round(MV.dichte(mo.meshes[b]), 3) for b in SCHW]
    dkopf = round(MV.dichte(mo.meshes[9]), 3)
    Z.append(zeile('M4 Texeldichte', 'Kopf und Schwanz 0,045-0,091 Texel/E.', 'Kopf %.3f; Schwanz %s' % (dkopf, dsch),
                   0.045 <= dkopf <= 0.091 and all(0.045 <= x <= 0.091 for x in dsch)))
    return Z


# ---------------------------------------------------------------------------------------------------- Gang (T7)
def gang(emd, lauf, takte=(85,)):
    """T7: Feder-Stand des Laufs (Gelenke aus debug.log "[hangar-takt] hN Schwanz B y ...") je Takt in der Kamera der Vorlage 2:
    die Gelenke werden aus der Laufpose in den Raum von Knochen 8 umgerechnet und an Knochen 8 der Pose "85" gehaengt (b2-Render mit dem
    Feder-Stand des Laufs, Messblatt Abschnitt 7 T7). Grund: im Lauf zeigt die Spielkamera 50A0/12 den Birkin h84-h135 von vorn (Gang
    auf die Kamera zu), der Schwanz liegt dort hinter dem Koerper; h85 ist im Lauf Clip 11 (Halten), nicht der Gang der Vorlage."""
    import riegel_v3 as R3
    import birkin_buehne as BB
    mo = b2.modell(emd)
    Gk, PO = R3.lauf_lesen(lauf), R3.lauf_posen(lauf)
    out = {}
    p85 = G.posen()['85']
    R85, T85 = mo.pose(BB.kf_von_clip(mo, p85['clip'], p85['bild']))
    for h in takte:
        if h not in Gk or h not in PO:
            continue
        pl = PO[h]
        Rl, Tl = mo.pose(BB.kf_von_clip(mo, pl['clip'], pl['bild']))
        g = pl['gier']; s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
        Yl = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
        Pl = (Gk[h] - np.array([pl['x'], 0.0, pl['z']])) @ Yl              # Welt -> Modell der Laufpose
        R8l, T8l = Rl[8].astype(float) / 4096.0, Tl[8].astype(float)
        lok = (Pl - T8l) @ R8l                                              # Raum von Knochen 8 (Zeilen: R8l.T @ (P - T8))
        R8, T8 = R85[8].astype(float) / 4096.0, T85[8].astype(float)
        Pm = lok @ R8.T + T8
        pose = p85
        R = [r.copy() for r in R85]; T = [t.copy() for t in T85]
        for j in range(7):
            b = 16 + j
            Rb = R[b].astype(float) / 4096.0
            Rn = R3._min_rot(Rb[:, 1], Pm[j + 1] - Pm[j]) @ Rb
            R[b] = np.round(Rn * 4096).astype(np.int64); T[b] = np.round(Pm[j]).astype(np.int64)
        img, ids, iv = G.zeichne(mo, pose, R_T=(R, T), voll=True)
        ok, so, H = G.h_port(iv)
        try:
            s_ = schwanz_mass_h(img, ids, ok, so)
        except Exception as e:  # noqa: BLE001
            out[h] = dict(fehler=str(e)); continue
        if s_ is None:
            out[h] = dict(fehler='kein Schwanz sichtbar'); continue
        soll = dict(T1=(0.15, 0.767), T2=(-0.245, 0.036), T3=(-0.265, 0.257), T4=(-0.245, 0.397))
        abw = {k: max(abs(s_[k][0] - v[0]), abs(s_[k][1] - v[1])) for k, v in soll.items()}
        out[h] = dict(pose=pl, **{k: tuple(round(float(x), 3) for x in s_[k]) for k in soll}, abw=abw,
                      ok=all(v <= 0.06 for v in abw.values()), bild=(img, ids))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd')
    ap.add_argument('--lauf')
    ap.add_argument('--takte', default='85')
    ap.add_argument('--json'); ap.add_argument('--md')
    ap.add_argument('--kein-exit', action='store_true')
    a = ap.parse_args()
    P = G.posen()
    mo = b2.modell(a.emd)
    Z = []
    Z += machart(a.emd)
    zk, ik, _ = kopf_vorn(mo, P); Z += zk
    zs, is_, _ = kopf_seite(mo, P); Z += zs
    zt, st, _ = schwanz(mo, P); Z += zt
    if a.lauf:
        gg = gang(a.emd, a.lauf, [int(x) for x in a.takte.split(',')])
        for h, v in gg.items():
            if 'fehler' in v:
                Z.append(zeile('T7 Gang h%d' % h, 'T1-T4 +-0,06 H', v['fehler'], False)); continue
            Z.append(zeile('T7 Gang h%d' % h, 'T1-T4 je +-0,06 H', ' '.join('%s %s (%.3f)' % (k, v[k], v['abw'][k]) for k in ('T1', 'T2', 'T3', 'T4')),
                           v['ok'], 'Laufpose clip %(clip)d bild %(bild)d gier %(gier)d' % v['pose']))
        lp = lichtprobe(a.lauf)
        for h, (x, y, d, n) in lp.items():
            Z.append(zeile('F0 Lichtgegenprobe h%d' % h, 'Rumpf b2 gegen FRAMEDUMP dE00 <= 3', 'b2 %s / Spiel %s dE00 %.1f (%d px)' % (
                '/'.join('%.1f' % v for v in x), '/'.join('%.1f' % v for v in y), d, n), d <= 3))
    zf, ef, _ = koerper_farben(mo, P); Z += zf
    rot = [z for z in Z if not z['ok']]
    for z in Z:
        print('%-4s %-34s soll: %-60s ist: %s %s' % ('OK' if z['ok'] else 'ROT', z['mass'], z['soll'], z['ist'],
                                                     ('[' + z['note'] + ']') if z['note'] else ''))
    print('ROT: %d von %d' % (len(rot), len(Z)))
    if a.json:
        json.dump(dict(emd=a.emd, sha256=hashlib.sha256(open(a.emd, 'rb').read()).hexdigest(), zeilen=Z,
                       kopf_vorn=ik, kopf_seite=is_, schwanz={k: v for k, v in st.items() if k not in ('C', 'breite', 'maske')}),
                  open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    if a.md:
        with open(a.md, 'w', encoding='utf-8') as f:
            f.write('| Mass | Soll | Ist | |\n|---|---|---|---|\n')
            for z in Z:
                f.write('| %s | %s | %s | %s |\n' % (z['mass'], z['soll'], z['ist'] + ((' (' + z['note'] + ')') if z['note'] else ''),
                                                     'OK' if z['ok'] else 'ROT'))
    if rot and not a.kein_exit:
        sys.exit(1)


if __name__ == '__main__':
    main()
