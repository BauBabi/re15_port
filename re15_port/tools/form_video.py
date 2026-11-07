#!/usr/bin/env python3
"""Vorwaertsfilter Spielbild -> Video und seine Umkehrung fuer die Bahnsteig-Form.

    from form_video import video_filter, video_umkehr
    bild = video_filter(rgb, bild=2)          # rgb 320x240 oder 640x480, uint8 -> 640x480 uint8

    python re15_port/tools/form_video.py pruefung   # analysis/.../form/farbe_filter_pruefung.png + Masszahlen
    python re15_port/tools/form_video.py umkehr     # analysis/.../form/farbe_bild{1,2}_echt.png

⛔ KEIN RE-WERT. Das hier ist die Uebertragung der Fernseh-Abfilmung "Biohazard 1.5 Final Build.mp4", gemessen
an 39 Standbildern desselben Videos gegen die Engine-dekodierten Hintergruende (tests/unit/probe_bg_dump.c).
Keine Zahl hier gehoert in die Engine. Messung, Belege, Fehler: analysis/befunde_runde41/form/farbe.md

Die Kette, Stufe fuer Stufe, mit der Messung dahinter:
  1 Geometrie     Spielbild 640x480 -> Video (Homographie). Bild 2 gemessen (ROOM5090 Cut 11, ECC 0.675).
                  Bild 1 hat keinen Schnitt auf der Disc: Median der 35 registrierten Dias (ECC > 0.78).
  2 Licht         L = g^2.2. Unschaerfe im Licht schlaegt Unschaerfe im Signal; kappa(8 px) geht dann auf 0.
  3 Optik         Gauss sigma 2 px, dazu Streulicht KA*G(32 px) + KB*G(128 px). Probe an der im Spiel schwarzen
                  Tuer in Bild 2: vorhergesagt L (0.0092,0.00084,0.00016), gemessen (0.0086,0.00098,0.00011).
  4 Kennlinie     Rampe e -> V je Kanal: LO schwarz, HI weiss, weiche Enden K. Fuer dunkle Szenen fest
                  (15 Dias, Belichtung +-5..10 %): die Kamera steht am Verstaerkungsanschlag. EX ist die
                  Kanalbalance des Einzelbilds. Bild 2 gemessen an den Waenden, Probe am weissen Mantel der Frau.
  5 Ueberlauf     Kanal ueber HI laeuft in die anderen (CCD-Blooming). Darum wird die rote Lampe im Spiel
                  (161,12,8) im Video (208,198,199). WEISS IM VIDEO HEISST NICHT WEISS IM SPIEL.
  6 Chroma        Farbdifferenzen zusaetzlich unscharf, sigma 6 x 3 px.
  7 Zeilen/Korn   Zeilenmuster und Rauschen, fester Saat-Wert: gleicher Eingang, gleiches Bild.
"""
import argparse, os, sys
import numpy as np
import cv2

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
FORM = os.path.join(REPO, 'analysis', 'befunde_runde41', 'form')
HILFE = os.path.join(FORM, 'lesehilfen')

GAMMA = 2.2
SIG_OPTIK = 2.0                 # px Video. Kante 10-90 Median 4.5-5.1 px = sigma 1.8-2.0; SSIM-Optimum 2.5 enthaelt Registrierfehler
SIG_GLOW = (32.0, 128.0)        # px Video
KA = np.array([0.1333, 0.0985, 0.1050])
KB = np.array([0.1384, 0.0963, 0.0820])
LO = np.array([22.4047, 17.6708, 9.0197])   # e, Video-Schwarz
HI = np.array([89.3726, 78.4299, 62.8290])  # e, Video-Weiss
K = 8.9257                      # Weichheit der Rampenenden
W = 227.0                       # Weiss des Videos (Plateau in allen Dias)
UEBERLAUF = np.array([2.5, 3.0, 3.5])       # V je e-Stufe ueber HI, Quelle R, G, B
UE_WEICH = 4.0                  # e-Stufen: Daten R +12 bei e 85, +57 bei 115; B -9 bei 65, +84 bei 85
CHROMA = (6.0, 3.0)             # sigma x, y in px

