#!/usr/bin/env python3
"""elza_gesten.py - Runde 39 Spur I (funk10f1): Posen-Streifen der ELZA-Szenengesten.

Wie tools/r34n_d/gesten_streifen.py (Leon, PL00), nur mit Elzas Basis PL04.MD1/.EMR/.TIM — der Spieler-Familie,
die der Port fuer Elza laedt (platform/pc/main.c "[pl] Spieler-Familie PL04"). Record 0 eines Elza-Raums
(Marker Bit 0 = Spieler, Binder FUN_8001b3f8) ist Elzas Gestenbibliothek; dieses Werkzeug zeigt sie Bild fuer Bild,
damit die Clip-Wahl der Funkszene am BILD begruendet ist (nicht ueber die Nummer).

Aufruf:
  elza_gesten.py streifen <ROOM.RDT> <rec> <clip> <out.png> [--bilder 0,4,..] [--rueck] [--ansichten vorn,rechts,oben]
  elza_gesten.py arme     <ROOM.RDT> <rec> <clip> [<clip> ...]    # Handbahn je Bild (wie r34n_d/gesten_arme.py)
  elza_gesten.py katalog  <ROOM.RDT> <rec> <out.png> [--clips 0,1,..] [--n 6] [--groesse 96]
                          # je Clip eine Zeile: n Bilder gleichverteilt, Seiten- + Vorderansicht
"""
import argparse, math, os, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, "..", "r34n_d"))
import gesten_streifen as G                               # noqa: E402
E = G.E
R = G.R
PLD = G.PLD


class Elza(G.Leon):
    """gesten_streifen.Leon mit Elzas Basis PL04 (MD1/EMR/TIM)."""

    def __init__(self, rdt_pfad, rec, basis="PL04"):
        self.meshes = E.md1_parse(open(os.path.join(PLD, basis + ".MD1"), "rb").read())
        b = E.emr_parse(open(os.path.join(PLD, basis + ".EMR"), "rb").read())
        self.tex, self.pal, self.clut_y = E.tim_decode(open(os.path.join(PLD, basis + ".TIM"), "rb").read())
        self.texh, self.texw = self.tex.shape
        d = open(rdt_pfad, "rb").read()
        s, size = R.rbj_block(d)
        r = d[s:s + size]
        total = E.u32(r, 0)
        pre, edd = E.u32(r, total + rec * 8), E.u32(r, total + rec * 8 + 4)
        o_arm, o_frm, count, ksize = (E.u16(r, pre + 4), E.u16(r, pre + 6), E.u16(r, pre + 8), E.u16(r, pre + 10))
        if count != b.bone_count:
            raise SystemExit("RBJ-Record hat %d Knochen, %s %d" % (count, basis, b.bone_count))
        kf_off = pre + 4 + o_frm
        sk = E.Skel()
        sk.bone_count, sk.kf_size = b.bone_count, ksize
        sk.rel, sk.parent = b.rel, b.parent
        sk.kf_data = r[kf_off:edd]
        sk.kf_count = len(sk.kf_data) // ksize
        self.skel = sk
        self.clips, self.frames = E.edd_parse(r[edd:])


def streifen(a):
    G.Leon = lambda rdt, rec: Elza(rdt, rec)                 # gesten_streifen.main baut ueber G.Leon
    sys.argv = [sys.argv[0], a.rdt, str(a.rec), str(a.clip), a.out] + \
               (["--bilder", a.bilder] if a.bilder else []) + (["--rueck"] if a.rueck else []) + \
               ["--ansichten", a.ansichten, "--groesse", str(a.groesse)]
    G.main()


def arme(a):
    sys.path.insert(0, os.path.join(HIER, "..", "r34n_d"))
    import gesten_arme as A                                # noqa: E402
    A.G.Leon = lambda rdt, rec: Elza(rdt, rec)
    sys.argv = [sys.argv[0], a.rdt, str(a.rec)] + [str(c) for c in a.clips]
    A.main()


