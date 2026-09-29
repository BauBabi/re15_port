#!/usr/bin/env python3
"""zensus_alles.py - T1 komplett: Zensus, Ausschnitte (RE1.5 + RE2), Mass, Blaetter, Boegen.

Aufruf (Repo-Wurzel):
  python re15_port/tools/tueren/zensus_alles.py          # alles neu (ca. 6 min)
  python re15_port/tools/tueren/zensus_alles.py --bilder-behalten   # Ausschnitte nicht neu rechnen

Ausgaben unter build/r31_tueren/t1/:
  zensus.json            alles maschinenlesbar (Saetze, Seiten mit Ausschnitten und Top-5,
                         Tueren mit Kategorie, RE2-Seiten, Validierung, Pruefung)
  re15_seiten/  re2_seiten/   Ausschnitte (_aus 3-fach), entzerrte Blaetter (_entz 128x218), Vollbilder
  re2/DOORxx.png, re2/uebersicht.png
  boegen/ROOMxxx0.png
"""
import collections
import datetime
import json
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import zensus_lib as L          # noqa: E402
import zensus_paare as P        # noqa: E402
import zensus_ausschnitte as A  # noqa: E402
import zensus_mass as M         # noqa: E402
import zensus_bogen as G        # noqa: E402
import zensus_pruefung as PR    # noqa: E402


def _j(o):
    return json.loads(json.dumps(o, default=lambda x: x.tolist() if hasattr(x, "tolist") else
                                 (sorted(x) if isinstance(x, set) else str(x))))


def main(argv):
    from PIL import Image
    behalten = "--bilder-behalten" in argv
    os.makedirs(L.AUS, exist_ok=True)
    Z = P.bauen()
    P.bericht(Z)
    j15 = os.path.join(L.AUS, "re15_seiten.json")
    j2 = os.path.join(L.AUS, "re2_seiten.json")
    if not behalten or not os.path.exists(j15):
        a15 = A.re15_alle(Z)
        json.dump(A._json(a15), open(j15, "w"), indent=0)
    if not behalten or not os.path.exists(j2):
        a2 = A.re2_alle()
        json.dump(A._json(a2), open(j2, "w"), indent=0)
    a15 = json.load(open(j15))
    a2 = json.load(open(j2))
    print("Ausschnitte: RE1.5 %d Seiten (%d sichtbar), RE2 %d Seiten (%d sichtbar)" % (
        len(a15), sum(1 for v in a15.values() if v["gewaehlt"]), len(a2), sum(1 for v in a2 if v.get("gewaehlt"))))
    # Mass: Validierung und Top-5 je RE1.5-Seite (Maximum ueber die gewaehlten Cuts)
    val = M.validieren()
    ref = M.Referenz()
    for s in Z["seiten"]:
        s["ausschnitte"] = a15.get(s["id"])
        if s["ausschnitte"]:
            k = s["ausschnitte"]["kante"]
            # Laenge der Tuerkante: RE1.5-Standard 2000 (06_massstab §6); >= 3000 = breite Oeffnung
            # (Doppeltuer, Tor) - nur Hinweis fuer die Bildwelle
            s["kantenlaenge"] = round(float(np.hypot(k["b"][0] - k["a"][0], k["b"][1] - k["a"][1])))
        if not s["ausschnitte"] or not s["ausschnitte"]["gewaehlt"]:
            s["kandidaten"] = []
            continue
        best = {}
        for g in s["ausschnitte"]["gewaehlt"]:
            rgb = np.asarray(Image.open(os.path.join(L.REPO, g["entzerrt"])).convert("RGB"))
            for r in ref.werte(rgb):
                if r["archiv"] not in best or r["wert"] > best[r["archiv"]]["wert"]:
                    r = dict(r, cut=g["cut"])
                    best[r["archiv"]] = r
        s["kandidaten"] = sorted(best.values(), key=lambda r: -r["wert"])[:5]
    pruef = PR.main()
    blaetter = G.re2_blaetter()
    boegen = G.kontaktboegen(_j(Z))
    # Zahlen
    tueren = Z["tueren"]
    kat = collections.Counter(t["kategorie"] for t in tueren if t["physisch"])
    seiten = Z["seiten"]
    begehbar = [s for s in seiten if not s["nullflaeche"]]
    sichtbar = [s for s in begehbar if s["ausschnitte"] and s["ausschnitte"]["gewaehlt"]]
    phys = [t for t in tueren if t["physisch"]]
    ohne_cut = []
    for t in phys:
        ss = [s for s in seiten if s.get("tuer") == t["id"] and not s["nullflaeche"]]
        if not any(s["ausschnitte"] and s["ausschnitte"]["gewaehlt"] for s in ss):
            ohne_cut.append(t["id"])
    raeume = collections.OrderedDict()
    for s in begehbar:
        raeume.setdefault(s["basis"] + "0", 0)
        raeume[s["basis"] + "0"] += 1
    zahlen = dict(
        rdt=len(L.re15_rdts()), stubs=len(Z["stubs"]), saetze=len(Z["saetze"]),
        saetze_viereck=sum(1 for s in Z["saetze"] if s["form"] == "viereck"),
        tuerseiten=len(seiten), tuerseiten_flaeche0=len(seiten) - len(begehbar),
        tuerseiten_begehbar=len(begehbar), tuerseiten_sichtbar=len(sichtbar),
        zwillingsseiten=sum(1 for s in seiten if s.get("zwilling_von")),
        engine_zeilen=Z["engine_zeilen"], engine_ohne_satz=len(Z["engine_fehlend"]),
        tueren=len(tueren), physische_tueren=len(phys),
        physisch_inert=sum(1 for t in phys if t["inert"]),
        paare=dict(collections.Counter(p["art"] for p in Z["paare"])),
        kategorien_physisch=dict(kat),
        kategorien_alle=dict(collections.Counter(t["kategorie"] for t in tueren)),
        physisch_ohne_sichtbaren_cut=ohne_cut,
        re2_seiten=len(a2), re2_sichtbar=sum(1 for v in a2 if v.get("gewaehlt")),
    )
    aus = dict(stand=datetime.datetime.now().isoformat(timespec="seconds"),
               werkzeug="re15_port/tools/tueren/zensus_alles.py", zahlen=zahlen,
               raeume=[dict(raum=r, seiten=n, bogen=boegen.get(r[:3], {}).get("bogen")) for r, n in raeume.items()],
               alias=Z["alias"], lifte=Z["lifte"], stubs=Z["stubs"],
               tueren=tueren, seiten=seiten, saetze=Z["saetze"], paare=Z["paare"],
               re2_seiten=a2, re2_blaetter=blaetter, validierung=val, pruefung=pruef)
    json.dump(_j(aus), open(os.path.join(L.AUS, "zensus.json"), "w"), indent=1)
    print(json.dumps(zahlen, indent=1))
    return aus


if __name__ == "__main__":
    main(sys.argv[1:])
