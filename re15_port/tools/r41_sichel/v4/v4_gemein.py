"""SF V4 (Bauer): gemeinsame Helfer fuer Bau, Riegel und Leisten nach SF_MESSBLATT_V4.md.

Quellen (alle Zahlen aus dem Messblatt V4, Abschnitt in Klammern; keine RE-Werte, die Sichel-Form gibt es auf der Disc nicht):
  Vorlage 1 = Version_80%/Beschreibung1_Vorlage.png = Videobild n1485 (h135), entblendet I' = 17 + (I - 17) / k,
              k = 0,766 / 0,764 / 0,768 (Abschnitt 1, Zeile "Abblende Vorlage 1").
  Vorlage 2 = Version_80%/Beschreibung2_Vorlage.png = n1435 (h85), nicht abgeblendet.
  Eichung Video <-> Port-Schirm: V = a + b x P, a = 9,42 / 11,96 / 10,94, b = 0,9979 / 0,9901 / 0,9964, Port vorher Gauss
              sigma 1,5 (Abschnitt 5 "Vergleichsregel").
  Kopfbox Original vorn x 368-416 y 139-191, seitlich x 412-467 y 190-263 (Abschnitt 2 / p_riegelfenster.py).
Port-Render: b2.zeichne (Spielkamera 50A0 Cut 12, RDT-Licht), Hintergrund = BSS ROOM50A0 Schnitt 12 (Engine-Dekoder
probe_bg_dump, dieselbe Kette wie im Spiel) per Homographie kamera_50A0_12.json ins Videobild.
"""
import os, sys, json
import numpy as np, cv2
from PIL import Image
from skimage.color import rgb2lab, deltaE_ciede2000

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v3'))
sys.path.insert(0, os.path.dirname(os.path.dirname(HIER)))
import b2                                   # noqa: E402
import birkin_buehne as BB                  # noqa: E402

BAUM = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(HIER))))
V80 = r'C:/workspace/git/reAi_v2/Version_80%/'
SFV4 = os.environ.get('SFV4', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/'
                      r'c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4')
SFV2 = os.path.join(os.path.dirname(SFV4), 'sfv2')
REF2 = os.path.join(BAUM, 're15_port', 'shared_assets', 'PSX', 'EMD', 'EM30_HANGAR.EMD')
EMD_V4 = os.path.join(SFV4, 'emd', 'EM30_V4.EMD')
EMD_V3 = os.path.join(SFV4, 'emd', 'EM30_V3.EMD')         # V3 r2, neu gebaut aus HEAD (sha256 1952713c... = Lauf sfv3/r/V3r2)
POSEN = os.path.join(HIER, 'posen_v4.json')

K1 = np.array([0.766, 0.764, 0.768])                        # Messblatt V4 Abschnitt 1 "Abblende Vorlage 1"
SW = 17.0                                                   # Schwarzwert (Abschnitt 1)
EA_ = np.array([9.42, 11.96, 10.94])                        # Eichung a (Abschnitt 5)
EB_ = np.array([0.9979, 0.9901, 0.9964])                    # Eichung b (Abschnitt 5)
SIGMA = 1.5                                                 # Vergleichsregel (Abschnitt 5)
KB_ORIG = {1: (368, 139, 416, 191), 2: (412, 190, 467, 263)}   # Kopfbox Original (Abschnitt 2 / p_riegelfenster.py)
H_ORIG = {1: 302.0, 2: 253.0}                               # H1 / H2 (Abschnitt 1)
SCHWANZ = tuple(range(16, 23))


def vorlage(b):
    a = np.asarray(Image.open(V80 + 'Beschreibung%d_Vorlage.png' % b).convert('RGB')).astype(float)
    return np.clip(SW + (a - SW) / K1, 0, 255) if b == 1 else a


def videoraum(port_rgb):
    """Vergleichsregel: Port 640x480 -> Gauss sigma 1,5 -> V' = a + b x P."""
    p = cv2.GaussianBlur(np.asarray(port_rgb, np.float32), (0, 0), SIGMA)
    return np.clip(EA_ + EB_ * p, 0, 255)


def port_ziel(v_rgb):
    """Videoraum -> Port-Schirm (Abschnitt 5 Umrechnung Schritt 1: P = (V - a) / b)."""
    return (np.asarray(v_rgb, float) - EA_) / EB_


def lab(rgb):
    a = np.asarray(rgb, float)
    return rgb2lab(np.clip(a.reshape(-1, 1, 3) / 255.0, 0, 1)).reshape(a.shape)


def de00(l1, l2):
    return float(deltaE_ciede2000(np.asarray(l1, float).reshape(1, 1, 3), np.asarray(l2, float).reshape(1, 1, 3))[0, 0])


def lum(a):
    return np.asarray(a, float) @ np.array([0.299, 0.587, 0.114])


