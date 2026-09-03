"""Auswahlblatt: einen Cut als beschriftete Superpixel-Karte rendern.

WOZU: Der Editor (editor.py) verlangt eine Maus. Dieselbe Auswahl laesst sich aber auch
treffen, indem man das Bild ANSIEHT und die Nummern der Vordergrund-Flaechen nennt —
und genau das ist der Teil, an dem die Automatik scheiterte: ein gelerntes Modell
erkennt in einem unbekannten Raum keinen Vordergrund (gemessen 20-26 % Praezision), ein
Blick darauf schon. Die Zerlegung in Flaechen macht weiterhin das Bild, nicht das Urteil:
die Raender folgen den echten Objektkanten.

FEINHEIT: 90 Flaechen waren ZU GROB (Nutzer-Befund 2026-09-03, error01.png): in
ROOM1140 Cut 2 lagen Wandbild, Wand, Sofa und Stativ in EINER Flaeche. Sie liess sich
also nur ganz oder gar nicht waehlen — gewaehlt hiess: die Wand verdeckt Leon.
Standard ist deshalb 220. Damit die Nummern dabei lesbar bleiben, wird das Blatt in
Kacheln ausgegeben (--kachel x0,y0,x1,y1), nicht als ein Bild.
Gemessen gegen die Kuenstler-Silhouetten (build/kalib.py, ROOM1150 Cut 1/2/3):
    seg  90  Praezision 67.2 %  Ausbeute 89.0 %
    seg 220  Praezision 70.6 %  Ausbeute 89.8 %
    seg 320  Praezision 70.8 %  Ausbeute 91.6 %   (Nummern nicht mehr handhabbar)

Aufruf:
    python re15_port/tools/maske/blatt.py ROOM1140 0 --out build/blaetter
    python re15_port/tools/maske/blatt.py ROOM1140 2 --kachel 160,0,320,240 --zoom 6
"""
import argparse
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw
from skimage.segmentation import slic, mark_boundaries

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from geom import load_bg, load_rdt, original_has_masks

ZOOM = 3          # per --zoom ueberschreibbar


def segment(bg, n_seg):
    return slic(bg, n_segments=n_seg, compactness=14, start_label=1, channel_axis=2)


def aufhellen(bg, gamma=0.55):
    """Die Raeume sind dunkel; ohne Aufhellung ist auf dem Blatt nichts zu erkennen.
    Betrifft NUR die Anzeige — die Maskenpixel kommen spaeter aus dem Originalbild."""
    x = bg.astype(np.float64) / 255.0
    return np.power(np.clip(x, 0, 1), gamma)


def sheet(bg, seg, sel=None, zoom=None, kachel=(0, 0, 320, 240)):
    """-> PIL-Bild, ZWEI Felder nebeneinander.

    Links das reine (aufgehellte) Bild — dort wird die Szene beurteilt.
    Rechts dasselbe mit duennen Flaechengrenzen und Nummern — dort wird abgelesen.
    Beides in einem Bild, weil ein einzelnes Feld entweder lesbar ODER beurteilbar ist:
    die Grenzen ueberdecken genau die Kanten, an denen man Vordergrund erkennt.
    """
    base = aufhellen(bg)
    if sel is not None and sel.any():
        base = base.copy()
        base[sel] = base[sel] * 0.45 + np.array([1.0, 0.15, 0.8]) * 0.55
    Z = zoom or ZOOM
    x0, y0, x1, y1 = kachel
    W, H = x1 - x0, y1 - y0
    links = Image.fromarray((base * 255).astype(np.uint8)).crop(kachel)
    img = (mark_boundaries(base, seg, color=(1, 1, 0), mode="inner") * 255).astype(np.uint8)
    rechts = Image.fromarray(img).crop(kachel)
    pil = Image.new("RGB", (W * Z * 2 + 8, H * Z), (30, 30, 30))
    pil.paste(links.resize((W * Z, H * Z), Image.LANCZOS), (0, 0))
    pil.paste(rechts.resize((W * Z, H * Z), Image.NEAREST), (W * Z + 8, 0))
    d = ImageDraw.Draw(pil)
    OFF = W * Z + 8
    for s in np.unique(seg):
        ys, xs = np.nonzero(seg == s)
        if len(ys) < 12:                      # Winzflaechen nicht beschriften
            continue
        mx, my = int(xs.mean()), int(ys.mean())
        if not (x0 <= mx < x1 and y0 <= my < y1):
            continue
        cx, cy = (mx - x0) * Z + OFF, (my - y0) * Z
        txt = str(int(s))
        # Kontrastkasten, sonst verschwindet die Zahl im Bild
        d.rectangle([cx - 9, cy - 8, cx + 9 + 6 * (len(txt) - 1), cy + 8], fill=(0, 0, 0))
        d.text((cx - 6, cy - 6), txt, fill=(255, 255, 0))
    return pil


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("cut", type=int, nargs="?", default=None)
    ap.add_argument("--cd", default="re15_port/shared_assets/PSX")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--out", default="build/blaetter")
    ap.add_argument("--segments", type=int, default=220)
    ap.add_argument("--zoom", type=int, default=3)
    ap.add_argument("--kachel", default="", help="Ausschnitt x0,y0,x1,y1 (320x240)")
    a = ap.parse_args()

    room = a.room.upper()
    rid = int(room[4:], 16)
    rdt, _ = load_rdt(a.cd, room)
    if not rdt:
        raise SystemExit("RDT fehlt: %s" % room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    cuts = [a.cut] if a.cut is not None else \
        [c for c in range(rdt[1]) if not original_has_masks(rdt, cam, c)]

    os.makedirs(a.out, exist_ok=True)
    for c in cuts:
        bg = load_bg(a.ppm, rid, c)
        if bg is None:
            print("  %s Cut %d: Hintergrund fehlt" % (room, c))
            continue
        seg = segment(bg, a.segments)
        np.save(os.path.join(a.out, "%s_%02d_s%d_seg.npy" % (room, c, a.segments)), seg)
        kach = tuple(int(v) for v in a.kachel.split(",")) if a.kachel else (0, 0, 320, 240)
        p = os.path.join(a.out, "%s_%02d_s%d_%s.png"
                         % (room, c, a.segments, a.kachel.replace(",", "_") or "voll"))
        sheet(bg, seg, zoom=a.zoom, kachel=kach).save(p)
        print("  %s -> %d Flaechen" % (p, len(np.unique(seg))))


if __name__ == "__main__":
    main()
