"""Runde 41 Spur Q (Abnahme M 5 Mangel 1, OFFEN 18 des Dossiers M): ein eingesetztes Atlas-Teil auf ein Fenster je OT-Stufe umlegen.

Dossier: analysis/befunde_runde41/Q_restpunkte.md, Punkt 1.

BEFUND: platzieren_r41.py gibt jeder Bildspalte eines Teils EINE Tiefe (Kopf Regel 6, Spaltenmaximum). Das Teil 8 in ROOM6010/6011 Cut 5
(Treppenwange, Zelle @0x0092C) laeuft im Bild schraeg nach unten, eine Spalte reicht dort ueber bis zu 11 OT-Stufen (R113: bucket 116..126).
Die Spaltentiefe ist die ferne Stufe; eine Figur hinter den nahen Punkten derselben Spalte liegt vor der Spaltentiefe und bleibt sichtbar
(M_abnahme_5.md 1.6: 82 von 96 Figurpunkten). Der Sektion fehlt dafuer kein Platz: Spalten in Stuecke je Stufe zu teilen braucht 155
Rechtecke (113 + 155 = 268 > 255, Byte-Grenze @0x8003928c), aber die Stufen des Teils sind nur 15 - genau so viele wie es Rechtecke hat.

REGEL: je OT-Stufe des Teils EIN Rechteck. Sein Fenster im Vordergrundatlas (_PRIcc.TIM) traegt nur die Punkte dieser Stufe, der Rest des
Fensters ist durchsichtig (Index 0, wie die Luecken der Gelaender); RECHTECK-TIEFE = die Stufe, also je Punkt genau bucket(Eintritt).
Stufe je Punkt wie tiefe_teil: Koerper aus der Modell-Zeile, Kamera des Spiels (platzieren_r41.ansicht_spiel), bucket = (1023*vz)>>16
(@0x8002565c sra v1,v1,4 mit ZSF3 341 @0x80066c70), Masken-OT = depth @0x80039650. Eine diagonale Stufe passt in kein Unterfenster des
alten Rechtecks, deshalb neue Fenster an freien Stellen (schliessen_r41.frei_platz); die alten Texel des Teils werden frei. Palette und
alle Fenster der uebrigen Rechtecke bleiben bytegleich, Rechteck-Zahl, Reihenfolge und Modell-Tabelle (Rechteck -> Tiefe) ebenso, solange
die Stufen des Teils die Tiefen seiner Rechtecke sind. Leon- und Elza-Datei bleiben bytegleich.

Aufruf:
  python re15_port/tools/maske/stufen_r41.py --raum 6010 --cut 5 --von 113 --tabelle <gen_r41_masken36_zellen.inc>
"""
import argparse
import os
import re
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)

import geom                      # noqa: E402
import koerper_r41 as K          # noqa: E402
import maskenbild                # noqa: E402
import platzieren_r41 as P       # noqa: E402
import schliessen_r41 as SR      # noqa: E402

PSX = SR.PSX
MASKS = SR.MASKS
ZEILE = re.compile(r"\s*\{ 0x([0-9A-F]{4}),\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),")


