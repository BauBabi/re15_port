# -*- coding: utf-8 -*-
"""Bildbeleg zum Funkeln: was der Nutzer an der Figur SIEHT.

Die Figur wird als Flaeche gesetzt (der Renderer zeichnet sie zuerst), dann blittet der
Zeichner die Maskenpunkte mit der Farbe aus dem Atlas darueber - genau wie bg_pc.c. Die
Sprenkel erscheinen damit als Hintergrundpunkte AUF der Figur.
"""
import io, os, struct, sys
import numpy as np
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme
from PIL import Image, ImageDraw

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
OUT = 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten'
FIGUR = np.array([70, 110, 165])          # Platzhalterfarbe der Figur


def atlas_rgb(tb):
    idx, clut = maskenbild.lies_tim_bytes(tb)
    c = np.asarray(clut, np.uint32)
    pal = np.stack([((c & 0x1f) << 3), (((c >> 5) & 0x1f) << 3), (((c >> 10) & 0x1f) << 3)], 1).astype(np.uint8)
    return idx, pal[idx]


for room, cut, wx, wz in (('ROOM10F0', 4, 4600, 7200), ('ROOM10F0', 5, -3200, 4000)):
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    bg = geom.load_bg(PPM, rid, cut)
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx, arg = atlas_rgb(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    sp = np.load('build/r19/sprenkel_%s_C%d.npy' % (room, cut))
    pf = abnahme.proj(R, t, H, wx, 0, wz)
    pk = abnahme.proj(R, t, H, wx, -abnahme.KOPF, wz)
    fsx, fsy, fvz = pf
    ksx, ksy, kvz = pk
    hw = abnahme.HALB * H / fvz
    x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
    y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
    koerper = np.zeros((240, 320), bool)
    koerper[y0:y1, x0:x1] = True
    rows = np.arange(y0, y1)
    vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, rows)
    vzs = np.where(ok, vzs, fvz)[:, None]
    vd = np.zeros((240, 320), bool)
    vd[y0:y1, x0:x1] = geometrie.verdeckt(tief[y0:y1, x0:x1], vzs)
    # Was der Zeichner ausgibt: Figur, darueber die Maskenpunkte mit Atlasfarbe
    farbe = np.zeros((240, 320, 3), np.int32)
    for (sx, sy, X, Y, w, h, dep) in ms:
        ax0, ax1 = max(0, X), min(320, X + w)
        ay0, ay1 = max(0, Y), min(240, Y + h)
        if ax1 <= ax0 or ay1 <= ay0:
            continue
        s = idx[sy + (ay0 - Y):sy + (ay1 - Y), sx + (ax0 - X):sx + (ax1 - X)]
        c = arg[sy + (ay0 - Y):sy + (ay1 - Y), sx + (ax0 - X):sx + (ax1 - X)]
        if s.shape != (ay1 - ay0, ax1 - ax0):
            continue
        op = s != 0
        sub = farbe[ay0:ay1, ax0:ax1]
        sub[op] = c[op]
        farbe[ay0:ay1, ax0:ax1] = sub
    szene = bg.astype(np.int32).copy()
    szene[koerper] = FIGUR
    gemalt = koerper & deck & vd
    szene[gemalt] = farbe[gemalt]
    # Zuschnitt auf den Koerper
    cx0, cx1 = max(0, x0 - 12), min(320, x1 + 12)
    cy0, cy1 = max(0, y0 - 8), min(240, y1 + 8)
    Zm = 7
    A = szene[cy0:cy1, cx0:cx1].astype(np.uint8)
    mark = szene[cy0:cy1, cx0:cx1].astype(np.int32).copy()
    m = (koerper & sp & vd)[cy0:cy1, cx0:cx1]
    mark[m] = np.array([255, 0, 0])
    w = (cx1 - cx0) * Zm
    hh = (cy1 - cy0) * Zm
    im = Image.new('RGB', (w * 2 + 10, hh + 18), (12, 12, 12))
    im.paste(Image.fromarray(A).resize((w, hh), Image.NEAREST), (0, 18))
    im.paste(Image.fromarray(mark.astype(np.uint8)).resize((w, hh), Image.NEAREST), (w + 10, 18))
    d = ImageDraw.Draw(im)
    d.text((4, 4), '%s C%d  Standplatz (%d,%d): Figur (blau) mit den darueber geblitteten Maskenpunkten | '
                   'rechts dieselben Punkte, die SPRENKEL rot markiert (%d Punkte)'
           % (room, cut, wx, wz, int(m.sum())), fill=(255, 255, 255))
    p = os.path.join(OUT, 'funkeln_%s_C%d.png' % (room, cut))
    im.save(p)
    print(p, 'Figurpunkte', int(koerper.sum()), 'geblittet', int(gemalt.sum()), 'davon Sprenkel', int(m.sum()))
