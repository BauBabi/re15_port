#!/usr/bin/env python3
"""Varianten, Mass fuer "gezeichnet" (E15) an der Textur selbst, alle Modelle gleich ausgelesen (benutzter Bereich = von Flaechen gedeckte Texel,
je CLUT-Zeile ihre Farben). python vm_gezeichnet.py AUS.json "Name=form.json" ... [--spiel]. Kein RE-Wert.

 - gleich: Anteil gleicher Nachbartexel (waagrecht und senkrecht, beide benutzt).
 - hart: Anteil der Nachbarpaare mit Helligkeitssprung ueber 40 % der mittleren Helligkeit (relativ, damit dunkle und helle Texturen vergleichbar sind).
 - gerade: Laenge gerader Kanten (Canny auf Y, HoughLinesP, Stuecke ab 6 Texel) je 1000 benutzte Texel.
 - toene8: Median der verschiedenen Farben je 8 x 8-Feld (nur Felder ganz im benutzten Bereich).
 - spektrum: Anteil der Helligkeitsenergie oberhalb 0,25 Schwingungen je Texel an der Energie oberhalb 0,03 (Feinstruktur gegen Flaechen).
Mit --spiel zusaetzlich fuer Modelle mit form.json: Feinstruktur im gefilterten Render in der Figurmaske gegen den Screenshot
(Varianz der Hochpass-Helligkeit Y - Gauss(Y, 2) relativ zu Y^2, und Spektralanteil oberhalb 0,15 Schwingungen je Pixel)."""
import json
import os
import re
import sys

import cv2
import numpy as np

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402
import form_video as FV        # noqa: E402

YK = np.array([.299, .587, .114], np.float32)


def textur_von(mo):
    """EA.Modell-artig (meshes mit tuv/quv, tex Indizes, pal [Zeilen, 256, 3], clut_y) -> (rgb, benutzt)."""
    H, W = mo.tex.shape
    rgb = np.zeros((H, W, 3), np.uint8)
    benutzt = np.zeros((H, W), bool)
    for m in mo.meshes:
        for faces, uvs, vier in ((m.tf, m.tuv, False), (m.qf, m.quv, True)):
            for uv in uvs:
                po = (uv[5] & 0xF) * 128
                zeile = ((uv[2] >> 6) & 0x1FF) - mo.clut_y
                zeile = zeile if 0 <= zeile < len(mo.pal) else 0
                pts = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                if vier:
                    pts = [pts[0], pts[1], (uv[9] + po, uv[10]), pts[2]]
                mk = np.zeros((H, W), np.uint8)
                cv2.fillPoly(mk, [np.array(pts, np.int32)], 1)
                s = (mk > 0) & ~benutzt
                rgb[s] = mo.pal[zeile][mo.tex[s]]
                benutzt |= mk > 0
    return rgb, benutzt


def re2(kind):
    import birkin_bahnsteig as BB
    W = os.path.normpath(os.path.join(TOOLS, '..', '..'))
    inc = open(os.path.join(W, 're15_port/engine/src/gen/re2_ems_toc.inc'), encoding='utf-8', errors='replace').read()
    toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S).group(1))]
    ems = open(os.path.join(W, 're15_port/shared_assets/RE2/CDEMD0.EMS'), 'rb').read()

    def rec(r):
        i = ((kind - 0x10) * 4 + r) * 2
        return ems[toc[i] * 0x800:toc[i] * 0x800 + toc[i + 1]]
    BB.KIND = kind
    return BB.modell(rec(3), rec(2)), rec(2)


