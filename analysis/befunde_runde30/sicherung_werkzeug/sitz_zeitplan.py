#!/usr/bin/env python3
"""SITZ-KANDIDATEN ueber den ZEITPLAN der Szene sub04 (Fortsetzungs-Agent, berichtigt).

Berichtigung gegenueber sitz_kandidaten.py / sitz_analyse.py des Vorgaengers:
  DECKEL_WEG war dort 240 ("For 24"). Der For-Record @Datei 0x0FC0 ist aber
  `0d 00 18 00 0f 00` = Blocklaenge 0x18, ZAEHLER 0x0F = 15 (For-Handler @0x8003f564
  `lh t1,2(t0)` Laenge, @0x8003f568 `lhu a1,4(t0)` Zaehler). 15 x 10 = 150.
  GEMESSEN in der Engine: build/r30_sicherung/probe_fahrt.txt (Deckel1 z=150, Deckel2 z=-150).

Zeitplan (Bild = Engine-Tick nach scd_event_fire(4); im Spiellauf F = Bild + 91), gemessen
mit probe_r30_sicherung_fahrt:
  Bild   5        Pos_set @0x0FB4 -> y=-305, Cut_chg 4 @0x0FB2
  Bild  10..24    Deckel +-10 je Bild (For 15 @0x0FC0)          -> +-150 ab Bild 24
  Bild  55..145   Plattform -10 je Bild (For 91 @0x0FF6)        -> -1215
  Bild 134        y=-1105 <= -1100 = Modal-Schranke des Bestands (sicherung_1150.c:28)

Je Kandidat: sichtbare Pixel (WAHRE Verdeckung, Z-Puffer, 320x240) fuer jedes 3. Bild von
Bild 5 bis 133 — also genau die Spanne, in der der Spieler die Szene sieht, bevor das Modal
sie ueberdeckt.

    python analysis/befunde_runde30/sicherung_werkzeug/sitz_zeitplan.py
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import (REPO, cuts, md1_lesen, prop_rot, props, rdt_laden,  # noqa: E402
                     tim_lesen, inc_bytes, verketten, view_bauen, view_x_welt)
from raster import Bild, flaechen, tim_als_rgba  # noqa: E402

ZIEL = os.path.join(REPO, "build", "r30_sicherung")
PLATTFORM_XZ = (-20700, -17460)      # Pos_set @0x0FB4 / Obj_model_set @0x0E00
PLATTFORM_ROT = (0, 2048, 0)         # Obj_model_set @0x0E00
F = 3

KANDIDATEN = [
    ("BESTAND Deck laengs",     (-628, -927, 784),   (0, 0, 0)),
    ("K1 Kuppelfach quer",      (-280, -1062, 1260), (0, 1024, 0)),
    ("K1L Kuppelfach laengs",   (-280, -1062, 1260), (0, 0, 0)),
    ("K2a Regalfach A quer",    (-150, -116, 478),   (0, 1024, 0)),
    ("K2b Regalfach B quer",    (-150, -117, 1332),  (0, 1024, 0)),
]


def lage(bild):
    """-> (plattform_y, deckel_weg) im Engine-Bild `bild` (Messung probe_fahrt.txt)."""
    d = 0 if bild < 10 else min(150, 10 * (bild - 9))
    y = -305 if bild < 55 else max(-1215, -305 - 10 * (bild - 54))
    return y, d


def zeichne(b, view, mesh, tex, wrot, wpos, kennung):
    cr, ct = view_x_welt(view, wrot, wpos)
    for (p, uv, po) in flaechen(mesh):
        q = []
        for v in p:
            vx = ((v[0] * cr[0] + v[1] * cr[1] + v[2] * cr[2]) >> 12) + ct[0]
            vy = ((v[0] * cr[3] + v[1] * cr[4] + v[2] * cr[5]) >> 12) + ct[1]
            vz = ((v[0] * cr[6] + v[1] * cr[7] + v[2] * cr[8]) >> 12) + ct[2]
            if vz < 64:
                q = None
                break
            q.append(((160 + view["H"] * vx / vz) * F, (120 + view["H"] * vy / vz) * F, vz))
        if q:
            b.dreieck(q, uv, tex, kennung)


def main():
    rdt = rdt_laden("ROOM1150.RDT")
    pr = props(rdt)
    m = [md1_lesen(p["md1"])["meshes"][0] for p in pr]
    tex = [tim_als_rgba(tim_lesen(p["tim"])) for p in pr]
    zord = os.path.join(ZIEL, "sicherung_zordnung.md1")
    sm = md1_lesen(open(zord, "rb").read())["meshes"][0]
    stex = tim_als_rgba(tim_lesen(inc_bytes("re15_sicherung_tim")))
    view = view_bauen(cuts(rdt)[4])
    prot = prop_rot(*PLATTFORM_ROT)
    bilder = list(range(5, 134, 3))
    print("Bilder %d..%d im Schritt 3 = %d Stichbilder; Modal des Bestands in Bild 134"
          % (bilder[0], bilder[-1], len(bilder)))
    print("%-24s %-22s %-14s | sichtbar in | groesste | kleinste>0 | Mittel  | Pixel x Bilder"
          % ("Kandidat", "Sitz", "rot"))
    blatt = []
    for name, sitz, rot in KANDIDATEN:
        werte = []
        for n in bilder:
            py, dz = lage(n)
            ppos = [PLATTFORM_XZ[0], py, PLATTFORM_XZ[1]]
            b = Bild(320 * F, 240 * F)
            zeichne(b, view, m[0], tex[0], prot, ppos, 0)
            for k, s in ((1, dz), (2, -dz)):
                r, t = verketten(prot, ppos, prop_rot(0, 0, 0), [0, 0, s])
                zeichne(b, view, m[k], tex[k], r, t, k)
            r, t = verketten(prot, ppos, prop_rot(*rot), list(sitz))
            zeichne(b, view, sm, stex, r, t, 4)
            k = int((b.wer == 4).sum()) / float(F * F)
            werte.append(k)
            if n in (26, 92, 131):
                img = Image.fromarray(b.rgb).resize((320, 240), Image.BOX)
                ImageDraw.Draw(img).text((3, 3), "%s  Bild %d  %.0f px" % (name, n, k),
                                         fill=(255, 255, 0))
                blatt.append(img)
        w = np.array(werte)
        da = w > 0.5
        print("%-24s %-22s %-14s | %3d von %3d | %7.1f  | %7.1f    | %6.1f  | %8.0f"
              % (name, str(sitz), str(rot), int(da.sum()), len(w), w.max(),
                 w[da].min() if da.any() else 0, w.mean(), w.sum() * 3))
        print("      Verlauf (px je Stichbild): " + " ".join("%d" % round(x) for x in w))
    bl = Image.new("RGB", (330 * 3, 250 * len(KANDIDATEN)), (255, 0, 255))
    for i, im in enumerate(blatt):
        bl.paste(im, ((i % 3) * 330, (i // 3) * 250))
    bl.save(os.path.join(ZIEL, "sitz_zeitplan.png"))
    print("geschrieben: build/r30_sicherung/sitz_zeitplan.png")


if __name__ == "__main__":
    main()
