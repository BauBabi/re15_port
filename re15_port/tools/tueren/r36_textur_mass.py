#!/usr/bin/env python3
"""r36_textur_mass.py - Runde 36 Spur G (Punkt 2): Mass fuer "herausstechend billig" einer Tuertextur.

Nutzer: "Die Tür Textur zwischen ROOM 10C0 und 10D0 bei der Türanimation ist herausstechend billig im Vergleich
zum Rest." -> "der Rest" = die uebrigen Tuersequenzen = 55 RE2-Archive (info/re2leon/COMMON/DOOR) + die
Port-Archive (shared_assets/RE15DOOR). Gemessen wird das Tuerblatt (v 0..217, Mesh 0 aller Standardblaetter,
tueren_rest_plan.md 2.1) in Helligkeit L (Rec.601):

  fein   = std(L - G1(L)) / mean(L)          Feinzeichnung (Faser, Kratzer, Kanten) auf 1-Texel-Skala
  fleck  = std(G1(L) - G4(L)) / mean(L)      Mittelskala (Flecken 2..8 Texel)
  form   = std(G4(L)) / mean(L)              Grossform (Felder, Rahmen, Kantenschatten, Fenster)
  scharf = fein / fleck                      < 1: "verschwommene Flecken" (hochgezogenes Rauschen), RE2 > 1

(G_s = Gauss mit sigma s Texel.) Messwerte, kein Geschmacksurteil: Ausreisser gegen die RE2-Verteilung.

Aufruf: python r36_textur_mass.py [--json <pfad>] [--extra NAME=pfad.DO2 ...]
"""
import argparse
import glob
import json
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
sys.path.insert(0, os.path.join(PORT, "tools", "tor"))
import do2_format as F  # noqa: E402

LUM = np.array([0.299, 0.587, 0.114])
BV = 218


def _gauss(a, s):
    r = int(3 * s + 0.5)
    x = np.arange(-r, r + 1, dtype=np.float64)
    k = np.exp(-x * x / (2 * s * s)); k /= k.sum()
    p = np.pad(a, r, mode="reflect")
    p = np.apply_along_axis(lambda v: np.convolve(v, k, "valid"), 1, p)
    return np.apply_along_axis(lambda v: np.convolve(v, k, "valid"), 0, p)


def blatt_l(pfad):
    d = open(pfad, "rb").read()
    do2 = F.Do2.lesen(d)
    t, _ = F.Tim.lesen(do2.tim)
    rgba = np.asarray(t.als_bild(), np.float64)
    L = rgba[:BV, :, :3] @ LUM
    sichtbar = rgba[:BV, :, 3] > 0
    return L, sichtbar


def mass(pfad):
    L, s = blatt_l(pfad)
    if s.mean() < 0.5:              # objektlose/Gitter-Archive: zu wenig deckende Texel
        return None
    g1, g4 = _gauss(L, 1.0), _gauss(L, 4.0)
    m = max(float(L[s].mean()), 1.0)
    fein = float((L - g1)[s].std()) / m
    fleck = float((g1 - g4)[s].std()) / m
    form = float(g4[s].std()) / m
    return dict(fein=round(fein, 4), fleck=round(fleck, 4), form=round(form, 4),
                scharf=round(fein / max(fleck, 1e-6), 3), hell=round(m, 1))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", default="")
    ap.add_argument("--extra", nargs="*", default=[])
    a = ap.parse_args()
    zeilen = {}
    for p in sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "COMMON", "DOOR", "DOOR*.DO2"))):
        r = mass(p)
        if r: zeilen["RE2 " + os.path.basename(p)[:-4]] = r
    for p in sorted(glob.glob(os.path.join(PORT, "shared_assets", "RE15DOOR", "*.DO2"))):
        r = mass(p)
        if r: zeilen["PORT " + os.path.basename(p)[:-4]] = r
    for e in a.extra:
        n, p = e.split("=", 1)
        r = mass(p)
        if r: zeilen["NEU " + n] = r
    re2 = np.array([[v["fein"], v["fleck"], v["form"], v["scharf"]] for k, v in zeilen.items() if k.startswith("RE2")])
    lo, hi = re2.min(0), re2.max(0)
    print("RE2 (%d Texturen): fein %.4f..%.4f  fleck %.4f..%.4f  form %.4f..%.4f  scharf %.3f..%.3f" % (
        len(re2), lo[0], hi[0], lo[1], hi[1], lo[2], hi[2], lo[3], hi[3]))
    print("RE2 Median: fein %.4f fleck %.4f form %.4f scharf %.3f" % tuple(np.median(re2, 0)))
    for k, v in sorted(zeilen.items(), key=lambda kv: kv[1]["scharf"]):
        aus = []
        for i, n in enumerate(("fein", "fleck", "form", "scharf")):
            if v[n] < lo[i]: aus.append(n + "<RE2min")
            if v[n] > hi[i]: aus.append(n + ">RE2max")
        print("%-12s fein %.4f fleck %.4f form %.4f scharf %.3f hell %5.1f %s" % (
            k, v["fein"], v["fleck"], v["form"], v["scharf"], v["hell"], " ".join(aus)))
    if a.json:
        json.dump(dict(re2_min=lo.tolist(), re2_max=hi.tolist(), re2_median=np.median(re2, 0).tolist(), zeilen=zeilen),
                  open(a.json, "w"), indent=1)


if __name__ == "__main__":
    main()