def masse(rgb, b):
    Y = rgb.astype(np.float32) @ YK
    my = max(float(Y[b].mean()), 1.0)
    gl, ha, n = 0, 0, 0
    for a, c, mb in ((rgb[:, 1:], rgb[:, :-1], b[:, 1:] & b[:, :-1]), (rgb[1:], rgb[:-1], b[1:] & b[:-1])):
        eq = (a == c).all(-1)[mb]
        dy = np.abs(a.astype(np.float32) @ YK - c.astype(np.float32) @ YK)[mb]
        gl += int(eq.sum())
        ha += int((dy > 0.4 * my).sum())
        n += int(mb.sum())
    Yn = np.where(b, Y, my)
    ed = cv2.Canny(np.clip(Yn * 255.0 / max(Y[b].max(), 1), 0, 255).astype(np.uint8), 40, 100) > 0
    ed &= cv2.erode(b.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    lin = cv2.HoughLinesP(ed.astype(np.uint8) * 255, 1, np.pi / 180, 8, minLineLength=6, maxLineGap=1)
    gerade = 0.0 if lin is None else float(sum(np.hypot(x2 - x1, y2 - y1) for x1, y1, x2, y2 in lin[:, 0]))
    t8 = []
    H, W = b.shape
    q = (rgb // 8).astype(np.int32)
    code = q[..., 0] + 32 * q[..., 1] + 1024 * q[..., 2]
    for y in range(0, H - 7, 8):
        for x in range(0, W - 7, 8):
            if b[y:y + 8, x:x + 8].all():
                t8.append(len(np.unique(code[y:y + 8, x:x + 8])))
    # Spektrum: benutzter Bereich, aussen Mittel, Hann-Fenster
    Z = (Yn - my) * b
    hann = np.outer(np.hanning(H), np.hanning(W))
    F = np.abs(np.fft.fftshift(np.fft.fft2(Z * hann))) ** 2
    fy = np.fft.fftshift(np.fft.fftfreq(H))[:, None]
    fx = np.fft.fftshift(np.fft.fftfreq(W))[None, :]
    fr = np.hypot(fy, fx)
    sp = float(F[fr > 0.25].sum() / max(F[fr > 0.03].sum(), 1e-9))
    return dict(gleich=round(gl / max(n, 1), 3), hart=round(ha / max(n, 1), 3), gerade=round(gerade * 1000.0 / max(b.sum(), 1), 1),
                toene8=float(np.median(t8)) if t8 else None, spektrum=round(sp, 3), mittel_y=round(my, 1))


def spiel_fein(k):
    aus = {}
    for n in (1, 2):
        s, _a, m, _ = k.rendern(n)
        g = FV.video_filter(s, bild=n, entzerren=False, rauschen=False)
        z = (P.ziel_laden(n)[1] == 255) & ~P.logo_maske()
        z = cv2.erode(z.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
        r = {}
        for nm, a in (('render', g), ('screenshot', P.original(n))):
            Y = a.astype(np.float32) @ YK
            hp = Y - cv2.GaussianBlur(Y, (0, 0), 2.0)
            r[nm] = round(float((hp[z] ** 2).mean() / max((Y[z] ** 2).mean(), 1e-6)), 4)
        aus['bild%d' % n] = r
    return aus


def main():
    ziel = sys.argv[1]
    R = {}
    for z in [a for a in sys.argv[2:] if not a.startswith('--')]:
        nm, pf = z.split('=', 1)
        k = P.Kandidat(pf)
        rgb, b = textur_von(k.mo)
        R[nm] = masse(rgb, b)
        if '--spiel' in sys.argv:
            R[nm]['spiel_fein'] = spiel_fein(k)
    for nm, kind in (('RE2 G2 0x31', 0x31), ('RE2 G3 0x33', 0x33), ('RE2 G4 0x34', 0x34), ('RE2 Zombie 0x10', 0x10), ('RE2 Hund 0x20', 0x20)):
        mo, _t = re2(kind)
        R[nm] = masse(*textur_von(mo))
    R['RE1.5 Zombie 0x10'] = masse(*textur_von(EA.Modell(0x10)))
    json.dump(R, open(ziel, 'w', encoding='utf-8'), indent=1)
    print('%-20s %6s %6s %7s %6s %8s %6s' % ('', 'gleich', 'hart', 'gerade', 'toene8', 'spektrum', 'Y'))
    for nm, v in R.items():
        print('%-20s %6.3f %6.3f %7.1f %6s %8.3f %6.1f %s' % (nm, v['gleich'], v['hart'], v['gerade'], v['toene8'], v['spektrum'], v['mittel_y'], v.get('spiel_fein', '')))


if __name__ == '__main__':
    main()
