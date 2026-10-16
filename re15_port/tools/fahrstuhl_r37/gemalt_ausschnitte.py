#!/usr/bin/env python3
"""gemalt_ausschnitte.py - Runde 37 Spur C "fahrstuhl": die GEMALTEN Fahrstuhltueren (RE1.5-Hintergruende)
ausschneiden und entzerren (analysis/befunde_runde37/C_fahrstuhl.md, Abschnitt "Messung vorher / gemalt").

Werkzeug der Runde 31 (tools/tueren/zensus_ausschnitte.schneide: Kantenwahl, Projektion mit der exakten Kamera
der RDT, Verfeinerung an Helligkeitskanten, Homographie auf 128 x 218) - hier fuer die sechs Fahrstuhl-Einstiege
und die Kabinentuer von innen. Eingaben je Seite = Door_aot_set-Rechteck (Datei-Offset im Kommentar) und die
Ankunft der Gegenseite (Null-Rechteck-Tuer der Kabine, Spawn x/z). Hintergruende: extracted/PSX/STAGEn/ROOMsrr/
(aus probe_bg_dump, PPM -> BMP, dieselbe MDEC-Kette wie im Spiel).

Aufruf: python re15_port/tools/fahrstuhl_r37/gemalt_ausschnitte.py <ausgabeverzeichnis>
Alle Werte unten sind DATEN-Bytes der RDTs (selbst gelesen), keine Schaetzung.
"""
import json
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, "..", "tueren"))
import zensus_lib as L          # noqa: E402
import zensus_ausschnitte as A  # noqa: E402


def rechteck(x, z, w, d):
    return [(x, z), (x + w, z), (x + w, z + d), (x, z + d)]


SEITEN = [
    # Kennung, Raum, Rechteck (Door_aot_set pc[6..13]), Band, Ankunft der Gegenseite (Kabinen-Ausgang)
    # ROOM1040.RDT @0x01096 Slot 0: x 0xA668 z 0xD508 w 0x08FC d 0x0578; ROOM1080.RDT @0x00482 -> (-21936, -11000)
    ("S013", "1040", rechteck(-22936, -11000, 2300, 1400), 0, [(-21936, -11000)]),
    # ROOM10C0.RDT @0x00E82 Slot 1: x 0x01F4 z 0x170C w 0x07D0 d 0x03E8; ROOM1080.RDT @0x004A2 -> (1450, 7300)
    ("S040", "10C0", rechteck(500, 5900, 2000, 1000), 0, [(1450, 7300)]),
    # ROOM1120.RDT @0x00CB6 Slot 1: x 0x012C z 0x170C w 0x07D0 d 0x03E8; ROOM1080.RDT @0x004C2 -> (1300, 7300)
    ("S057", "1120", rechteck(300, 5900, 2000, 1000), 0, [(1300, 7300)]),
    # ROOM4000.RDT @0x0107A Slot 0: x 0x9430 z 0xD508 w 0x07D0 d 0x1004; ROOM4020.RDT @0x0063A -> (-26516, -8963)
    ("S215", "4000", rechteck(-27600, -11000, 2000, 4100), 0, [(-26516, -8963)]),
    # ROOM5000.RDT @0x008A4 Slot 3: x 0xCEFA z 0xC11C w 0x0FA0 d 0x03E8; ROOM4020.RDT @0x0065A -> (-10550, -14750)
    ("S260", "5000", rechteck(-12550, -16100, 4000, 1000), 0, [(-10550, -14750)]),
    # ROOM50C0.RDT @0x0094A Slot 1: x 0xDECC z 0xD440 w 0x0640 d 0x0D48; ROOM4020.RDT @0x0067A -> (-6975, -10005)
    ("S290", "50C0", rechteck(-8500, -11200, 1600, 3400), 0, [(-6975, -10005)]),
]


def main():
    aus = sys.argv[1] if len(sys.argv) > 1 else os.path.join(L.REPO, "build", "r37_fahrstuhl", "gemalt")
    os.makedirs(aus, exist_ok=True)
    erg = {}
    for kennung, raum, pts, band, ankunft in SEITEN:
        r = A.schneide("re15", L.Raum("re15", raum), pts, band, ankunft, "%s_ROOM%s" % (kennung, raum), aus,
                       max_cuts=3)
        erg[kennung] = r
        print("%s ROOM%s: Kante %s, gewaehlt %s" % (kennung, raum, r["kante"]["grund"],
              [(g["cut"], g["W"], round(g["dn"]), round(g["dt"])) for g in r["gewaehlt"]]))
    with open(os.path.join(aus, "ausschnitte.json"), "w") as f:
        json.dump(erg, f, indent=1, default=lambda o: o.item() if hasattr(o, "item") else str(o))


if __name__ == "__main__":
    main()
