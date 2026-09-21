# -*- coding: utf-8 -*-
"""Bildbelege fuer das Dossier."""
import os
import sys
import numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from zensus import container
from paare import lies_tim_bytes, rgb
from bloecke import deckung
from bloecke2 import bloecke

MASKS = "re15_port/shared_assets/PSX/MASKS"
OUT = "analysis/befunde_2026-09-21/10f0-doppel-masken"
MARKE = os.path.join("C:\\", "workspace", "git", "reAi_v2", "re15_port", "build",
                     "platform", "pc", "befund_10F0_F335_marke1.bmp")
os.makedirs(OUT, exist_ok=True)
S = 3   # Vergroesserung


def lade(room, cut):
    res, _ = container(os.path.join(MASKS, "%s.MSK" % room))
    masks = res[cut][0]
    idx, clut = lies_tim_bytes(open(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)), "rb").read())
    bl = bloecke(masks)
    regs = []
    for (s, e, k, ax, ay) in bl:
        d = np.zeros((240, 320), bool)
        for m in masks[s:e]:
            d |= deckung(idx, m)
        regs.append(d)
    return masks, idx, clut, bl, regs


def grund():
    im = Image.open(MARKE).convert("RGB").resize((320, 240), Image.LANCZOS)
    a = np.asarray(im, np.int32)
    return (a // 3 + 50).clip(0, 255).astype(np.uint8)      # abgedunkelt


FARBEN = [(255, 90, 90), (90, 200, 255), (255, 210, 60), (150, 255, 120),
          (255, 140, 255), (120, 160, 255), (255, 170, 80), (180, 255, 220),
          (255, 120, 180), (200, 200, 120), (120, 255, 200)]


def platte(basis, regs, welche, titel):
    a = basis.copy()
    for n, i in enumerate(welche):
        c = np.array(FARBEN[i % len(FARBEN)], np.int32)
        m = regs[i]
        a[m] = ((a[m].astype(np.int32) * 0.25) + c * 0.75).astype(np.uint8)
    im = Image.fromarray(a).resize((320 * S, 240 * S), Image.NEAREST)
    d = ImageDraw.Draw(im)
    d.rectangle([0, 0, 320 * S - 1, 16], fill=(0, 0, 0))
    d.text((4, 4), titel, fill=(255, 255, 255))
    return im


def nebeneinander(bilder, pfad):
    w = sum(b.width for b in bilder) + 8 * (len(bilder) - 1)
    h = max(b.height for b in bilder)
    im = Image.new("RGB", (w, h), (20, 20, 20))
    x = 0
    for b in bilder:
        im.paste(b, (x, 0)); x += b.width + 8
    im.save(pfad)
    print("  ->", pfad, im.size)


def objektkarte(room, cut, doppel, pfad):
    masks, idx, clut, bl, regs = lade(room, cut)
    basis = grund() if cut == 4 else (np.zeros((240, 320, 3), np.uint8) + 40)
    behalten = [i for i in range(len(regs)) if i not in doppel]
    A = platte(basis, regs, behalten, "%s C%d  %d eigenstaendige Objekte, %d Masken"
               % (room, cut, len(behalten), sum(bl[i][1] - bl[i][0] for i in behalten)))
    B = platte(basis, regs, sorted(doppel), "dieselben Bildpunkte NOCH EINMAL: %d Doppel-Objekte, %d Masken"
               % (len(doppel), sum(bl[i][1] - bl[i][0] for i in doppel)))
    nebeneinander([A, B], pfad)


def atlaskarte(room, cut, doppel, pfad):
    masks, idx, clut, bl, regs = lade(room, cut)
    pal = rgb(clut)
    bild = pal[idx.astype(np.int32)].astype(np.uint8)
    bild[idx == 0] = (25, 25, 30)
    im = Image.fromarray(bild).resize((256 * 2, 256 * 2), Image.NEAREST)
    d = ImageDraw.Draw(im)
    for o, (s, e, k, ax, ay) in enumerate(bl):
        f = (255, 60, 60) if o in doppel else (70, 230, 120)
        for m in masks[s:e]:
            d.rectangle([m[0] * 2, m[1] * 2, (m[0] + m[5]) * 2 - 1, (m[1] + m[6]) * 2 - 1],
                        outline=f)
    d.rectangle([0, 0, im.width - 1, 16], fill=(0, 0, 0))
    d.text((4, 4), "%s C%d Atlas 256x256 — rot = Kacheln der Doppel-Objekte" % (room, cut),
           fill=(255, 255, 255))
    im.save(pfad)
    print("  ->", pfad, im.size)


def leonkasten(pfad):
    masks, idx, clut, bl, regs = lade("ROOM10F0", 4)
    im = Image.open(MARKE).convert("RGB")     # 960x720 = 3x
    d = ImageDraw.Draw(im)
    # Leons Kasten an der Marke F335
    x0, x1, y0, y1 = 99, 121, 81, 118
    for o, (s, e, k, ax, ay) in enumerate(bl):
        if o not in (0, 7):
            continue
        f = (255, 60, 60) if o == 7 else (90, 200, 255)
        for m in masks[s:e]:
            d.rectangle([m[2] * 3, m[3] * 3, (m[2] + m[5]) * 3 - 1, (m[3] + m[6]) * 3 - 1],
                        outline=f)
            d.text((m[2] * 3 + 2, m[3] * 3 + 2), str(m[4]), fill=f)
    d.rectangle([x0 * 3, y0 * 3, x1 * 3, y1 * 3], outline=(255, 255, 0), width=2)
    im = im.crop((40 * 3, 45 * 3, 165 * 3, 145 * 3)).resize((125 * 6, 100 * 6), Image.NEAREST)
    d = ImageDraw.Draw(im)
    d.rectangle([0, 0, im.width - 1, 18], fill=(0, 0, 0))
    d.text((4, 4), "Marke F335: gelb = Leons Kasten x99..121 y81..118;  blau = Objekt 0 "
                   "(Lasso, 9 Kacheln);  rot = Objekt 7 (Quader-Doppel, 4 Kacheln); Zahl = Tiefe",
           fill=(255, 255, 255))
    im.save(pfad)
    print("  ->", pfad, im.size)


if __name__ == "__main__":
    objektkarte("ROOM10F0", 4, {7, 8, 9}, os.path.join(OUT, "c4_objekte.png"))
    objektkarte("ROOM10F0", 5, {4, 5, 6}, os.path.join(OUT, "c5_objekte.png"))
    atlaskarte("ROOM10F0", 4, {7, 8, 9}, os.path.join(OUT, "c4_atlas.png"))
    leonkasten(os.path.join(OUT, "c4_leon_f335.png"))
