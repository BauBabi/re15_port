"""Alle Gegenmodelle bauen und mit DERSELBEN Figur messen.

Gemessen wird mit probe_r22_10f0_figur (posierte PL00, Renderer-Transform, Urteil
depth < (1023*vz)>>16) gegen den jeweiligen Maskenbaum (R22_MASKEN_DIR).

Ausgegeben je Variante und Cut:
   Texel     Deckung der Maske
   MARKE     verdeckte / gezeichnete Figurpunkte an der Nutzer-Marke F335
   PFAD      ueber die wirklich gespielten Bilder (befund.log): Bilder ueber der
             Haelfte und Anteil verdeckter Figurpunkte
   ZONE      ueber die begehbaren Standplaetze der Anker-Zone
"""
import os
import re
import subprocess
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
os.chdir(WURZEL)
sys.path.insert(0, HIER)

import variante

SONDE = os.path.abspath("build_r22/tests/unit/probe_r22_10f0_figur.exe")
PFAD = "analysis/befunde_2026-09-21/10f0-quader-silhouette/messung/pfad_10f0_aus_befundlog.txt"

VARIANTEN = [
    ("v0_auslieferung", {}),
    ("w1_ohnekunst", dict(ohne_doppel=True, dunkel="ohnekunst")),
    ("w3_fern", dict(ohne_doppel=True, dunkel="fern")),
]


def messen(verz, cut):
    env = dict(os.environ)
    env["R22_MASKEN_DIR"] = os.path.abspath(verz)
    p = subprocess.run([SONDE, str(cut), "0", PFAD], capture_output=True, text=True, env=env)
    t = p.stdout
    g = {}
    m = re.search(r"MASKEN texel=(\d+)", t)
    g["texel"] = int(m.group(1)) if m else -1
    m = re.search(r"MASKEN cut=\d+ rechtecke=(\d+)", t)
    g["rects"] = int(m.group(1)) if m else -1
    m = re.search(r"MARKE verdeckt=(\d+) frei=(\d+) anteil=([\d.]+)%", t)
    if m:
        g["marke_verd"], g["marke_frei"], g["marke_p"] = int(m.group(1)), int(m.group(2)), float(m.group(3))
    m = re.search(r"PFADSUMME cut=\d+ zeilen=\d+ davon_cut=(\d+) mitkontakt=(\d+) ueberhalb=(\d+) "
                  r"figurpunkte=(\d+) verdeckt=(\d+) anteil=([\d.]+)%", t)
    if m:
        g["pfad_bilder"] = int(m.group(1)); g["pfad_kontakt"] = int(m.group(2))
        g["pfad_ueber"] = int(m.group(3)); g["pfad_p"] = float(m.group(6))
    m = re.search(r"ZONE cut=\d+ plaetze=(\d+) mitkontakt=(\d+) ueberhalb=(\d+) "
                  r"figurpunkte=(\d+) verdeckt=(\d+) anteil=([\d.]+)%", t)
    if m:
        g["zone_pl"] = int(m.group(1)); g["zone_kontakt"] = int(m.group(2))
        g["zone_ueber"] = int(m.group(3)); g["zone_p"] = float(m.group(6))
    return g


if __name__ == "__main__":
    print("%-22s %3s %6s %5s | %5s %5s %6s | %4s %4s %6s | %5s %4s %6s"
          % ("Variante", "cut", "texel", "rect", "verd", "ges", "MARKE",
             "Bild", "ueb", "PFAD", "kont", "ueb", "ZONE"))
    for name, sch in VARIANTEN:
        verz = os.path.join("build/r22", name)
        import io
        alt = sys.stdout
        sys.stdout = io.StringIO()
        try:
            variante.bauen(verz, **sch)
        finally:
            sys.stdout = alt
        for cut in (4, 5):
            g = messen(verz, cut)
            print("%-22s %3d %6d %5d | %5d %5d %5.1f%% | %4d %4d %5.2f%% | %5d %4d %5.2f%%"
                  % (name, cut, g["texel"], g["rects"], g["marke_verd"],
                     g["marke_verd"] + g["marke_frei"], g["marke_p"],
                     g["pfad_bilder"], g["pfad_ueber"], g["pfad_p"],
                     g["zone_kontakt"], g["zone_ueber"], g["zone_p"]))
