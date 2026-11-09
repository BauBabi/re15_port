#!/usr/bin/env python3
"""K5f Tafeln (roh, kein Gamma; gleiche Kette/gleiches Licht je Zeile fuer alle Spalten):
    python kf_k5f_tafeln.py <K5f-Ordner> <aus-Ordner>
01_bild1 / 02_bild2: Vorlage | K5e heute (Spiel-Asset K5e_f1) | K5f; Zeile 1 Disc-Licht x k ohne Filter, Zeile 2 mit Videofilter (Saat 15).
03_details_nah: Ausschnitte der Kette (Schwanzspitze, Gelenke, Buckel, Schultern, Krallen) + Kopf/Hals nah unter weissem Licht: K5e | K5f.
04_spiel: echtes Spielfenster (gdigrab, kf_k5f_fenster.sh): Original-Standbild | K5e heute | K5f (5080 Bild 1, 5090 Bild 2, 5090 Griff).
05_kopf: Kopf und Hals-Unterseite nah, weisses Licht, Clip 2 und Clip 1, von vorn/seitlich/unten: K5c | K5e | K5f.
06_rundum: weisses Licht (Tafel-07-Licht), 4 Winkel, Clip 2: K5c | K5e | K5f.
07_familie: weisses Licht, gleiche Behandlung: RE2-G4 Original (Clip 0) | Hangar-Birkin EM30 (Clip 0) | K5e (Clip 2) | K5f (Clip 2).
08_gegner_spiellicht: Framedumps des echten Renderers im eigenen Raumlicht (Messblatt 1.1), Figur per Schluesselmaske ausgeschnitten und auf
  gleiche Hoehe (300 px, nearest) gebracht: Zombie 1140 | Gorilla 11C0 | EM30 50A0 | Tyrant 50A0 | K5e 5080 | K5f 5080.
"""
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
sys.path.insert(0, HIER)
import rl  # noqa: E402
import riegel_k5d as R  # noqa: E402
import kf_k5d_tafeln as T  # noqa: E402
import kf_k5e_tafeln as T5  # noqa: E402

v4f = R.v4f
reihe, stapeln, kachel, speichern = T.reihe, T.stapeln, T.kachel, T.speichern
K5E = os.path.join(K5, 'ruhe', 'K5e_f1')
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f/spiel'
FENSTER = os.path.join(K5, 'k5f', 'spiel')


def nah(mo, clip, az, el, meshes=(21, 22, 23)):
    img, K = rl.weiss_id(mo, clip, az, el, 760)
    m = np.isin(K['mesh'], meshes)
    if not m.any():
        return cv2.resize(img, (300, 300), interpolation=cv2.INTER_AREA)
    ys, xs = np.nonzero(m)
    cy, cx = int((ys.min() + ys.max()) / 2), int((xs.min() + xs.max()) / 2)
    r = int(max(ys.max() - ys.min(), xs.max() - xs.min()) / 2 + 12)
    a = img[max(cy - r, 0):cy + r, max(cx - r, 0):cx + r]
    return cv2.resize(a, (300, 300), interpolation=cv2.INTER_NEAREST)


def figur(normal, key, bild, hoehe=300):
    n = cv2.cvtColor(cv2.imread(os.path.join(SP, normal, bild)), cv2.COLOR_BGR2RGB)
    k = cv2.cvtColor(cv2.imread(os.path.join(SP, key, bild)), cv2.COLOR_BGR2RGB)
    m = (n != k).any(2)
    ys, xs = np.nonzero(m)
    y0, y1, x0, x1 = max(ys.min() - 6, 0), ys.max() + 7, max(xs.min() - 6, 0), xs.max() + 7
    a = n[y0:y1, x0:x1].copy()
    f = hoehe / float(a.shape[0])
    a = cv2.resize(a, (max(int(a.shape[1] * f), 1), hoehe), interpolation=cv2.INTER_NEAREST)
    return a, int(m.sum())


