"""Bau EINES Cuts nach den Regeln der Phase 2 (2026-09-19) — Silhouette = Freistellung
punktgenau, Tiefe aus der Geometrie (geometrie.py), Rechtecke je Tiefenstufe, Abnahme
VOR dem Schreiben (abnahme.py).

Ablauf je Cut:
  1. Objekte aus der Auswahl: png (Alpha > SCHWELLE, oben=0, grow=0, fuellen=aus — die
     Aufweitung malte 1..62 % Hintergrund mit Objekttiefe ueber die Figur, Audit 1.4 a),
     polygon (eigene Quelle, bleibt bis der Nutzer ein PNG liefert), quader (Silhouette
     und Tiefe aus der Zelle, geom.quader_tiefe).
  2. Je Objekt die Kamera-z-Karte (geometrie.tiefe_geometrie bzw. quader_tiefe), dann die
     Rasterung nach der Original-Regel (geometrie.rastern).
  3. Zerlegung in Rechtecke mit einer Tiefenstufe (geometrie.zerlegung; Stufe und
     Statistik = die in kalib_geometrie.py GEMESSENEN Werte, s. raum.py).
  4. Atlas mit rechteck-eigener Deckung (atlas.build_rechtecke), Sektion im
     Original-Layout (geom.pack_section).
  5. Abnahme: (a) Deckung == Soll bitgenau ueber den Sektions-/Atlas-Leser,
     (b) Standplatz-Schiene VORverd == 0, (c) Pruefbild. Faellt (a) oder (b), wird
     NICHTS geschrieben und der Cut steht mit seiner Restliste im Bericht.
"""
import os
import struct

import numpy as np

import abnahme
import atlas as atlasmod
import geom
import geometrie
import maske_aus_png
import maskenbild

ALPHA_SCHWELLE = 110     # ⛔ Heuristik (Audit §5, Gegenpruefung #10): schneidet den 1-2 px
                         # Anti-Alias-Saum weg; Nutzer-Entscheidung, nicht gemessen.


def objekt_region(o, bg, ppm_bg_fn=None):
    """-> (region bool 240x320, quelle) fuer png / polygon / quader-Objekte."""
    if "png" in o:
        if not all(k in o for k in ("x", "y")):
            raise SystemExit('   ⛔ "%s": Lage (x, y) fehlt — mit maske_aus_png.py messen und eintragen'
                             % o.get("name", "?"))
        r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1), alpha_schwelle=ALPHA_SCHWELLE)
        return (r if r is not None else np.zeros((240, 320), bool)), "png"
    if "polygon" in o:
        import raum as _raum
        return _raum.polygon_region(o["polygon"], o.get("loch")), "polygon"
    if "quader" in o:
        return None, "quader"      # Silhouette kommt aus der Zelle (unten)
    raise SystemExit('   ⛔ "%s": keine zulaessige Quelle (png / polygon / quader)' % o.get("name", "?"))


def kaesten_anwenden(r, o, e):
    import anwenden
    for k in anwenden.kaesten(o.get("plus", "")):
        r[k[1]:k[3], k[0]:k[2]] = True
    for k in anwenden.kaesten(o.get("minus", "")):
        r[k[1]:k[3], k[0]:k[2]] = False
    for k in anwenden.kaesten(e.get("minus", "") or ""):
        r[k[1]:k[3], k[0]:k[2]] = False
    return r


def ebene_des_objekts(o, rdt, cam, cut, rid, bericht):
    """Bodenebene y0 des Objekts: 'ebene' aus der Auswahl (gemessen) hat Vorrang, sonst das
    einzige begehbare Band, sonst das Blickziel des Kamerasatzes, wenn eindeutig
    (geom.ebene_aus_kamera) — sonst Abbruch wie bisher in raum.py."""
    if o.get("ebene") is not None:
        return int(o["ebene"])
    baender = sorted(geom.begehbare_baender(rid))
    if len(baender) == 1:
        return -baender[0] * geom.BAND_HOEHE
    r = geom.ebene_aus_kamera(rdt, cam, cut, rid)
    if r is None or r[1] >= geom.GUETE_MAX:
        raise SystemExit('   ⛔ "%s": Bodenebene nicht eindeutig (Baender %s) — "ebene" messen und eintragen'
                         % (o.get("name", "?"), baender))
    bericht.append("Bodenebene y=%d aus dem Blickziel (Guete %.2f)" % (r[0], r[1]))
    return int(r[0])


