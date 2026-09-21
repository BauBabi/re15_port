"""Die sechs doppelten Quader-Eintraege aus auswahl.json entfernen und das _warum
um die Runde 22 ergaenzen. Einmalig; liegt beim Dossier, damit die Aenderung
nachvollziehbar bleibt.
"""
import collections
import io
import json
import os

os.chdir(os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                      "..", "..", "..", "..")))

P = "analysis/esp_masken_2026-09-03/auswahl.json"
WEG = {"4": [(-1600, 12200), (-1800, 8200), (-1900, 4300)],
       "5": [(1600, 5700), (1600, 1900), (1500, -2000)]}
NEU = (
    "\n⛔ 2026-09-21 Nutzer-Marke befund_10F0_F335_marke1.bmp (\"Leon ist da grossteils "
    "transparent\"), Runde 22: an der Marke waren 426 von 837 gezeichneten Figurpunkten "
    "verdeckt. 272 davon durch die Freistellung des Stuhls (richtig, er steht davor), 154 "
    "durch Tiefschwarz - und ALLE 154 trugen das Etikett von Zelle 17, also der Zelle MIT "
    "Freistellung. Zwei Aenderungen, beide gemessen "
    "(analysis/befunde_2026-09-21/10f0-umsetzung.md):\n"
    "  (1) Die Quader-Eintraege der drei Zellen, die schon ein szene-Lasso tragen, sind WEG. "
    "Sie bauten dasselbe Objekt ein zweites Mal (C4 30 von 105 Maskenplaetzen, C5 39 von 105); "
    "die gedeckte Flaeche bleibt punktgenau gleich.\n"
    "  (2) raum.py: Regel 1 ist jetzt woertlich das, was ihr Kommentar sagt - nur die LOECHER "
    "der eigenen Kunst statt eines Saums von 7 Bildpunkten (gemessen hat die Freistellung in C4 "
    "GENAU 0 und in C5 GENAU 1 dunkles Loch, die Saum-Begruendung trug also nicht); Regel 2 "
    "bedient nur noch Zellen OHNE Freistellung. Wirkung: Marke 50,9 -> 32,5 % (Zugabe 0), "
    "gespielter Weg C4 12 von 30 Bildern ueber der Haelfte -> 0, Beruehrung an begehbaren "
    "Standplaetzen 152 -> 153 (Pulte und Konsolenbank behalten ihre Maske).")


def main():
    d = json.loads(io.open(P, encoding="utf-8").read(),
                   object_pairs_hook=collections.OrderedDict)
    r = d["ROOM10F0"]
    for c, lst in WEG.items():
        objs = r[c]["objekte"]
        vor = len(objs)
        r[c]["objekte"] = [o for o in objs
                           if not ("quader" in o
                                   and (int(o["quader"][0]), int(o["quader"][1])) in lst)]
        print("cut %s: %d -> %d Objekte" % (c, vor, len(r[c]["objekte"])))
        if NEU not in r[c]["_warum"]:
            r[c]["_warum"] = r[c]["_warum"] + NEU
    io.open(P, "w", encoding="utf-8", newline="\n").write(
        json.dumps(d, ensure_ascii=False, indent=1) + "\n")
    print("geschrieben: %s" % P)


if __name__ == "__main__":
    main()
