# -*- coding: utf-8 -*-
"""Auswertung einer mutanten_teil.py-Kampagne: <ordner>/ergebnis.tsv + Protokoll -> Zaehlung je Klasse/Funktion,
Ueberlebende mit Beschreibung, Zeitgrenzen. Aufruf: mutanten_auswertung.py <ordner> <protokoll.txt>"""
import collections
import re
import sys

ordner, protokoll = sys.argv[1], sys.argv[2]
beschr = {}
for z in open(protokoll, encoding="utf-8", errors="replace"):
    m = re.match(r"(ERKANNT|UEBERLEBT)\s+rc=(-?\d+)\s+(\d+)s\s+(\S+)\s+(.*)$", z.rstrip("\n"))
    if m:
        beschr[m.group(4)] = m.group(5)
je_klasse = collections.defaultdict(lambda: [0, 0, 0])      # gesamt, ueberlebt, zeitgrenze
ueber, zeit = [], []
for z in open(ordner + "/ergebnis.tsv", encoding="utf-8"):
    name, rc, rot, dauer, _k = z.rstrip("\n").split("\t")
    k = name[0]
    je_klasse[k][0] += 1
    if rc == "0":
        je_klasse[k][1] += 1
        ueber.append(name)
    if rc == "-9":
        je_klasse[k][2] += 1
        zeit.append(name)
gesamt = sum(v[0] for v in je_klasse.values())
print("Mutanten: %d, ueberlebt: %d, davon Zeitgrenze erkannt: %d" % (gesamt, len(ueber), len(zeit)))
for k in sorted(je_klasse):
    g, u, t = je_klasse[k]
    print("  Klasse %s: %3d gesamt, %3d erkannt, %3d ueberlebt, %3d per Zeitgrenze" % (k, g, g - u, u, t))
print("Ueberlebende:")
for n in ueber:
    print("  %-46s %s" % (n, beschr.get(n, "")))
print("Zeitgrenze (Gate haengt -> Selbsttest faellt ebenfalls):")
for n in zeit:
    print("  %-46s %s" % (n, beschr.get(n, "")))
