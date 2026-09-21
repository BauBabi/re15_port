# -*- coding: utf-8 -*-
"""Zensus A, Schritt 9: liegt die UNTERKANTE des Wand-Kastens auf der GEMALTEN
Wand-Fussleiste? Gemessen je Bildspalte, an den Texeln - nicht an der Bbox.

Die gemalte Fussleiste ist der Uebergang von der dunklen Wandflaeche zum hellen
Fliesenboden. Kriterium je Spalte: die unterste Zeile, ab der die Helligkeit
(Summe der Kanaele) ueber der Schwelle liegt UND es darunter hell bleibt. Die
Schwelle wird NICHT geraten, sondern aus dem Bild selbst genommen (Otsu ueber die
Spalte-Umgebung); zusaetzlich wird die Sensitivitaet gegen drei Schwellen gezeigt.
"""
import io, os, struct, sys
import numpy as np
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, maskenbild, abnahme

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'


def basis_gemalt(bg, schwelle):
    """-> (320,) Zeile des ersten hellen Punkts von oben nach unten, NaN wenn keiner."""
    hell = bg.astype(np.int32).sum(2) > schwelle
    out = np.full(320, np.nan)
    for x in range(320):
        rows = np.nonzero(hell[:, x])[0]
        if len(rows):
            out[x] = rows.min()
    return out


for room, cut in (('ROOM1100', 1), ('ROOM1100', 2)):
    rid = int(room[4:], 16)
    bg = geom.load_bg(PPM, rid, cut)
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    # Maskenunterkante je Spalte
    unten = np.full(320, np.nan)
    for x in range(320):
        rows = np.nonzero(deck[:, x])[0]
        if len(rows):
            unten[x] = rows.max()
    s = bg.astype(np.int32).sum(2)
    print('=' * 92)
    print('%s C%d  Helligkeitssumme: Median %d, P10 %d, P90 %d' % (room, cut, np.median(s), np.percentile(s, 10), np.percentile(s, 90)))
    for schwelle in (90, 120, 150, 180):
        gb = basis_gemalt(bg, schwelle)
        # nur Spalten, in denen die Maske bis unter die gemalte Basis reicht
        sp = np.nonzero(np.isfinite(unten) & np.isfinite(gb))[0]
        # Ueberstand = Maskenunterkante UNTER der gemalten Wandbasis (positiv = auf dem Boden)
        d = unten[sp] - gb[sp]
        # Spalten, in denen die Maske bis zum Bildrand laeuft, sind nicht auswertbar
        ok = unten[sp] < 239
        sp2 = sp[ok]; d2 = d[ok]
        if len(d2) == 0:
            print('  Schwelle %3d: keine auswertbare Spalte' % schwelle); continue
        ueber = int((d2 > 1).sum())
        print('  Schwelle %3d: %3d auswertbare Spalten (ABDECKUNG %.0f %% der %d Maskenspalten) | '
              'Ueberstand Median %+.1f, P90 %+.1f, Max %+.0f px | %d Spalten > 1 px auf dem Boden (%.0f %%)'
              % (schwelle, len(sp2), 100.0 * len(sp2) / max(1, int(np.isfinite(unten).sum())),
                 int(np.isfinite(unten).sum()), np.median(d2), np.percentile(d2, 90), d2.max(),
                 ueber, 100.0 * ueber / len(d2)))
    # Wieviele Maskenpunkte liegen UNTER der gemalten Wandbasis? (Schwelle 120)
    gb = basis_gemalt(bg, 120)
    ys = np.arange(240)[:, None]
    unterhalb = deck & (ys > gb[None, :]) & np.isfinite(gb)[None, :]
    print('  Maskenpunkte UNTER der gemalten Wandbasis (Schwelle 120): %d von %d (%.1f %%)'
          % (int(unterhalb.sum()), int(deck.sum()), 100.0 * unterhalb.sum() / max(1, deck.sum())))
