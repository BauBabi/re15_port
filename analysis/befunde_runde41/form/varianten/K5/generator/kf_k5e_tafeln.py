#!/usr/bin/env python3
"""K5e Tafeln (roh, kein Gamma; gleiche Kette/gleiches Licht je Zeile fuer alle Spalten):
    python kf_k5e_tafeln.py <K5e-Ordner> <aus-Ordner>            -> 01, 02, 03, 05, 06, 07
    python kf_k5e_tafeln.py --spiel <Spiel-Ordner> <aus-Ordner>  -> 04 (echtes Spielfenster, kf_k5e_spiel.sh)
01_bild1 / 02_bild2: Vorlage | K5c heute (Spiel-Asset) | K5e; Zeile 1 Disc-Licht x k ohne Filter, Zeile 2 mit Videofilter (Saat 15).
03_details_nah: Schwanzspitze, Gelenke, Buckel vorn, Schultern, Krallen vorn (Ausschnitte der Kette), Kopf/Lippe/Unterkiefer nah
  (weisses Licht wie Familien-Tafel 07, Clip 2 Ruhe und Clip 1 Maul offen) K5c | K5e.
05_kopf_lippe: Kopf nah, weisses Licht, 6 Blickwinkel, Clip 2 und Clip 1: K5c | K5d | K5e.
06_rundum: weisses Licht (rl.weiss = render.blick der Tafel 07), 4 Winkel, Clip 2: K5c | K5d | K5e.
07_familie: weisses Licht, gleiche Behandlung (gleiche Groesse/Licht, keine Aufhellung): RE2-G4 Original (Clip 0) | Hangar-Birkin EM30 RE1.5
  (Clip 0) | K5c (Clip 2) | K5e (Clip 2), 4 Winkel.
"""
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'ruhe', 'skripte'))
sys.path.insert(0, HIER)
import rl  # noqa: E402
import riegel_k5d as R  # noqa: E402
import kf_k5d_tafeln as T  # noqa: E402

v4f = R.v4f
reihe, stapeln, kachel, speichern = T.reihe, T.stapeln, T.kachel, T.speichern


def spiel(sp, aus):
    lade = lambda p: cv2.cvtColor(cv2.imread(os.path.join(sp, p)), cv2.COLOR_BGR2RGB)
    leer = np.full((480, 640, 3), 24, np.uint8)
    z = []
    for titel, n, datei in (('5080 Schnitt 1, Bild-1-Lage (Clip 0 Bild 11)', 1, 'pose1_nachstellung_r5080.png'),
                            ('5080 Schnitt 1, Ruhe (Clip 2)', 0, 'ruhe_r5080_s1.png'),
                            ('5090 Schnitt 11, Bild-2-Pose (Clip 12 Bild 0)', 2, 'pose2_r5090_s11.png'),
                            ('5090 Schnitt 11, Ruhe (Clip 2)', 0, 'ruhe_r5090_s11.png')):
        z.append(reihe([v4f.original(n) if n else leer, lade('heute_' + datei), lade('k5e_' + datei)],
                       ['Original-Standbild Bild %d' % n if n else '(kein Original-Standbild zu Ruhe)', 'K5c heute (Spiel-Asset): ' + titel,
                        'K5e: ' + titel]))
    speichern(os.path.join(aus, '04_spiel.png'), stapeln(z, 'K5e im echten Spielfenster (gdigrab, kein AUTOSHOT/SOFTWARE_RENDER, Variante ueber '
                                                            'RE15_RE2_ASSET_ROOT): Original | K5c heute | K5e, roh'))


def weiss(mo, clip, az, el, gross=380):
    b = rl.weiss(mo, clip, az, el, gross)
    return b[0] if isinstance(b, tuple) else b


def kopf_nah(mo, clip, az, el):
    """Kopf (Meshes 22/23) nah unter weissem Licht: Render 760 px, Ausschnitt um die Kopfpixel (Kennung)."""
    img, K = rl.weiss_id(mo, clip, az, el, 760)
    m = np.isin(K['mesh'], (21, 22, 23))
    if not m.any():
        return cv2.resize(img, (300, 300), interpolation=cv2.INTER_AREA)
    ys, xs = np.nonzero(m)
    cy, cx = int((ys.min() + ys.max()) / 2), int((xs.min() + xs.max()) / 2)
    r = int(max(ys.max() - ys.min(), xs.max() - xs.min()) / 2 + 12)
    y0, x0 = max(cy - r, 0), max(cx - r, 0)
    a = img[y0:cy + r, x0:cx + r]
    return cv2.resize(a, (300, 300), interpolation=cv2.INTER_NEAREST)


