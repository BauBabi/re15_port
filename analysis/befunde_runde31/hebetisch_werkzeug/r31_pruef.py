# -*- coding: utf-8 -*-
"""Runde 31 / H — exakte Nachpruefung EINES Sitzpaars in ROOM1150 UND ROOM1151 (Dreiecksrechnung
r31_geo.Fach.luft, Proben = Punkte + Kanten in 16 Stuecken + Flaechenmitten).
r31_pruef.py Sx Sz Sry Gx Gy Gz Gry"""
import os, sys
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
import r31_geo as G  # noqa
a = [int(v) for v in sys.argv[1:8]]
s_pos, s_ry = (a[0], -1062, a[1]), a[2]
g_pos, g_ry = (a[3], a[4], a[5]), a[6]
sich = G.proben(G.inc_md1('sicherung_prop.inc', 're15_sicherung_md1'), 16)
gran = G.proben(G.inc_md1('granate_prop.inc', 're15_granate_md1'), 16)
for raum in ('ROOM1150.RDT', 'ROOM1151.RDT'):
    f = G.Fach(raum)
    ws = G.platzieren(sich, s_pos, s_ry)
    wg = G.platzieren(gran, g_pos, g_ry)
    ls, lg = f.luft(ws), f.luft(wg)
    ab = G.abstand_stab(wg, s_pos, s_ry)
    tief_s = max(p[1] for p in ws) - G.BODEN
    tief_g = max(p[1] for p in wg) - G.BODEN
    print('%s  Sicherung %s ry %d: Luft Kuppel zu %.2f, Deckel offen %.2f, ausserhalb Achteck %d, tiefster Punkt %+.1f zum Boden'
          % (raum, s_pos, s_ry, ls[0], ls[1], ls[2], tief_s))
    print('%s  Granate   %s ry %d: Luft Kuppel zu %.2f, Deckel offen %.2f, ausserhalb Achteck %d, tiefster Punkt %+.1f zum Boden'
          % (raum, g_pos, g_ry, lg[0], lg[1], lg[2], tief_g))
    print('%s  Abstand Granate -> Sicherungs-Mantel %.2f (Proben %d/%d)' % (raum, ab, len(ws), len(wg)))
    zs = [p[2] for p in ws]; zg = [p[2] for p in wg]
    print('%s  z-Bereich Sicherung %.0f..%.0f, Granate %.0f..%.0f; Oeffnung offen %d..%d'
          % (raum, min(zs), max(zs), min(zg), max(zg), G.OFFEN_LO, G.OFFEN_HI))
