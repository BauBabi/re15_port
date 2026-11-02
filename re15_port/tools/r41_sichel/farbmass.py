"""Runde 41 Spur SF Nachbesserung 1: Farbmass Birkin-Form gegen Video 4 (Haut, Schwanz, Sichel), offline an der Buehne.

    python re15_port/tools/r41_sichel/farbmass.py [EMD]        # Standard: shared_assets/PSX/EMD/EM30_HANGAR.EMD

Render (tools/birkin_buehne.py: RID-Kamera + RDT-Licht ROOM50A0 Schnitt 12, Szenenlage (-8488,0,-21651), Gier Bezug-1270
wie die Hangar-Szene, Clip 0x0B Bild 20) auf 320x240 halbiert = Massstab der Port-Fensteraufnahme (szene_lauf.sh).
Gegenprobe Lauf n1 (echtes Fenster, h40-h64): Haut L 92,9 / Rotanteil 0,174 / hell (116,92,89); dieselbe Datei hier
L 93,9 / 0,158 / (115,91,87) -> die Buehne misst wie das Fenster.
  Haut     Kasten PSX x150-240 y95-150, Haut = r > g+8 und r > b+4; L = Luma, rot = Anteil r > 1,5 g.
  Schwanz  Bildpunkte, die sich aendern, wenn Knochen 16-22 nicht gezeichnet werden; 2-Mittel auf L -> dunkel/hell.
  Sichel   ebenso fuer Knochen 15.
Videowerte (Messung N1.4-N1.7, Dossier SF_sichel.md) stehen zum Vergleich in der Ausgabe.
"""
import math, os, sys
import numpy as np
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..'))
import birkin_buehne as BB          # noqa: E402

BX, BZ, SX, SZ = -8488, -21651, -12470, -17540           # birkin_formen.c BF_BIRKIN_*/BF_SM_*
G0 = int(round(math.atan2(-(SZ - BZ), SX - BX) * 4096 / (2 * math.pi))) & 0xfff
VIDEO = dict(haut=dict(L=66.0, rot=0.328, hell=(78, 62, 61), rotf=(72, 40, 40)),
             schwanz=dict(dunkel=(92, 54, 55), hell=(137, 90, 91), anteil_dunkel=0.63),
             sichel=dict(farbe=(21, 23, 18), L_zu_haut=0.58))


def _render(mo, verbergen=(), bild=20):
    r = BB.Raum('50A0'); s = r.schnitt(12)
    kam = BB.Kamera(s['pos'], s['ziel'], s['fov'] >> 7)
    for b in verbergen:
        m = mo.meshes[b]; m.tf, m.qf = [], []
    ziel = np.full((480, 640, 3), 40, np.uint8)
    tris = BB.dreiecke(mo, BB.kf_von_clip(mo, 0x0B, bild), (BX, 0, BZ), (G0 - 1270) & 0xfff, kam, r.licht(12))
    BB.rastern(tris, mo, ziel, BB.abbildung_laden('50A0', 12))
    return np.asarray(Image.fromarray(ziel).resize((320, 240), Image.BILINEAR), dtype=float)


def _zwei(P):
    L = P @ [0.299, 0.587, 0.114]
    c1, c2 = np.percentile(L, 25), np.percentile(L, 85)
    for _ in range(20):
        lab = np.abs(L - c1) > np.abs(L - c2)
        c1, c2 = L[~lab].mean(), L[lab].mean()
    return np.median(P[~lab], 0).round().astype(int).tolist(), np.median(P[lab], 0).round().astype(int).tolist(), round(float((~lab).mean()), 2)


def mass(blob, bild=20):
    voll = _render(BB.modell(blob), (), bild)
    c = voll[95:150, 150:240].reshape(-1, 3); r, g, b = c[:, 0], c[:, 1], c[:, 2]
    c = c[(r > g + 8) & (r > b + 4)]; r, g = c[:, 0], c[:, 1]
    rot = r > 1.5 * g
    haut = dict(L=round(float((c @ [0.299, 0.587, 0.114]).mean()), 1), rot=round(float(rot.mean()), 3),
                hell=np.median(c[~rot], 0).round().astype(int).tolist(), rotf=np.median(c[rot], 0).round().astype(int).tolist())
    ohne_s = _render(BB.modell(blob), range(16, 23), bild)
    d = np.abs(voll - ohne_s).sum(2) > 30
    dk, hl, ant = _zwei(voll[d])
    schwanz = dict(n=int(d.sum()), dunkel=dk, hell=hl, anteil_dunkel=ant)
    ohne_k = _render(BB.modell(blob), (15,), bild)
    d = np.abs(voll - ohne_k).sum(2) > 30
    P = voll[d]; Lk = float((P @ [0.299, 0.587, 0.114]).mean())
    sichel = dict(n=int(d.sum()), farbe=np.median(P, 0).round().astype(int).tolist(), L_zu_haut=round(Lk / haut['L'], 2))
    return dict(haut=haut, schwanz=schwanz, sichel=sichel)


if __name__ == '__main__':
    emd = sys.argv[1] if len(sys.argv) > 1 else os.path.join(BB.PSX, 'EMD', 'EM30_HANGAR.EMD')
    m = mass(open(emd, 'rb').read())
    for k in ('haut', 'schwanz', 'sichel'):
        print('%-8s Modell %s\n%-8s Video  %s' % (k, m[k], '', VIDEO[k]))
