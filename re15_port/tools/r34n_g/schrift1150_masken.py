"""Spur G2 (Runde 34 Nacht) — ROOM1150/1151: die blinkenden sprite.pri-Gruppen sichtbar machen.

Mess-Werkzeug, KEIN Port-Code. Liest die Kuenstler-Masken eines Cuts (Kamera +0x1C,
Parser-Regeln wie FUN_800392d4) und den Vordergrundatlas des Cuts, listet je Gruppe die Records
und setzt zwei Bilder zusammen:
  AN  = Hintergrund + alle Masken (so zeichnet FUN_80039590, wenn Record-Byte0 Bit0 = 1)
  AUS = Hintergrund + alle Masken AUSSER den Gruppen, die sub05 per Opcode 0x45 auf 0 setzt
Der Unterschied AN/AUS ist genau das, was das Original im Takt von sub05 umschaltet.

ATLAS: Standard = BSS/<ROOM>/PRI<cut>.TIM (fuer ROOM1151 die Datei von ROOM1150, gleiche BSS).
Gemessen 2026-09-30: diese Datei ist BYTEGLEICH zum Atlas, den der Port im Spiel entpackt
(probe_r34n_g_sld = re15_sld_used_len + re15_sld_atlas_from_chunk, 1:1-Port von FUN_800c47e8).
⛔ NICHT tools/maske/original.atlas() benutzen: dessen Python-Entpacker weicht fuer ROOM1150 Cut 2
in 11978 von 65536 Texeln vom Port-Entpacker ab (Befund G2, Abschnitt 8).

    python re15_port/tools/r34n_g/schrift1150_masken.py <bss-ppm-praefix> <ausgabe-ordner>
        [--room ROOM1150] [--cut 2] [--gruppen 6-11]

<bss-ppm-praefix>: Ausgabe von probe_r34n_g_bss (…_cutNN.ppm).
"""
import argparse
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, "..", "maske"))
import original  # noqa: E402  (nur load_rdt; der Atlas kommt aus der TIM-Datei, s. oben)
import maskenbild  # noqa: E402  (lies_tim)

CD = "re15_port/shared_assets/PSX"


def clut_rgb(c):
    """PSX-15-Bit -> RGB8 (Kanal << 3, wie der Port-Atlas-Lader bg_pc.c)."""
    r = (c & 0x1F) << 3
    g = ((c >> 5) & 0x1F) << 3
    b = ((c >> 10) & 0x1F) << 3
    return r, g, b


