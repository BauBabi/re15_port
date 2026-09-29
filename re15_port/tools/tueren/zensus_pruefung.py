#!/usr/bin/env python3
"""zensus_pruefung.py - Pruefung des Blatt-Verfahrens an den von Hand vermessenen Tueren
(analysis/tor_1170/06_massstab.md §4.3, verfeinerte Ecken). Abweichung je Ecke in Pixeln,
einmal fuer die reine Datenlage (Rechteck + Gegen-Ankunft) und einmal nach der
Bild-Verfeinerung (zensus_bild.verfeinern).

Aufruf: python re15_port/tools/tueren/zensus_pruefung.py
Schreibt build/r31_tueren/t1/pruefung.json und pruefung_<raum>_c<cut>.png.
"""
import json
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import zensus_lib as L     # noqa: E402
import zensus_bild as B    # noqa: E402

# 06_massstab.md §4.3 "verfeinert = Messwert" (TL, TR, BR, BL), Guete A (RE1.5) und A/B (RE2)
MESS = [
    ("re15", "1000", 3, (1200, -900, 1000, 2000), [(136.49, 58.55), (195.48, 52.59), (196.37, 173.79), (135.06, 170.42)]),
    ("re15", "1000", 6, (1200, 2200, 1000, 2000), [(155.80, 37.11), (230.12, 36.98), (219.82, 150.97), (157.38, 151.31)]),
    ("re15", "1010", 0, (4150, 5900, 1000, 2000), [(63.21, 56.91), (130.70, 57.18), (133.07, 163.20), (70.64, 164.35)]),
    ("re15", "1010", 4, (4150, -4950, 1000, 2000), [(166.46, 50.01), (240.72, 50.02), (228.73, 162.18), (164.00, 158.11)]),
    ("re15", "2060", 0, (7950, 2950, 1000, 2000), [(142.06, 66.51), (187.87, 58.98), (189.29, 176.95), (142.02, 172.80)]),
    ("re2", "60B0", 4, (-6011, -26269, 1000, 2100), [(126.13, 29.83), (185.77, 30.85), (182.23, 148.35), (131.09, 152.92)]),
    ("re2", "3060", 0, (-17187, -28473, 1720, 1530), [(121.05, 42.32), (165.86, 41.38), (164.77, 146.08), (126.74, 140.64)]),
    ("re2", "5050", 3, (-11806, -26045, 1300, 1500), [(80.72, 30.38), (134.30, 36.29), (133.55, 168.25), (79.46, 171.04)]),
    ("re2", "20E0", 3, (-17100, -13900, 1800, 1800), [(93.86, 75.70), (134.62, 76.82), (136.13, 161.86), (97.48, 165.33)]),
    ("re2", "2040", 0, (4967, -17787, 1930, 2270), [(217.37, 11.36), (281.34, 13.88), (257.11, 122.86), (206.56, 124.15)]),
    ("re2", "40A0", 8, (-8522, -2619, 1800, 3400), [(112.49, 11.66), (169.48, 13.40), (167.78, 111.15), (119.68, 111.06)]),
]


def ankunft_fuer(spiel, raum, satz, alle):
    """Ankunftsorte (x,z) der Rueckrichtung in diesem Raum: Tuersaetze eines anderen Raums
    mit Ziel = dieser Raum, deren Ankunft nahe (<= 2500) am Rechteck liegt."""
    aus = []
    for (nm, t) in alle:
        if L.raumname(t["ziel_stage"], t["ziel_raum"]) != raum[:3] or nm[:3] == raum[:3]:
            continue
        if L.abstand_viereck(satz["pts"], t["ziel"][0], t["ziel"][2]) <= 2500:
            aus.append((t["ziel"][0], t["ziel"][2]))
    return aus


_ALLE = {}


