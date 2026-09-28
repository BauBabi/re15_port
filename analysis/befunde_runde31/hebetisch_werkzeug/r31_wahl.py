# -*- coding: utf-8 -*-
"""Runde 31 / H — Wahl des Sitzpaars ueber den Rasterer (r31_raster.Szene), NUR PLANUNG.
Stufe 1: jeder zulaessige Sitz (r31_suche: Kuppel zu/offen, Achteck) allein gerendert, Start
(Plattform y=-305) und oben (y=-1205, Stand im Sleep 30 @0x101A), Deckel offen (+-150).
Stufe 2: die besten Kombinationen gemeinsam gerendert (gegenseitige Verdeckung), Abstand >= 1.
Ziel: Granate LINKS, Sicherung RECHTS (Schirm-Schwerpunkte), beide gut sichtbar."""
import os, sys, itertools
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
import r31_raster as RS  # noqa
import r31_suche as Su  # noqa
import r31_geo as G  # noqa

B = os.path.join(H, '..', '..', '..', 'build', 'r31_hebetisch')
sz = RS.Szene()
sich, gran = G.modelle()
S_ok = np.load(os.path.join(B, 's_ok.npy'))
G_ok = np.load(os.path.join(B, 'g_ok.npy'))


def einzeln(liste, art):
    aus = []
    for e in liste:
        if art == 'S':
            sitz = (int(e[1]), -1062, int(e[2]), int(e[0]))
            r = [RS.Szene.auswerten(sz.render(py, 150, s_sitz=sitz))['S'] for py in (-305, -1205)]
        else:
            sitz = (int(e[1]), int(e[2]), int(e[3]), int(e[0]))
            r = [RS.Szene.auswerten(sz.render(py, 150, g_sitz=sitz))['G'] for py in (-305, -1205)]
        if r[0][0] and r[1][0]:
            aus.append((sitz, r[0][0], r[0][1], r[1][0], r[1][1]))
    return aus


# Stufe 1 — Unterstichprobe (Raster 20 in x/z fuer die Sicherung, 10/10 fuer die Granate)
s_liste = [s for s in S_ok if int(s[1]) % 20 == 0 and int(s[2]) % 20 == 0 and int(s[0]) % 128 == 0]
g_liste = [g for g in G_ok if int(g[1]) % 10 == 0 and int(g[3]) % 10 == 0 and int(g[0]) % 512 == 0 and int(g[2]) == -1091]
print('Stufe 1: %d Sicherungs-, %d Granaten-Sitze' % (len(s_liste), len(g_liste)))
se = einzeln(s_liste, 'S')
ge = einzeln(g_liste, 'G')
np.save(os.path.join(B, 'se.npy'), np.array([list(s[0]) + list(s[1:]) for s in se]))
np.save(os.path.join(B, 'ge.npy'), np.array([list(g[0]) + list(g[1:]) for g in ge]))
# rechts-staerkste Sicherungen / links-staerkste Granaten mit genug Flaeche
se_r = sorted([s for s in se if s[1] >= 250 and s[3] >= 250], key=lambda s: -(s[2] + s[4]))[:25]
ge_l = sorted([g for g in ge if g[1] >= 200 and g[3] >= 150], key=lambda g: (g[2] + g[4]))[:25]
print('Sicherung, am weitesten rechts (Schwerpunkt Start/oben):')
for s in se_r[:10]:
    print('  %s  Start %d px x%.1f  oben %d px x%.1f' % (s[0], s[1], s[2], s[3], s[4]))
print('Granate, am weitesten links:')
for g in ge_l[:10]:
    print('  %s  Start %d px x%.1f  oben %d px x%.1f' % (g[0], g[1], g[2], g[3], g[4]))
# Stufe 2
erg = []
for s, g in itertools.product(se_r, ge_l):
    Pg = Su.dreh(gran, g[0][3]) + np.array(g[0][:3])
    ab = Su.abstand(Pg, (s[0][0], -1062, s[0][2]), s[0][3])
    if ab < 1:
        continue
    r = [RS.Szene.auswerten(sz.render(py, 150, s[0], g[0])) for py in (-305, -1205)]
    if not all(x['S'][0] and x['G'][0] for x in r):
        continue
    trenn = min(x['S'][1] - x['G'][1] for x in r)
    flaeche = min(min(x['S'][0], x['G'][0]) for x in r)
    erg.append((trenn, flaeche, s[0], g[0], ab, r))
erg.sort(key=lambda e: -(min(e[0], 25) * 20 + e[1]))
for e in erg[:20]:
    print('Trennung %.1f px, kleinste Flaeche %d | S %s G %s | Abstand %.1f | Start S %d x%.1f G %d x%.1f | oben S %d x%.1f G %d x%.1f'
          % (e[0], e[1], e[2], e[3], e[4], e[5][0]['S'][0], e[5][0]['S'][1], e[5][0]['G'][0], e[5][0]['G'][1],
             e[5][1]['S'][0], e[5][1]['S'][1], e[5][1]['G'][0], e[5][1]['G'][1]))
