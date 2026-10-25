"""Runde 41 Spur M, Nachbesserung 2 (Abnahme 1, Mangel 1, Weg (a)): Generator-Rechtecke der Stages 3..6 mit ZELLEN-MODELL
(Kollisionszelle + gemessene Hoehe) bekommen die Zellen-Tiefe, und der Ausschluss urteilt fuer sie in 3D statt im Grundriss.

Dossier: analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 2".

Ausgangslage (NB1, tiefe_zelle_r41.py): 370 der 1317 vor NB1 ausgeschlossenen Rechtecke haben ein Zellen-Modell; 135 davon nahm
das Grundriss-Netz der Spur P trotz Zellen-Tiefe weiter heraus, 104 bekamen keine Tiefe, weil die Zellen-Tiefe NAEHER als die
alte lag ("nicht ferner"). Das Grundriss-Netz (test_r37_masken36.c zelle_dazwischen) kennt nur Zellen DESSELBEN Bands, laesst
Zellen aus, in deren Grundriss die Kamera steht, und kennt keine Hoehe - fuer ein Rechteck MIT Modell ist die richtige Frage
die 3D-Frage: liegt der verdeckte Figurpunkt auf dem Sehstrahl hinter dem Koerper des eigenen Objekts (Zellform x [Bandboden,
Bandboden + gemessene Hoehe])? Diese Frage stellt test_r37_masken36.c jetzt zusaetzlich (r41_koerper_davor) mit der Tabelle,
die dieses Werkzeug schreibt.

Regel (dieselbe Modellbildung wie NB1 Schritt 2, tiefe_zelle_r41.kandidaten mit ERBEN; neu: NAEHER):
  1. Kandidaten = alle Rechtecke, die VOR NB1 ausgeschlossen waren (--vor, Tabelle 95b9f38d) - die NB1-Menge.
  2. Modell je Objekt wie NB1 (Sperrzelle mit der Mehrheit der Bodenkontakte, Hoehe per IoU, Sehstrahl gegen den Koerper,
     Fehltreffer erben); Rechteck-Tiefe = max bucket(vz) (bucket = (1023*vz)>>16, Figur-OT @0x8002565c sra v1,v1,4 mit
     ZSF3 = 341 @0x80066c70; Masken-OT = depth @0x80039650). Auch wenn sie naeher als die alte liegt.
  3. MSK: Tiefenfeld (+4 im Satz) = Zellen-Tiefe; Tabelle tests/unit/gen_r41_masken36_zellen.inc: je Rechteck die Zelle
     (Bytes und RDT-Offset als Beleg), Band, Hoehe, Saeule, Tiefe.
  4. Danach rechnet test_r37_masken36 "erzeuge" den Ausschluss neu; Rechtecke, die weiter aus sind, bekommen mit --zurueck
     ihre Tiefe von VOR NB2 zurueck und fallen aus der Tabelle (wie NB1).
Nur Tiefenfelder aendern sich; Atlas, Lage, Gruppen, Leon/Elza bytegleich.

Aufruf:
  python re15_port/tools/maske/zellen3d_r41.py --cache <Regionen-Cache> --vor <Tabelle vor NB1> --quelle <MSK vor NB2>
         --out <Ordner> --tabelle <gen_r41_masken36_zellen.inc> [--zurueck <Tabelle nach erzeuge>] [ROOMxxxx ...]
"""
import argparse
import json
import os
import re
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)

import geom                      # noqa: E402
import schliessen_r41 as SR      # noqa: E402
import tiefe_zelle_r41 as TZ     # noqa: E402

PSX = SR.PSX


