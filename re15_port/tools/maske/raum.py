"""Ganzen Raum aus der Auswahldatei erzeugen — und das ERGEBNIS zum Ansehen rendern.

⛔ WARUM ES DAS GIBT (Nutzer-Befund 2026-09-03, error01/02/03.png): die Auswahl wurde
bis dahin per Kommandozeile Cut fuer Cut angewendet und nur die AUSWAHL angesehen
(pruefblatt.py). Drei Fehler blieben dadurch unentdeckt, weil sie erst in der
NACHBEARBEITUNG entstehen: eine Wandflaeche, die mit dem Sofa in einer Superpixel-
Flaeche lag; eine Spaltenfuellung, die den Gang zwischen den Stuehlen zumauerte; ein
Objekt, das schlicht fehlte. Alle drei sind im ERGEBNISBILD (maskenbild.py) sofort zu
sehen. Deshalb erzeugt dieses Werkzeug beides in einem Lauf: Maske schreiben und
Ergebnisbild rendern.

Auswahlformat (analysis/esp_masken_2026-09-03/auswahl.json):
    "ROOM1140": {
      "2": {"segments": 90, "ids": [30, 38, ...],
            "minus": "200,50,320,112",      Kaesten abziehen (Wand/Boden wegschneiden)
            "plus":  "",                    Kaesten hinzufuegen
            "oben": 4, "grow": 1, "fuellen": "komponente"}
    }
Eine blosse Liste statt des Objekts wird als {"ids": [...]} mit den Standardwerten
gelesen (altes Format).

Aufruf:
    python re15_port/tools/maske/raum.py ROOM1140            (alle Cuts der Datei)
    python re15_port/tools/maske/raum.py ROOM1140 2          (nur Cut 2)
"""
import argparse
import json
import os
import struct
import subprocess
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import anwenden
import geom
import maskenbild
from geom import load_bg, load_rdt

AUSWAHL = "analysis/esp_masken_2026-09-03/auswahl.json"
CD = "re15_port/shared_assets/PSX"
STD = {"segments": 220, "oben": 4, "grow": 1, "fuellen": "komponente",
       "minus": "", "plus": "", "ids": []}


def eintrag(v):
    e = dict(STD)
    if isinstance(v, list):
        e["ids"] = v
        e["segments"] = 90          # altes Format entstand mit 90
        e["oben"] = 6
        e["fuellen"] = "global"
    else:
        e.update(v)
    return e


def objekt_regionen(room, cut, e, ppm, blattdir):
    """-> [(name, region, fuss)] — die EINZELNEN Gegenstaende des Cuts.

    Format in der Auswahldatei:
      "objekte": [{"name": "Konferenztisch", "segments": 90, "ids": [...]},
                  {"name": "US-Fahne", "segments": 220, "ids": [...], "fuss": 178,
                   "minus": "..."}]
    "fuss" ist die Bildzeile des Bodenkontakts und wird gebraucht, wenn die Silhouette
    ihn nicht zeigt (duenne Stange/Beine) — siehe geom.depth_map_objekt.
    """
    from scipy import ndimage
    rid = int(room[4:], 16)
    aus = []
    for o in e.get("objekte") or []:
        seg = anwenden.lade_seg(blattdir, room, cut, o.get("segments", e["segments"]), ppm, rid)
        r = np.isin(seg, o["ids"])
        for k in anwenden.kaesten(o.get("plus", "")):
            r[k[1]:k[3], k[0]:k[2]] = True
        for k in anwenden.kaesten(o.get("minus", "")):
            r[k[1]:k[3], k[0]:k[2]] = False
        oben = o.get("oben", e["oben"]); grow = o.get("grow", e["grow"])
        if oben > 0:
            g = r.copy()
            for k in range(1, oben + 1):
                g[:-k] |= r[k:]
            r = g
        if grow > 0:
            r = ndimage.binary_dilation(r, iterations=grow)
        fuell = o.get("fuellen", e["fuellen"])
        if fuell != "aus":
            r = anwenden.spalten_fuellen(r, fuell)
        for k in anwenden.kaesten(o.get("minus", "")):
            r[k[1]:k[3], k[0]:k[2]] = False
        for k in anwenden.kaesten(e["minus"]):
            r[k[1]:k[3], k[0]:k[2]] = False
        # ⛔ SCHRAEGE UNTERKANTE. Ein Tisch endet im Bild an einer DIAGONALEN — mit
        # achsenparallelen Kaesten ist die nicht zu treffen, und genau darunter lag in
        # ROOM1140 Cut 4 Maske auf blankem Teppich (gemessen: 649 Bildpunkte, die
        # Tischstruktur endet in Spalte 120 bei y=140, die Maske lief bis y=176..213).
        # "unterkante": "x0,y0,x1,y1" schneidet alles unterhalb der Geraden durch die
        # beiden Punkte weg; die Gerade wird nach links und rechts verlaengert.
        uk = o.get("unterkante")
        if uk:
            x0, y0, x1, y1 = [float(v) for v in uk.split(",")]
            if x1 != x0:
                m = (y1 - y0) / (x1 - x0)
                xs = np.arange(320)
                ys = y0 + m * (xs - x0)
                for x in range(320):
                    yy = int(round(ys[x]))
                    if yy < 239:
                        r[max(0, yy + 1):, x] = False
        if r.any():
            aus.append((o.get("name", "?"), r, o.get("fuss")))
    return aus