# Kanalbalance EX (Faktor auf e). Bild 2: Waende, Bootstrap +-(0.02,0.03,0.04).
# Bild 1: Mittel aus Szenen-Nachbarn (0.89,0.83,0.83) und Leon-Gesicht unter Ambient (0.98,0.94,0.985); +-0.08.
EX = {1: np.array([0.935, 0.887, 0.908]), 2: np.array([0.951, 0.665, 0.620])}

# Spiel 640x480 -> Video.
H_GEO = {
    1: np.array([[0.95619763, 0.08914374, -34.97503281],
                 [-0.05686561, 1.12922709, -9.7005806],
                 [-0.00022169, 0.00019826, 1.0]]),
    2: np.array([[0.99342448, 0.07417727, -23.2096201],
                 [-0.04547224, 1.16279475, -27.51075257],
                 [-0.00022186, 0.00019763, 1.0]]),
}
# Zeilenmuster: Periode px, Amplitude V im Hellen. Bild 2 helle Bodenflaeche (290-430,425-475): 5.0-5.6 px, Amp 3.4.
# Bild 1 Boden: Periode 2 px, Amp 0.7. Korn: Hochpass-Std in hellen Flaechen 4-7, Korrelationslaenge 2-3 px.
ZEILEN = {1: (2.0, 0.7), 2: (5.2, 3.4)}
KORN = {1: 2.5, 2: 2.0}
SAAT = 15

_ST = np.array([[2, 0, 0.5], [0, 2, 0.5], [0, 0, 1]], float)
_UG = np.linspace(-2.5, 3.5, 6001)


def _soft(u):
    return (np.logaddexp(0, K*u) - np.logaddexp(0, K*(u-1))) / K


_SG = _soft(_UG) * W


def rampe(e):
    """e (Spiel, gammakodiert 0..255) -> V, ohne Ueberlauf."""
    return W * _soft((e - LO) / (HI - LO))


def rampe_inv(v):
    u = np.interp(np.clip(v, 0.5, W - 0.5), _SG, _UG)
    return LO + u * (HI - LO)


def e_zu_licht(e):
    return np.power(np.maximum(e, 0) / 255.0, GAMMA)


def licht_zu_e(l):
    return np.power(np.maximum(l, 0), 1 / GAMMA) * 255.0


def _glow(l):
    return (KA * cv2.GaussianBlur(l, (0, 0), SIG_GLOW[0]) +
            KB * cv2.GaussianBlur(l, (0, 0), SIG_GLOW[1]))


def _ueberlauf(e):
    o = UE_WEICH * np.logaddexp(0, (e - HI) / UE_WEICH) * UEBERLAUF
    return o.sum(-1, keepdims=True) - o     # jeder Kanal bekommt die Ueberlaeufe der ANDEREN


def _chroma(v):
    y = v @ np.array([.299, .587, .114])
    cb = cv2.GaussianBlur(v[..., 2] - y, (0, 0), sigmaX=CHROMA[0], sigmaY=CHROMA[1])
    cr = cv2.GaussianBlur(v[..., 0] - y, (0, 0), sigmaX=CHROMA[0], sigmaY=CHROMA[1])
    r, b = y + cr, y + cb
    return np.stack([r, (y - .299*r - .114*b) / .587, b], -1)


def geometrie(rgb, bild, homographie=None):
    """Spielbild (320x240 oder 640x480) in die Bildlage des Videos."""
    h, w = rgb.shape[:2]
    hg = H_GEO[bild] if homographie is None else homographie
    if (w, h) == (320, 240):
        m = hg @ _ST
    elif (w, h) == (640, 480):
        m = hg
    else:
        raise ValueError('Eingang muss 320x240 oder 640x480 sein, ist %dx%d' % (w, h))
    return cv2.warpPerspective(rgb, m, (640, 480), flags=cv2.INTER_LINEAR, borderMode=cv2.BORDER_REPLICATE)