def bau_cut(room, cut, e, rdt, cam, bg, floor_by_band, tol, statistik, out_dir, bild_dir,
            schreiben=True):
    """-> dict Bericht (geschrieben ja/nein, Zahlen) — s. raum.py fuer die Ausgabe."""
    rid = int(room[4:], 16)
    v = geom.cut_view(rdt, cam, cut)
    if not v or v[2] <= 0:
        return {"fehler": "kein Kamerasatz"}
    R, t, H = v
    zeilen = []
    objekte = []          # (name, region, dep int, vz float, quelle, y0)
    soll = np.zeros((240, 320), bool)
    for o in e.get("objekte") or []:
        name = o.get("name", "?")
        r, quelle = objekt_region(o, bg)
        y0 = ebene_des_objekts(o, rdt, cam, cut, rid, zeilen)
        band = int(round(-y0 / float(geom.BAND_HOEHE)))
        if quelle == "quader":
            q = [int(x) for x in o["quader"]]
            vz, tr = geometrie.quader_auf_band(R, t, H, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4], y0)
            r = kaesten_anwenden(tr.copy(), o, e)
            vzm = np.where(r, vz, 0.0)
            zeilen.append('"%s": quader x%d..%d z%d..%d Hoehe %d auf y=%d, %d Punkte' % (name, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4], y0, int(r.sum())))
        else:
            r = kaesten_anwenden(r, o, e)
            if not r.any():
                zeilen.append('"%s": leere Silhouette' % name); continue
            ber = []
            vzm, info = geometrie.tiefe_geometrie(rdt, R, t, H, r, y0, band, o, None, ber)
            for b in ber:
                zeilen.append('"%s" (%s, ebene %d): %s' % (name, quelle, y0, b))
            if vzm is None:
                zeilen.append('"%s": KEINE Tiefe (%s) — Objekt bleibt ohne Maske' % (name, info.get("quelle")))
                continue
        dep = geometrie.rastern(vzm) * r
        if (r & (dep == 0)).any():
            return {"fehler": 'Objekt "%s": %d Punkte ohne Tiefe' % (name, int((r & (dep == 0)).sum())), "zeilen": zeilen}
        objekte.append((name, r, dep, vzm, quelle, y0))
        soll |= r
    if not objekte:
        return {"fehler": "keine Objekte", "zeilen": zeilen}
    # (3) Zerlegung — die FEINSTE Stufe, die die Engine-Grenzen (105 Rechtecke, 256x256
    # Atlas) haelt; tol ist die Obergrenze. Kalibrierung (kalib_geometrie.py, 107 Kuenstler-
    # Cuts): Stufe 1 haelt nur in 49, Stufe 8 in allen 107; die Kuenstler selbst streuen
    # innerhalb EINES Rechtecks median 9 Buckets. Haelt auch tol nicht: LAUT, nicht vergroebern.
    rects = None
    stufe = None
    for s in (1, 2, 3, 4, 6, 8, 12, 16):
        if s > tol:
            break
        try:
            rects, wahl = geometrie.zerlegung([(n, r, d) for (n, r, d, _, _, _) in objekte], s,
                                              bericht=None, statistik=statistik)
            stufe = s
            break
        except RuntimeError as ex:
            letzter = ex
    if rects is None:
        return {"fehler": "UEBERLAUF bis Stufe %d: %s" % (tol, letzter), "zeilen": zeilen, "soll": soll}
    zeilen.append("zerlegung Stufe %d: %s -> %d Rechtecke, %d Atlaspunkte"
                  % (stufe, ", ".join("%s=%s/%s" % (w[0][:14], w[1], "voll" if w[2] is None else w[2]) for w in wahl),
                     len(rects), sum(r[2] * r[3] for r in rects)))
    # (4) Atlas + Sektion
    try:
        tim, place = atlasmod.build_rechtecke(bg, rects)
    except RuntimeError as ex:
        return {"fehler": "ATLAS: %s" % ex, "zeilen": zeilen, "soll": soll}
    groups, masks = [], []
    for i, (x, y, w, h, tiefe, op) in enumerate(rects):
        ax, ay = place[i]
        groups.append((1, x - ax, y - ay))
        masks.append((ax, ay, x, y, w, h, int(tiefe)))
    sec = geom.pack_section(groups, masks)
    # (5a) Deckung == Soll ueber den Leser, den auch die Sonde nutzt
    blob = geom.pack_container({cut: sec}, rdt[1])
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(tim)[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    fehlt = int((soll & ~deck).sum()); zuviel = int((deck & ~soll).sum())
    # (5b) Standplatz-Schiene
    stand, stand_y0 = abnahme.standlinie([(r, vzm, y0_) for (_, r, _, vzm, _, y0_) in objekte])
    sch = abnahme.standplatz_schiene(R, t, H, deck, tief, stand, floor_by_band, stand_y0=stand_y0)
    tiefen = sorted(set(m[6] for m in masks))
    bericht = {"zeilen": zeilen, "rects": len(masks), "stufen": len(tiefen), "stufe": stufe,
               "tmin": min(tiefen), "tmax": max(tiefen), "fehlt": fehlt, "zuviel": zuviel,
               "soll_px": int(soll.sum()), "schiene": sch, "wahl": wahl,
               "quellen": sorted(set(q for (_, _, _, _, q, _) in objekte)),
               "atlas_px": sum(r[2] * r[3] for r in rects)}
    # (5c) Pruefbild — immer, auch bei Ablehnung (der Nutzer soll sehen, warum)
    if bild_dir:
        p = abnahme.pruefbild(bg, soll, deck, ms, "%s C%d  %d Rechtecke, Tiefen %d..%d, VORverd %d HINTfrei %d/%d"
                              % (room, cut, len(masks), min(tiefen), max(tiefen), sch["VORverd"], sch["HINTfrei"], sch["HINTn"]),
                              os.path.join(bild_dir, "%s_C%d.png" % (room, cut)))
        bericht["bild"] = p
    ok = (fehlt == 0 and zuviel == 0 and sch["VORverd"] == 0)
    bericht["ok"] = ok
    if ok and schreiben and out_dir:
        os.makedirs(out_dir, exist_ok=True)
        open(os.path.join(out_dir, "%s_PRI%02d.TIM" % (room, cut)), "wb").write(tim)
        abnahme.pbm_schreiben(os.path.join(out_dir, "%s_PRI%02d.PBM" % (room, cut)), soll)
        # Sidecar fuer test_pri_kopfschnitt: Standlinie je Spalte (Kamera-z, -1 = keine)
        with open(os.path.join(out_dir, "%s_PRI%02d.STAND" % (room, cut)), "w") as f:
            f.write("# Zeile 1: Standlinie je Bildspalte (Kamera-z des untersten opaken Punkts im Tiefenmodell), -1 = keine Maske\n")
            f.write(" ".join("-1" if np.isnan(s) else "%d" % int(round(s)) for s in stand) + "\n")
            f.write("# Zeile 2: Bodenebene y0 je Bildspalte, auf der die Standlinie gilt (VOR/HINTER wird auf ihr verglichen)\n")
            f.write(" ".join("%d" % int(v) for v in stand_y0) + "\n")
        bericht["sektion"] = sec
    return bericht