def katalog(a):
    from PIL import Image, ImageDraw
    mo = Elza(a.rdt, a.rec)
    clips = [int(c) for c in a.clips.split(",")] if a.clips else [c for c in range(len(mo.clips)) if mo.clips[c][1] > 1]
    Gr, lab = a.groesse, 14
    views = [("rechts", E.V_RIGHT), ("vorn", E.V_FRONT)]
    img = Image.new("RGB", (40 + a.n * len(views) * Gr, len(clips) * (Gr + lab) + lab), (18, 19, 22))
    dr = ImageDraw.Draw(img)
    dr.text((4, 1), "%s rec%d (Elza PL04): je Clip %d Bilder, links Seite (rechts), rechts Vorn" %
            (os.path.basename(a.rdt), a.rec, a.n), fill=(232, 233, 236))
    for zi, c in enumerate(clips):
        first, n = mo.clips[c]
        bilder = sorted(set([int(round(i * (n - 1) / max(1, a.n - 1))) for i in range(a.n)]))
        alle = [mo.geometrie(mo.kf_von(c, b))[0] for b in bilder]
        lo = np.min([v.min(0) for v in alle], axis=0); hi = np.max([v.max(0) for v in alle], axis=0)
        zentrum = (lo + hi) / 2.0
        massstab = Gr * 0.86 / float((hi - lo).max())
        y0 = lab + zi * (Gr + lab)
        dr.text((2, y0 + Gr // 2 - 6), "c%d %dB" % (c, n), fill=(214, 138, 74))
        for vi, (nm, view) in enumerate(views):
            for bi, b in enumerate(bilder):
                V, N, F = mo.geometrie(mo.kf_von(c, b))
                t = G.kachel(mo, V, N, F, view, Gr, massstab, zentrum)
                x = 40 + (vi * a.n + bi) * Gr
                img.paste(t, (x, y0))
                dr.text((x + 2, y0 + Gr), "b%d" % b, fill=(150, 153, 160))
    img.save(a.out)
    print("%s rec%d: %d Clips -> %s" % (os.path.basename(a.rdt), a.rec, len(clips), a.out))


def bahn(a):
    """Handbahn je Bild im Modellraum der Figur (Posenkette wie emd_ansichtsblatt.Modell.pose): x = vorn (Gierung 0 =
    Modell-Vorderseite +X, gesten_streifen --kamera), y = unten, z; Hoehe = Abstand zur tiefsten Mesh-Ecke (Fusssohle)
    desselben Bildes. Knochen 11 = rechte Hand (Waffenhand, Skill re15-weapon-render), 14 = linke Hand."""
    mo = Elza(a.rdt, a.rec)
    for c in a.clips:
        first, n = mo.clips[c]
        print("=== %s rec%d clip %d (%d Bilder): Bild  rechte Hand (vorn, Hoehe, z)  linke Hand (vorn, Hoehe, z)" %
              (os.path.basename(a.rdt), a.rec, c, n))
        for b in range(n):
            kf = mo.kf_von(c, b)
            R, T = mo.pose(kf)
            V, _, _ = mo.geometrie(kf)
            boden = float(V[:, 1].max())
            r, l = T[11].astype(float), T[14].astype(float)
            print("  b%-3d R (%6.0f %6.0f %6.0f)   L (%6.0f %6.0f %6.0f)" %
                  (b, r[0], boden - r[1], r[2], l[0], boden - l[1], l[2]))


def main():
    ap = argparse.ArgumentParser()
    sp = ap.add_subparsers(dest="cmd", required=True)
    s = sp.add_parser("streifen")
    s.add_argument("rdt"); s.add_argument("rec", type=int); s.add_argument("clip", type=int); s.add_argument("out")
    s.add_argument("--bilder", default=""); s.add_argument("--rueck", action="store_true")
    s.add_argument("--ansichten", default="vorn,rechts,oben"); s.add_argument("--groesse", type=int, default=150)
    m = sp.add_parser("arme")
    m.add_argument("rdt"); m.add_argument("rec", type=int); m.add_argument("clips", type=int, nargs="+")
    k = sp.add_parser("katalog")
    k.add_argument("rdt"); k.add_argument("rec", type=int); k.add_argument("out")
    k.add_argument("--clips", default=""); k.add_argument("--n", type=int, default=6)
    k.add_argument("--groesse", type=int, default=96)
    h = sp.add_parser("bahn")
    h.add_argument("rdt"); h.add_argument("rec", type=int); h.add_argument("clips", type=int, nargs="+")
    a = ap.parse_args()
    {"streifen": streifen, "arme": arme, "katalog": katalog, "bahn": bahn}[a.cmd](a)


if __name__ == "__main__":
    main()