def video_filter(rgb, bild=1, entzerren=True, rauschen=True, saat=SAAT, ex=None, homographie=None):
    """Sauberes Spielbild -> so, wie das Video es zeigte.

    rgb        uint8, 320x240 oder 640x480, RGB
    bild       1 oder 2, waehlt Kanalbalance, Zeilenmuster und Geometrie
    entzerren  True: Spielbild in die Bildlage des Videos legen. False: 640x480-Eingang bleibt, wo er ist
               (fuer Renderings, die schon in Video-Koordinaten stehen); 320x240 wird dann nur verdoppelt.
    rauschen   False laesst Stufe 7 weg, fuer Messungen
    ex         ersetzt die Kanalbalance (3 Werte)
    homographie  ersetzt H_GEO[bild] (3x3, Spiel 640x480 -> Video), z.B. fuer die Dias der Pruefung
    -> uint8 640x480 RGB
    """
    if bild not in (1, 2):
        raise ValueError('bild muss 1 oder 2 sein')
    rgb = np.asarray(rgb)
    if rgb.ndim != 3 or rgb.shape[2] != 3:
        raise ValueError('rgb muss HxBx3 sein')
    g = rgb.astype(np.float32) / 255.0
    if entzerren:
        g = geometrie(g, bild, homographie)
    elif g.shape[:2] == (240, 320):
        g = cv2.resize(g, (640, 480), interpolation=cv2.INTER_NEAREST)
    elif g.shape[:2] != (480, 640):
        raise ValueError('Eingang muss 320x240 oder 640x480 sein')
    l = np.power(np.maximum(g, 1e-5), GAMMA)
    x = cv2.GaussianBlur(l, (0, 0), SIG_OPTIK) + _glow(l)
    balance = EX[bild] if ex is None else np.asarray(ex, float)
    e = licht_zu_e(x) * balance
    v = np.minimum(rampe(e) + _ueberlauf(e), W)
    v = np.clip(_chroma(v), 0, W)
    if rauschen:
        per, amp = ZEILEN[bild]
        y = np.arange(480, dtype=np.float32)
        zeile = np.sin(2*np.pi*y/per)[:, None, None] * amp * np.power(v/W, 2)
        rng = np.random.default_rng(saat)
        korn = cv2.GaussianBlur(rng.normal(0, 1, (480, 640)).astype(np.float32), (0, 0), 1.2)
        korn = korn / korn.std() * KORN[bild]
        v = v + zeile + korn[..., None]
    return np.clip(v + 0.5, 0, 255).astype(np.uint8)


def video_umkehr(video, bild=1):
    """Videobild (RGB uint8 640x480) -> geschaetztes Spielbild e, float 0..255, in Video-Bildlage.

    Fehler je Pixel (rund 620000 Stichproben je Kanal mit bekanntem Spielwert): Bild 2 +-10, Bild 1 +-13 fuer e < 70.
    Geclippte Kanaele sind Untergrenzen. Vollweisse Flaechen tragen KEINE Farbinformation.
    """
    v = cv2.GaussianBlur(np.asarray(video, np.float32), (0, 0), 0.7)
    l = e_zu_licht(rampe_inv(v))
    x = l.copy()
    for _ in range(4):
        x = np.maximum(l - _glow(x), 0)
    x = np.maximum(x, 0.25 * l)                 # nur fuer die Anzeige: kein Punktrauschen an der Null
    e = licht_zu_e(x)
    # Volle Clips: keine Kanalteilung, sonst wird Weiss farbig.
    t = np.clip((v.min(2) - 150) / 65.0, 0, 1)
    t = (t*t*(3 - 2*t))[..., None]
    ex = EX[bild] + (1 - EX[bild]) * t
    e = e / ex
    # Rot nahe am Anschlag: Gruen und Blau tragen Ueberlauf, der Rest darf Rot nicht ueberholen.
    r = np.clip((v[..., 0] - 170) / 42.0, 0, 1)
    r = r*r*(3 - 2*r)
    # Dasselbe im Saum von Vollclips: dort steckt Ueberlauf aus der Nachbarschaft in Gruen und Blau.
    saum = cv2.dilate((v.min(2) >= 195).astype(np.uint8), cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (25, 25)))
    r = np.maximum(r, cv2.GaussianBlur(saum.astype(np.float32), (0, 0), 3))
    for c in (1, 2):
        e[..., c] = e[..., c]*(1 - r) + np.minimum(e[..., c], e[..., 0])*r
    return e