def records(rdt, cam, cut):
    """Wie original.artist_rects, zusaetzlich die rohe Record-Adresse (Datei-Offset)."""
    po = struct.unpack_from("<I", rdt, cam + cut * 32 + 0x1C)[0]
    gc, mc = struct.unpack_from("<HH", rdt, po)
    if gc in (0, 0xFFFF) or mc == 0:
        return po, []
    p = po + 4
    gn, gd = [], []
    for _ in range(gc):
        n_, base, dx, dy = struct.unpack_from("<HHhh", rdt, p)
        gn.append(n_); gd.append((dx, dy, base)); p += 8
    out, gi, used = [], 0, 0
    for _ in range(sum(gn)):
        rec_off = p
        sx, sy, dxl, dyl = rdt[p], rdt[p + 1], rdt[p + 2], rdt[p + 3]
        dep, size = struct.unpack_from("<HH", rdt, p + 4); p += 8
        if (size & 0xF000) == 0:
            w, h = struct.unpack_from("<HH", rdt, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay, base = gd[gi]
        used += 1
        X = ((dxl + ax + 0x8000) & 0xFFFF) - 0x8000
        Y = ((dyl + ay + 0x8000) & 0xFFFF) - 0x8000
        out.append(dict(off=rec_off, gruppe=gi + 1, sx=sx, sy=sy, X=X, Y=Y, w=w, h=h,
                        tiefe=dep, size=size))
    return po, out


def lade_atlas(room, cut, pfad=None):
    """(idx 256x256, clut) des Vordergrundatlas; Standard = Alt-Datei == Port-Entpacker."""
    if pfad is None:
        pfad = os.path.join(CD, "BSS", "ROOM%s0" % room[4:7], "PRI%02d.TIM" % cut)
    t = maskenbild.lies_tim(pfad)
    if t is None:
        raise SystemExit("Atlas fehlt: %s" % pfad)
    return t


def blit(img, recs, idx, clut):
    """Masken wie FUN_80039590 (SPRT, ABE aus, Index 0 durchsichtig) auf ein RGB-Bild."""
    o = img.copy()
    for m in recs:
        for yy in range(m["h"]):
            Y = m["Y"] + yy
            if Y < 0 or Y >= 240:
                continue
            for xx in range(m["w"]):
                X = m["X"] + xx
                if X < 0 or X >= 320:
                    continue
                u = (m["sx"] + xx) & 0xFF
                v = (m["sy"] + yy) & 0xFF
                if v >= idx.shape[0] or u >= idx.shape[1]:
                    continue
                i = int(idx[v, u])
                if i == 0:
                    continue
                o[Y, X] = clut_rgb(int(clut[i]))
    return o


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("ppm_praefix")
    ap.add_argument("out")
    ap.add_argument("--room", default="ROOM1150")
    ap.add_argument("--cut", type=int, default=2)
    ap.add_argument("--gruppen", default="6-11")
    ap.add_argument("--atlas", default=None, help="TIM des Vordergrundatlas (Standard BSS/ROOMxxx0/PRIcc.TIM)")
    a = ap.parse_args()
    g0, g1 = [int(x) for x in a.gruppen.split("-")]
    blink = set(range(g0, g1 + 1))
    room = a.room.upper()
    rdt, _ = original.load_rdt(CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    po, recs = records(rdt, cam, a.cut)
    idx, clut = lade_atlas(room, a.cut, a.atlas)
    bg = np.asarray(Image.open("%s_cut%02d.ppm" % (a.ppm_praefix, a.cut)).convert("RGB"), np.uint8)
    os.makedirs(a.out, exist_ok=True)
    print("%s Cut %d: sprite.pri @0x%05X, %d Records, Atlas %dx%d" % (room, a.cut, po, len(recs),
                                                                        idx.shape[1], idx.shape[0]))
    for m in recs:
        if m["gruppe"] in blink:
            sub = idx[m["sy"]:m["sy"] + m["h"], m["sx"]:m["sx"] + m["w"]]
            opak = int((sub != 0).sum())
            print("  Gruppe %2d  Record @0x%05X  src(%3d,%3d) dst(%4d,%4d) %2dx%2d  Tiefe %4d  "
                  "size 0x%04X  opak %d" % (m["gruppe"], m["off"], m["sx"], m["sy"], m["X"], m["Y"],
                                            m["w"], m["h"], m["tiefe"], m["size"], opak))
    an = blit(bg, recs, idx, clut)
    aus = blit(bg, [m for m in recs if m["gruppe"] not in blink], idx, clut)
    diff = np.abs(an.astype(int) - aus.astype(int)).sum(axis=2)
    ys, xs = np.nonzero(diff)
    if len(xs):
        print("  AN/AUS unterscheiden sich in %d Pixeln, Rechteck x%d..%d y%d..%d, max |d| %d"
              % (len(xs), xs.min(), xs.max(), ys.min(), ys.max(), diff.max()))
        # Helligkeit im Unterschiedsbereich
        box = (slice(ys.min(), ys.max() + 1), slice(xs.min(), xs.max() + 1))
        for name, im in (("BSS", bg), ("AN", an), ("AUS", aus)):
            sub = im[box].astype(float)
            print("    %-3s mittel R %.1f G %.1f B %.1f" % (name, sub[..., 0].mean(), sub[..., 1].mean(),
                                                           sub[..., 2].mean()))
    Image.fromarray(an).save(os.path.join(a.out, "%s_cut%02d_AN.png" % (room, a.cut)))
    Image.fromarray(aus).save(os.path.join(a.out, "%s_cut%02d_AUS.png" % (room, a.cut)))
    # Vergroesserter Vergleich des Bereichs
    if len(xs):
        x0, x1 = max(0, xs.min() - 12), min(320, xs.max() + 13)
        y0, y1 = max(0, ys.min() - 12), min(240, ys.max() + 13)
        Z = 4
        tiles = []
        for name, im in (("BSS (Hintergrund)", bg), ("AN: Masken Gruppe %d-%d gezeichnet" % (g0, g1), an),
                         ("AUS: Gruppen %d-%d per 0x45 auf 0" % (g0, g1), aus)):
            t = Image.fromarray(im[y0:y1, x0:x1]).resize(((x1 - x0) * Z, (y1 - y0) * Z), Image.NEAREST)
            c = Image.new("RGB", (t.width, t.height + 14), (30, 30, 30))
            c.paste(t, (0, 14)); ImageDraw.Draw(c).text((2, 1), name, fill=(255, 255, 0))
            tiles.append(c)
        W = sum(t.width for t in tiles) + 8 * (len(tiles) - 1)
        s = Image.new("RGB", (W, tiles[0].height), (30, 30, 30))
        x = 0
        for t in tiles:
            s.paste(t, (x, 0)); x += t.width + 8
        p = os.path.join(a.out, "%s_cut%02d_vergleich.png" % (room, a.cut))
        s.save(p)
        print("  Vergleichsbild:", p, "(Ausschnitt x%d..%d y%d..%d)" % (x0, x1 - 1, y0, y1 - 1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