def region_of(room, cut, e, ppm, blattdir):
    """Auswahl -> fertige Region.

    MEHRERE FEINHEITEN je Cut sind erlaubt ("gruppen"): eine Auswahl gilt immer nur
    zusammen mit IHRER Segmentierung, und eine grobe Auswahl (Tisch) laesst sich nicht
    in eine feine umrechnen. Statt alles neu zu waehlen, wenn ein Detail feiner
    gefasst werden muss, werden die Regionen vereinigt.
    """
    from scipy import ndimage
    rid = int(room[4:], 16)
    r = np.zeros((240, 320), bool)
    for g in e.get("gruppen") or [{"segments": e["segments"], "ids": e["ids"]}]:
        seg = anwenden.lade_seg(blattdir, room, cut, g["segments"], ppm, rid)
        r |= np.isin(seg, g["ids"])
    for k in anwenden.kaesten(e["plus"]):
        r[k[1]:k[3], k[0]:k[2]] = True
    for k in anwenden.kaesten(e["minus"]):
        r[k[1]:k[3], k[0]:k[2]] = False
    if e["oben"] > 0:
        o = r.copy()
        for k in range(1, e["oben"] + 1):
            o[:-k] |= r[k:]
        r = o
    if e["grow"] > 0:
        r = ndimage.binary_dilation(r, iterations=e["grow"])
    if e["fuellen"] != "aus":
        r = anwenden.spalten_fuellen(r, e["fuellen"])
    for k in anwenden.kaesten(e["minus"]):
        r[k[1]:k[3], k[0]:k[2]] = False
    return r


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("cut", type=int, nargs="?")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--blatt", default="build/blaetter")
    ap.add_argument("--out", default=os.path.join(CD, "MASKS"))
    ap.add_argument("--bild", default="build/maskenbild")
    a = ap.parse_args()

    room = a.room.upper()
    rid = int(room[4:], 16)
    aus = json.load(open(AUSWAHL, encoding="utf-8"))
    if room not in aus:
        raise SystemExit("%s steht nicht in %s" % (room, AUSWAHL))
    rdt, _ = load_rdt(CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]

    cuts = sorted(int(c) for c in aus[room] if not c.startswith("_"))
    if a.cut is not None:
        cuts = [a.cut]
    secs = {}
    for cut in cuts:
        e = eintrag(aus[room][str(cut)])
        bg = load_bg(a.ppm, rid, cut)
        if bg is None:
            print("  Cut %d: Hintergrund fehlt" % cut)
            continue
        objekte = objekt_regionen(room, cut, e, a.ppm, a.blatt)
        if objekte:
            res = anwenden.bau_objektweise(rdt, cam, cut, objekte, bg, a.out, room)
            flaeche = np.zeros((240, 320), bool)
            for (_, r_, _) in objekte:
                flaeche |= r_
            r = flaeche
        else:
            r = region_of(room, cut, e, a.ppm, a.blatt)
            res = anwenden.build(rdt, cam, rid, cut, r, bg, a.out, room)
        if not res:
            print("  Cut %d: nichts erzeugt" % cut)
            continue
        secs[cut], n = res
        print("  Cut %d: %5.1f %% Bildflaeche, %3d Rechtecke%s"
              % (cut, 100 * r.mean(), n,
                 ("  [%s]" % ", ".join("%s%s" % (nm, "" if f is None else " Fuss y=%d" % f)
                                        for (nm, _, f) in objekte)) if objekte else ""))

    # Container schreiben: bestehende Sektionen anderer Cuts erhalten
    path = os.path.join(a.out, "%s.MSK" % room)
    alt = {}
    if os.path.exists(path):
        b = open(path, "rb").read()
        if b[:4] == b"R15M":
            _, nc = struct.unpack_from("<II", b, 4)
            offs = struct.unpack_from("<%dI" % nc, b, 12)
            ends = sorted([o for o in offs if o] + [len(b)])
            for c in range(nc):
                if offs[c]:
                    alt[c] = b[offs[c]:min(x for x in ends if x > offs[c])]
    if a.cut is None:
        alt = {}                     # ganzer Raum wird neu gebaut
    alt.update(secs)
    open(path, "wb").write(geom.pack_container(alt, rdt[1]))
    print("  %s: %d Cuts im Container" % (os.path.basename(path), len(alt)))

    for cut in secs:
        r = maskenbild.bild(room, cut, a.ppm, a.bild)
        if r:
            print("  %s" % r[0])


if __name__ == "__main__":
    main()
