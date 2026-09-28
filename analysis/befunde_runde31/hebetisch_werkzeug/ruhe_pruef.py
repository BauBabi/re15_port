# -*- coding: utf-8 -*-
"""Runde 31 / H — Auswertung des Mess-Protokolls RE15_HEBETISCH_LOG (hebetisch_1150.c):
je Fahrt letztes Bewegungsbild vor der Ruhe, erstes Ruhebild, erstes Bild mit offener Aufnahme,
Bilder mit offener Aufnahme und wie viele davon NICHT auf y=-1205, Ruhebilder ohne Aufnahme und
erstes Bild der Abfahrt. ruhe_pruef.py <hebetisch.log>"""
import re, sys
z = []
for l in open(sys.argv[1]):
    m = re.match(r'F(\d+) y=(-?\d+) ruht=(\d) pc=(-?\d+) modal=(\d+)', l)
    if m:
        z.append(tuple(int(v) for v in m.groups()))
fahrt = []
akt = None
for i, (f, y, r, pc, mo) in enumerate(z):
    if akt is None and y > -5000:
        akt = dict(start=f, bew=None, ruhe=None, modal=None, n_modal=0, n_falsch=0, ruhe_ohne=0, ab=None, pcs=set())
    if akt is None:
        continue
    if i and z[i - 1][1] != y and akt['ruhe'] is None:
        akt['bew'] = f
    if r and akt['ruhe'] is None:
        akt['ruhe'] = f
    if r:
        akt['pcs'].add(pc)
    if mo:
        akt['n_modal'] += 1
        if akt['modal'] is None: akt['modal'] = f
        if y != -1205: akt['n_falsch'] += 1
    elif r:
        akt['ruhe_ohne'] += 1
    if akt['ruhe'] is not None and akt['ab'] is None and not r and y != -1205:
        akt['ab'] = f
    if y <= -5000:
        fahrt.append(akt); akt = None
if akt: fahrt.append(akt)
for k, a in enumerate(fahrt, 1):
    print('Fahrt %d (Pos_set -305 in Bild %d): letzte Bewegung vor der Ruhe Bild %s, erstes Ruhebild %s, '
          'erstes Bild mit offener Aufnahme %s; Bilder mit offener Aufnahme %d, davon NICHT y=-1205: %d; '
          'Ruhebilder ohne Aufnahme %d; Abfahrt ab Bild %s; sub04-PC in der Ruhe %s'
          % (k, a['start'], a['bew'], a['ruhe'], a['modal'], a['n_modal'], a['n_falsch'], a['ruhe_ohne'],
             a['ab'], ', '.join('0x%04X' % p for p in sorted(a['pcs']))))
