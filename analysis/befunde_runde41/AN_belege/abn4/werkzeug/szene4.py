#!/usr/bin/env python3
"""szene4.py - Abnahme 4: Reihenfolge der Annette-Szenenschritte im debug.log (Schrittliste wie integration_r41_annette leon_tor)
und Kurzfassung (Bild je Schritt). Aufruf: python szene4.py <debug.log>"""
import sys, re
d = open(sys.argv[1], errors="replace").read()
schritte = ["Ton 0 ", "Szenenstart statt sub04", "Ton 1 ", "Sturz:", "gelandet, Drohen:", "Ton 2 ", r"Hieb \(RE2",
            r"Ausholen \(RE2", "Blut-Skala", "Treffer:", "Schnitt weg, liegt, Schrei", r"Kampfstart \(sub04\), Form frei"]
pos = -1; ok = True
for s in schritte:
    m = re.search(r"\[annette\] F(\d+) " + s, d)
    if not m: print("FEHLT:", s); ok = False; continue
    p = m.start(); f = m.group(1)
    if p < pos: print("REIHENFOLGE falsch:", s); ok = False
    pos = p; print("F%-5s %s" % (f, s.replace("\\", "")))
for k in ("Ersatzmodell RE2/ANNETTE_4D.EMD", "Ersatzmodell RE2/ANNETTE_4D_BLUT.EMD", "Wiedereintritt nach der Szene: Leiche liegt",
          "Frau im Kittel (0x4D, @0x0122A) verworfen", "EXIT_AT"):
    print("%-48s %s" % (k, "ja" if k in d else "nein"))
print("Reihenfolge vollstaendig:", ok)
