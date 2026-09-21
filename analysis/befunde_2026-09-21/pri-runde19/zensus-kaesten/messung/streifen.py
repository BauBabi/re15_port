# -*- coding: utf-8 -*-
"""Zensus A, Schritt 10: WIRKT der Ueberstand des Wand-Kastens auf den Boden?

Der Streifen = Maskenpunkte, die in einer auswertbaren Spalte UNTER der gemalten
Fussleiste liegen (Schritt 9b). Dort ist im Bild BODEN gemalt, die Maske traegt aber die
Tiefe der Wand-Vorderflaeche, die naeher ist als der Boden an dieser Stelle. Gemessen:
wie viele begehbare Standplaetze haben Koerperpunkte IN diesem Streifen, und wie viele
davon verdeckt die Maske - ohne Quote.
"""
import io, os, struct, sys
import numpy as np
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
DUMP = 'build/floor_dump.txt'

for room, cut in (('ROOM1100', 1), ('ROOM1100', 2)):
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    bg = geom.load_bg(PPM, rid, cut)
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    fl = np.load('build/r19/fl_%s_C%d.npy' % (room, cut))
    ys = np.arange(240)[:, None]
    streifen = deck & np.isfinite(fl)[None, :] & (ys >= np.where(np.isfinite(fl), fl, 1e9)[None, :])
    floor = abnahme.floor_aus_dump(DUMP, rid)
    ges = 0; beruehrt = 0; verd = 0; schlimm = []
    for band, pts in floor.items():
        yfoot = -band * 0x708
        for (wx, wz) in pts:
            pf = abnahme.proj(R, t, H, wx, yfoot, wz)
            pk = abnahme.proj(R, t, H, wx, yfoot - abnahme.KOPF, wz)
            if pf is None or pk is None:
                continue
            ges += 1
            fsx, fsy, fvz = pf
            ksx, ksy, kvz = pk
            hw = abnahme.HALB * H / fvz
            x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
            y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
            if x1 <= x0 or y1 <= y0:
                continue
            box = streifen[y0:y1, x0:x1]
            if not box.any():
                continue
            beruehrt += 1
            rows = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, rows)
            vzs = np.where(ok, vzs, fvz)[:, None]
            n = int((box & geometrie.verdeckt(tief[y0:y1, x0:x1], vzs)).sum())
            verd += n
            if n:
                schlimm.append((n, band, int(wx), int(wz)))
    schlimm.sort(reverse=True)
    print('=' * 96)
    print('%s C%d  Streifen (Maske auf gemaltem Boden): %d Punkte' % (room, cut, int(streifen.sum())))
    print('  ABDECKUNG: %d Standplaetze projiziert, %d haben Koerperpunkte IM Streifen (%.1f %%)'
          % (ges, beruehrt, 100.0 * beruehrt / max(1, ges)))
    print('  davon VERDECKT die Maske: %d Koerperpunkte an %d Standplaetzen' % (verd, len(schlimm)))
    for x in schlimm[:6]:
        print('     Band %d (%d,%d): %d Punkte' % (x[1], x[2], x[3], x[0]))