def main():
    neu, aus = sys.argv[1], sys.argv[2]
    os.makedirs(aus, exist_ok=True)
    st = {'K5e heute': R.Stand(K5E), 'K5f': R.Stand(neu)}
    vor = {n: v4f.original(n) for n in (1, 2)}
    for n, datei in ((1, '01_bild1.png'), (2, '02_bild2.png')):
        z1 = reihe([vor[n]] + [st[k].d[n]['sauber'] for k in st], ['Vorlage Bild %d (Video-Standbild)' % n] + ['%s: Disc-Licht x k, ohne Filter' % k for k in st])
        z2 = reihe([vor[n]] + [st[k].d[n]['video'][15] for k in st], ['Vorlage Bild %d (Video-Standbild)' % n] + ['%s: Disc-Licht x k + Videofilter Saat 15' % k for k in st])
        speichern(os.path.join(aus, datei), stapeln([z1, z2], 'K5f nicht blass (Nutzer 2026-11-09), Bild %d: Vorlage | K5e heute (Spiel-Asset K5e_f1) | K5f. '
                                                       'Gleiche Szenenkette je Zeile, roh, kein Gamma.' % n))
    mods = {'K5c': rl.fl.lade_var(rl.BASIS), 'K5e': rl.fl.lade_var(K5E), 'K5f': rl.fl.lade_var(neu)}
    zeilen = []
    fall = [('Schwanzspitze (Bild 1, Mesh 6)', 1, (205, 380, 345, 480)), ('Gelenke Beine/Arme (Bild 1)', 1, (190, 250, 440, 400)),
            ('Buckel vorn (Bild 2)', 2, (240, 140, 380, 230)), ('Schultern vorn (Bild 2)', 2, (200, 190, 440, 290)),
            ('Krallen vorn (Bild 2)', 2, (150, 290, 500, 480))]
    for titel, n, box in fall:
        f = 3 if (box[2] - box[0]) <= 150 else 2
        for art in ('video', 'sauber'):
            bilder = [kachel(vor[n], box, f)] + [kachel(st[k].d[n]['video'][15] if art == 'video' else st[k].d[n]['sauber'], box, f) for k in st]
            zeilen.append(reihe(bilder, ['Vorlage: ' + titel, 'K5e heute (%s)' % ('Videofilter' if art == 'video' else 'ohne Filter'),
                                         'K5f (%s)' % ('Videofilter' if art == 'video' else 'ohne Filter')]))
    kz = []
    for clip, az, el in ((2, 60, 10), (2, 0, -60), (1, 60, 10), (1, 90, -45)):
        for k in ('K5e', 'K5f'):
            kz.append((nah(mods[k], clip, az, el), '%s Kopf/Hals Clip %d %d/%d' % (k, clip, az, el)))
    zeilen.append(reihe([a for a, _t in kz[:4]], [t for _a, t in kz[:4]]))
    zeilen.append(reihe([a for a, _t in kz[4:]], [t for _a, t in kz[4:]]))
    speichern(os.path.join(aus, '03_details_nah.png'), stapeln(zeilen, 'K5f Details nah: Vorlage | K5e heute | K5f (Ausschnitte x2/x3 nearest, Kette wie 01/02, roh). '
                                                                       'Kopf/Hals: weisses Licht wie Tafel 07 (0,55 + 0,45 N.L), roh.'))
    # 04 Spiel
    lade = lambda p: cv2.cvtColor(cv2.imread(os.path.join(FENSTER, p)), cv2.COLOR_BGR2RGB)
    leer = np.full((480, 640, 3), 24, np.uint8)
    z = []
    for titel, n, datei in (('5080 Schnitt 1, Bild-1-Lage (Clip 0 Bild 11)', 1, 'pose1_r5080.png'),
                            ('5090 Schnitt 11, Bild-2-Pose (Clip 12 Bild 0)', 2, 'pose2_r5090_s11.png'),
                            ('5090 Schnitt 11, Griff (Clip 27 Bild 20, Hals-Unterseite)', 0, 'griff_r5090_s11.png')):
        z.append(reihe([v4f.original(n) if n else leer, lade('heute_' + datei), lade('k5f_' + datei)],
                       ['Original-Standbild Bild %d' % n if n else '(kein Original-Standbild zur Griff-Pose)', 'K5e heute (Spiel-Asset): ' + titel, 'K5f: ' + titel]))
    speichern(os.path.join(aus, '04_spiel.png'), stapeln(z, 'K5f im echten Spielfenster (gdigrab, kein AUTOSHOT/SOFTWARE_RENDER, Variante ueber '
                                                            'RE15_RE2_ASSET_ROOT): Original | K5e heute | K5f, roh'))
    # 05 Kopf + Hals-Unterseite
    zeilen = []
    for clip in (2, 1):
        for az, el in ((60, 10), (0, -60), (90, -45)):
            ks = [(nah(mods[k], clip, az, el), '%s Clip %d (%s) %d/%d' % (k, clip, 'Ruhe' if clip == 2 else 'Maul offen', az, el)) for k in ('K5c', 'K5e', 'K5f')]
            zeilen.append(reihe([a for a, _t in ks], [t for _a, t in ks]))
    speichern(os.path.join(aus, '05_kopf.png'), stapeln(zeilen, 'Kopf, Lippe, Unterkiefer und Hals-Unterseite nah, weisses Licht wie Tafel 07: K5c | K5e heute | '
                                                                'K5f, Clip 2 Ruhe und Clip 1 Maul offen, roh'))
    # 06 rundum
    zeilen = []
    for wn, az, el in rl.WINKEL4:
        zeilen.append(reihe([T5.weiss(mods[k], 2, az, el) for k in ('K5c', 'K5e', 'K5f')], ['%s, %s (Clip 2)' % (k, wn) for k in ('K5c', 'K5e heute', 'K5f')]))
    speichern(os.path.join(aus, '06_rundum.png'), stapeln(zeilen, 'Rundum unter weissem Licht (Tafel-07-Licht, 4 Winkel, Ruhe-Clip): K5c | K5e heute | K5f, roh'))
    # 07 Familie
    g4 = rl.PL.re2(0x34)
    g4.T = rl._Tex(g4)
    em = rl.fl.lade_em30()
    fam = (('RE2-G4 Original (Typ 0x34, Clip 0)', g4, 0), ('Hangar-Birkin EM30 RE1.5 (Clip 0)', em, 0), ('K5e heute (Clip 2)', mods['K5e'], 2),
           ('K5f (Clip 2)', mods['K5f'], 2))
    zeilen = []
    for wn, az, el in rl.WINKEL4:
        zeilen.append(reihe([T5.weiss(mo, c, az, el) for _t, mo, c in fam], ['%s, %s' % (t, wn) for t, _mo, _c in fam]))
    speichern(os.path.join(aus, '07_familie.png'), stapeln(zeilen, 'Familie unter weissem Licht, gleiche Behandlung (Licht 0,55 + 0,45 N.L, gleiche Bildgroesse, '
                                                                   'keine Aufhellung): RE2-G4 | Hangar-Birkin RE1.5 | K5e heute | K5f, roh'))
    # 08 Gegner im Spiellicht
    reihen_ = [('Zombie EM10 1140 (Raumlicht)', 'zombie_n', 'zombie_k', 'dump_000600.ppm'), ('Gorilla EM27 11C0', 'gorilla_normal', 'gorilla_key', None),
               ('EM30 Hangar 50A0 Schnitt 4', 'hc4_normal', 'hc4_key', 'dump_000600.ppm'), ('Tyrant EM2B 50A0 Schnitt 4', 'hc4_normal', 'hc4_key2b', 'dump_000600.ppm'),
               ('K5e heute 5080 Bild-1-Lage', 'f80_k5e_f1_normal', 'f80_k5e_f1_key', 'dump_000440.ppm'), ('K5f 5080 Bild-1-Lage', 'f80_k5f_normal', 'f80_k5f_key', 'dump_000440.ppm')]
    bilder, titel = [], []
    for t, n, k, b in reihen_:
        if b is None:
            best = None
            for f in sorted(os.listdir(os.path.join(SP, n))):
                if f.endswith('.ppm') and os.path.exists(os.path.join(SP, k, f)):
                    a_ = cv2.imread(os.path.join(SP, n, f))
                    c_ = (a_ != cv2.imread(os.path.join(SP, k, f))).any(2).sum()
                    if best is None or c_ > best[0]:
                        best = (c_, f)
            b = best[1]
        a, px = figur(n, k, b)
        bilder.append(a)
        titel.append('%s (%s, %d px)' % (t, b[:-4], px))
    speichern(os.path.join(aus, '08_gegner_spiellicht.png'), stapeln([reihe(bilder[:3], titel[:3]), reihe(bilder[3:], titel[3:])],
                                                                     'Gegner im eigenen Raumlicht, Framedump des echten Renderers, Ausschnitt um die Schluesselmaske (mit Umfeld) '
                                                                     'auf gleiche Hoehe 300 px (nearest), roh'))
    print('Tafeln fertig:', aus)


if __name__ == '__main__':
    main()
