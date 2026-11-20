#!/usr/bin/env python3
"""Runde 42, Strang fabrikplan Stufe 2 (Nachtrag 20.11.2026): Liste der Sprachaufnahmen v0.8.31 fuer die Probe
tests/unit/probe_r42_aufnahmen_v0831.c.

Quelle: die Aufnahmeliste des Orchestrators FEHLENDE_AUFNAHMEN_2026-11-20.md (Teile A, B, C = Spiel v0.8.31; D = v0.8.32).
Der Nutzer hat am 20.11.2026 ~18:30 alle Dateien geliefert ("Ok, saemtliche Sprachaufnahmen liegen vor."), Commit ce5ec405f.
Je Zeile: Teil, Raum (4 Hex), Nummer (= Message-Id = Dateistamm mainNN), Sprecher, Text (ohne Regie-/Altvermerke).

Aufruf: python aufnahmen_v0831_bauen.py <FEHLENDE_AUFNAHMEN_2026-11-20.md> > tests/unit/gen_r42_aufnahmen_v0831.inc
"""
import re
import sys


def zeilen(md):
    teil = ""
    for line in md.splitlines():
        m = re.match(r"## ([A-G])\.", line)
        if m:
            teil = m.group(1)
            continue
        if not teil or teil not in "ABCD" or not line.startswith("| `synchro/"):
            continue
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        p = re.search(r"synchro/STAGE(\d)/room([0-9A-F]{4})/main(\d\d)\.wav", cells[0])
        if not p:
            raise SystemExit("Zeile ohne Pfad: " + line)
        text = re.sub(r"\*\*\(.*?\)\*\*", "", cells[2])
        text = re.sub(r"\*\(.*?\)\*", "", text).strip()
        yield teil, int(p.group(1)), p.group(2), int(p.group(3)), cells[1], text


def c_str(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def main():
    md = open(sys.argv[1], encoding="utf-8").read()
    rows = list(zeilen(md))
    n = {t: sum(1 for r in rows if r[0] == t) for t in "ABCD"}
    if (n["A"], n["B"], n["C"], n["D"]) != (13, 1, 61, 35):
        raise SystemExit("Anzahl je Teil %r (Soll A 13, B 1, C 61, D 35)" % (n,))
    out = sys.stdout
    out.reconfigure(newline="\n")
    out.write("/* ERZEUGT von tools/r42_namen/aufnahmen_v0831_bauen.py - nicht von Hand aendern.\n")
    out.write(" * Quelle: FEHLENDE_AUFNAHMEN_2026-11-20.md (Teile A %d, B %d, C %d = v0.8.31; D %d = v0.8.32), Aufnahmen Commit ce5ec405f.\n"
              % (n["A"], n["B"], n["C"], n["D"]))
    out.write(" * { Teil, Raum, Nummer (= Message-Id = synchro/STAGEn/roomXXXX/mainNN.wav), Sprecher, Text } */\n")
    for teil, stage, raum, nr, sprecher, text in rows:
        if int(raum[0]) != stage:
            raise SystemExit("STAGE %d passt nicht zu room%s" % (stage, raum))
        out.write("    { '%s', 0x%su, %2d, %s, %s },\n" % (teil, raum, nr, c_str(sprecher), c_str(text)))


if __name__ == "__main__":
    main()
