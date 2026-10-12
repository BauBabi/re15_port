#!/usr/bin/env python3
"""gemalt_fluegel.py - Runde 37 Spur C: die gemalten Fahrstuhl-FLUEGEL (RE1.5-Hintergruende) entzerren und mitteln.

Quelle je Bildpunkt: Hintergrund aus der BSS, PPM 320x240 von probe_bg_dump (dieselbe MDEC-Kette wie im Spiel):
    re15_port/build/tests/unit/probe_bg_dump.exe re15_port/shared_assets/PSX <bg> 1
Die Ecken je Fluegel (TL, TR, BR, BL, Bildpunkte 320x240) sind an Helligkeitskanten abgelesen (Zeilen-/Spalten-
profile im Dossier C_fahrstuhl.md "U1"): Kopfbalken (L = 0), Nischenkante, Mittelfuge (Helligkeitsminimum),
Rahmen rechts, Bodenlinie. Ausrichtung jedes entzerrten Fluegels: Spalte 0 = AUSSENKANTE, letzte Spalte = FUGE
(der rechte Fluegel wird dafuer gespiegelt) - so lassen sich beide Fluegel und beide Etagen mitteln.

Ausgabe: <aus>/fluegel_<raum>_<seite>.png (60 x 128), <aus>/feld_flur.png / feld_kabine.png (Mittel der Fluegel,
gerundet; Eingabe von fahrstuhl_archiv_bauen.py), <aus>/profil.json (Zeilen-/Spaltenprofil L, Median/Mittel RGB),
Vorschau <aus>/fluegel_uebersicht.png.
Aufruf: python re15_port/tools/fahrstuhl_r37/gemalt_fluegel.py <bg-Verzeichnis mit ROOMsrrcc.ppm> <aus>
"""
import json
import os
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(HIER, "..", "tueren"))
import zensus_bild as B   # noqa: E402  (homographie / entzerren, Runde 31)

W, H = 60, 128   # entzerrter Fluegel (halbe Texturaufloesung 120 x 256)

# (Raum-Cut, Seite, Fluegel, [TL, TR, BR, BL]) - Bildpunkte, abgelesen (Dossier U1)
FLUEGEL = [
    ("10C00", "links",  [(71, 46), (95, 45), (104, 154), (82, 162)]),
    ("10C00", "rechts", [(95, 45), (116, 43), (118, 152), (104, 154)]),
    ("11200", "links",  [(72, 42), (96, 42), (106, 156), (82, 164)]),
    ("11200", "rechts", [(96, 42), (117, 41), (121.5, 151), (106, 156)]),
    # Kabine von innen (ROOM10800 Cut 0, Aufsicht schraeg): Rahmen innen + Mittelfuge, Etagenanzeige darueber
    ("10800", "links",  [(73, 53), (101, 34), (124, 112), (99, 128)]),
    ("10800", "rechts", [(101, 33), (131, 17), (148, 96), (124, 112)]),
]


def bild(bg, raum_cut):
    return np.asarray(Image.open(os.path.join(bg, "ROOM%s.ppm" % raum_cut)).convert("RGB"))


def main():
    bg = sys.argv[1]
    aus = sys.argv[2]
    os.makedirs(aus, exist_ok=True)
    felder = {"flur": [], "kabine": []}
    vorschau = []
    for rc, seite, quad in FLUEGEL:
        rgb = bild(bg, rc)
        ent, anteil = B.entzerren(rgb, np.array(quad, float), w=W, h=H)
        if seite == "rechts":
            ent = ent[:, ::-1]                      # Fuge nach rechts, Aussenkante links
        Image.fromarray(ent).save(os.path.join(aus, "fluegel_%s_%s.png" % (rc, seite)))
        (felder["kabine"] if rc == "10800" else felder["flur"]).append(ent.astype(np.float64))
        vorschau.append(ent)
    prof = {}
    for k, fs in felder.items():
        f = np.mean(fs, 0)
        Image.fromarray(np.clip(np.round(f), 0, 255).astype(np.uint8)).save(os.path.join(aus, "feld_%s.png" % k))
        L = f @ np.array([0.299, 0.587, 0.114])
        prof[k] = dict(zeilen=[round(float(x), 1) for x in L.mean(1)],
                       spalten=[round(float(x), 1) for x in L.mean(0)],
                       median_rgb=[round(float(x), 1) for x in np.median(f.reshape(-1, 3), 0)],
                       mittel_rgb=[round(float(x), 1) for x in f.reshape(-1, 3).mean(0)])
    json.dump(prof, open(os.path.join(aus, "profil.json"), "w"), indent=1)
    big = np.concatenate([np.pad(v, ((0, 0), (0, 4), (0, 0))) for v in vorschau], 1)
    Image.fromarray(big.astype(np.uint8)).resize((big.shape[1] * 3, big.shape[0] * 3), Image.NEAREST).save(
        os.path.join(aus, "fluegel_uebersicht.png"))
    for k in prof:
        print(k, "median", prof[k]["median_rgb"], "mittel", prof[k]["mittel_rgb"])
        print("  zeilen(16er)", [prof[k]["zeilen"][i] for i in range(0, H, 16)])
        print("  spalten(6er)", [prof[k]["spalten"][i] for i in range(0, W, 6)])


if __name__ == "__main__":
    main()
