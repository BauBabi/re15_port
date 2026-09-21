# -*- coding: utf-8 -*-
"""Zensus A, Schritt 11: IST DIE KASTEN-FLAECHE UEBERHAUPT EINE FLAECHE?

Das Bild kasten_roh_vs_wirksam_ROOM10F0_C4.png zeigt, dass die wirksame Flaeche der
Buerostuhl-Kaesten kein Moebel ist, sondern ein Sprenkelmuster ueber Pult, Papieren und
Bodenschatten - alles, was in der Kasten-Silhouette dunkel genug ist (Tiefschwarz-Regel,
Summe < 45, raum.py:322). Gemessen wird deshalb die Koernigkeit:

  Komponenten, Median-Komponentengroesse, Anteil der Punkte in Komponenten < 10 Punkten.

⛔ Eine Maske aus Einzelpunkten kann eine Figur nicht verdecken - sie erzeugt ein Raster
aus Hintergrundpunkten auf ihr. Zum Vergleich das NULLMODELL: dieselbe Koernigkeit fuer
die Nutzer-Lassos desselben Cuts (von Hand freigestellte, zusammenhaengende Flaechen).
"""
import io, json, os, struct, sys
import numpy as np
from scipy import ndimage as nd
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, maskenbild, abnahme, raum, maske_aus_png

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))


def koernig(reg):
    lab, n = nd.label(reg)
    if n == 0:
        return 0, 0, 0.0, 0
    gr = np.bincount(lab.ravel())[1:]
    klein = int(gr[gr < 10].sum())
    return n, int(np.median(gr)), 100.0 * klein / max(1, int(reg.sum())), int(gr.max())


for room, cut in (('ROOM10F0', 4), ('ROOM10F0', 5), ('ROOM1100', 1), ('ROOM1100', 2)):
    rid = int(room[4:], 16)
    e = raum.eintrag(AUS[room][str(cut)])
    objs = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    print('=' * 108)
    print('%s C%d' % (room, cut))
    print('  %-42s %7s %6s %8s %9s %8s' % ('Objekt', 'Punkte', 'Komp.', 'Median', '<10 px', 'groesste'))
    for o in objs:
        n, med, anteil, mx = koernig(o[1])
        art = 'Quader' if o[9] is not None and 'Buerostuhl' in o[0] or 'wand' in o[0].lower() or 'block' in o[0].lower() else 'Lasso+Zelle'
        print('  %-42s %7d %6d %8d %8.1f %% %8d   [%s]'
              % (o[0][:42], int(o[1].sum()), n, med, anteil, mx, art))
    # NULLMODELL: die rohen Nutzer-Lassos dieses Cuts
    print('  -- NULLMODELL: rohe Nutzer-Lassos (von Hand freigestellt) --')
    for o in AUS[room][str(cut)].get('objekte') or []:
        if 'png' not in o or not all(k in o for k in ('x', 'y')):
            continue
        r = maske_aus_png.setze(o['png'], o['x'], o['y'], o.get('massstab', 1))
        if r is None:
            continue
        n, med, anteil, mx = koernig(r)
        print('  %-42s %7d %6d %8d %8.1f %% %8d' % (o['png'][-12:], int(r.sum()), n, med, anteil, mx))
