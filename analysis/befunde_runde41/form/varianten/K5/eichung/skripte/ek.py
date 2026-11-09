"""Gemeinsame Hilfen der Farbeichung (nur Messung). Pfade, Lader fuer Leon (PL00) und die Frau (Typ 0x4D), Szenen-Kameras, Disc-Licht."""
import os, sys, json
import numpy as np, cv2
WT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
FORM = WT + '/analysis/befunde_runde41/form'
K5 = FORM + '/varianten/K5'
EICH = K5 + '/eichung'
DATEN = EICH + '/daten'
TAFELN = EICH + '/tafeln'
SCR = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/eichung'
sys.path.insert(0, WT + '/re15_port/tools')
sys.path.insert(0, K5 + '/farbe_lesung/V4F_skripte')
import emd_ansichtsblatt as EA   # noqa
import form_buehne as FB         # noqa
import form_pruefung as P        # noqa
import form_video as FV          # noqa
import form_kamera as FK         # noqa
PSX = WT + '/re15_port/shared_assets/PSX'


def video(n):
    """Mittel ueber alle 24 Videobilder des Dias (e1_bilder.py), float32 RGB 640x480. Die float-Fassung liegt im Scratchpad
    (3,7 MB je Bild, nicht im Baum); fehlt sie, gilt das gerundete daten/bildN_mittel.png (Abweichung <= 0,5)."""
    import os
    p = SCR + '/bild%d_mittel.npy' % n
    if os.path.exists(p):
        return np.load(p)
    return lade_rgb(DATEN + '/bild%d_mittel.png' % n).astype(np.float32)


def lade_rgb(p):
    return cv2.imread(p)[:, :, ::-1].copy()


class _Mo(EA.Modell):
    def __init__(self):
        pass


def leon():
    d = PSX + '/PLD/'
    mo = _Mo()
    mo.meshes = EA.md1_parse(open(d + 'PL00.MD1', 'rb').read())
    mo.skel = EA.emr_parse(open(d + 'PL00.EMR', 'rb').read())
    mo.clips, mo.frames = EA.edd_parse(open(d + 'PL00.EDD', 'rb').read())
    tim = open(d + 'PL00.TIM', 'rb').read()
    return mo, FB.Textur(tim), tim


def frau():
    mo = EA.Modell(0x4D)
    d = EA.emd_dir(mo.blob)
    tim = mo.blob[d[8]:]
    return mo, FB.Textur(tim), tim


def kf(mo, clip, bild):
    f0, n = mo.clips[clip]
    return mo.frames[f0 + min(bild, n - 1)] & 0xFFF


def kamera(n):
    return P.kamera(n)


def licht(n):
    return P.licht_von_disc(n)          # Bild 2: 5090 Schnitt 11; Bild 1: 5080 Schnitt 2 (wie die Szenenkette der Form)


def boden(n, xy):
    return FK.strahl_auf_boden(FK.lade_kamera(n), np.array(xy, float))


def lab(rgb):
    a = np.asarray(rgb, np.float32).reshape(-1, 3) / 255.0
    return cv2.cvtColor(np.clip(a, 0, 1)[None], cv2.COLOR_RGB2LAB)[0].reshape(np.shape(rgb))


def rgb_von_lab(L):
    a = np.asarray(L, np.float32).reshape(1, -1, 3)
    return np.clip(cv2.cvtColor(a, cv2.COLOR_LAB2RGB)[0] * 255, 0, 255).reshape(np.shape(L))


def de76(a, b):
    return np.linalg.norm(lab(a) - lab(b), axis=-1)


def weiss_textur(tex):
    t = FB.Textur.__new__(FB.Textur)
    t.px, t.clut_y, t.h, t.w = tex.px, tex.clut_y, tex.h, tex.w
    t.lut = tex.lut.copy()
    t.lut[..., :3] = np.where(t.lut[..., 3:4] > 0, 255, 0)
    return t


def figur(mo, tex, k, lage, gier, n, licht_an=True, grund=None):
    """Render in Video-Bildlage (Kamera n), Rueckgabe (sauber, Maske, Schattierung 0..255 je Kanal, Albedo-Render)."""
    kam = kamera(n)
    L = licht(n) if licht_an else None
    g = np.zeros((480, 640, 3), np.uint8) if grund is None else grund.copy()
    tr = FB.dreiecke(mo, k, lage, gier, kam, L)
    m = FB.rastern(tr, tex, g, kam.abbildung)
    sh = np.zeros((480, 640, 3), np.uint8)
    FB.rastern(tr, weiss_textur(tex), sh, kam.abbildung)
    alb = np.zeros((480, 640, 3), np.uint8)
    FB.rastern(FB.dreiecke(mo, k, lage, gier, kam, None), tex, alb, kam.abbildung)
    return g, m, sh, alb