_bg = None
def hintergrund():
    """BSS ROOM50A0 Schnitt 12 (probe_bg_dump) im Videobild 640x480 (Homographie b2.raum)."""
    global _bg
    if _bg is None:
        p = os.path.join(SFV4, 'bg', 'ROOM50A12.ppm')
        bss = np.asarray(Image.open(p).convert('RGB'))
        _r, _k, M, _l = b2.raum()
        _bg = cv2.warpPerspective(bss, np.asarray(M, float), (640, 480), flags=cv2.INTER_LINEAR)
    return _bg.copy()


def posen():
    return json.load(open(POSEN))


UNTEN = 160      # Zusatzzeilen unter dem Bild: die Sohle liegt in Pose 135 unter Zeile 479 (H_port braucht sie)


def zeichne(mo, pose, licht=True, R_T=None, weiss=False, knochen=None, voll=False):
    """b2.zeichne mit BSS-Hintergrund auf einer um UNTEN Zeilen verlaengerten Leinwand (Sohle ausserhalb des Bildes messbar).
    Rueckgabe (Bild 640x480, ids 640x480) bzw. mit voll=True (Bild, ids, ids_voll).
    weiss=True: alle Texel des Modells = 248 (weisser Pruefkoerper, Messblatt V4 Abschnitt 5 Schritt 2)."""
    bg = np.vstack([hintergrund(), np.full((UNTEN, 640, 3), 60, np.uint8)])
    lut0 = mo.lut
    if weiss:
        w = lut0.copy(); w[..., :3] = np.where(w[..., 3:4] > 0, 248, 0)
        mo.lut = w
    try:
        img, ids = b2.zeichne(mo, pose, bg, R_T=R_T, knochen=knochen, licht=licht)
    finally:
        mo.lut = lut0
    return (img[:480], ids[:480], ids) if voll else (img[:480], ids[:480])


def h_port(ids_voll):
    """H_port = Sohle - Oberkante der Koerper-Ids ohne 16-22 (b2.ok_sohle), auf der verlaengerten Leinwand."""
    ok, so = b2.ok_sohle(ids_voll, ausser=SCHWANZ)
    return ok, so, so - ok


def kopfbox(ids):
    ys, xs = np.nonzero(ids == 9)
    return (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())) if len(xs) else None


def fenster(box, anteile):
    """Anteils-Fenster (u0, v0, u1, v1) einer Box (x0, y0, x1, y1) -> Pixelbox inkl. (wie p_riegelfenster.py)."""
    x0, y0, x1, y1 = box; w, h = x1 - x0, y1 - y0
    u0, v0, u1, v1 = anteile
    return (int(round(x0 + u0 * w)), int(round(y0 + v0 * h)), int(round(x0 + u1 * w)), int(round(y0 + v1 * h)))


def mittel(img, bx, maske=None):
    x0, y0, x1, y1 = bx
    sub = img[y0:y1 + 1, x0:x1 + 1].reshape(-1, 3)
    if maske is not None:
        sub = sub[maske[y0:y1 + 1, x0:x1 + 1].ravel()]
    return sub.mean(0) if len(sub) else np.full(3, np.nan)


def blob(img_v, such, art, ids=None, kopf=None):
    """p_blob-Verfahren (Pruefer V4): Extrem (3x3-Mittel) im Suchfenster, Schwelle = Mitte aus Extrem und Median 21x21 um das
    Extrem, 4er-Komponente mit dem Extrem. Rueckgabe dict(px, bbox, w_h, sp, rgb, ext, umg, maske)."""
    L = lum(img_v)
    x0, y0, x1, y1 = such
    win = cv2.blur(L[y0:y1 + 1, x0:x1 + 1].astype(np.float32), (3, 3))
    if kopf is not None:
        km = kopf[y0:y1 + 1, x0:x1 + 1]
        win = np.where(km, win, -1e9 if art == 'hell' else 1e9)
    iy, ix = np.unravel_index(np.argmax(win) if art == 'hell' else np.argmin(win), win.shape)
    px, py = ix + x0, iy + y0
    ext = float(win[iy, ix]); umg = float(np.median(L[py - 10:py + 11, px - 10:px + 11]))
    thr = (ext + umg) / 2
    m = ((L > thr) if art == 'hell' else (L < thr)).astype(np.uint8)
    if kopf is not None:
        m &= kopf.astype(np.uint8)
    n, lab_, st, cen = cv2.connectedComponentsWithStats(m, connectivity=4)
    k = lab_[py, px]
    mk = lab_ == k
    ys, xs = np.nonzero(mk)
    return dict(px=int(len(xs)), bbox=(int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())),
                w_h=(int(xs.max() - xs.min() + 1), int(ys.max() - ys.min() + 1)), sp=(float(xs.mean()), float(ys.mean())),
                rgb=img_v[mk].mean(0), ext=ext, umg=umg, schwelle=thr, maske=mk, extrem=(int(px), int(py)))


def r1(x, n=1):
    return [round(float(v), n) for v in np.ravel(x)]
