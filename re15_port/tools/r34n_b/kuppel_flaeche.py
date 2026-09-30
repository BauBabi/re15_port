#!/usr/bin/env python3
"""Spur B (Runde 34 Nacht): TREFFERFLAECHE DER KUPPEL in Cut 4 (ROOM1150/1151).

Projiziert die ausgelieferte Geometrie mit der Port-Mathematik (analysis/befunde_runde30/
sicherung_werkzeug/r30_lib.py = camera_common.c + main.c-Prop-Projektion, in Runde 31/32 gegen
Framedumps auf < 1 px geprueft) im CURSOR-ZUSTAND: Plattform = Prop 0 bei (-20700,-305,-17460)
rot_y 2048 (sub04 Pos_set @0x0FB4 `32 00 24 af cf fe cc bb`, Obj_model_set @0x0E00 rot 0x0800),
Deckel Prop 1/2 geschlossen (Anhaengeform pc[5]=0xC0, lokal (0,0,0), @0x0E22/@0x0E44),
Kamera Cut 4 (RDT-Kameratabelle @0x00E0).

Teile:
  DECKEL  = alle Flaechen von Prop 1 und Prop 2 (MD1 aus der RDT-Modelltabelle @RDT+0x30)
  PODEST  = die Flaechen von Prop 0, deren Punkte ALLE im Kuppel-Podest liegen
            (y in [-1036,-886], x in [-490,-70], z in [870,1650] Plattform-lokal;
             Runde 31 hebetisch.md §0/§1.1: Podest y -886..-1036, Achteck x[-485..-74] z[875..1645])
Ausgabe: Flaechen-/Pixelzahlen, bbox, konvexe Huelle (320x240, ganzzahlig gerundet), Masken-PNGs,
Ueberlagerung auf einen Framedump (optional), Deckung gegen eine Differenzmaske (optional).
Aufruf: kuppel_flaeche.py <ROOM1150.RDT|ROOM1151.RDT> [--bild f.ppm] [--diff maske.png] [--aus ordner]
"""
import sys, os, math, struct
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "befunde_runde30", "sicherung_werkzeug"))
from r30_lib import rdt_laden, cuts, props, md1_lesen, view_bauen, prop_rot, view_x_welt, projiziere  # noqa
from PIL import Image, ImageDraw

PLATTFORM = (-20700, -305, -17460)   # sub04 Pos_set @0x0FB4
ROT_Y = 2048                          # main00 Obj_model_set @0x0E00 rot_y 0x0800

def flaechen(md1):
    out = []
    for me in md1["meshes"]:
        for t in me["tris"]:
            out.append([me["tv"][v] for v in t])
        for q in me["quads"]:
            p = [me["qv"][v] for v in q]
            out.append([p[0], p[1], p[3], p[2]])   # Viereck-Umlauf v0 v1 v3 v2
    return out

def im_podest(poly):
    return all(-1036 <= y <= -886 and -490 <= x <= -70 and 870 <= z <= 1650 for (x, y, z) in poly)

def huelle(pts):
    pts = sorted(set(pts))
    if len(pts) < 3: return pts
    def kreuz(o, a, b): return (a[0]-o[0])*(b[1]-o[1]) - (a[1]-o[1])*(b[0]-o[0])
    lo, hi = [], []
    for p in pts:
        while len(lo) >= 2 and kreuz(lo[-2], lo[-1], p) <= 0: lo.pop()
        lo.append(p)
    for p in reversed(pts):
        while len(hi) >= 2 and kreuz(hi[-2], hi[-1], p) <= 0: hi.pop()
        hi.append(p)
    return lo[:-1] + hi[:-1]

def main():
    name = sys.argv[1]
    aus = sys.argv[sys.argv.index("--aus") + 1] if "--aus" in sys.argv else "."
    os.makedirs(aus, exist_ok=True)
    d = rdt_laden(name)
    c4 = cuts(d)[4]
    v = view_bauen(c4)
    P = props(d)
    cr, ct = view_x_welt(v, prop_rot(0, ROT_Y, 0), PLATTFORM)
    teile = {"deckel": [], "podest": []}
    for k in (1, 2):
        for f in flaechen(md1_lesen(P[k]["md1"])):
            teile["deckel"].append(f)
    for f in flaechen(md1_lesen(P[0]["md1"])):
        if im_podest(f): teile["podest"].append(f)
    print("%s Cut 4 @0x%X pos %s tgt %s fov %d H %d | Prop1 MD1 @0x%X, Prop2 MD1 @0x%X, Prop0 MD1 @0x%X"
          % (name, c4["off"], c4["pos"], c4["tgt"], c4["fov"], v["H"], P[1]["md1_off"], P[2]["md1_off"], P[0]["md1_off"]))
    masken = {}
    alle_pts = []
    for teil, fl in teile.items():
        m = Image.new("L", (960, 720), 0); dr = ImageDraw.Draw(m)
        n = 0
        for f in fl:
            s = [projiziere(v, cr, ct, p) for p in f]
            if any(q is None for q in s): continue
            dr.polygon([(q[0] * 3, q[1] * 3) for q in s], fill=255)
            alle_pts += [(q[0], q[1]) for q in s]
            n += 1
        masken[teil] = m
        px = sum(1 for x in m.getdata() if x)
        bb = m.getbbox()
        print("  %-7s %3d Flaechen, %6d Pixel (960x720), bbox320 %s" % (teil, n, px,
              tuple(round(b / 3, 1) for b in bb) if bb else None))
        m.save(os.path.join(aus, "maske_%s.png" % teil))
    ges = Image.new("L", (960, 720), 0)
    ges.paste(255, mask=masken["deckel"]); ges.paste(255, mask=masken["podest"])
    ges.save(os.path.join(aus, "maske_kuppel.png"))
    h = huelle(alle_pts)
    print("  KONVEXE HUELLE (320x240, %d Ecken): %s" % (len(h), ", ".join("(%d,%d)" % (round(x), round(y)) for x, y in h)))
    mx = sum(x for x, y in h) / len(h); my = sum(y for x, y in h) / len(h)
    print("  Eckenmittel (320): (%.1f, %.1f)" % (mx, my))
    if "--bild" in sys.argv:
        b = Image.open(sys.argv[sys.argv.index("--bild") + 1]).convert("RGB")
        ov = b.copy(); dr = ImageDraw.Draw(ov)
        dr.polygon([(x * 3, y * 3) for x, y in h], outline=(255, 0, 0))
        rot = Image.new("RGB", b.size, (255, 0, 0))
        ov = Image.composite(Image.blend(ov, rot, 0.35), ov, masken["deckel"])
        gelb = Image.new("RGB", b.size, (255, 255, 0))
        ov = Image.composite(Image.blend(ov, gelb, 0.35), ov, masken["podest"])
        ov.save(os.path.join(aus, "ueberlagerung.png"))
        ov.resize((320, 240), Image.NEAREST).save(os.path.join(aus, "ueberlagerung_320.png"))
    if "--diff" in sys.argv:
        dm = Image.open(sys.argv[sys.argv.index("--diff") + 1]).convert("L")
        a = list(ges.getdata()); b2 = list(dm.getdata())
        inn = sum(1 for x, y in zip(a, b2) if x and y); nur_geo = sum(1 for x, y in zip(a, b2) if x and not y)
        print("  Deckung gegen Differenzmaske (Plattform da/nicht da): %d von %d Kuppel-Pixeln gerendert (%.2f %%), %d nicht"
              % (inn, inn + nur_geo, 100.0 * inn / max(1, inn + nur_geo), nur_geo))

main()
