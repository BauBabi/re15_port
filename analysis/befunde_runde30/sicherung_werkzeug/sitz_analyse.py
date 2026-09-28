#!/usr/bin/env python3
"""Wo SITZT die Sicherung auf dem Hebetisch, und wo ist das FACH, das die Deckel freigeben?

Misst in PLATTFORM-Koordinaten (MD1 von ROOM1150 Prop 0, die beiden Deckelhaelften Prop 1/2
haengen ueber pc[5]=0xC0 an derselben Matrix) und rechnet die Ansicht aus Cut 4 mit einem
Z-Puffer nach.

Ausgaben unter build/r30_sicherung/:
  draufsicht.png       orthographisch von oben, Deckel ZU / Deckel OFFEN (+-240 in z)
  seitenansicht.png    orthographisch von -X (= Blickrichtung von Cut 4)
  cut4_zpuffer_*.png   Cut-4-Perspektive mit wahrer Verdeckung, Sicherung magenta markiert

    python analysis/befunde_runde30/sicherung_werkzeug/sitz_analyse.py
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import (REPO, cuts, inc_bytes, md1_lesen, projiziere, prop_rot, props,  # noqa: E402
                     rdt_laden, tim_lesen, verketten, view_bauen, view_x_welt)
from raster import Bild, flaechen, tim_als_rgba  # noqa: E402

ZIEL = os.path.join(REPO, "build", "r30_sicherung")

# Bestand des Ports (include/re15_sicherung.h:57-59)
SITZ = (-628, -927, 784)
# Plattform: Obj_model_set @0x0E00 rot=(0,2048,0); Pos_set @0x0FB4 -> (-20700,-305,-17460)
PLATTFORM_XZ = (-20700, -17460)
PLATTFORM_ROT = (0, 2048, 0)
# Deckel-Oeffnung: For 24 @0x0FC0, je Durchlauf Speed_set achse 2 = +10 (Prop 1) / -10 (Prop 2)
DECKEL_WEG = 24 * 10


def bbox(punkte):
    a = np.array(punkte)
    return a.min(0), a.max(0)


def main():
    os.makedirs(ZIEL, exist_ok=True)
    rdt = rdt_laden("ROOM1150.RDT")
    pr = props(rdt)
    m = [md1_lesen(p["md1"])["meshes"][0] for p in pr]
    tex = [tim_als_rgba(tim_lesen(p["tim"])) for p in pr]
    sm = md1_lesen(inc_bytes("re15_sicherung_md1"))["meshes"][0]
    stex = tim_als_rgba(tim_lesen(inc_bytes("re15_sicherung_tim")))

    print("== 1. Masse in Plattform-Koordinaten (+Y = unten) ==")
    for i in (0, 1, 2):
        lo, hi = bbox(m[i]["tv"] + m[i]["qv"])
        print("  Prop %d  x[%d..%d] y[%d..%d] z[%d..%d]  (%d Tri, %d Quad)  MD1 @Datei 0x%X"
              % (i, lo[0], hi[0], lo[1], hi[1], lo[2], hi[2], len(m[i]["tris"]),
                 len(m[i]["quads"]), pr[i]["md1_off"]))
    lo, hi = bbox(sm["tv"] + sm["qv"])
    print("  Sicherung (lokal) x[%d..%d] y[%d..%d] z[%d..%d], Sitz %s" %
          (lo[0], hi[0], lo[1], hi[1], lo[2], hi[2], SITZ))
    print("  -> Sicherung in Plattform-Koordinaten x[%d..%d] y[%d..%d] z[%d..%d]" %
          (lo[0] + SITZ[0], hi[0] + SITZ[0], lo[1] + SITZ[1], hi[1] + SITZ[1],
           lo[2] + SITZ[2], hi[2] + SITZ[2]))

    # --- das Fach: was liegt UNTER den Deckeln? -----------------------------------
    d1lo, d1hi = bbox(m[1]["tv"] + m[1]["qv"])
    d2lo, d2hi = bbox(m[2]["tv"] + m[2]["qv"])
    fx0, fx1 = min(d1lo[0], d2lo[0]), max(d1hi[0], d2hi[0])
    fz0, fz1 = min(d1lo[2], d2lo[2]), max(d1hi[2], d2hi[2])
    print("\n== 2. Das Fach unter den Deckeln ==")
    print("  Deckel-Grundriss (beide Haelften, ZU): x[%d..%d] z[%d..%d], Naht bei z=%d"
          % (fx0, fx1, fz0, fz1, d1lo[2]))
    print("  Deckel-Hoehe y[%d..%d]" % (min(d1lo[1], d2lo[1]), max(d1hi[1], d2hi[1])))
    alle = flaechen(m[0])
    im_fach = []
    for (p, uv, po) in alle:
        a = np.array(p)
        if (a[:, 0].min() >= fx0 - 5 and a[:, 0].max() <= fx1 + 5 and
                a[:, 2].min() >= fz0 - 5 and a[:, 2].max() <= fz1 + 5):
            im_fach.append((p, uv))
    print("  Plattform-Dreiecke VOLLSTAENDIG im Deckel-Grundriss: %d" % len(im_fach))
    waag = {}
    for (p, uv) in im_fach:
        a = np.array(p)
        if a[:, 1].min() == a[:, 1].max():
            y = int(a[0, 1])
            waag.setdefault(y, []).append((a, uv))
    for y in sorted(waag):
        a = np.concatenate([x[0] for x in waag[y]])
        us = [t for x in waag[y] for t in x[1]]
        print("    waagerechte Flaeche y=%d: x[%d..%d] z[%d..%d]  (%d Dreiecke)  UV u[%d..%d] v[%d..%d]"
              % (y, a[:, 0].min(), a[:, 0].max(), a[:, 2].min(), a[:, 2].max(), len(waag[y]),
                 min(t[0] for t in us), max(t[0] for t in us),
                 min(t[1] for t in us), max(t[1] for t in us)))
    print("  Sitz der Sicherung x=%d z=%d liegt %s des Deckel-Grundrisses"
          % (SITZ[0], SITZ[2],
             "INNERHALB" if fx0 <= SITZ[0] <= fx1 and fz0 <= SITZ[2] <= fz1 else "AUSSERHALB"))

    # --- Draufsicht ---------------------------------------------------------------
    def ortho(achsen, tiefe_achse, tiefe_vz, deckel_offen, datei, skala=0.5, rand=40,
              titel=""):
        plo, phi = bbox(m[0]["tv"] + m[0]["qv"])
        a0, a1 = achsen
        w = int((phi[a0] - plo[a0]) * skala) + 2 * rand
        h = int((phi[a1] - plo[a1]) * skala) + 2 * rand
        b = Bild(w, h)

        def pt(v, off=(0, 0, 0)):
            q = (v[0] + off[0], v[1] + off[1], v[2] + off[2])
            return ((q[a0] - plo[a0]) * skala + rand, (q[a1] - plo[a1]) * skala + rand,
                    q[tiefe_achse] * tiefe_vz)
        for (p, uv, po) in flaechen(m[0]):
            b.dreieck([pt(v) for v in p], uv, tex[0], 0)
        dz = DECKEL_WEG if deckel_offen else 0
        for (p, uv, po) in flaechen(m[1]):
            b.dreieck([pt(v, (0, 0, dz)) for v in p], uv, tex[1], 1)
        for (p, uv, po) in flaechen(m[2]):
            b.dreieck([pt(v, (0, 0, -dz)) for v in p], uv, tex[2], 2)
        for (p, uv, po) in flaechen(sm):
            b.dreieck([pt(v, SITZ) for v in p], uv, stex, 4, farbe=None)
        n4 = int((b.wer == 4).sum())
        img = Image.fromarray(b.rgb)
        d = ImageDraw.Draw(img)
        ys, xs = np.nonzero(b.wer == 4)
        if len(xs):
            d.rectangle((xs.min() - 3, ys.min() - 3, xs.max() + 3, ys.max() + 3),
                        outline=(255, 0, 255))
        d.text((4, 4), titel, fill=(255, 255, 0))
        return img, n4

    o1, n1 = ortho((2, 0), 1, 1, False, None, titel="von OBEN, Deckel ZU (rechts=+z, unten=+x)")
    o2, n2 = ortho((2, 0), 1, 1, True, None, titel="von OBEN, Deckel OFFEN (+-240 in z)")
    blatt = Image.new("RGB", (o1.size[0] * 2 + 10, o1.size[1]), (255, 0, 255))
    blatt.paste(o1, (0, 0))
    blatt.paste(o2, (o1.size[0] + 10, 0))
    blatt.save(os.path.join(ZIEL, "draufsicht.png"))
    print("\n== 3. Draufsicht (wahre Verdeckung von oben) ==")
    print("  sichtbare Sicherungs-Pixel von oben: Deckel zu %d, Deckel offen %d  (Skala 0.5 px/Einheit)"
          % (n1, n2))
    s1, _ = ortho((2, 1), 0, 1, True, None,
                  titel="von -X (Blickrichtung Cut 4), Deckel OFFEN (rechts=+z, unten=+y)")
    s1.save(os.path.join(ZIEL, "seitenansicht.png"))

    # --- Cut 4 mit Z-Puffer --------------------------------------------------------
    print("\n== 4. Cut 4 (sub04 Cut_chg 4 @0x0FB2) — Groesse und wahre Verdeckung ==")
    c4 = cuts(rdt)[4]
    print("  Kamera @Datei 0x%X: fov=%d (H=%d) pos=%s tgt=%s pri=0x%X"
          % (c4["off"], c4["fov"], c4["fov"] >> 7, c4["pos"], c4["tgt"], c4["pri"]))
    view = view_bauen(c4)
    F = 3
    for py, offen, marke in ((-305, False, "unten_zu"), (-305, True, "unten_offen"),
                             (-1095, True, "vor_modal"), (-1205, True, "hochpunkt")):
        prot = prop_rot(*PLATTFORM_ROT)
        ppos = [PLATTFORM_XZ[0], py, PLATTFORM_XZ[1]]
        b = Bild(320 * F, 240 * F)

        def zeichne(mesh, tx, lokal_t, kennung):
            r, t = verketten(prot, ppos, prop_rot(0, 0, 0), list(lokal_t))
            cr, ct = view_x_welt(view, r, t)
            for (p, uv, po) in flaechen(mesh):
                q = [projiziere(view, cr, ct, v) for v in p]
                if any(x is None for x in q):
                    continue
                # Unterpixel: dieselbe Projektion, aber in 3facher Aufloesung gerechnet
                qq = []
                for v in p:
                    vx = ((v[0] * cr[0] + v[1] * cr[1] + v[2] * cr[2]) >> 12) + ct[0]
                    vy = ((v[0] * cr[3] + v[1] * cr[4] + v[2] * cr[5]) >> 12) + ct[1]
                    vz = ((v[0] * cr[6] + v[1] * cr[7] + v[2] * cr[8]) >> 12) + ct[2]
                    qq.append(((160 + view["H"] * vx / vz) * F, (120 + view["H"] * vy / vz) * F, vz))
                b.dreieck(qq, uv, tx, kennung)
        # Plattform selbst: lokal = Einheit -> verketten mit (0,0,0)
        cr0, ct0 = view_x_welt(view, prot, ppos)
        for (p, uv, po) in flaechen(m[0]):
            qq = []
            ok = True
            for v in p:
                vx = ((v[0] * cr0[0] + v[1] * cr0[1] + v[2] * cr0[2]) >> 12) + ct0[0]
                vy = ((v[0] * cr0[3] + v[1] * cr0[4] + v[2] * cr0[5]) >> 12) + ct0[1]
                vz = ((v[0] * cr0[6] + v[1] * cr0[7] + v[2] * cr0[8]) >> 12) + ct0[2]
                if vz < 64:
                    ok = False
                    break
                qq.append(((160 + view["H"] * vx / vz) * F, (120 + view["H"] * vy / vz) * F, vz))
            if ok:
                b.dreieck(qq, uv, tex[0], 0)
        dz = DECKEL_WEG if offen else 0
        zeichne(m[1], tex[1], (0, 0, dz), 1)
        zeichne(m[2], tex[2], (0, 0, -dz), 2)
        vorher = b.wer.copy()
        zeichne(sm, stex, SITZ, 4)
        ys, xs = np.nonzero(b.wer == 4)
        # Groesse OHNE Verdeckung: alles allein zeichnen
        allein = Bild(320 * F, 240 * F)
        b2 = b
        b = allein
        zeichne(sm, stex, SITZ, 4)
        b = b2
        ya, xa = np.nonzero(allein.wer == 4)
        if len(xa):
            print("  Plattform y=%5d Deckel %-5s: Sicherung UNVERDECKT %d px (3x) = bbox 320x240 "
                  "x%.1f..%.1f y%.1f..%.1f (%.1f x %.1f px)"
                  % (py, "offen" if offen else "zu", len(xa), xa.min() / F, xa.max() / F,
                     ya.min() / F, ya.max() / F, (xa.max() - xa.min() + 1) / F,
                     (ya.max() - ya.min() + 1) / F))
        print("      davon nach wahrer Verdeckung sichtbar: %d px (3x) = %.0f %%; verdeckt durch: %s"
              % (len(xs), 100.0 * len(xs) / max(1, len(xa)),
                 ", ".join("Prop %d: %d px" % (k, int(((allein.wer == 4) & (b.wer == k)).sum()))
                           for k in (0, 1, 2))))
        img = Image.fromarray(b.rgb)
        d = ImageDraw.Draw(img)
        if len(xa):
            d.rectangle((xa.min() - 4, ya.min() - 4, xa.max() + 4, ya.max() + 4),
                        outline=(255, 0, 255))
        d.text((4, 4), "Cut 4, Plattform y=%d, Deckel %s (Z-Puffer, ohne Hintergrund)"
               % (py, "offen" if offen else "zu"), fill=(255, 255, 0))
        img.save(os.path.join(ZIEL, "cut4_zpuffer_%s.png" % marke))

    # --- Winkel Laengsachse <-> Blickrichtung --------------------------------------
    import math
    dx = c4["tgt"][0] - c4["pos"][0]
    dy = c4["tgt"][1] - c4["pos"][1]
    dz = c4["tgt"][2] - c4["pos"][2]
    n = math.sqrt(dx * dx + dy * dy + dz * dz)
    # Laengsachse der Sicherung: lokal +X, Plattform rot_y=2048 -> Welt -X
    cosw = abs(-1 * dx) / n
    print("\n== 5. Ausrichtung ==")
    print("  Blickrichtung Cut 4 = (%d,%d,%d), Laengsachse der Sicherung in der Welt = (-1,0,0)"
          % (dx, dy, dz))
    print("  Winkel zwischen beiden: %.1f Grad  -> die 406 lange Sicherung erscheint auf "
          "sin = %.2f ihrer Laenge verkuerzt" % (math.degrees(math.acos(cosw)),
                                                 math.sin(math.acos(cosw))))


if __name__ == "__main__":
    main()
