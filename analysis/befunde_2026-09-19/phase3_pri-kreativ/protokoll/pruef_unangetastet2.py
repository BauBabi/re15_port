# Die IM BAUM LIEGENDEN Sektionen der vier unangetasteten Cuts mit der Abnahme der
# Phase 3 durchrechnen (nicht neu bauen): Deckung gegen die Sollflaeche der Auswahl,
# Standplatz-Schiene gegen die Standlinie des Phase-3-Tiefenmodells.
import os, sys, json, struct
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
import geom, geometrie, bau_p2, abnahme, maskenbild as MB, raum as R

MASKS = "re15_port/shared_assets/PSX/MASKS"
FAELLE = [("ROOM10F0", 4), ("ROOM10F0", 5), ("ROOM1100", 1), ("ROOM1100", 2)]
aus = json.load(open(R.AUSWAHL, encoding="utf-8"))
for (room, cut) in FAELLE:
    rid = int(room[4:], 16)
    rdt, _ = R.load_rdt(R.CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    bg = R.load_bg("build/bg_ppm", rid, cut)
    Rm, t, H = geom.cut_view(rdt, cam, cut)
    e = aus[room][str(cut)]
    soll = np.zeros((240, 320), bool)
    objekte = []
    for o in e["objekte"]:
        r, q = bau_p2.objekt_region(o, bg)
        y0 = bau_p2.ebene_des_objekts(o, rdt, cam, cut, rid, [])
        band = int(round(-y0 / float(geom.BAND_HOEHE)))
        if q == "quader":
            qd = [int(v) for v in o["quader"]]
            vz, tr = geometrie.quader_auf_band(Rm, t, H, qd[0], qd[0] + qd[2], qd[1], qd[1] + qd[3], qd[4], y0)
            r = bau_p2.kaesten_anwenden(tr.copy(), o, e)
            vzm = np.where(r, vz, 0.0)
        else:
            r = bau_p2.kaesten_anwenden(r, o, e)
            vzm, info = geometrie.tiefe_geometrie(rdt, Rm, t, H, r, y0, band, o, None, [])
            if vzm is None:
                continue
        soll |= r
        objekte.append((r, vzm, y0))
    ms = MB.masken(open(os.path.join(MASKS, "%s.MSK" % room), "rb").read(), cut) or []
    tim = MB.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))
    deck, tief = abnahme.deckung_und_tiefe(ms, tim[0])
    stand, stand_y0 = abnahme.standlinie(objekte)
    floor = abnahme.floor_aus_dump("build/p2/floor_p2.txt", rid)
    s = abnahme.standplatz_schiene(Rm, t, H, deck, tief, stand, floor, stand_y0=stand_y0)
    tiefen = sorted(set(m[6] for m in ms))
    print("%s C%d (im Baum): %d Rechtecke, %d Tiefenstufen (%d..%d) | Soll %d px, "
          "fehlt %d / zuviel %d | VOR n=%d verdeckt=%d teil=%d | HINTER n=%d frei=%d -> %s"
          % (room, cut, len(ms), len(tiefen), min(tiefen), max(tiefen), int(soll.sum()),
             int((soll & ~deck).sum()), int((deck & ~soll).sum()),
             s["VORn"], s["VORverd"], s["VORteil"], s["HINTn"], s["HINTfrei"],
             "BESTEHT" if (soll & ~deck).sum() == 0 and (deck & ~soll).sum() == 0 and s["VORverd"] == 0
             else "FAELLT DURCH"))
    if s["vor_rest"]:
        print("    VOR-Rest: %s" % ", ".join("(%d,%d,%d,%.2f)" % q for q in s["vor_rest"][:8]))
    abnahme.pruefbild(bg, soll, deck, ms, "%s C%d (Bestand)" % (room, cut),
                      "build/p3/bilder_unangetastet/%s_C%d_bestand.png" % (room, cut))
