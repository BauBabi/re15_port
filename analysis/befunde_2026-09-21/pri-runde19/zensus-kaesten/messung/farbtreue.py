# -*- coding: utf-8 -*-
"""Zensus A, Schritt 3: eine PRI-Maske muss UNSICHTBAR sein, wenn nichts davor steht.
Gemessen wird die Farbe, die die Engine blittet (CLUT[idx]<<3, bg_pc.c:156-159), gegen
die Farbe des Hintergrunds an derselben Stelle. Abdeckung wird mitgegeben."""
import glob, os, struct, sys
import numpy as np
sys.path.insert(0, 're15_port/tools/maske')
import geom, maskenbild, abnahme

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'


def atlas_rgb(tim_bytes):
    idx, clut = maskenbild.lies_tim_bytes(tim_bytes)
    c = np.asarray(clut, np.uint32)
    r = ((c & 0x1f) << 3).astype(np.uint8)
    g = (((c >> 5) & 0x1f) << 3).astype(np.uint8)
    b = (((c >> 10) & 0x1f) << 3).astype(np.uint8)
    pal = np.stack([r, g, b], 1)                  # 256x3, genau wie bg_pc.c:157-159
    return idx, pal[idx]                          # (H,W), (H,W,3)


def mess(room, cut):
    rid = int(room[4:], 16)
    bg = geom.load_bg(PPM, rid, cut)
    if bg is None:
        return None
    p = os.path.join(CD, 'MASKS', room + '.MSK')
    if not os.path.exists(p):
        return None
    blob = open(p, 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    tp = os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut))
    if not ms or not os.path.exists(tp):
        return None
    idx, arg = atlas_rgb(open(tp, 'rb').read())
    blit = np.zeros((240, 320, 3), np.int32)
    deck = np.zeros((240, 320), bool)
    for (sx, sy, X, Y, w, h, dep) in ms:
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0:
            continue
        s = idx[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
        c = arg[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
        if s.shape != (y1 - y0, x1 - x0):
            continue
        op = s != 0
        sub = blit[y0:y1, x0:x1]
        sub[op] = c[op]
        blit[y0:y1, x0:x1] = sub
        deck[y0:y1, x0:x1] |= op
    d = np.abs(blit - bg.astype(np.int32)).max(2)
    e = d[deck]
    return dict(n=int(deck.sum()), mean=float(e.mean()), p50=float(np.percentile(e, 50)),
                p95=float(np.percentile(e, 95)), mx=int(e.max()),
                gt8=int((e > 8).sum()), gt16=int((e > 16).sum()), gt32=int((e > 32).sum()),
                deck=deck, d=d, bg=bg, blit=blit)


def zeile(room, cut, r):
    return ('  %-8s C%d  %6d Maskenpunkte | Farbfehler max-Kanal: Mittel %5.1f  Median %4.0f  '
            'P95 %4.0f  Max %3d | >8: %6d (%5.1f %%)  >16: %6d (%5.1f %%)  >32: %5d (%4.1f %%)'
            % (room, cut, r['n'], r['mean'], r['p50'], r['p95'], r['mx'],
               r['gt8'], 100.0 * r['gt8'] / r['n'], r['gt16'], 100.0 * r['gt16'] / r['n'],
               r['gt32'], 100.0 * r['gt32'] / r['n']))


if __name__ == '__main__':
    ZIELE = [('ROOM10F0', 4), ('ROOM10F0', 5), ('ROOM1100', 1), ('ROOM1100', 2), ('ROOM10D0', 7)]
    print('== Die fuenf Kasten-Cuts ==')
    for room, cut in ZIELE:
        r = mess(room, cut)
        if r:
            print(zeile(room, cut, r))
    print()
    print('== NULLMODELL: alle uebrigen Cuts mit nachgezeichneter Maske (nur Nutzer-PNGs) ==')
    ges = []
    for p in sorted(glob.glob(os.path.join(CD, 'MASKS', 'ROOM*.MSK'))):
        room = os.path.basename(p)[:-4]
        for cut in range(16):
            if (room, cut) in ZIELE:
                continue
            r = mess(room, cut)
            if r:
                ges.append((room, cut, r))
    print('  %d Cuts gemessen (ABDECKUNG des Nullmodells)' % len(ges))
    alle = np.array([x[2]['mean'] for x in ges])
    q = np.array([100.0 * x[2]['gt16'] / x[2]['n'] for x in ges])
    print('  Mittel des Farbfehlers ueber alle Cuts: Median %.1f, P90 %.1f, Max %.1f'
          % (np.percentile(alle, 50), np.percentile(alle, 90), alle.max()))
    print('  Anteil Punkte >16: Median %.1f %%, P90 %.1f %%, Max %.1f %%'
          % (np.percentile(q, 50), np.percentile(q, 90), q.max()))
    print('  die 8 schlechtesten (Anteil >16):')
    for room, cut, r in sorted(ges, key=lambda x: -x[2]['gt16'] / x[2]['n'])[:8]:
        print(zeile(room, cut, r))
