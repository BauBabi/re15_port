"""Prueflatt: die getroffene Auswahl mit Nummern UEBER dem Bild zeigen.

⛔ DAS IST DIE EINZIGE PRUEFUNG, DIE NACHWEISLICH TRAEGT. Drei automatische Versuche
sind an ihrer eigenen Kontrolle gescheitert (Offline-Zaehlung, In-Game-Zaehlung,
Farbaehnlichkeit zum Boden) — jeder bewertete die ORIGINAL-Masken der Kuenstler
schlechter als meine fehlerhaften. Gefunden wurde der Fehler in ROOM1140 Cut 3
(11 von 20 Flaechen auf dem Teppich) allein dadurch, dass die Auswahl mit ihren
Nummern ueber dem Bild lag und man sie ansehen konnte.

Links das reine Bild, rechts die Markierung mit Nummern — so ist auf einen Blick zu
sehen, WELCHE Nummer auf Teppich statt auf Moebeln liegt.

Aufruf:
    python re15_port/tools/maske/pruefblatt.py ROOM1140 0
    python re15_port/tools/maske/pruefblatt.py ROOM1140          (alle Cuts)
"""
import argparse
import json
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from geom import load_bg
from blatt import aufhellen

AUSWAHL = "analysis/esp_masken_2026-09-03/auswahl.json"


def blatt(room, cut, ids, ppm, blattdir, out):
    rid = int(room[4:], 16)
    seg = np.load(os.path.join(blattdir, "%s_%02d_seg.npy" % (room, cut)))
    bg = load_bg(ppm, rid, cut)
    if bg is None:
        return None
    m = np.isin(seg, ids)
    base = aufhellen(bg)
    ov = base.copy()
    ov[m] = ov[m] * 0.35 + np.array([1.0, 0.15, 0.8]) * 0.65
    pil = Image.fromarray((ov * 255).astype(np.uint8)).resize((960, 720), Image.NEAREST)
    d = ImageDraw.Draw(pil)
    for s in ids:
        ys, xs = np.nonzero(seg == s)
        if len(ys) < 25:
            continue
        cx, cy = int(xs.mean()) * 3, int(ys.mean()) * 3
        d.rectangle([cx - 11, cy - 9, cx + 11, cy + 9], fill=(0, 0, 0))
        d.text((cx - 7, cy - 6), str(s), fill=(0, 255, 0))
    sheet = Image.new("RGB", (960 * 2 + 8, 720), (30, 30, 30))
    sheet.paste(Image.fromarray((base * 255).astype(np.uint8)).resize((960, 720), Image.LANCZOS), (0, 0))
    sheet.paste(pil, (968, 0))
    os.makedirs(out, exist_ok=True)
    p = os.path.join(out, "%s_%02d.png" % (room, cut))
    sheet.save(p)
    return p, 100.0 * m.mean()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("cut", type=int, nargs="?")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--blatt")
    ap.add_argument("--out", default="build/pruefblaetter")
    a = ap.parse_args()
    room = a.room.upper()
    sel = json.load(open(AUSWAHL, encoding="utf-8")).get(room, {})
    blattdir = a.blatt or ("build/blaetter_%s" % room[4:].lower())
    cuts = [str(a.cut)] if a.cut is not None else sorted(sel, key=int)
    for c in cuts:
        if c not in sel:
            print("  Cut %s: keine Auswahl hinterlegt" % c); continue
        r = blatt(room, int(c), sel[c], a.ppm, blattdir, a.out)
        if r:
            print("  %s  %.1f %% markiert" % (r[0], r[1]))


if __name__ == "__main__":
    main()