# ---------------------------------------------------------------- Pruefung ----
_MASKE2 = [(425, 480, 0, 262), (150, 400, 120, 420), (215, 480, 405, 560), (310, 400, 140, 240), (390, 480, 250, 460)]


def _lese(p):
    b = cv2.imread(p)
    if b is None:
        raise SystemExit('fehlt: ' + p)
    return b[:, :, ::-1]


def _mass(a, b, m):
    d = np.abs(a.astype(np.float32) - b.astype(np.float32))[m]
    return float(d.mean()), float(np.median(d))


def _ssim(a, b, m):
    from skimage.metrics import structural_similarity as ssim
    ga = cv2.cvtColor(a, cv2.COLOR_RGB2GRAY)
    gb = cv2.cvtColor(b, cv2.COLOR_RGB2GRAY)
    _, karte = ssim(ga, gb, full=True, data_range=255)
    return float(karte[m].mean())


def pruefung():
    z = []
    b2 = _lese(os.path.join(FORM, 'bild2.png'))
    p2 = _lese(os.path.join(HILFE, 'farbe_port_ROOM50911.png'))
    m2 = np.ones((480, 640), bool)
    for y0, y1, x0, x1 in _MASKE2:
        m2[y0:y1, x0:x1] = False
    roh = (geometrie(p2.astype(np.float32) / 255, 2) * 255).astype(np.uint8)
    fil = video_filter(p2, bild=2, rauschen=False)
    fr = video_filter(p2, bild=2)
    a, b, c, d = _mass(roh, b2, m2), _mass(fil, b2, m2), _ssim(roh, b2, m2), _ssim(fil, b2, m2)
    print('Bild 2, Port ROOM5090 Cut 11 ohne Figur, Frau, Feuer, Logo: %d px' % m2.sum())
    print('  roh    MAE %.1f  MedAE %.1f  SSIM %.3f' % (a[0], a[1], c))
    print('  Filter MAE %.1f  MedAE %.1f  SSIM %.3f' % (b[0], b[1], d))
    print('  je Kanal Filter MAE', [round(_mass(fil[..., k], b2[..., k], m2)[0], 1) for k in range(3)])
    print('  Mittelfarbe Video', np.round(b2[m2].mean(0), 1), 'Filter', np.round(fil[m2].mean(0), 1), 'roh', np.round(roh[m2].mean(0), 1))
    z.append(('Bild 2 Video', b2))
    z.append(('Port Cut 11 roh', roh))
    z.append(('Port Cut 11 Filter', fr))
    diff = np.clip(128 + (fil.astype(np.int16) - b2.astype(np.int16)), 0, 255).astype(np.uint8)
    z.append(('Filter - Video, grau = 0', diff))

    b1 = _lese(os.path.join(FORM, 'bild1.png'))
    p1 = _lese(os.path.join(HILFE, 'farbe_port_ROOM50803.png'))
    z.append(('Bild 1 Video', b1))
    z.append(('Port 5080 Cut 3 roh, andere Kamera', (geometrie(p1.astype(np.float32)/255, 1)*255).astype(np.uint8)))
    z.append(('Port 5080 Cut 3 Filter', video_filter(p1, bild=1)))
    # Hin und zurueck: Umkehrung, dann wieder Filter. Muss das Video treffen, ohne Figur auszunehmen.
    rund = video_filter(np.clip(video_umkehr(b1, 1) + .5, 0, 255).astype(np.uint8), bild=1, entzerren=False, rauschen=False)
    ml = np.ones((480, 640), bool)
    ml[425:480, 0:262] = False
    print('Hin und zurueck Bild 1: MAE %.1f  MedAE %.1f' % _mass(rund, b1, ml))
    rund2 = video_filter(np.clip(video_umkehr(b2, 2) + .5, 0, 255).astype(np.uint8), bild=2, entzerren=False, rauschen=False)
    print('Hin und zurueck Bild 2: MAE %.1f  MedAE %.1f' % _mass(rund2, b2, ml))
    z.append(('Bild 1: Umkehr, dann Filter', rund))

    v = _lese(os.path.join(HILFE, 'farbe_dia144.png'))
    p = _lese(os.path.join(HILFE, 'farbe_port_ROOM50901.png'))
    hm = np.loadtxt(os.path.join(HILFE, 'farbe_dia144_H.txt'))
    f = video_filter(p, bild=1, rauschen=False, homographie=hm)
    rr = (geometrie(p.astype(np.float32)/255, 1, hm)*255).astype(np.uint8)
    m = ml
    a, b, c, e = _mass(rr, v, m), _mass(f, v, m), _ssim(rr, v, m), _ssim(f, v, m)
    print('Dia 144 (ROOM5090 Cut 1), Kurve und Balance von Bild 1 (Hold-out-Zahlen: farbe.md Abschnitt 3):')
    print('  roh    MAE %.1f  MedAE %.1f  SSIM %.3f' % (a[0], a[1], c))
    print('  Filter MAE %.1f  MedAE %.1f  SSIM %.3f' % (b[0], b[1], e))
    z.append(('Dia 144 Video', v))
    z.append(('Port 5090 Cut 1 roh', rr))
    z.append(('Port 5090 Cut 1 Filter, Kurve Bild 1', video_filter(p, bild=1, homographie=hm)))
    z.append(('Filter - Video, grau = 0', np.clip(128 + (f.astype(np.int16) - v.astype(np.int16)), 0, 255).astype(np.uint8)))
    kacheln = []
    for t, b in z:
        k = cv2.resize(b, (480, 360), interpolation=cv2.INTER_AREA).copy()
        cv2.putText(k, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.putText(k, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0), 1, cv2.LINE_AA)
        kacheln.append(k)
    reihen = [np.hstack(kacheln[i:i+4]) for i in range(0, len(kacheln), 4)]
    ziel = os.path.join(FORM, 'farbe_filter_pruefung.png')
    cv2.imwrite(ziel, np.vstack(reihen)[:, :, ::-1])
    print('geschrieben:', ziel)


def umkehr():
    for n in (1, 2):
        e = video_umkehr(_lese(os.path.join(FORM, 'bild%d.png' % n)), n)
        cv2.imwrite(os.path.join(FORM, 'farbe_bild%d_echt.png' % n), np.clip(e + .5, 0, 255)[:, :, ::-1].astype(np.uint8))
        cv2.imwrite(os.path.join(HILFE, 'farbe_bild%d_echt_hell.png' % n), np.clip(e*2.2, 0, 255)[:, :, ::-1].astype(np.uint8))
        print('geschrieben: farbe_bild%d_echt.png' % n)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('befehl', choices=('pruefung', 'umkehr'))
    a = ap.parse_args()
    {'pruefung': pruefung, 'umkehr': umkehr}[a.befehl]()


if __name__ == '__main__':
    main()
