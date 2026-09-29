#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""unterteilung_vergleich.py <vorher> <nachher> - Runde 32 (analysis/befunde_runde32/tueren_unterteilung.md).

Vergleicht zwei Laeufe von unterteilung_lauf.sh: je Seite Anfang + Mitte bitgleich, gruppiert nach Archiv
(zuordnung.json); Archive OHNE Objekt-Flag 0x20 muessen gleich bleiben. Dazu die Tor-Serien tor0/tor1.
"""
import collections, filecmp, glob, json, os, sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
ZUORDNUNG = os.path.join(REPO, "analysis", "befunde_runde31", "tueren_03", "zuordnung.json")
# Archive mit Door_model_set-Flag 0x20 (tuerskript_dump.py dis, alle Skripte; Abschnitt 1.5 des Dossiers)
TEILEN = {"DOOR06", "DOOR09", "DOOR13", "DOOR15", "DOOR1A", "DOOR1B", "DOOR1C", "DOOR1D", "DOOR23", "DOOR24", "DOOR25", "DOOR29"}


def main(va, na):
    d = json.load(open(ZUORDNUNG, encoding="utf-8"))
    arch = {s["id"]: s["archiv"] for t in d["tueren"] for s in t["seiten"] if s.get("abgedeckt")}
    g = collections.defaultdict(lambda: [0, 0, []]); fehl = []
    for sid, a in sorted(arch.items(), key=lambda k: int(k[0][1:])):
        for art in ("anfang", "mitte"):
            f1 = os.path.join(va, "bogen", "%s_%s.ppm" % (sid, art)); f2 = os.path.join(na, "bogen", "%s_%s.ppm" % (sid, art))
            if not (os.path.exists(f1) and os.path.exists(f2)):
                fehl.append(sid + "_" + art); continue
            gl = filecmp.cmp(f1, f2, shallow=False)
            g[a][0 if gl else 1] += 1
            if not gl and a not in TEILEN: g[a][2].append(sid + "_" + art)
    for a in sorted(g):
        print("%s %-7s gleich %3d  verschieden %3d %s" % (a, "(0x20)" if a in TEILEN else "(ohne)", g[a][0], g[a][1], " ".join(g[a][2])))
    ohne_v = sum(g[a][1] for a in g if a not in TEILEN); ohne_g = sum(g[a][0] for a in g if a not in TEILEN)
    print("ohne 0x20: gleich %d verschieden %d; mit 0x20: gleich %d verschieden %d; fehlend %d"
          % (ohne_g, ohne_v, sum(g[a][0] for a in g if a in TEILEN), sum(g[a][1] for a in g if a in TEILEN), len(fehl)))
    ok = ohne_v == 0 and not fehl
    for v in (0, 1):
        fs = sorted(glob.glob(os.path.join(va, "tor%d" % v, "*.ppm")))
        if not fs: continue
        n = sum(1 for f in fs if os.path.exists(os.path.join(na, "tor%d" % v, os.path.basename(f)))
                and filecmp.cmp(f, os.path.join(na, "tor%d" % v, os.path.basename(f)), shallow=False))
        print("tor%d: %d von %d Bildern gleich" % (v, n, len(fs)))
        ok = ok and n == len(fs)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1], sys.argv[2]))
