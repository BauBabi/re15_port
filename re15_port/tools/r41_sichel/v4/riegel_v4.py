"""Riegel V4 der Sichel-Form (analysis/befunde_runde41/SF_MESSBLATT_V4.md Abschnitt 7) - Exit 1, wenn eine Zeile rot ist.

    python riegel_v4.py <EMD> [--lauf debug.log] [--fd LAUFORDNER] [--json aus.json] [--md aus.md] [--kein-exit]

Render fuer alle Riegel: b2.zeichne (Spielkamera 50A0 Schnitt 12, RDT-Licht), 640 x 480, Hintergrund = BSS 50A0/12 (v4_gemein), Posen
posen_v4.json "135" (vorn, Vorlage 1) und "85" (seitlich, Vorlage 2). Keyframe ohne Feder (Ruhe = Keyframe, Messblatt Abschnitt 4).
H_port = Sohle - Oberkante der Koerper-Ids ohne 16-22 auf der nach unten verlaengerten Leinwand (in Pose 135 liegt die Sohle unter
Zeile 479). Kopfbox = Bbox der Pixel mit Id 9. Farben: Vergleichsregel (Gauss sigma 1,5, V' = a + b x P, Lab D65, dE00 skimage).
Messfenster der Id-Masken 2 px erodiert (Regel "2 px erodiertes Fenster" der Lesung).
Abweichungen vom Riegeltext (im Ausdruck vermerkt):
  T1: Runde 2 misst wie das Messblatt (unterster Pixel von Id 16 an Id 7/8); nur wenn Id 16 nicht sichtbar ist, die Ersatzregel von
      Runde 1 (unterster Schwanz-Pixel an Id 7/8).
Runde 2 (Pruefung V4 r1): T2-Drehung nach dem Zahlen-Pruefer (Tangenten +-0,025 H an den Enden eines 0,10-H-Fensters; Original 66-70
  Grad), T4 prueft die Sohlenhoehe, neuer Riegel TA (Nutzerpunkte absolut), T9 (Achsenfarbe, Sicht-Pruefung), Komponenten von K1/S1 im
  2 px erodierten Kopf gezaehlt, S1 verlangt 1 Komponente, F7/F8 an der Lage der Messblatt-Fenster im Oberarm (obere 58/56 % der
  Zeilen; Id ganz als F7g/F8g ausgegeben), FS-Zeilen (Sicht-Boxen: Oberarme unten, Rumpf + Schultern mit Rotanteil, Unterarme, Beine).
  F10: die Klaue (B)-rechts = (W)-links ist Id 12 (EM30-Knochen 10-12 = Arm links); Id 15 ist die dunkle Sichel (B)-links. Gemessen
       wird Id 12 (Lage wie das Originalfenster x 482-507), Id 15 wird mit ausgegeben.
Runde 3 (Pruefung V4 r2 Zahlen/Sicht, Entscheide O1-O6 des Orchestrators):
  G1 (O1): Geometrie = Ecken, Normalen, Dreiecke/Vierecke, UV-Koordinaten, Skelett, Keyframes, EDD; das CLUT-Feld der Koerper-Meshes darf
      sich aendern (eigene Zeilen 488/489 fuer Mesh 14/10) und wird ausgegeben.
  T2 (O2): Drehung nach t2_mass_v4.py (Pruefer-Mittellinie), Ziel = Mittel der Original-Masken, Toleranz = max(Spanne, 5 Grad).
  T7 (O3): koerperfest am Lauf (t7_koerperfest_v4.py: Kette gegen das Feder-Ziel der gezeichneten Pose, Biegung/Knick, Strecke,
      Rueckfaltung, Boden mit Mesh-Radien und Dornspitze) statt Bildhoehe in der Vorlage-2-Kamera.
  K11 (O4): Struktur des Kopfes vorn = mittlerer Lum-Gradient (Sobel) im 2 px erodierten Kopf (Id 9) im Videoraum; Ziel und Toleranz aus
      dem Original (9 um +-3 px verschobene Kopfboxen: Mittel; Toleranz = groesser von Spanne der 9 Boxen und halber Spanne der 4
      Kopfviertel), dazu Kopf >= Rumpf (Original 1,57 x).
  FS Unterarm (W)-rechts (O5): Id 14 gegen die Box 298-325 x 262-340 der Sicht-Pruefung V4 r2 (dE00, dL, Anteil L* > 30).
  F7/F8: Lage der Messblatt-Fenster im Oberarm (Runde 2) UND Messblatt-Wortlaut (Id ganz) als eigene Zeilen mit echter Bewertung
      (Pruefung V4 r2 Zahlen 4.2: nicht fest OK); dazu der ganze Oberarm gegen die Oberarm-Boxen der Sicht-Pruefung.
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
import t2_mass_v4 as T2M
import r4_mass as R4

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


# V4 Runde 2 (Sicht-Pruefung V4 r1 1.3): Schwanzfarbe entlang der Achse - 7 Achsenpunkte der Original-Maske (Leser A,
# pruefung_sicht/messen_sicht.py ACHSE), Ziel = Lab-Mittel des 5x5-Fensters in Vorlage 2 (roh); am Port der naechste Schwanzpixel.
ACHSE = [(330, 236), (310, 243), (292, 262), (289, 280), (288, 300), (287, 318), (290, 335)]
ACHSE_MITTE = ACHSE[1:5]          # Bogen und senkrechtes Stueck (Klasse 'mitte' im Generator)
_achse_ziel = {}


def achse_ziele():
    if not _achse_ziel:
        o2 = G.vorlage(2)
        for (x, y) in ACHSE:
            _achse_ziel[(x, y)] = G.lab(o2[y - 2:y + 3, x - 2:x + 3].reshape(-1, 3)).mean(0)
    return _achse_ziel


def ziel(k):
    """Ziel-Lab (Videoraum) einer Region: ZIEL, dazu T9M = Mittel der Achsenpunkte ACHSE_MITTE."""
    z = achse_ziele()
    if k == 'T9M':
        return tuple(float(v) for v in np.mean([z[p] for p in ACHSE_MITTE], 0))
    if k == 'T9O':
        return tuple(float(v) for v in np.mean([z[p] for p in ACHSE[1:3]], 0))
    if k == 'T9S':
        return tuple(float(v) for v in np.mean([z[p] for p in ACHSE[3:5]], 0))
    return ZIEL[k]


def achse_regionen(ids, V=None):
    """Je Achsenpunkt: Maske 5x5 um den naechsten Schwanzpixel (nur Schwanz-Ids) und Ziel-Lab."""
    sw = np.isin(ids, SCHW)
    sy, sx = np.nonzero(sw)
    out = {}
    if not len(sx):
        return out
    for (x, y), z in achse_ziele().items():
        j = int(np.argmin((sx - x) ** 2 + (sy - y) ** 2)); qx, qy = int(sx[j]), int(sy[j])
        m = np.zeros(ids.shape, bool); m[qy - 2:qy + 3, qx - 2:qx + 3] = True
        out['A%d_%d' % (x, y)] = (m & sw, tuple(float(v) for v in z))
    return out


def er(m, k=2):
    return nd.binary_erosion(m, iterations=k)


def lab1(rgb):
    return G.lab(np.asarray(rgb, float).reshape(1, 3))[0]


def chroma(L):
    return np.hypot(L[..., 1], L[..., 2])


def zeile(name, soll, ist, ok, note='', info=False):
    """info=True: Ausgabe ohne Bewertung (z. B. die Messblatt-Lesart 'Id ganz' von F6/F7 nach Entscheid O7)."""
    return dict(mass=name, soll=soll, ist=ist, ok=True if info else bool(ok), note=note, info=bool(info))


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
    # Komponenten gezaehlt im 2 px erodierten Kopf (Regel "2 px erodiertes Fenster" der Lesung, wie der Zahlen-Pruefer pz_mess.blob):
    # an der Kopfkontur mischt die Weichzeichnung sigma 1,5 den hellen Hintergrund ein (Runde 2: 1-px-Saum an der Oberkante seitlich)
    lb, k = nd.label(hell & er(maske, 2))                # 4er wie p_blob
    groessen = nd.sum(hell, lb, range(1, k + 1)) if k else []
    n_komp = int(sum(1 for g in groessen if g >= 4))
    lb_v, _kv = nd.label(hell)
    mk = lb_v == lb_v[py, px]
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
    so_ = struktur_original(); gk, gr = kopf_struktur(V, ids)
    Z.append(zeile('K11 Kopf-Struktur vorn', 'Lum-Gradient Kopf %.1f +-%.1f (Original, 9 Boxen %.1f-%.1f, Viertel %.1f-%.1f); Kopf >= Rumpf (Original %.2f x)' % (
        so_['ziel'], so_['tol'], min(so_['werte']), max(so_['werte']), min(so_['viertel']), max(so_['viertel']), so_['verh']),
        'Kopf %.1f, Rumpf %.1f (%.2f x)' % (gk, gr, gk / max(gr, 1e-6)), abs(gk - so_['ziel']) <= so_['tol'] and gk >= gr,
        'Pruefung V4 r2 Sicht 1.1: Runde 2 Kopf 27,8 / Rumpf 32,4'))
    info = dict(H=H, ok=ok, sohle=so, kopfbox=kb, blob=dict((k, v) for k, v in b.items() if k != 'maske'),
                fenster={k: (v[1], [round(float(x), 1) for x in v[0]]) for k, v in kv.items()})
    return Z, info, (img, ids, V, b)


def _grad(Lm):
    return np.hypot(cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 1, 0), cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 0, 1))


def _box(b):
    m = np.zeros((480, 640), bool); m[b[1]:b[3] + 1, b[0]:b[2] + 1] = True
    return m


_struktur = {}


def struktur_original():
    """K11 (O4): Lum-Gradient (Sobel) des Originals (Vorlage 1 entblendet) in der Kopfbox (Messblatt Abschnitt 2, x 368-416 y 139-191)
    und der Rumpfbox (Sicht-Pruefung V4 r1/r2, x 352-448 y 196-292), je 2 px erodiert. Ziel = Mittel ueber 9 um +-3 px verschobene
    Kopfboxen; Toleranz = groesser von (Spanne der 9 Boxen, halbe Spanne der 4 Kopfviertel)."""
    if not _struktur:
        g = _grad(G.lum(G.vorlage(1)))
        K, R = G.KB_ORIG[1], (352, 196, 448, 292)
        werte = [float(g[er(_box((K[0] + dx, K[1] + dy, K[2] + dx, K[3] + dy)))].mean()) for dx in (-3, 0, 3) for dy in (-3, 0, 3)]
        xm, ym = (K[0] + K[2]) // 2, (K[1] + K[3]) // 2
        viertel = [float(g[er(_box(b), 1)].mean()) for b in ((K[0], K[1], xm, ym), (xm, K[1], K[2], ym), (K[0], ym, xm, K[3]),
                                                              (xm, ym, K[2], K[3]))]
        rumpf = float(g[er(_box(R))].mean())
        _struktur.update(ziel=float(np.mean(werte)), tol=max(float(np.ptp(werte)), 0.5 * float(np.ptp(viertel))), werte=werte,
                         viertel=viertel, rumpf=rumpf, verh=float(np.mean(werte)) / rumpf)
    return _struktur


def kopf_struktur(V, ids):
    g = _grad(G.lum(V))
    return float(g[er(ids == 9)].mean()), float(g[er(ids == 8)].mean())


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
    Z.append(zeile('S1 Auge seitlich', '1 heller Fleck (1 Komponente); 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H', '%s dE00 %.1f; %.3f x %.3f H; %d Komp.' % (
        '/'.join('%.1f' % v for v in la), d1, w, h, b['n']), d1 <= 6 and 0.028 <= w <= 0.052 and 0.040 <= h <= 0.070 and b['n'] == 1))
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
    kreuz = np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]])
    # T1 (Messblatt V4 Abschnitt 7): unterster Pixel von Id 16 an der Grenze zum Rumpf (Id 7/8). Runde 2: Id 16 ist sichtbar (der
    # Ansatz liegt vor der Flanke, Pruefung V4 r1 Zahlen 4.1); nur wenn Id 16 fehlt, gilt die Ersatzregel von Runde 1 (Ids 16-22).
    g16 = m & (ids == 16) & nd.binary_dilation(rumpf, structure=kreuz)
    t1_regel = 'Id 16 an Id 7/8'
    grenze = g16
    if not grenze.any():
        grenze = m & nd.binary_dilation(rumpf, structure=kreuz); t1_regel = 'Ersatz: Ids 16-22 an Id 7/8 (Id 16 nicht sichtbar)'
    if not grenze.any():
        grenze = m & nd.binary_dilation(body); t1_regel = 'Ersatz: Ids 16-22 am Koerper'
    gy, gx = np.nonzero(grenze)
    i = int(np.argmax(gy + 1e-3 * gx))
    beg = np.array([float(gx[i]), float(gy[i])])
    # Mittellinie ab dem vordersten Schwanzpixel (Saat wie Pruefer pz_mess.schwanz: groesstes x an der Koerpergrenze)
    an = m & nd.binary_dilation(body)
    ay, ax_ = np.nonzero(an if an.any() else m)
    Q = np.zeros_like(m); Q[ay[ax_ >= ax_.max() - 1], ax_[ax_ >= ax_.max() - 1]] = True
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
    # T2-Drehung nach dem Zahlen-Pruefer (V4 r1, pz_t2orig.drehung): in jedem 0,10-H-Fenster der Bogenlaenge hinter beginn - 0,10 H die
    # Richtungsaenderung zwischen der zentrierten Tangente +-0,025 H am Fensteranfang und am Fensterende; Maximum. Am Original-Schwanz
    # (Leser-Masken) ergibt dasselbe Verfahren 66-70 Grad (SF_PRUEFUNG_V4_r1_zahlen.md 4.2).
    sl = np.r_[0, np.cumsum(np.hypot(*np.diff(C, axis=0).T))]
    at = lambda x: np.array([np.interp(x, sl, C[:, 0]), np.interp(x, sl, C[:, 1])])
    dreh_t = 0.0
    for x in np.arange(0, sl[-1] - 0.10 * H, 0.5):
        if at(x)[0] > beg[0] - 0.10 * H:
            continue
        t0 = at(x + 0.025 * H) - at(max(0.0, x - 0.025 * H)); t1 = at(min(sl[-1], x + 0.125 * H)) - at(x + 0.075 * H)
        w2 = np.degrees(np.arctan2(t1[1], -t1[0]) - np.arctan2(t0[1], -t0[0]))
        dreh_t = max(dreh_t, float(w2))
    je = int(np.argmax(C[:, 1]))
    # Knick: schmalste Stelle zwischen Biegung (+0,03 H) und Ende (-0,03 H)
    a0, a1 = jb + rand, max(jb + rand + 1, je - rand)
    jk = int(a0 + np.argmin(breite[a0:a1])) if a1 > a0 else je
    P = lambda j: C[j]
    rel = lambda p: ((p[0] - beg[0]) / H, (p[1] - beg[1]) / H)
    # T5: Breite bei f der Achse Knick -> Beginn (Index jk -> 0)
    # Runde 2: die Achse beginnt am gemessenen Beginn (der Ansatz liegt vor der Flanke, die Mittellinie davor gehoert nicht zu T5)
    j0 = int(np.argmin(np.hypot(C[:jk + 1, 0] - beg[0], C[:jk + 1, 1] - beg[1]))) if jk > 0 else 0
    f_br = {}
    for f in np.round(np.arange(0.05, 0.96, 0.05), 2):
        f_br[float(f)] = float(breite[int(round(j0 + (jk - j0) * (1 - f)))] / H)
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
                biegung=P(jb), T2=rel(P(jb)), drehung=dreh_t, drehung_alt=float(abs(dth[jb])), t1_regel=t1_regel, knick=P(jk), T3=rel(P(jk)), ende=P(je), T4=rel(P(je)),
                ende_ueber_sohle=(so - C[je][1]) / H, T5=f_br, taille=taille, schmal_vor_knick=schmal_kette, end_breite=end_mittel,
                end_laenge=end_len, end_richtung=end_dir, end_vor=float(vor), C=C, breite=breite, jb=jb, jk=jk, je=je, maske=m)


def schwanz(mo, P):
    img, ids, iv = G.zeichne(mo, P['85'], voll=True)
    ok, so, H = G.h_port(iv)
    s = schwanz_mass_h(img, ids, ok, so)
    Z = []
    t1 = s['T1']
    Z.append(zeile('T1 Schwanz Beginn', '0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03)', '%.3f H hinter, %.3f H ueber' % t1,
                   abs(t1[0] - 0.15) <= 0.03 and abs(t1[1] - 0.767) <= 0.03, s['t1_regel']))
    t2 = s['T2']
    # Runde 3 (O2): EIN Verfahren (t2_mass_v4.py, Pruefer-Mittellinie), Ziel/Toleranz aus den Original-Masken
    to = T2M.original(); tp = T2M.port(ids, s['H'])
    dp = tp['drehung'] if tp else float('nan')
    # Runde 4 (Pruefung V4 r3 Zahlen 4.1): auch die LAGE mit dem O2-Werkzeug (Mitte des Fensters der groessten Drehung, relativ zum
    # T1-Beginn); die Bauer-Mittellinie (s['T2']) wird nur noch ausgegeben. Original mit demselben Werkzeug -0,268 / +0,056.
    tl = R4.t2_lage_port(ids, s['H'], s['beginn'])
    lo = R4.t2_lage_original()
    Z.append(zeile('T2 Biegung', 'Lage dx -0,245 dy +0,036 (+-0,03) nach t2_mass_v4 (Original %.3f / %.3f); Drehung %.1f +-%.1f Grad (Original %s)' % (
        np.mean([d['dx'] for d in lo]), np.mean([d['dy'] for d in lo]), to['ziel'], to['tol'], '/'.join('%.1f' % w for w in to['werte'])),
        'dx %.3f dy %.3f; %.1f Grad (Bauer-Mittellinie: dx %.3f dy %.3f)' % (tl['dx'], tl['dy'], dp, t2[0], t2[1]),
        abs(tl['dx'] + 0.245) <= 0.03 and abs(tl['dy'] - 0.036) <= 0.03 and abs(dp - to['ziel']) <= to['tol']))
    t3 = s['T3']
    Z.append(zeile('T3 Knick (Taille)', 'dx -0,265 dy +0,257 (+-0,03)', 'dx %.3f dy %.3f' % t3,
                   abs(t3[0] + 0.265) <= 0.03 and abs(t3[1] - 0.257) <= 0.03))
    t4 = s['T4']
    Z.append(zeile('T4 Ende', 'dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle', 'dx %.3f dy %.3f; %.3f H ueber Sohle' % (
        t4[0], t4[1], s['ende_ueber_sohle']), abs(t4[0] + 0.245) <= 0.03 and abs(t4[1] - 0.397) <= 0.03 and
        abs(s['ende_ueber_sohle'] - 0.37) <= 0.03))
    # Runde 2 (Pruefung V4 r1 Zahlen 4.1): die beschrifteten Punkte absolut in derselben Kamera (Pose 85 = Vorlage 2), Abstand je Achse
    # <= 0,03 H; Nutzer-Linienenden Messblatt V4 Abschnitt 1a: Beginn (357,243), Knick (290,308), Ende (295,343)
    nutzer = dict(Beginn=((357, 243), s['beginn']), Knick=((290, 308), s['knick']), Ende=((295, 343), s['ende']))
    ab = {k: ((q[0] - p[0]) / s['H'], (q[1] - p[1]) / s['H']) for k, (p, q) in nutzer.items()}
    Z.append(zeile('TA Schwanz absolut', 'Beginn (357,243), Knick (290,308), Ende (295,343) je Achse +-0,03 H',
                   '; '.join('%s (%.0f,%.0f) %+.3f/%+.3f H' % (k, nutzer[k][1][0], nutzer[k][1][1], *ab[k]) for k in ab),
                   all(abs(v[0]) <= 0.03 and abs(v[1]) <= 0.03 for v in ab.values())))
    f = s['T5']
    soll = {0.1: (0.03, 0.015), 0.3: (0.065, 0.02), 0.5: (0.125, 0.025), 0.7: (0.14, 0.025)}
    ok5 = all(abs(f[k] - v) <= t for k, (v, t) in soll.items())
    reihe = [f[k] for k in sorted(f) if 0.1 <= k <= 0.7 + 1e-9]
    abfall = max([reihe[i] - reihe[i + 1] for i in range(len(reihe) - 1)] + [0.0])
    Z.append(zeile('T5 Breite nach oben', 'f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01', '%.3f/%.3f/%.3f/%.3f H; Abfall %.3f' % (
        f[0.1], f[0.3], f[0.5], f[0.7], abfall), ok5 and abfall <= 0.01))
    # Runde 4 (Pruefung V4 r3 Zahlen 4.2): zweites, unabhaengiges Breitenmass (Pruefer pz3_t5orig: quer zur t2_mass_v4-Mittellinie)
    q5 = R4.t5_quer_port(ids, s['H'], s['beginn'], s['knick'])
    qo = R4.t5_quer_original()
    ok5q = all(abs(q5['f'][k] - v) <= t for k, (v, t) in soll.items()) and q5['abfall'] <= 0.01
    Z.append(zeile('T5q Breite quer (unabhaengig)', 'f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 (Original 3 Masken: Abfall %s)' % (
        '/'.join('%.3f' % d['abfall'] for d in qo)), '%.3f/%.3f/%.3f/%.3f H; Abfall %.3f; Profil f 0..1 %s' % (
        q5['f'][0.1], q5['f'][0.3], q5['f'][0.5], q5['f'][0.7], q5['abfall'], q5['profil']), ok5q))
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
    # T9 (Sicht-Pruefung V4 r1 1.3): Farbe entlang der Achse, je Punkt 5x5 um den naechsten Schwanzpixel (Videoraum) gegen das Original;
    # Riegel: Mittel dE00 <= 6 (Regionsregel des Messblatts) und Mittel-L* / a* der 7 Punkte innerhalb 4 / 3 vom Original
    ar = achse_regionen(ids)
    werte = []
    for k9, (m9, z9) in ar.items():
        la9 = lab1(V[m9].mean(0)) if m9.sum() else np.full(3, np.nan)
        werte.append((k9, la9, z9, G.de00(la9, z9) if m9.sum() else 99.0))
    if werte:
        dm = float(np.mean([w[3] for w in werte]))
        lo = np.mean([w[2] for w in werte], 0); lp = np.mean([w[1] for w in werte], 0)
        ok9 = dm <= 6 and abs(lp[0] - lo[0]) <= 4 and abs(lp[1] - lo[1]) <= 3
        Z.append(zeile('T9 Schwanzachse', '7 Achsenpunkte: Mittel dE00 <= 6; L*/a* Mittel +-4/+-3 zum Original %.1f/%.1f' % (lo[0], lo[1]),
                       'Mittel dE00 %.1f; L*/a* %.1f/%.1f; je Punkt %s' % (dm, lp[0], lp[1], ' '.join('%.1f' % w[3] for w in werte)), ok9))
    return Z, s, (img, ids, V, fr, dict(F15=r15, F16=r16, F17=r17, F18=r18))


# ---------------------------------------------------------------------------------------------------- Koerperfarben
def koerper_farben(mo, P):
    img, ids = G.zeichne(mo, P['135'])
    V = G.videoraum(img); Lab = G.lab(V)
    rg = (V[..., 0] - V[..., 1]) >= 15

    def zeilenteil(m, f0, f1):
        ys_ = np.nonzero(m.any(1))[0]
        if not len(ys_):
            return m
        y0_, y1_ = ys_.min(), ys_.max() + 1
        b_ = np.zeros(m.shape, bool); b_[int(round(y0_ + f0 * (y1_ - y0_))):int(round(y0_ + f1 * (y1_ - y0_)))] = True
        return m & b_
    # Runde 2: F7/F8 an der Lage der Messblatt-Fenster im Oberarm (F7 y 167-212 im Arm y 160-250 = obere 58 %, F8 y 182-227 im Arm
    # y 182-262 = obere 56 %); die Id-Masken ganz (Runde-1-Mass) werden als F7g/F8g mit ausgegeben
    # Runde 5 (Entscheid O7): F6 an der Fensterlage des Messblatts (rote Id-8-Bildpunkte im Fenster 372-424 x 197-232); 'F6g' = Id 8 ganz
    # (Messblatt-Wortlaut, nur noch Ausgabe)
    fb6 = np.zeros(ids.shape, bool); fb6[197:233, 372:425] = True
    reg = {'F6': er(ids == 8) & rg & fb6, 'F6g': er(ids == 8) & rg,
           'F7': zeilenteil(er(ids == 13), 0.0, 0.58), 'F8': zeilenteil(er(ids == 10), 0.0, 0.56),
           'F9': er(np.isin(ids, (0, 1, 4))) & (Lab[..., 2] < 0),
           'F10': er(ids == 12, 1) & rg, 'F10_id15': er(ids == 15, 1) & rg, 'F7g': er(ids == 13), 'F8g': er(ids == 10),
           # Sicht-Pruefung V4 r1 1.4 (Boxen pruefung_sicht/messen_sicht.py BOX1, Original-Ziele hier gemessen): Oberarme unten, Rumpf
           # gesamt, Unterarme, Beine
           'FS_oa_bl_u': zeilenteil(er(ids == 13), 0.58, 1.0), 'FS_oa_br_u': zeilenteil(er(ids == 10), 0.56, 1.0),
           'FS_rumpf': er(np.isin(ids, (7, 8))), 'FS_unterarm': er(ids == 11), 'FS_beine': er(np.isin(ids, (2, 3, 5, 6))),
           # Runde 3: Unterarm (W)-rechts = (B)-links (O5) und die ganzen Oberarme gegen die Oberarm-Boxen der Sicht-Pruefung
           'FS_unterarm_wr': er(ids == 14), 'FS_oa_bl': er(ids == 13), 'FS_oa_br': er(ids == 10)}
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
        Z.append(zeile('F %s' % k + (' (Fensterlage, O7)' if k in ('F6', 'F7') else ''), '%s dE00 <= 5, |dL| <= 4' % '/'.join('%.1f' % v for v in ZIEL[k]),
                       '%s dE00 %.1f dL %+.1f (%d px)' % ('/'.join('%.1f' % v for v in la), dd, dl, n), dd <= 5 and abs(dl) <= 4,
                       'Id 12 (Klaue (B)-rechts); Id 15: %s (%d px)' % ('/'.join('%.1f' % v for v in erg['F10_id15'][0]),
                                                                       erg['F10_id15'][1]) if k == 'F10' else ''))
    mt = float(np.mean(des))
    Z.append(zeile('F Mittel', 'Mittel dE00 <= 4', '%.2f' % mt, mt <= 4))
    # Runde 5 (Entscheid O7, Messblatt V4 "Nachtrag Runde 5"): F6 und F7 an der Fensterlage; die Lesart "Id ganz" verlangt einen ganz roten
    # Rumpf bzw. einen hellen ganzen Oberarm und widerspricht dem Original (Sicht-Pruefung V4 r4 1.5) - nur noch Ausgabe (INFO).
    # F8 nach Wortlaut (Id 10 ganz) bleibt ein Riegel (nicht Gegenstand von O7).
    for k in ('F6g', 'F7g', 'F8g'):
        la, n = erg[k]; zk = ZIEL[k[:2]]
        dd, dl = G.de00(la, zk), la[0] - zk[0]
        Z.append(zeile('F %s Messblatt-Wortlaut (Id ganz)' % k[:2], '%s dE00 <= 5, |dL| <= 4%s' % (
            '/'.join('%.1f' % v for v in zk), ' (INFO, O7: kein Riegel)' if k != 'F8g' else ''),
            '%s dE00 %.1f dL %+.1f (%d px)' % ('/'.join('%.1f' % v for v in la), dd, dl, n), dd <= 5 and abs(dl) <= 4,
            'Entscheid O7: gemessen wird an der Fensterlage' if k != 'F8g' else 'Runde 3: Mesh 10 mit eigener CLUT-Zeile 489 (O1)',
            info=(k != 'F8g')))
    o1 = G.vorlage(1)
    sicht = dict(FS_oa_bl_u=([(300, 213, 345, 250)], 'Oberarm (B)-links unten'), FS_oa_br_u=([(455, 228, 500, 262)], 'Oberarm (B)-rechts unten'),
                 FS_rumpf=([(352, 196, 448, 292), (318, 160, 360, 190), (425, 165, 470, 195)], 'Rumpf + Schultern'),
                 FS_unterarm=([(468, 268, 500, 330)], 'Unterarm (B)-rechts = (W)-links Id 11'), FS_beine=([(356, 352, 392, 430)], 'Unterschenkel'),
                 FS_unterarm_wr=([(298, 262, 325, 340)], 'Unterarm (B)-links = (W)-rechts Id 14'))
    for k, (boxen, nm) in sicht.items():
        mm = np.zeros((480, 640), bool)
        for (x0, y0, x1, y1) in boxen:
            b_ = np.zeros((480, 640), bool); b_[y0:y1 + 1, x0:x1 + 1] = True; mm |= er(b_)
        op = o1[mm]
        zo = lab1(op.mean(0)); rot_o = float(((op[:, 0] - op[:, 1]) >= 15).mean())
        la, n = erg[k]
        rp = V[reg[k]]; rot_p = float(((rp[:, 0] - rp[:, 1]) >= 15).mean()) if len(rp) else float('nan')
        dd = G.de00(la, zo) if n else 99.0
        ok_ = dd <= 5 and abs(la[0] - zo[0]) <= 4 and abs(rot_p - rot_o) <= 0.15
        zus = ''
        if k == 'FS_unterarm_wr':     # O5: keine weissen Flecken - Anteil L* > 30 hoechstens Original + 0,05 (Original 0)
            hell_o = float((G.lab(op)[:, 0] > 30).mean()); hell_p = float((G.lab(rp)[:, 0] > 30).mean()) if len(rp) else 1.0
            p90 = float(np.percentile(G.lab(rp)[:, 0], 90)) if len(rp) else 99.0; p90o = float(np.percentile(G.lab(op)[:, 0], 90))
            ok_ = ok_ and hell_p <= hell_o + 0.05
            zus = '; Anteil L* > 30 %.3f (Original %.3f, <= +0,05); L* p90 %.1f (Original %.1f)' % (hell_p, hell_o, p90, p90o)
        Z.append(zeile('FS %s' % nm, '%s, Rotanteil %.2f; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15%s' % (
            '/'.join('%.1f' % v for v in zo), rot_o, '; Anteil L* > 30 <= Original + 0,05' if k == 'FS_unterarm_wr' else ''),
            '%s dE00 %.1f dL %+.1f; Rotanteil %.2f (%d px)%s' % ('/'.join('%.1f' % v for v in la), dd, la[0] - zo[0], rot_p, n, zus),
            ok_, 'Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen %s' % boxen))
    # Runde 5 (O7): F7 an der Fensterlage zusammen mit der Sicht-Box des unteren Oberarms
    f7 = next(z for z in Z if z['mass'].startswith('F F7 (Fensterlage'))
    fs7 = next(z for z in Z if z['mass'] == 'FS Oberarm (B)-links unten')
    f7['ok'] = f7['ok'] and fs7['ok']
    f7['note'] = 'O7: obere 58 %% von Id 13 (Fensterlage) und FS Oberarm (B)-links unten (%s)' % ('OK' if fs7['ok'] else 'ROT')
    # F0 Gueltigkeit: Wand links im Render (BSS-Hintergrund)
    w = lab1(G.mittel(V, (40, 150, 119, 299)))
    d0 = G.de00(w, ZIEL['F0'])
    Z.append(zeile('F0 Gueltigkeit Farbvergleich', 'Wand links 64,7/0,3/-2,1 dE00 <= 2', '%s dE00 %.2f' % ('/'.join('%.1f' % v for v in w), d0),
                   d0 <= 2))
    return Z, erg, (img, ids, V, img2, ids2, V2, reg, reg2)

# ---------------------------------------------------------------------------------------------------- Runde 4 (Pruefung V4 r3)
def schwanz_form(mo, P):
    """Sicht-Pruefung V4 r3 1.3 (Klasse A "Vierkant-Balken"): Querprofil und Oberkontur des waagerechten Teils (r4_mass, Verfahren des
    Sicht-Pruefers), Original = Vorlage 2 roh in den 3 Leser-Masken."""
    img, ids, iv = G.zeichne(mo, P['85'], voll=True)
    _ok, _so, H = G.h_port(iv)
    fp = R4.form_port(img, ids); fo = R4.form_original()
    st_max = max(d['quer'][x]['stufe3px'] for d in fo for x in R4.XS_QUER)
    pl_max = max(d['quer'][x]['plateau'] for d in fo for x in R4.XS_QUER)
    q = fp['quer']
    okq = all(q[x] is not None and q[x]['stufe3px'] <= st_max + 1e-9 and q[x]['plateau'] <= pl_max + 1e-9 for x in R4.XS_QUER)
    Z = [zeile('TQ Schwanz Querprofil', 'x 310/320/330/340: Stufe auf 3 px <= %.1f L*, Plateau-Anteil <= %.2f (Original-Maximum, 3 Masken)' % (st_max, pl_max),
               '; '.join('x %d %s' % (x, ('Stufe %.1f Plateau %.2f' % (q[x]['stufe3px'], q[x]['plateau'])) if q[x] else '-') for x in R4.XS_QUER), okq,
               'Sicht-Pruefung V4 r3 1.3: V4 r3 16,6-19,9 L*, 0,33-0,43')]
    rms_o = [d['kontur']['rms'] for d in fo]
    rms_min = min(rms_o) - (max(rms_o) - min(rms_o))
    ko = fo[0]['kontur']['kante']; kp = fp['kontur']['kante']
    ab = {x: (abs(kp[x] - ko[x]) / H if kp[x] is not None else 1.0) for x in R4.X_KANTE}
    okk = fp['kontur']['rms'] >= rms_min and all(v <= 0.03 for v in ab.values())
    Z.append(zeile('TK Schwanz Oberkontur', 'x 284-332: RMS zur Geraden >= %.2f px (Original %s minus Spanne); Oberkante x 276-292 je Spalte '
                   '<= 0,03 H vom Original %s' % (rms_min, '/'.join('%.2f' % r for r in rms_o), '/'.join(str(ko[x]) for x in R4.X_KANTE)),
                   'RMS %.2f px; Oberkante %s (max %.3f H)' % (fp['kontur']['rms'], '/'.join(str(kp[x]) for x in R4.X_KANTE), max(ab.values())),
                   okk, 'Sicht-Pruefung V4 r3 1.3: V4 r3 0,6 px, Oberkante 237/235/232/230/228; Toleranz Oberkante = TA (0,03 H)'))
    return Z, dict(port=fp, original=fo)


# ---------------------------------------------------------------------------------------------------- Runde 5 (Entscheide O8-O10)
def schwanz_verlauf(mo, P):
    """O8 (Pruefung V4 r4 Sicht 1.3 Klasse A): Quernaht im waagerechten Teil (r5_mass.naht_waagerecht = Sicht-Pruefer schwanz_naehte.py),
    Farbe/Helligkeit an 5 Stationen entlang der Achse (r5_mass.schwanz_achse_*), Rotanteil a* > 12 im waagerechten Teil."""
    import r5_mass as M5
    img, ids, iv = G.zeichne(mo, P['85'], voll=True)
    _ok, _so, H = G.h_port(iv)
    s = schwanz_mass_h(img, ids, _ok, _so)
    V = G.videoraum(img); sw = np.isin(ids, SCHW)
    o2 = G.vorlage(2); mo_ = R4.masken_original()
    no = [M5.naht_waagerecht(o2, t)['max'] for t in mo_]
    npt = M5.naht_waagerecht(V, sw)
    Z = [zeile('TN Schwanz Quernaht waagerecht', 'groesster Sprung dE76 ueber 3 Spalten (x 280-376, Mitte der Maske) <= %.1f (Original-Maximum, 3 Masken %s)' % (
        max(no), '/'.join('%.1f' % v for v in no)), '%.1f bei x %s' % (npt['max'], npt['bei']), npt['max'] <= max(no) + 1e-9,
        'Sicht-Pruefung V4 r4 1.3: V4 r4 16,7 an Mesh 17|18 / 16|17')]
    ao = M5.schwanz_achse_original()
    ap = M5.schwanz_achse_port(img, ids, H, s['beginn'], s['knick'], s['ende'])
    teile, ok_v = [], True
    for nm, _a, _f in M5.STATIONEN:
        lo = np.nanmean([d['stationen'][nm]['lab'] for d in ao], 0)       # Maske hi endet vor dem Endstueck (nan)
        lp = np.array(ap['stationen'][nm]['lab']) if ap else np.full(3, np.nan)
        okk = abs(lp[0] - lo[0]) <= 4 and abs(lp[1] - lo[1]) <= 3
        ok_v &= bool(okk)
        teile.append('%s %.1f/%.1f (Original %.1f/%.1f)%s' % (nm, lp[0], lp[1], lo[0], lo[1], '' if okk else ' ROT'))
    Z.append(zeile('TV Schwanz Verlauf entlang der Achse', 'L*/a* an Wurzel, Mitte, Biegung, senkrecht, Endstueck je +-4 / +-3 zum Original (Toleranz '
                   'wie T9; Stationen r5_mass.STATIONEN)', '; '.join(teile), ok_v, 'Sicht-Pruefung V4 r4 1.3: Helligkeit verkehrt, an der Biegung L* 7,3'))
    ro = [M5.schwanz_rot(o2, t, False) for t in mo_]
    rp = M5.schwanz_rot(img, sw)
    Z.append(zeile('TR Schwanz Rotanteil waagerecht', 'Anteil a* > 12 (x 284-356, y < 262) %.3f +-0,05 (Original %s; Toleranz wie FO-Schultern)' % (
        float(np.mean(ro)), '/'.join('%.3f' % v for v in ro)), '%.3f' % rp, abs(rp - float(np.mean(ro))) <= 0.05,
        'Sicht-Pruefung V4 r4 1.3: V4 r4 0,19'))
    return Z, dict(naht=npt, achse=ap, achse_original=ao, rot=rp)


def ausreisser_farben(mo, P):
    """O9: Ausreisserfarben je Koerper-Mesh (r5_mass.ausreisser) in beiden Messansichten."""
    import r5_mass as M5
    Z, info = [], {}
    for ans, vl in (('135', 'Vorlage 1'), ('85', 'Vorlage 2')):
        img, ids = G.zeichne(mo, P[ans])
        r = M5.ausreisser(img, ids, ans)
        info[ans] = r
        bew = {b: v for b, v in r.items() if v['anteil'] is not None}
        schlecht = {b: round(v['anteil'], 3) for b, v in bew.items() if v['anteil'] > M5.AF_ANTEIL}
        gelb = sum(v['gelb'] for v in r.values())
        bmax = max(bew, key=lambda b: bew[b]['anteil'])
        Z.append(zeile('AF Ausreisserfarben Pose %s' % ans,
                       'je Mesh 0-14 Anteil Bildpunkte > %.0f dE76 von jeder Farbe der Original-Region (%s) <= %.2f; gelbe Bildpunkte gezaehlt' % (
                           M5.AF_DE, vl, M5.AF_ANTEIL),
                       'max %.3f (Id %d); ueber der Grenze %s; gelb %d px (je Id %s)' % (
                           bew[bmax]['anteil'], bmax, schlecht or 'keine', gelb, {b: v['gelb'] for b, v in r.items() if v['gelb']} or '-'),
                       not schlecht, 'Pruefung V4 r4 Sicht 1.5: V4 r4 Mesh 13 45 gelbe Texel; Id 15 ohne Region (Referenz-2-Farben)'))
    return Z, info


def oberkoerper_vorn(mo, P):
    """Sicht-Pruefung V4 r3 1.5 (Klasse A "Rot-Verteilung"): Buehne Pose 135 gegen Vorlage 1 (r4_mass.oberkoerper_*), Riegel nach dem
    Vorschlag des Sicht-Pruefers: Rot Mitte/aussen >= 2,75, Schultern a* > 12 <= 0,05; dazu Anteil a* > 18 <= Original."""
    img, ids = G.zeichne(mo, P['135'])
    d = R4.oberkoerper_buehne(img, ids); o = R4.oberkoerper_original()
    a18o, a18p = o['a18'] / o['n'], d['a18'] / d['n']
    ok = d['rot_m_a'] >= 2.75 and d['schulter_a12'] <= 0.05 and a18p <= a18o
    return [zeile('FO Oberkoerper vorn Rot-Verteilung (Buehne)', 'Rot Mitte/aussen >= 2,75 (Original %.2f); Schultern a*>12 <= 0,05 (Original %.3f); '
                  'Anteil a*>18 <= %.3f (Original)' % (o['rot_m_a'], o['schulter_a12'], a18o),
                  'Mitte/aussen %.2f (Drittel %.2f/%.2f/%.2f); Schultern %.3f; a*>18 %.3f' % (
                      d['rot_m_a'], d['B_links']['rot'], d['Mitte']['rot'], d['B_rechts']['rot'], d['schulter_a12'], a18p), ok,
                  'Sicht-Pruefung V4 r3 1.5 / Riegelvorschlag Abschnitt 3; V4 r3 1,21 / 0,089')], dict(port=d, original=o)


def spielbild(lauf):
    """Sicht-Pruefung V4 r3 1.1 / 1.5 im SPIELBILD (RE15_FRAMEDUMP des Laufs, registriert ueber nb2neu/c6.port): Kopf-Innenflaeche
    (Gradient in der Originalspanne, glatte Pixel), Oberkoerper (Rot Mitte/aussen, Schultern, Anteil a* > 18 gegen das Videobild)."""
    d = os.path.dirname(lauf)
    sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
    os.environ['NB6_RUNS'] = os.path.dirname(d)
    import c6
    ko = R4.kopf_innen_original()
    g = R4._grad_lum(G.vorlage(1)); x0, y0, x1, y1 = R4.KOPF_INNEN_ORIG
    glatt_max = max(float((g[y0 + dy:y1 + dy + 1, x0 + dx:x1 + dx + 1] < 8).mean()) for dx in (-3, 0, 3) for dy in (-3, 0, 3))
    Z, info = [], {}
    # Runde 5 (O10): kraeftiges Rot a* > 22 im Spielbild "in der Groessenordnung des Videos" - Fenster des Sicht-Pruefers V4 r4
    # (pruefung_sicht_r4/rotkappe_spiel.py: h85 300-480 x 150-380, h100 300-480 x 150-330) bzw. die Oberkoerper-Polygone (h118/h135);
    # Grenze: hoechstens 10 x Video (eine Groessenordnung), mindestens 1 Promille der Flaeche (Zaehlgrenze bei Video 0)
    for h, fen in ((85, (300, 150, 480, 380)), (100, (300, 150, 480, 330)), (118, None), (135, None)):
        fd = c6.port(os.path.abspath(d), h)
        if fd is None:
            continue
        fd = cv2.cvtColor(fd, cv2.COLOR_BGR2RGB).astype(float)
        vid = cv2.cvtColor(c6.vid(h), cv2.COLOR_BGR2RGB).astype(float) if h != 135 else G.vorlage(1)
        if fen is not None:
            x0, y0, x1, y1 = fen
            mp = np.zeros((480, 640), bool); mp[y0:y1, x0:x1] = True; mv = mp
        else:
            mp = R4.poly_maske(R4.PS_SPIEL[h]); mv = R4.poly_maske(R4.PV_SPIEL[h])
        np_ = int((G.lab(G.videoraum(fd)[mp])[:, 1] > 22).sum()); nv = int((G.lab(vid[mv])[:, 1] > 22).sum())
        gr = max(10 * nv, int(0.001 * mp.sum()))
        Z.append(zeile('FR Kraeftiges Rot Spielbild h%d' % h, 'Anzahl a* > 22 <= max(10 x Video %d, 1 Promille %d) = %d' % (
            nv, int(0.001 * mp.sum()), gr), '%d' % np_, np_ <= gr, 'Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)'))
    for h in (118, 135):
        fd = c6.port(os.path.abspath(d), h)
        if fd is None:
            continue
        fd = cv2.cvtColor(fd, cv2.COLOR_BGR2RGB).astype(float)
        k = R4.kopf_innen_spiel(fd, h)
        Z.append(zeile('KS Kopf-Struktur Spielbild h%d' % h, 'Innenflaeche Gradient %.1f-%.1f (Original, Box +-3 px); glatte Pixel <= %.3f (Original max)' % (
            ko['spanne'][0], ko['spanne'][1], glatt_max), 'Gradient %.1f; glatt %.3f' % (k['grad'], k['glatt']),
            ko['spanne'][0] <= k['grad'] <= ko['spanne'][1] and k['glatt'] <= glatt_max, 'Sicht-Pruefung V4 r3 1.1: V4 r3 h135 27,2 / 0,197, h118 26,9 / 0,126'))
        vid = c6.vid(h)
        vid = (cv2.cvtColor(vid, cv2.COLOR_BGR2RGB).astype(float) if h == 118 else G.vorlage(1))
        dv = R4.drittel(vid, R4.poly_maske(R4.PV_SPIEL[h]))
        mp = R4.poly_maske(R4.PS_SPIEL[h])
        dp = R4.drittel(G.videoraum(fd), mp)
        a18v, a18p = dv['a18'] / dv['n'], dp['a18'] / dp['n']
        Z.append(zeile('FO Oberkoerper Spielbild h%d' % h, 'Rot Mitte/aussen >= 2,75 (Video %.2f); Schultern a*>12 <= 0,05 (Video %.3f); Anteil a*>18 <= Video %.3f' % (
            dv['rot_m_a'], dv['schulter_a12'], a18v), 'Mitte/aussen %.2f (Drittel %.2f/%.2f/%.2f); Schultern %.3f; a*>18 %.3f; L* p10 %.1f (Video %.1f)' % (
            dp['rot_m_a'], dp['B_links']['rot'], dp['Mitte']['rot'], dp['B_rechts']['rot'], dp['schulter_a12'], a18p, dp['L_p10'], dv['L_p10']),
            dp['rot_m_a'] >= 2.75 and dp['schulter_a12'] <= 0.05 and a18p <= a18v,
            'Sicht-Pruefung V4 r3 1.5 (Polygone von Hand, oberkoerper_spiel.py); V4 r3 h118 1,18 / 0,13'))
        info[h] = dict(kopf=k, oberkoerper=dp, video=dv)
    return Z, info


def kopf_buehne320(mo, P):
    img, ids = G.zeichne(mo, P['135'])
    k = R4.kopf_innen_buehne(img, ids, True); ko = R4.kopf_innen_original()
    return [zeile('KS Kopf-Struktur Buehne 320 x 240', 'Innenflaeche Gradient %.1f-%.1f (Original); Naeherung des Spielbilds' % ko['spanne'],
                  'Gradient %.1f; glatt %.3f' % (k['grad'], k['glatt']), ko['spanne'][0] <= k['grad'] <= ko['spanne'][1],
                  'Buehne Pose 135 ueber 320 x 240 (INTER_AREA wie c6.port); V4 r3 28,0')]



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
    # Runde 3 (O1): Geometrie je Mesh = Ecken, Normalen, Flaechen-Indizes, UV-Koordinaten; das CLUT-Feld darf abweichen (ausgegeben)
    def geo(m):
        return (tuple(map(tuple, m.tv)), tuple(map(tuple, m.tn)), tuple(map(tuple, m.tf)), tuple(map(tuple, m.qv)), tuple(map(tuple, m.qn)),
                tuple(map(tuple, m.qf)), tuple(tuple(u[i] for i in range(len(u)) if i % 3 != 2) for u in m.tuv),
                tuple(tuple(u[i] for i in range(len(u)) if i % 3 != 2) for u in m.quv), tuple(u[5] for u in m.tuv), tuple(u[5] for u in m.quv))
    abw = [b for b in list(range(9)) + list(range(10, 16)) if geo(mo.meshes[b]) != geo(mr.meshes[b])]
    clut_neu = {b: (sorted({u[2] >> 6 for u in list(mr.meshes[b].tuv) + list(mr.meshes[b].quv)}),
                    sorted({u[2] >> 6 for u in list(mo.meshes[b].tuv) + list(mo.meshes[b].quv)}))
                for b in list(range(9)) + list(range(10, 16))}
    clut_neu = {b: v for b, v in clut_neu.items() if v[0] != v[1]}
    bytegleich = [b for b in list(range(9)) + list(range(10, 16)) if MV.mesh_bytes(mo.meshes[b]) == MV.mesh_bytes(mr.meshes[b])]
    sk, skr = mo.skel, mr.skel
    skel = all(tuple(sk.rel[b]) == tuple(skr.rel[b]) and sk.parent[b] == skr.parent[b] for b in range(16))
    kf = sk.kf_count == skr.kf_count and all(
        all(tuple(EA.kf_angles(sk, k, b)) == tuple(EA.kf_angles(skr, k, b)) for b in range(16)) and
        tuple(EA.kf_pos(sk, k)) == tuple(EA.kf_pos(skr, k)) for k in range(sk.kf_count))
    edd = blob[d[1]:d[2]] == rb[dr[1]:dr[2]]
    sonst = all(blob[d[i]:d[i + 1]] == rb[dr[i]:dr[i + 1]] for i in (0, 3, 4, 5, 6))
    Z = []
    Z.append(zeile('G1 Koerper-Geometrie = Ref2', 'Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV gleich (O1: CLUT-Feld frei); Skelett/Keyframes 0-15, EDD gleich',
                   'Geometrie abweichend %s; CLUT-Feld geaendert %s; byte-gleich %d von 15; Skelett %s; Keyframes %s; EDD %s; uebrige Sektionen %s; Ref2 sha256 %s' % (
                       abw or 'keine', {b: '%s -> %s' % v for b, v in clut_neu.items()} or 'keine', len(bytegleich), skel, kf, edd, sonst,
                       hashlib.sha256(rb).hexdigest()[:8]), not abw and skel and kf and edd))
    ds = sum(MV.dreiecke(mo.meshes[b]) for b in SCHW)
    Z.append(zeile('M1 Dreiecke Schwanz', '<= 212', '%d (%s)' % (ds, [MV.dreiecke(mo.meshes[b]) for b in SCHW]), ds <= 212))
    dk = MV.dreiecke(mo.meshes[9])
    Z.append(zeile('M2 Dreiecke Kopf', '<= 118', '%d' % dk, dk <= 118))
    tim = blob[d[8]:]
    bpp8 = (EA.u32(tim, 4) & 7) == 1
    zeilen9 = sorted({uv[2] >> 6 for uv in list(mo.meshes[9].tuv) + list(mo.meshes[9].quv)})
    zeilen_s = sorted({uv[2] >> 6 for b in SCHW for uv in list(mo.meshes[b].tuv) + list(mo.meshes[b].quv)})
    clut_h = mo.pal.shape[0]
    zeilen_k = sorted({uv[2] >> 6 for b in list(range(9)) + list(range(10, 16)) for uv in list(mo.meshes[b].tuv) + list(mo.meshes[b].quv)})
    ok3 = bpp8 and all(480 <= z < 480 + clut_h for z in zeilen9 + zeilen_s + zeilen_k)
    Z.append(zeile('M3 Farbtiefe', '8-bpp-TIM, Texel aus der CLUT', '8 bpp %s; CLUT-Zeilen Kopf %s, Schwanz %s, Koerper %s (TIM %d Zeilen)' % (
        bpp8, zeilen9, zeilen_s, zeilen_k, clut_h), ok3))
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
        # Runde 3 (O3): T7 koerperfest (t7_koerperfest_v4.py) statt Bildhoehe in der Vorlage-2-Kamera (gang(), Runde 1/2)
        import t7_koerperfest_v4 as T7K
        Z += T7K.riegel(T7K.messen(a.emd, a.lauf))
        lp = lichtprobe(a.lauf)
        for h, (x, y, d, n) in lp.items():
            Z.append(zeile('F0 Lichtgegenprobe h%d' % h, 'Rumpf b2 gegen FRAMEDUMP dE00 <= 3', 'b2 %s / Spiel %s dE00 %.1f (%d px)' % (
                '/'.join('%.1f' % v for v in x), '/'.join('%.1f' % v for v in y), d, n), d <= 3))
    zf, ef, _ = koerper_farben(mo, P); Z += zf
    # Runde 4 (Pruefung V4 r3): Schwanzform, Oberkoerper-Rotverteilung, Kopfstruktur im Spielbild
    zq, iq = schwanz_form(mo, P); Z += zq
    zv, iv_ = schwanz_verlauf(mo, P); Z += zv               # Runde 5 (O8)
    za, ia = ausreisser_farben(mo, P); Z += za              # Runde 5 (O9)
    zo, io = oberkoerper_vorn(mo, P); Z += zo
    Z += kopf_buehne320(mo, P)
    ispiel = {}
    if a.lauf:
        zs4, ispiel = spielbild(a.lauf); Z += zs4
    rot = [z for z in Z if not z['ok']]
    for z in Z:
        print('%-4s %-34s soll: %-60s ist: %s %s' % ('INFO' if z.get('info') else ('OK' if z['ok'] else 'ROT'), z['mass'], z['soll'], z['ist'],
                                                     ('[' + z['note'] + ']') if z['note'] else ''))
    print('ROT: %d von %d (INFO-Zeilen nicht gezaehlt: %d)' % (len(rot), len([z for z in Z if not z.get('info')]),
                                                            len([z for z in Z if z.get('info')])))
    if a.json:
        json.dump(dict(emd=a.emd, sha256=hashlib.sha256(open(a.emd, 'rb').read()).hexdigest(), zeilen=Z,
                       kopf_vorn=ik, kopf_seite=is_, schwanz={k: v for k, v in st.items() if k not in ('C', 'breite', 'maske')}),
                  open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    if a.md:
        with open(a.md, 'w', encoding='utf-8') as f:
            f.write('| Mass | Soll | Ist | |\n|---|---|---|---|\n')
            for z in Z:
                f.write('| %s | %s | %s | %s |\n' % (z['mass'], z['soll'], z['ist'] + ((' (' + z['note'] + ')') if z['note'] else ''),
                                                     'INFO' if z.get('info') else ('OK' if z['ok'] else 'ROT')))
    if rot and not a.kein_exit:
        sys.exit(1)


if __name__ == '__main__':
    main()
