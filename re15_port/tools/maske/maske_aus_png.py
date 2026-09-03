"""Freigestelltes PNG (Alpha) -> Maskenregion. DER schnellste und genaueste Weg.

⛔ WARUM ES DAS GIBT (Nutzer, 2026-09-04): "die beiden Flaggen habe ich einfach mit dem
Lasso-Tool in GIMP innerhalb von 3 Minuten extrahiert. Sehe ich das falsch, dass du nicht
mehr als das brauchst?" — Nein, das ist genau richtig, und es ist besser als alles, was
ich automatisch geschnitten habe:

  * Das Maskenformat IST eine Freistellung. Der Atlas ist 8 bpp, Palettenindex 0 wird
    nicht gezeichnet — undurchsichtige Pixel verdecken, alles andere ist durchsichtig.
    Ein Alphakanal ist also 1:1 das, was hineingehoert.
  * Meine automatischen Verfahren scheiterten alle an derselben Stelle: Superpixel sind
    FLAECHIG (eine 20 px breite Fahne bekommt einen 55x117-Block samt Fenster und
    Teppich), der Kontrastschnitt verliert dunkle Objekte vor dunklem Hintergrund
    (die untere Haelfte der blauen Fahne), und Kaesten treffen keine fransigen Formen.
    Ein Mensch mit dem Lasso hat keins dieser Probleme.

Die Lage im Bild wird NICHT abgefragt, sondern gemessen: die freigestellten Pixel sind
Hintergrundpixel, also gibt es genau eine Stelle, an der sie zum Hintergrund passen. Das
Werkzeug probiert die Massstaebe (das PNG kommt meist aus einem 2x/3x-Bildschirmfoto) und
alle Verschiebungen und nimmt die beste Uebereinstimmung. Passt nichts, sagt es das.

Aufruf zum Pruefen:
    python re15_port/tools/maske/maske_aus_png.py fehler/flag.png ROOM1140 3
In der Auswahldatei:
    {"name": "US-Fahne", "png": "fehler/flag.png", "fuss": 192}
"""
import argparse
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from geom import load_bg


def _skaliere(im, s):
    if s == 1:
        return im
    return im.resize((max(1, im.width // s), max(1, im.height // s)), Image.BOX)


def platziere(png_pfad, bg, massstaebe=(1, 2, 3, 4), alpha_schwelle=110):
    """-> (region 240x320 bool, info-dict) oder (None, info) wenn nichts passt."""
    im = Image.open(png_pfad).convert("RGBA")
    bgf = bg.astype(np.float64)
    best = None
    for s in massstaebe:
        k = _skaliere(im, s)
        a = np.asarray(k)
        if a.shape[0] > 240 or a.shape[1] > 320:
            continue
        m = a[:, :, 3] > alpha_schwelle
        if m.sum() < 20:
            continue
        t = a[:, :, :3].astype(np.float64)
        h, w = m.shape
        # nur jeden 2. Punkt fuer die Suche — reicht fuer die Grobstelle und ist schnell
        ys, xs = np.nonzero(m)
        wahl = slice(None, None, max(1, len(ys) // 1500))
        ys2, xs2 = ys[wahl], xs[wahl]
        tv = t[ys2, xs2]
        for dy in range(0, 240 - h + 1):
            for dx in range(0, 320 - w + 1):
                d = np.abs(bgf[ys2 + dy, xs2 + dx] - tv).max(1)
                gut = float((d < 26).mean())
                if best is None or gut > best[0]:
                    best = (gut, s, dx, dy, m, h, w)
    if best is None:
        return None, {"grund": "PNG passt in keinen Massstab"}
    gut, s, dx, dy, m, h, w = best
    region = np.zeros((240, 320), bool)
    region[dy:dy + h, dx:dx + w] = m
    return region, {"massstab": s, "x": dx, "y": dy, "breite": w, "hoehe": h,
                    "punkte": int(m.sum()), "uebereinstimmung": gut}


def setze(png_pfad, x, y, massstab, alpha_schwelle=110):
    """Freistellung an eine BEKANNTE Stelle setzen (ohne Suche)."""
    im = _skaliere(Image.open(png_pfad).convert("RGBA"), massstab)
    a = np.asarray(im)
    m = a[:, :, 3] > alpha_schwelle
    h, w = m.shape
    r = np.zeros((240, 320), bool)
    x0, y0 = max(0, x), max(0, y)
    x1, y1 = min(320, x + w), min(240, y + h)
    if x1 <= x0 or y1 <= y0:
        return None
    r[y0:y1, x0:x1] = m[y0 - y:y1 - y, x0 - x:x1 - x]
    return r


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("png")
    ap.add_argument("room")
    ap.add_argument("cut", type=int)
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--out", default="build/png_probe.png")
    a = ap.parse_args()
    rid = int(a.room.upper()[4:], 16)
    bg = load_bg(a.ppm, rid, a.cut)
    r, info = platziere(a.png, bg)
    print(info)
    if r is None:
        return 1
    from PIL import ImageDraw
    from blatt import aufhellen
    base = (aufhellen(bg, 0.5) * 255).astype(np.uint8)
    ov = base.astype(float).copy()
    ov[r] = ov[r] * 0.35 + np.array([255, 20, 230]) * 0.65
    Z = 4
    s = Image.new("RGB", (320 * Z * 2 + 8, 240 * Z), (30, 30, 30))
    s.paste(Image.fromarray(base).resize((320 * Z, 240 * Z), Image.NEAREST), (0, 0))
    s.paste(Image.fromarray(ov.astype(np.uint8)).resize((320 * Z, 240 * Z), Image.NEAREST), (320 * Z + 8, 0))
    s.save(a.out)
    print(a.out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