def sca_beleg(rdt, zelle, typ, band):
    """-> (RDT-Offset, 12 Bytes) des ersten SCA-Eintrags mit diesen Werten (solide u0 & 1, floor >> 4 == band)."""
    s = struct.unpack_from("<I", rdt, 0x20)[0]
    n = sum(struct.unpack_from("<5I", rdt, s + 4))
    for i in range(n):
        o = s + 24 + 12 * i
        w, d, x, z = struct.unpack_from("<HHhh", rdt, o)
        t, u0, _u1, f = rdt[o + 8:o + 12]
        if (x, z, w, d) == tuple(zelle) and (t & 15) == typ and (u0 & 1) and (f >> 4) == band:
            return o, bytes(rdt[o:o + 12])
    raise SystemExit("Zelle %s Typ %d Band %d nicht im SCA" % (zelle, typ, band))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("raeume", nargs="*")
    ap.add_argument("--cache", required=True, help="Regionen-Cache (schliessen_r41 --cache)")
    ap.add_argument("--vor", required=True, help="Ausschluss-Tabelle VOR NB1 (Kandidaten)")
    ap.add_argument("--quelle", required=True, help="MSK-Ordner VOR NB2 (alte Tiefen)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--tabelle", required=True, help="Ziel: tests/unit/gen_r41_masken36_zellen.inc")
    ap.add_argument("--zurueck", default=None, help="Tabelle nach erzeuge: dort weiter ausgeschlossene -> alte Tiefe")
    ap.add_argument("--zuordnung", default=os.path.join(HIER, "zuordnung_r41.json"),
                    help="Zellen von Hand je Objekt (Beleg je Eintrag), s. Kopf von zuordnung_r41.json")
    a = ap.parse_args()
    TZ.ERBEN = True
    TZ.NAEHER = True
    if a.zuordnung and os.path.exists(a.zuordnung):
        for room, cuts in json.load(open(a.zuordnung, encoding="utf-8")).items():
            if room.startswith("ROOM"):
                for cut, liste in cuts.items():
                    TZ.ZUORDNUNG[(room, int(cut))] = liste
    os.makedirs(a.out, exist_ok=True)
    vor = TZ.ausschluss(a.vor)
    zur = TZ.ausschluss(a.zurueck) if a.zurueck else None
    zeilen, bericht = [], {}
    for f in sorted(os.listdir(a.quelle)):
        if not re.match(r"ROOM[3-6][0-9A-F]{2}0\.MSK$", f):
            continue
        room = f[:8]
        rid = int(room[4:], 16)
        if a.raeume and room not in a.raeume:
            continue
        blob = bytearray(open(os.path.join(a.quelle, f), "rb").read())
        if open(os.path.join(a.quelle, "ROOM%04X.MSK" % (rid | 1)), "rb").read() != bytes(blob):
            raise SystemExit("%s: Elza-Datei nicht bytegleich" % room)
        rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), "%s.RDT" % room), "rb").read()
        cam = struct.unpack_from("<I", rdt, 0x24)[0]
        secs, _n_cut = SR.sektionen(bytes(blob))
        b_raum = {}
        for cut, sek in sorted(secs.items()):
            kand = vor.get((rid, cut), set())
            if kand != vor.get((rid | 1, cut), set()):
                raise SystemExit("%s Cut %d: Kandidaten Leon/Elza verschieden" % (room, cut))
            if not kand:
                continue
            cache = os.path.join(a.cache, "%s_%02d.npz" % (room, cut))
            if not os.path.exists(cache):
                raise SystemExit("Regionen-Cache fehlt: %s" % cache)
            ks = TZ.kandidaten(room, cut, bytes(blob), sek, kand, rdt, cam, cache)
            offs = TZ.satz_offsets(bytes(blob), cut)
            n_neu = 0
            for k in ks:
                if k["neu"] is None:
                    continue
                if zur is not None and k["i"] in zur.get((rid, cut), set()):
                    k["zurueck"] = True          # trotz 3D-Regel weiter aus -> Tiefe von vor NB2 bleibt, kein Tabelleneintrag
                    continue
                z = k["zellen"][0]
                if any((c["zelle"], c["typ"], c["band"], c["hoehe"], c["saeule"]) !=
                       (z["zelle"], z["typ"], z["band"], z["hoehe"], z["saeule"]) for c in k["zellen"]):
                    # Rechteck ueber Objekte mit VERSCHIEDENEN Zellen: das 3D-Urteil kennt je Rechteck nur einen Koerper (welcher
                    # Punkt zu welchem Objekt gehoert, weiss der Ausschluss nicht) -> kein Modell, Tiefe bleibt wie vor NB2.
                    k["mehrere_zellen"] = True
                    continue
                struct.pack_into("<H", blob, offs[k["i"]] + 4, k["neu"])
                o, b = sca_beleg(rdt, z["zelle"], z["typ"], z["band"])
                zeilen.append((rid, cut, k["i"], z["zelle"], z["typ"], z["band"], z["hoehe"], z["saeule"], k["neu"], o, b,
                               z["iou"]))
                n_neu += 1
            b_raum[cut] = ks
            print("%s C%-2d Kandidaten %3d, mit Modell %3d" % (room, cut, len(kand), n_neu), flush=True)
        for r in (rid, rid | 1):
            open(os.path.join(a.out, "ROOM%04X.MSK" % r), "wb").write(bytes(blob))
        bericht[room] = b_raum
    json.dump(bericht, open(os.path.join(a.out, "zellen3d_r41.json"), "w"), indent=1)
    with open(a.tabelle, "w", newline="\n") as t:
        t.write("/* Erzeugt von re15_port/tools/maske/zellen3d_r41.py - NICHT von Hand aendern.\n"
                " * Runde 41 Spur M, Nachbesserung 2 (Abnahme 1, Mangel 1): Generator-Rechtecke der Stages 3..6 mit Zellen-Modell.\n"
                " * Je Zeile: Raum (Leon-Datei; Elza xxx1 ist bytegleich), Cut, Rechteck, SCA-Zelle x z breite tiefe, Typ, Band,\n"
                " * gemessene Hoehe (IoU-Maximum, negativ = nach oben), Saeule, Zellen-Tiefe im MSK; Beleg: RDT-Offset und die\n"
                " * 12 Bytes des SCA-Eintrags dort (u16 breite, u16 tiefe, s16 x, s16 z, typ, u0, u1, floor); Kommentar: IoU.\n"
                " * Benutzt von tests/unit/test_r37_masken36.c (r41_koerper_davor) und tests/unit/test_r41_masken36.c (zellen3d). */\n"
                "static const r41_zellen_modell_t r41_masken36_zellen[] = {\n")
        for (rid, cut, i, zl, typ, band, hoehe, saeule, neu, o, b, iou) in zeilen:
            t.write("    { 0x%04X, %2d, %3d, %6d, %6d, %5d, %5d, %d, %2d, %5d, %d, %3d, 0x%05X, { %s } },   /* IoU %.2f */\n"
                    % (rid, cut, i, zl[0], zl[1], zl[2], zl[3], typ, band, hoehe, 1 if saeule else 0, neu, o,
                       ", ".join("0x%02x" % c for c in b), iou))
        t.write("    { 0x0000, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 } }   /* Ende */\n};\n")
    print("Tabelle: %d Rechtecke mit Zellen-Modell -> %s" % (len(zeilen), a.tabelle), flush=True)


if __name__ == "__main__":
    main()
