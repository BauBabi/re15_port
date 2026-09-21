# -*- coding: utf-8 -*-
"""Zensus A, Schritt 9b: Ueberstand der Wand-Kasten-Unterkante auf den GEMALTEN Boden.

Gemessen je Bildspalte, VON UNTEN: der Fliesenboden ist hell (Helligkeitssumme 90..250),
die Wandflaeche dunkel (15..70) - die Profile stehen im Dossier. Die gemalte Fussleiste
einer Spalte ist die oberste Zeile des hellen Laufs, der den unteren Bildrand erreicht
(Luecken bis 2 Zeilen fuer die Fugenlinien geschlossen). Spalten, in denen der helle Lauf
den unteren Rand nicht erreicht (die Wand laeuft aus dem Bild), sind NICHT auswertbar und
werden als solche gezaehlt.

Ueberstand = Maskenunterkante - (Fussleiste - 1); positiv = die Maske liegt auf dem Boden.
"""
import io, os, sys
import numpy as np
from scipy import ndimage as nd
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, maskenbild, abnahme

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'


def fussleiste(bg, schwelle, luecke=2):
    """-> (320,) oberste Zeile des bodenhellen Laufs, der Zeile 239 erreicht; NaN sonst."""
    hell = bg.astype(np.int32).sum(2) > schwelle
    out = np.full(320, np.nan)
    for x in range(320):
        # ⛔ binary_closing erodiert am Arrayrand (border_value=0) und loescht die
        # letzten Zeilen - deshalb mit hellem Rand padden, schliessen, zurueckschneiden.
        pad = np.pad(hell[:, x], luecke + 1, mode='edge')
        c = nd.binary_closing(pad, np.ones(2 * luecke + 1, bool))[luecke + 1:-(luecke + 1)]
        if not c[239]:
            continue
        y = 239
        while y > 0 and c[y - 1]:
            y -= 1
        out[x] = y
    return out


for room, cut in (('ROOM1100', 1), ('ROOM1100', 2)):
    rid = int(room[4:], 16)
    bg = geom.load_bg(PPM, rid, cut)
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    unten = np.full(320, np.nan)
    for x in range(320):
        r = np.nonzero(deck[:, x])[0]
        if len(r):
            unten[x] = r.max()
    nmask = int(np.isfinite(unten).sum())
    print('=' * 100)
    print('%s C%d  %d Spalten mit Maske' % (room, cut, nmask))
    for schwelle in (70, 80, 100, 120):
        fl = fussleiste(bg, schwelle)
        sp = np.nonzero(np.isfinite(unten) & np.isfinite(fl))[0]
        if len(sp) == 0:
            print('  Schwelle %3d: keine auswertbare Spalte' % schwelle); continue
        d = unten[sp] - (fl[sp] - 1)
        print('  Schwelle %3d: %3d auswertbare Spalten (ABDECKUNG %.0f %%) | Ueberstand auf den Boden: '
              'Median %+.0f, P90 %+.0f, Max %+.0f px | %d Spalten mit > 1 px (%.0f %%)'
              % (schwelle, len(sp), 100.0 * len(sp) / nmask, np.median(d), np.percentile(d, 90), d.max(),
                 int((d > 1).sum()), 100.0 * (d > 1).sum() / len(sp)))
    fl = fussleiste(bg, 80)
    ys = np.arange(240)[:, None]
    auf_boden = deck & np.isfinite(fl)[None, :] & (ys >= np.where(np.isfinite(fl), fl, 1e9)[None, :])
    print('  Maskenpunkte AUF dem gemalten Boden (Schwelle 80, nur auswertbare Spalten): %d von %d (%.1f %%)'
          % (int(auf_boden.sum()), int(deck.sum()), 100.0 * auf_boden.sum() / max(1, deck.sum())))
    np.save('build/r19/fl_%s_C%d.npy' % (room, cut), fl)
