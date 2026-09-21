# -*- coding: utf-8 -*-
"""Messung 32: Wirkung HINTER dem Kasten, am Standplatz mit der groessten Zunahme
(1700, 27950). Kein Screenshot vorhanden - deshalb wird der Koerperkasten des Spielers
als Flaeche gezeichnet und gezeigt, welche seiner Punkte die Maske ALT/NEU verdeckt.
Das ist eine Deckungskarte, kein Renderbild, und wird so benannt.
"""
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import geom                                     # noqa: E402
import geometrie                                # noqa: E402
import maskenbild                               # noqa: E402

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
ALT = "C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/m4"
NEU = "re15_port/shared_assets/PSX/MASKS"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), np.uint8)
rdt, _ = geom.load_rdt(os.path.join("re15_port", "shared_assets", "PSX"), "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)


def deck_tief(ordner):
    blob = open(os.path.join(ordner, "ROOM10D0.MSK"), "rb").read()
    ms = maskenbild.masken(blob, 7) or []
    idx = maskenbild.lies_tim(os.path.join(ordner, "ROOM10D0_PRI07.TIM"))[0]
    return abnahme.deckung_und_tiefe(ms, idx)


dA, tA = deck_tief(ALT)
dB, tB = deck_tief(NEU)

WX, WZ = 1700, 27950
pf = abnahme.proj(R, t, H, WX, 0, WZ)
pk = abnahme.proj(R, t, H, WX, -abnahme.KOPF, WZ)
fsx, fsy, fvz = pf
ksx, ksy, kvz = pk
hw = abnahme.HALB * H / fvz
x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
print("Standplatz (%d,%d): Fuss-vz %.0f (Tiefe %d), Kasten-Standlinie 7337 (Tiefe 114)"
      % (WX, WZ, fvz, int(1023 * fvz / 65536.0)))
print("Koerperkasten x%d..%d y%d..%d (%d Punkte)" % (x0, x1, y0, y1, (x1 - x0) * (y1 - y0)))

ys = np.arange(y0, y1)
vzs, ok = geometrie.profil_spalte(R, t, H, WX, WZ, ys)
vzs = np.where(ok, vzs, fvz)
K = np.zeros((240, 320), bool)
K[y0:y1, x0:x1] = True
verdA = np.zeros((240, 320), bool)
verdB = np.zeros((240, 320), bool)
verdA[y0:y1, x0:x1] = dA[y0:y1, x0:x1] & geometrie.verdeckt(tA[y0:y1, x0:x1], vzs[:, None])
verdB[y0:y1, x0:x1] = dB[y0:y1, x0:x1] & geometrie.verdeckt(tB[y0:y1, x0:x1], vzs[:, None])
print("verdeckt ALT %d, NEU %d von %d Punkten des Kastens"
      % (verdA.sum(), verdB.sum(), K.sum()))


def karte(verd):
    a = (BG * 0.55).astype(np.uint8)
    a[K] = (a[K].astype(int) * 0.4 + np.array([60, 90, 190]) * 0.6).astype(np.uint8)
    a[verd] = (a[verd].astype(int) * 0.25 + np.array([255, 0, 190]) * 0.75).astype(np.uint8)
    im = Image.fromarray(a).resize((320 * 3, 240 * 3), Image.NEAREST)
    d = ImageDraw.Draw(im)
    d.rectangle([x0 * 3, y0 * 3, x1 * 3 - 1, y1 * 3 - 1], outline=(255, 255, 0))
    return im


ims = [karte(verdA), karte(verdB)]
w, h = ims[0].size
comb = Image.new("RGB", (w * 2 + 6, h), (40, 40, 40))
for i, im in enumerate(ims):
    comb.paste(im, (i * (w + 6), 0))
comb.save(OUT + "52_dahinter_deckungskarte.png")
print("52_dahinter_deckungskarte.png: blau = Koerperkasten des Spielers, magenta = von "
      "der Maske verdeckt. Links ALT, rechts NEU. DECKUNGSKARTE, kein Renderbild.")