def modell_zeilen(pfad, rid, cut):
    """Modell-Zeilen des Cuts aus gen_r41_masken36_zellen.inc: {Rechteck: Felder}."""
    z = {}
    for s in open(pfad, encoding="utf-8").read().splitlines():
        m = ZEILE.match(s)
        if m and int(m.group(1), 16) == rid and int(m.group(2)) == cut:
            z[int(m.group(3))] = dict(x=int(m.group(4)), z=int(m.group(5)), w=int(m.group(6)), d=int(m.group(7)), typ=int(m.group(8)),
                                      band=int(m.group(9)), hoehe=int(m.group(10)), saeule=int(m.group(11)), tiefe=int(m.group(12)))
    return z


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--raum", required=True)
    ap.add_argument("--cut", type=int, required=True)
    ap.add_argument("--von", type=int, required=True, help="erstes Rechteck des Teils; das Teil reicht bis zum Ende der Sektion")
    ap.add_argument("--tabelle", required=True, help="tests/unit/gen_r41_masken36_zellen.inc (nur gelesen)")
    ap.add_argument("--trocken", action="store_true")
    a = ap.parse_args()
    rid = int(a.raum, 16)
    room = "ROOM%04X" % rid
    rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), room + ".RDT"), "rb").read()
    blob = open(os.path.join(MASKS, room + ".MSK"), "rb").read()
    if open(os.path.join(MASKS, "ROOM%04X.MSK" % (rid | 1)), "rb").read() != blob:
        raise SystemExit("Elza-Datei nicht bytegleich")
    secs, n_cut = SR.sektionen(blob)
    sek = secs[a.cut]
    masks = sek["masks"]
    z = modell_zeilen(a.tabelle, rid, a.cut)
    koerper = set((z[i]["x"], z[i]["z"], z[i]["w"], z[i]["d"], z[i]["typ"], z[i]["band"], z[i]["hoehe"], z[i]["saeule"]) for i in range(a.von, len(masks)))
    if len(koerper) != 1:
        raise SystemExit("Rechtecke %d.. tragen nicht genau einen Koerper: %s" % (a.von, koerper))
    kz = z[a.von]
    R, t, H = P.ansicht_spiel(rdt, a.cut)
    ve, _va, hit = K.ein_aus(R, t, H, kz["typ"], kz["x"], kz["z"], kz["w"], kz["d"], kz["band"], kz["hoehe"], bool(kz["saeule"]))
    tp = os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, a.cut))
    tb = open(tp, "rb").read()
    idx = maskenbild.lies_tim(tp)[0].copy()
    if len(set(SR.bildlage(sek, m) + (m[6], m[7]) for m in masks[a.von:])) != len(masks) - a.von:
        raise SystemExit("Rechtecke doppelt")
    belegt = np.zeros(idx.shape, bool)
    for m in masks[:a.von]:
        belegt[m[1]:m[1] + m[7], m[0]:m[0] + m[6]] = True
    punkte = {}                                  # Stufe -> [(x, y, Index)]
    for i in range(a.von, len(masks)):
        m = masks[i]
        X, Y = SR.bildlage(sek, m)
        sx, sy, w, h = m[0], m[1], m[6], m[7]
        for yy in range(h):
            for xx in range(w):
                v = int(idx[sy + yy, sx + xx])
                if v == 0:
                    continue
                px, py = X + xx, Y + yy
                if not (0 <= px < 320 and 0 <= py < 240):
                    continue
                if not hit[py, px]:
                    raise SystemExit("Punkt (%d,%d) trifft den Koerper nicht: Erben (Regel 6) hier nicht vorgesehen" % (px, py))
                punkte.setdefault(int(K.bucket(ve[py, px])), []).append((px, py, v))
        if belegt[sy:sy + h, sx:sx + w].any():
            raise SystemExit("Fenster von R%d teilt Texel mit einem fremden Rechteck" % i)
        idx[sy:sy + h, sx:sx + w] = 0
    stufen = sorted(punkte, reverse=True)
    n_alt = len(masks) - a.von
    alt_tiefen = [masks[i][4] for i in range(a.von, len(masks))]
    print("%s C%d: Rechtecke %d..%d (%d), Stufen %s (%d), alte Tiefen %s" % (room, a.cut, a.von, len(masks) - 1, n_alt, stufen, len(stufen), alt_tiefen))
    if stufen != alt_tiefen:
        raise SystemExit("Stufen des Teils sind nicht die Tiefen seiner Rechtecke - Modell-Tabelle und Rechteckzahl wuerden sich aendern")
    neu = []
    flaeche = 0
    for d in stufen:
        xs = [p[0] for p in punkte[d]]
        ys = [p[1] for p in punkte[d]]
        x0, y0, x1, y1 = min(xs), min(ys), max(xs) + 1, max(ys) + 1
        w, h = x1 - x0, y1 - y0
        pl = SR.frei_platz(belegt, w, h)
        if pl is None:
            raise SystemExit("kein freier Platz fuer Stufe %d (%dx%d)" % (d, w, h))
        ax, ay = pl
        belegt[ay:ay + h, ax:ax + w] = True
        for px, py, v in punkte[d]:
            idx[ay + py - y0, ax + px - x0] = v
        neu.append((ax, ay, x0, y0, w, h, d))
        flaeche += w * h
        print("  Stufe %3d: %3d Punkte, Fenster %2dx%2d an (%d,%d), Bild (%d,%d)" % (d, len(punkte[d]), w, h, ax, ay, x0, y0))
    print("Fensterflaeche %d, Punkte %d" % (flaeche, sum(len(v) for v in punkte.values())))
    if a.trocken:
        return
    groups = [(g[0], g[2], g[3]) for g in sek["groups"]]
    if any(g[0] != 1 for g in groups[-n_alt:]):
        raise SystemExit("Teil steht nicht in Einzelgruppen am Ende der Sektion")
    groups = groups[:-n_alt]
    alt = []
    for q in masks[:a.von]:
        QX, QY = SR.bildlage(sek, q)
        alt.append((q[0], q[1], QX, QY, q[6], q[7], q[4]))
    for m in neu:
        groups.append((1, m[2] - m[0], m[3] - m[1]))
    sec = geom.pack_section(groups, alt + neu)
    o = struct.unpack_from("<%dI" % n_cut, blob, 12)
    alle = {}
    for c in range(n_cut):
        if o[c]:
            ende = min([p for p in o if p > o[c]] + [len(blob)])
            alle[c] = blob[o[c]:ende]
    alle[a.cut] = sec
    out = geom.pack_container(alle, n_cut)
    tim = P.SR_tim_mit_index(tb, idx)
    for r in (rid, rid | 1):
        open(os.path.join(MASKS, "ROOM%04X.MSK" % r), "wb").write(out)
        open(os.path.join(MASKS, "ROOM%04X_PRI%02d.TIM" % (r, a.cut)), "wb").write(tim)
    print("geschrieben: %s + Elza-Datei, MSK und _PRI%02d.TIM" % (room, a.cut))


if __name__ == "__main__":
    main()
