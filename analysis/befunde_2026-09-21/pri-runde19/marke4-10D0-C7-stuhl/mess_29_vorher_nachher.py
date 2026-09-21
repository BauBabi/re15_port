# -*- coding: utf-8 -*-
"""Messung 29: Vorher/Nachher am BILD, mit der Urteilsregel der Engine.

Regel: eine Maske gewinnt gegen die Figur, wenn Maskentiefe < (1023*vz)>>16
(geometrie.verdeckt; ZSF3 = 341 @0x80066c70/74, Figur-OT = otz>>4 @0x8002565c).
vz der Figur je Bildzeile = die senkrechte Weltlinie durch ihren Standort
(geometrie.profil_spalte) - dieselbe Rechnung, die die Abnahme benutzt.

SELBSTPRUEFUNG: mit der ALTEN Maske muss die Nachbildung den echten Screenshot an
den gedeckten Punkten reproduzieren. Erst wenn das stimmt, ist das Nachher-Bild
aussagekraeftig. Punkte, die die neue Maske NICHT mehr deckt, kann die Nachbildung
nicht erfinden (dort stand im Screenshot Hintergrund, nicht die Figur) - sie werden
gelb markiert, nicht geraten.
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import geom                                     # noqa: E402
import geometrie                                # noqa: E402
import maskenbild                               # noqa: E402

OUT = "analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/"
ALT = "C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/m4"
NEU = "re15_port/shared_assets/PSX/MASKS"
BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
REN = np.asarray(Image.open(
    "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp"
).convert("RGB").resize((320, 240), Image.NEAREST), int)

import struct                                   # noqa: E402
rdt, _ = geom.load_rdt(os.path.join("re15_port", "shared_assets", "PSX"), "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
PX, PZ = 418, 26497


def deck_tief(ordner):
    blob = open(os.path.join(ordner, "ROOM10D0.MSK"), "rb").read()
    ms = maskenbild.masken(blob, 7) or []
    idx = maskenbild.lies_tim(os.path.join(ordner, "ROOM10D0_PRI07.TIM"))[0]
    return abnahme.deckung_und_tiefe(ms, idx) + (len(ms),)


dA, tA, nA = deck_tief(ALT)
dB, tB, nB = deck_tief(NEU)
print("ALT: %d Rechtecke, Deckung %d | NEU: %d Rechtecke, Deckung %d" % (nA, dA.sum(), nB, dB.sum()))

ys = np.arange(240)
vz, ok = geometrie.profil_spalte(R, t, H, PX, PZ, ys)
vz = np.where(ok, vz, np.nan)
print("vz der Figur: Zeile 130 %.0f, 155 %.0f, 179 %.0f (befund.log: 5692/5883/6073)"
      % (vz[130], vz[155], vz[179]))
gewA = dA & geometrie.verdeckt(tA, vz[:, None])
gewB = dB & geometrie.verdeckt(tB, vz[:, None])
print("Maske gewinnt: ALT %d Punkte, NEU %d Punkte" % (gewA.sum(), gewB.sum()))

# SELBSTPRUEFUNG: an den Punkten, wo die ALTE Maske gewinnt, muss der Screenshot
# den Hintergrund zeigen.
sel = gewA
abw = (np.abs(REN - BG).sum(2) > 30) & sel
print("SELBSTPRUEFUNG: %d Punkte, an denen die ALTE Maske gewinnt; davon weichen %d "
      "vom Hintergrund ab (%.2f %%)" % (sel.sum(), abw.sum(), 100.0 * abw.sum() / sel.sum()))

# Nachher-Bild
neu = REN.astype(np.uint8).copy()
dazu = gewB & ~gewA          # jetzt zusaetzlich gedeckt -> Hintergrund einsetzen
weg = gewA & ~gewB           # nicht mehr gedeckt -> Figur waere dort, Farbe unbekannt
neu[dazu] = BG[dazu]
print("neu gedeckt (Hintergrund eingesetzt): %d | nicht mehr gedeckt (gelb markiert): %d"
      % (dazu.sum(), weg.sum()))
if weg.sum():
    wy, wx = np.nonzero(weg)
    print("   nicht mehr gedeckt: x%d..%d y%d..%d, Hintergrund-Helligkeit Median %d"
          % (wx.min(), wx.max(), wy.min(), wy.max(), np.median(BG[wy, wx].sum(1))))
mark = neu.copy()
mark[weg] = [255, 230, 0]

box = (46, 122, 82, 166)
S = 20
def cr(arr):
    return Image.fromarray(arr.astype(np.uint8)).crop(box).resize(
        ((box[2] - box[0]) * S, (box[3] - box[1]) * S), Image.NEAREST)


ims = [cr(BG), cr(REN), cr(mark)]
w, h = ims[0].size
comb = Image.new("RGB", (w * 3 + 12, h), (40, 40, 40))
for i, im in enumerate(ims):
    comb.paste(im, (i * (w + 6), 0))
comb.save(OUT + "50_vorher_nachher.png")
print("50_vorher_nachher.png: Hintergrund | Render VORHER | Nachbildung NACHHER "
      "(gelb = Punkte, die die neue Maske nicht mehr zeichnet)")

# und der ganze Stuhlbereich
box2 = (0, 112, 104, 240)
S2 = 9
def cr2(arr):
    return Image.fromarray(arr.astype(np.uint8)).crop(box2).resize(
        ((box2[2] - box2[0]) * S2, (box2[3] - box2[1]) * S2), Image.NEAREST)
ims = [cr2(REN), cr2(mark)]
w, h = ims[0].size
comb = Image.new("RGB", (w * 2 + 6, h), (40, 40, 40))
for i, im in enumerate(ims):
    comb.paste(im, (i * (w + 6), 0))
comb.save(OUT + "51_vorher_nachher_gross.png")
print("51_vorher_nachher_gross.png")
