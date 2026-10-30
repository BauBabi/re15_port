"""Runde 41 Spur M, Nachbesserung 4 (Abnahme 3, Mangel 2 / OFFEN 14): EIN Generator-Rechteck an einer Bildspalte teilen; der linke
Teil bekommt die Tiefe seines Koerpers (Zellen-Modell), der rechte behaelt seine Tiefe.

Dossier: analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 4".

ANLASS: ROOM6000/6001 Cut 5, Traeger-Mitte (Rechteck 37, Tiefe 176 aus der NB2-Zeile Zelle 13 @RDT+0x007F8 = Wandblock jenseits des
Gangs): Leon hinter der Ecke des Wandblocks Zelle 12 @RDT+0x007EC (`d8 4f c8 0f 4a e1 d6 e9 01 ff 00 23`) bleibt unter ihm sichtbar
(Abnahme 3 k6000_05a/b/c). kacheln_r41.py teilte das Rechteck mit Zelle 12; die Rueckholliste nahm die Teilung zurueck. Gemessen
(m41nb4/py/r37ecke.py, 9492 erreichbare Band-2-Plaetze von Cut 5, Figur +-450 x 0..3000 in 25er-Schritten): mit der Tiefe
max bucket(Eintritt in das Prisma der Zelle 12, Saeule) je Bildspalte wuerden in den Spalten 31..40 des Rechtecks 175..6158 Figurpunkte
verdeckt, deren Sehstrahl das Prisma NICHT vor ihnen trifft (Gang oestlich der Ecke, Strahl kreuzt z -5674 bei x ~12936 > 12578) - in
den Spalten 1..30 KEINER. Die Band-0-Plaetze der NB3-Begruendung gibt es nach der Saaten-Regel nicht mehr; der wirkliche Grund sind
diese Band-2-Plaetze.

REGEL: linker Teil = Spalten [X, X+spalten) des Rechtecks, gleiche Atlas-Abbildung (Quelle und Ziel um 0 verschoben), eigene Gruppe am
Ende der Sektion; Tiefe = max bucket(Eintritt) seiner gezeichneten Punkte in den Koerper der Zeile (koerper_r41.ein_aus, Form der
Zelle, Saeule = ohne Deckel) - so verdeckt er nie eine Figur VOR dem Koerper (Urteil depth < bucket(vz), @0x8002565c otz>>4 / ZSF3 341
@0x80066c70). Der rechte Teil behaelt Satz, Gruppe und Tiefe (Quelle und Ziel um `spalten` verschoben). Danach rechnet
test_r37_masken36 "erzeuge" den Ausschluss neu. Leon- und Elza-Datei bleiben bytegleich.

Aufruf:
  python re15_port/tools/maske/teilen_r41.py --raum 6000 --cut 5 --rechteck 37 --spalten 30 --zelle 0x007EC --hoehe -4100 --saeule 1 \
         --iou 0.71 --tabelle <gen_r41_masken36_zellen.inc>
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
import schliessen_r41 as SR      # noqa: E402

PSX = SR.PSX
MASKS = SR.MASKS


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--raum", required=True)
    ap.add_argument("--cut", type=int, required=True)
    ap.add_argument("--rechteck", type=int, required=True)
    ap.add_argument("--spalten", type=int, required=True)
    ap.add_argument("--zelle", required=True, help="RDT-Offset des SCA-Eintrags (hex)")
    ap.add_argument("--hoehe", type=int, required=True)
    ap.add_argument("--saeule", type=int, default=0)
    ap.add_argument("--iou", type=float, required=True)
    ap.add_argument("--tabelle", required=True)
    a = ap.parse_args()
    rid = int(a.raum, 16)
    room = "ROOM%04X" % rid
    rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), room + ".RDT"), "rb").read()
    off = int(a.zelle, 16)
    zb = rdt[off:off + 12]
    w, d, x, z = struct.unpack_from("<HHhh", zb, 0)
    typ, band = zb[8] & 15, zb[11] >> 4
    blob = open(os.path.join(MASKS, room + ".MSK"), "rb").read()
    if open(os.path.join(MASKS, "ROOM%04X.MSK" % (rid | 1)), "rb").read() != blob:
        raise SystemExit("Elza-Datei nicht bytegleich")
    secs, n_cut = SR.sektionen(blob)
    sek = secs[a.cut]
    m = sek["masks"][a.rechteck]
    X, Y = SR.bildlage(sek, m)
    sx, sy, bw, bh, d0 = m[0], m[1], m[6], m[7], m[4]
    if not (0 < a.spalten < bw):
        raise SystemExit("Spalten %d ausserhalb 1..%d" % (a.spalten, bw - 1))
    idx = maskenbild.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, a.cut)))[0]
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, a.cut)
    ve, va, tr = K.ein_aus(R, t, H, typ, x, z, w, d, band, a.hoehe, bool(a.saeule))
    eintritt = []
    for yy in range(bh):
        for xx in range(a.spalten):
            px, py = X + xx, Y + yy
            if not (0 <= px < 320 and 0 <= py < 240) or idx[sy + yy, sx + xx] == 0 or not tr[py, px]:
                continue
            eintritt.append(ve[py, px])
    if not eintritt:
        raise SystemExit("linker Teil trifft den Koerper nicht")
    tiefe = int(K.bucket(np.array(eintritt)).max())
    print("%s C%d R%d: X %d Y %d %dx%d Tiefe %d -> links %d Spalten Tiefe %d (Koerper %s, %d Punkte), rechts %d Spalten Tiefe %d" % (
        room, a.cut, a.rechteck, X, Y, bw, bh, d0, a.spalten, tiefe, a.zelle, len(eintritt), bw - a.spalten, d0))
    # Sektion neu: alter Satz a.rechteck schmaler (Quelle/Ziel + spalten), neuer Satz am Ende mit eigener Gruppe
    groups = [(g[0], g[2], g[3]) for g in sek["groups"]]
    masks = []
    for i, q in enumerate(sek["masks"]):
        QX, QY = SR.bildlage(sek, q)
        if i == a.rechteck:
            masks.append((q[0] + a.spalten, q[1], QX + a.spalten, QY, q[6] - a.spalten, q[7], q[4]))
        else:
            masks.append((q[0], q[1], QX, QY, q[6], q[7], q[4]))
    neu = (sx, sy, X, Y, a.spalten, bh, tiefe)
    groups.append((1, X - sx, Y - sy))
    masks.append(neu)
    sec = geom.pack_section(groups, masks)
    offs = struct.unpack_from("<%dI" % n_cut, blob, 12)
    alle = {}
    for c in range(n_cut):
        if offs[c]:
            ende = min([o for o in offs if o > offs[c]] + [len(blob)])
            alle[c] = blob[offs[c]:ende]
    alle[a.cut] = sec
    out = geom.pack_container(alle, n_cut)
    for r in (rid, rid | 1):
        open(os.path.join(MASKS, "ROOM%04X.MSK" % r), "wb").write(out)
    # Modell-Zeile fuer den neuen Satz (Index = alte Zahl)
    i_neu = len(sek["masks"])
    zeile = "    { 0x%04X, %2d, %3d, %6d, %6d, %5d, %5d, %d, %2d, %5d, %d, %3d, 0x%05X, { %s } },   /* IoU %.2f */" % (
        rid, a.cut, i_neu, x, z, w, d, typ, band, a.hoehe, 1 if a.saeule else 0, tiefe, off, ", ".join("0x%02x" % c for c in zb), a.iou)
    t_alt = open(a.tabelle, encoding="utf-8").read()
    rx = re.compile(r"\s*\{ 0x([0-9A-F]{4}),\s*(\d+),\s*(\d+),")
    zeilen = t_alt.split("\n")
    pos = None
    for k, zl in enumerate(zeilen):
        mm = rx.match(zl)
        if mm and int(mm.group(1), 16):
            key = (int(mm.group(1), 16), int(mm.group(2)), int(mm.group(3)))
            if key > (rid, a.cut, i_neu) and pos is None:
                pos = k
        elif mm and pos is None:
            pos = k                                   # Ende-Zeile
    zeilen.insert(pos, zeile)
    open(a.tabelle, "w", encoding="utf-8", newline="\n").write("\n".join(zeilen))
    print("neuer Satz %d, Modell-Zeile eingefuegt: %s" % (i_neu, zeile.strip()))


if __name__ == "__main__":
    main()
