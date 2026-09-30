#!/usr/bin/env python3
"""gesten_streifen.py - Spur D (Runde 34 Nacht): Posen-Streifen der Leon-Szenengesten.

Rendert Leon (PL00-Mesh/-Textur/-Skelett) in den Posen eines Clips aus dem Raum-RBJ
(RDT+0x5C, Record 0 = Spieler-Overlay) — Bild fuer Bild, in Vorder- und Draufsicht.
Damit laesst sich eine Geste am BILD zuordnen ("rechter Arm 180 Grad nach rechts",
"Arm-Schwung"), nicht nur ueber die Clip-Nummer.

Alles Format- und Posenrelevante ist ein Port der Engine-Leser (ueber emd_ansichtsblatt.py):
    MD1/EMR/EDD/TIM-Leser, 12-Bit-Euler, RotMatrix (@0x80068130), Posenkette
    (skeleton_common.c re15_skel_compute_pose), RBJ-Record (emd_common.c
    re15_emd_parse_rbj_record: Hierarchie + rel-Offsets aus PL00.EMR, Keyframes + EDD aus dem RBJ).
NICHT byte-true und bewusst so: Orthogonalprojektion + Lambert-Licht (Lesbarkeit), kein
Plc_neck-Kopfblick, keine Waffenhand (PLW) — die Figur zeigt nur die CLIP-Pose.

Aufruf:
  gesten_streifen.py <ROOM.RDT> <rec> <clip> <out.png> [--bilder 0,4,8,...] [--rueck]
  --rueck haengt den Rueckweg an (Plc_flg 0x80: Index = laenge-1-frame, anim_select_common.c).
"""
import argparse, os, sys
import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, ".."))
import emd_ansichtsblatt as E                          # noqa: E402
sys.path.insert(0, HIER)
import rbj_zensus as R                                 # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
PLD = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "PLD")


class Leon:
    """Gleiche Schnittstelle wie emd_ansichtsblatt.Modell, Quelle PL00.* + Raum-RBJ."""

    def __init__(self, rdt_pfad, rec):
        self.meshes = E.md1_parse(open(os.path.join(PLD, "PL00.MD1"), "rb").read())
        basis = E.emr_parse(open(os.path.join(PLD, "PL00.EMR"), "rb").read())
        self.tex, self.pal, self.clut_y = E.tim_decode(open(os.path.join(PLD, "PL00.TIM"), "rb").read())
        self.texh, self.texw = self.tex.shape
        d = open(rdt_pfad, "rb").read()
        s, size = R.rbj_block(d)
        r = d[s:s + size]
        total = E.u32(r, 0)
        pre, edd = E.u32(r, total + rec * 8), E.u32(r, total + rec * 8 + 4)
        o_arm, o_frm, count, ksize = (E.u16(r, pre + 4), E.u16(r, pre + 6),
                                      E.u16(r, pre + 8), E.u16(r, pre + 10))
        if count != basis.bone_count:
            raise SystemExit("RBJ-Record hat %d Knochen, PL00 %d" % (count, basis.bone_count))
        kf_off = pre + 4 + o_frm
        sk = E.Skel()
        sk.bone_count, sk.kf_size = basis.bone_count, ksize
        sk.rel, sk.parent = basis.rel, basis.parent
        sk.kf_data = r[kf_off:edd]
        sk.kf_count = len(sk.kf_data) // ksize
        self.skel = sk
        self.clips, self.frames = E.edd_parse(r[edd:])

    pose = E.Modell.pose
    geometrie = E.Modell.geometrie
    clip_row = None

    def clut_row(self, clut_id):
        return max(0, min(self.pal.shape[0] - 1, (clut_id >> 6) - self.clut_y))

    def page_ubase(self, page):
        return (page & 0xF) * 128

    def kf_von(self, clip, bild):
        first, n = self.clips[clip]
        e = self.frames[first + bild]
        return e & 0xFFF


def kachel(mo, V, N, F, view, groesse, massstab, zentrum):
    right, down, depth = (np.asarray(v, float) for v in view)
    ss = 2
    w = h = groesse * ss
    cx = w / 2 - float(np.dot(zentrum, right)) * massstab * ss
    cy = h / 2 - float(np.dot(zentrum, down)) * massstab * ss
    col, alp = E.render(mo, V, N, F, right, down, depth, w, h, massstab * ss, cx, cy)
    rgb = np.clip(col * 1.6, 0, 1) ** (1 / 1.15)
    bg = np.array([28, 30, 35], float) / 255.0
    out = rgb * alp[..., None] + bg * (1 - alp[..., None])
    return Image.fromarray((out * 255).astype(np.uint8)).resize((groesse, groesse), Image.LANCZOS)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("rdt"); ap.add_argument("rec", type=int); ap.add_argument("clip", type=int)
    ap.add_argument("out")
    ap.add_argument("--bilder", default="")
    ap.add_argument("--rueck", action="store_true")
    ap.add_argument("--groesse", type=int, default=150)
    ap.add_argument("--ansichten", default="vorn,oben,rechts",
                    help="Auswahl aus vorn,oben,rechts,links (Komma)")
    a = ap.parse_args()

    mo = Leon(a.rdt, a.rec)
    first, n = mo.clips[a.clip]
    bilder = [int(x) for x in a.bilder.split(",")] if a.bilder else list(range(0, n, max(1, n // 6))) + [n - 1]
    bilder = sorted(set(b for b in bilder if 0 <= b < n))
    folge = [("vor", b) for b in bilder]
    if a.rueck:
        folge += [("rueck", b) for b in bilder[::-1][1:]]

    # gemeinsamer Massstab ueber alle Posen des Clips (Figur ~ 1800 hoch)
    alle = []
    for _, b in folge:
        V, _, _ = mo.geometrie(mo.kf_von(a.clip, b))
        alle.append(V)
    lo = np.min([v.min(0) for v in alle], axis=0); hi = np.max([v.max(0) for v in alle], axis=0)
    zentrum = (lo + hi) / 2.0
    massstab = a.groesse * 0.86 / float((hi - lo).max())

    # Vorderansicht (Kamera vor der Figur) und Draufsicht; Achsen wie emd_ansichtsblatt
    alle_views = {"vorn": E.V_FRONT, "oben": E.V_TOP, "rechts": E.V_RIGHT, "links": E.V_LEFT}
    views = [(nm, alle_views[nm]) for nm in a.ansichten.split(",")]
    G = a.groesse
    lab = 18
    img = Image.new("RGB", (len(folge) * G, len(views) * G + lab * 2), (18, 19, 22))
    dr = ImageDraw.Draw(img)
    dr.text((4, 2), "%s rec%d clip%d (%d Bilder)%s" % (os.path.basename(a.rdt), a.rec, a.clip, n,
                                                      "  + Rueckweg (Plc_flg 0x80)" if a.rueck else ""),
            fill=(232, 233, 236))
    for i, (richt, b) in enumerate(folge):
        V, N, F = mo.geometrie(mo.kf_von(a.clip, b))
        for j, (nm, view) in enumerate(views):
            t = kachel(mo, V, N, F, view, G, massstab, zentrum)
            img.paste(t, (i * G, lab + j * G))
        dr.text((i * G + 4, lab + len(views) * G + 1),
                ("<" if richt == "rueck" else "") + "b%d" % b, fill=(214, 138, 74))
    for j, (nm, _) in enumerate(views):
        dr.text((2, lab + j * G + 2), nm, fill=(150, 153, 160))
    img.save(a.out)
    print("%s: %d Posen -> %s" % (os.path.basename(a.rdt), len(folge), a.out))


if __name__ == "__main__":
    main()