def alle_saetze(spiel):
    if spiel in _ALLE:
        return _ALLE[spiel]
    import glob
    aus = []
    if spiel == "re2":
        pfade = sorted(glob.glob(os.path.join(L.REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT")))
    else:
        pfade = L.re15_rdts()
    for p in pfade:
        nm = os.path.basename(p)[4:8].upper()
        if nm[0] not in "1234567":
            continue
        d = open(p, "rb").read()
        if len(d) < 0x100:
            continue
        try:
            ts = L.re2_tueren(d) if spiel == "re2" else L.re15_tueren(d)
        except Exception:
            continue
        for t in ts:
            aus.append((nm, t))
    _ALLE[spiel] = aus
    return aus


def pruefe(spiel, raum, cut, rect, mess, bilder=True):
    r = L.Raum(spiel, raum)
    ts = [t for t in (L.re2_tueren(r.d) if spiel == "re2" else L.re15_tueren(r.d)) if t["rect"] and tuple(t["rect"]) == tuple(rect)]
    satz = ts[0]
    ank = ankunft_fuer(spiel, raum, satz, alle_saetze(spiel))
    import zensus_ausschnitte as A
    kante, welt, _, _ = A.kante_waehlen(r, satz["pts"], satz["band"], ank)
    cam = r.kamera(cut)
    sv = B.sichtbarkeit(cam, welt, kante)
    mess = np.array(mess, float)
    q0 = np.array(sv["quad"])
    e0 = np.linalg.norm(q0 - mess, axis=1)
    lum = B.helligkeit(r.hintergrund(cut))
    v = B.verfeinern(cam, lum, kante, satz["band"], W0=L.blattbreite(spiel))
    q1 = np.array(v["quad"])
    e1 = np.linalg.norm(q1 - mess, axis=1)
    erg = dict(spiel=spiel, raum=raum, cut=cut, satz="0x%05X" % satz["pc"], band=satz["band"],
               ankunft=ank, kante=kante["grund"], sichtbar=sv["grund"], px_h=sv["px_h"],
               ecken_daten=q0.round(2).tolist(), ecken_verfeinert=q1.round(2).tolist(),
               abw_daten_px=e0.round(2).tolist(), abw_daten_mittel=float(e0.mean()),
               abw_verfeinert_px=e1.round(2).tolist(), abw_verfeinert_mittel=float(e1.mean()),
               dn=v["dn"], dt=v["dt"], W=v["W"], wert=v["wert"], wert_start=v["wert_start"],
               median=v["median"])
    if bilder:
        from PIL import ImageDraw
        rgb = r.hintergrund(cut)
        alle = np.vstack([mess, q0, q1])
        im, box = B.ausschnitt(rgb, alle, f=4)
        B.umriss(im, mess, box, 4, (0, 255, 0))
        B.umriss(im, q0, box, 4, (255, 255, 0))
        B.umriss(im, q1, box, 4, (255, 40, 40))
        os.makedirs(L.AUS, exist_ok=True)
        ziel = os.path.join(L.AUS, "pruefung_%s_%s_c%02d.png" % (spiel, raum, cut))
        im.save(ziel)
        erg["bild"] = L.rel(ziel)
    return erg


def main():
    aus = []
    for (spiel, raum, cut, rect, mess) in MESS:
        e = pruefe(spiel, raum, cut, rect, mess)
        aus.append(e)
        print("%-4s ROOM%s c%d  Daten %5.1f px  verfeinert %5.1f px  (dn %4.0f dt %4.0f W %4.0f; Wert %.1f / Start %.1f / Median %.1f) %s" % (
            spiel, raum, cut, e["abw_daten_mittel"], e["abw_verfeinert_mittel"], e["dn"], e["dt"], e["W"],
            e["wert"], e["wert_start"], e["median"], e["kante"]))
    for spiel in ("re15", "re2"):
        a = [e for e in aus if e["spiel"] == spiel]
        print("%s: Mittel Daten %.1f px, verfeinert %.1f px" % (
            spiel, np.mean([e["abw_daten_mittel"] for e in a]), np.mean([e["abw_verfeinert_mittel"] for e in a])))
    json.dump(aus, open(os.path.join(L.AUS, "pruefung.json"), "w"), indent=1)
    return aus


if __name__ == "__main__":
    main()