def main():
    if sys.argv[1] == '--spiel':
        spiel(sys.argv[2], sys.argv[3])
        return
    neu, aus = sys.argv[1], sys.argv[2]
    os.makedirs(aus, exist_ok=True)
    st = {'K5c heute': R.Stand(R.BASIS), 'K5e': R.Stand(neu)}
    vor = {n: v4f.original(n) for n in (1, 2)}
    for n, datei in ((1, '01_bild1.png'), (2, '02_bild2.png')):
        z1 = reihe([vor[n]] + [st[k].d[n]['sauber'] for k in st], ['Vorlage Bild %d (Video-Standbild)' % n] +
                   ['%s: Disc-Licht x k, ohne Filter' % k for k in st])
        z2 = reihe([vor[n]] + [st[k].d[n]['video'][15] for k in st], ['Vorlage Bild %d (Video-Standbild)' % n] +
                   ['%s: Disc-Licht x k + Videofilter Saat 15' % k for k in st])
        speichern(os.path.join(aus, datei), stapeln([z1, z2], 'K5e ruhige Haut (Nutzer 2026-11-08), Bild %d: Vorlage | K5c heute (Spiel-Asset) | K5e. '
                                                       'Gleiche Szenenkette je Zeile, roh, kein Gamma.' % n))
    mods = {'K5c': rl.fl.lade_var(rl.BASIS), 'K5d': rl.fl.lade_var(rl.K5D), 'K5e': rl.fl.lade_var(neu)}
    # 03 Details nah
    zeilen = []
    fall = [('Schwanzspitze (Bild 1, Mesh 6)', 1, (205, 380, 345, 480), 'video'),
            ('Gelenke Beine/Arme (Bild 1)', 1, (190, 250, 440, 400), 'video'),
            ('Buckel vorn (Bild 2, F01/F02)', 2, (240, 140, 380, 230), 'video'),
            ('Buckel vorn (Bild 2), ohne Filter', 2, (240, 140, 380, 230), 'sauber'),
            ('Schultern vorn (Bild 2)', 2, (200, 190, 440, 290), 'video'),
            ('Krallen vorn (Bild 2), Videofilter', 2, (150, 290, 500, 480), 'video'),
            ('Krallen vorn (Bild 2), ohne Filter', 2, (150, 290, 500, 480), 'sauber')]
    for titel, n, box, art in fall:
        f = 3 if (box[2] - box[0]) <= 150 else 2
        bilder = [kachel(vor[n], box, f)]
        for k in st:
            im = st[k].d[n]['video'][15] if art == 'video' else st[k].d[n]['sauber']
            bilder.append(kachel(im, box, f))
        zeilen.append(reihe(bilder, ['Vorlage: ' + titel, 'K5c heute', 'K5e']))
    kz = []
    for clip, az, el in ((2, 60, 10), (2, 0, 8), (1, 60, 10), (1, 0, 8)):
        for k in ('K5c', 'K5e'):
            kz.append((kopf_nah(mods[k], clip, az, el), '%s Kopf Clip %d (%s) %d/%d' % (k, clip, 'Ruhe' if clip == 2 else 'Maul offen', az, el)))
    zeilen.append(reihe([a for a, _t in kz[:4]], [t for _a, t in kz[:4]]))
    zeilen.append(reihe([a for a, _t in kz[4:]], [t for _a, t in kz[4:]]))
    speichern(os.path.join(aus, '03_details_nah.png'), stapeln(zeilen, 'K5e Details nah: Vorlage | K5c heute | K5e (Ausschnitte x2/x3 nearest, Kette wie 01/02, roh). '
                                                                       'Kopf: weisses Licht wie Tafel 07 (0,55 + 0,45 N.L), roh.'))
    # 05 Kopf/Lippe
    zeilen = []
    for clip in (2, 1):
        for az, el in ((60, 10), (0, 8), (-60, 10)):
            ks = [(kopf_nah(mods[k], clip, az, el), '%s Clip %d (%s) %d/%d' % (k, clip, 'Ruhe' if clip == 2 else 'Maul offen', az, el))
                  for k in ('K5c', 'K5d', 'K5e')]
            zeilen.append(reihe([a for a, _t in ks], [t for _a, t in ks]))
    speichern(os.path.join(aus, '05_kopf_lippe.png'), stapeln(zeilen, 'Kopf/Lippe/Unterkiefer nah, weisses Licht wie Tafel 07: K5c heute | K5d (abgelehnt) | K5e, '
                                                                      'Clip 2 Ruhe und Clip 1 Maul offen, roh'))
    # 06 rundum
    zeilen = []
    for wn, az, el in rl.WINKEL4:
        zeilen.append(reihe([weiss(mods[k], 2, az, el) for k in ('K5c', 'K5d', 'K5e')], ['%s, %s (Clip 2)' % (k, wn) for k in ('K5c heute', 'K5d abgelehnt', 'K5e')]))
    speichern(os.path.join(aus, '06_rundum.png'), stapeln(zeilen, 'Rundum unter weissem Licht (render.blick der Familien-Tafel 07, 4 Winkel, Ruhe-Clip): '
                                                                  'K5c heute | K5d | K5e, roh'))
    # 07 Familie
    g4 = rl.PL.re2(0x34)
    g4.T = rl._Tex(g4)
    em = rl.fl.lade_em30()
    fam = (('RE2-G4 Original (Typ 0x34, Clip 0)', g4, 0), ('Hangar-Birkin EM30 RE1.5 (Clip 0)', em, 0), ('K5c heute (Clip 2)', mods['K5c'], 2),
           ('K5e (Clip 2)', mods['K5e'], 2))
    zeilen = []
    for wn, az, el in rl.WINKEL4:
        zeilen.append(reihe([weiss(mo, c, az, el) for _t, mo, c in fam], ['%s, %s' % (t, wn) for t, _mo, _c in fam]))
    speichern(os.path.join(aus, '07_familie.png'), stapeln(zeilen, 'Familie unter weissem Licht, gleiche Behandlung (gleiches Licht 0,55 + 0,45 N.L, gleiche '
                                                                   'Bildgroesse, keine Aufhellung): RE2-G4 | Hangar-Birkin RE1.5 | K5c | K5e, roh'))
    print('Tafeln fertig:', aus)


if __name__ == '__main__':
    main()
