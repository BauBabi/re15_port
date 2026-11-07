# -*- coding: utf-8 -*-
"""Ueberblick RE2-Modelle: Handwerks-Eichmass, keine Formvorlage."""
from u_basis import *
taf = []
for k in ('K34', 'K33', 'K23', 'K28', 'K2B', 'K2E', 'K31', 'K30', 'K22', 'K20'):
    mo, tex = modell(k)
    f = lade(k)
    nb = len(f['knochen'])
    nf = sum(len(m['dreiecke']) + len(m['vierecke']) for m in f['meshes'][:nb])
    a, _ = bild(mo, tex, kf_von(mo, 0), 40, 15, 380, gamma=0.7)
    taf.append(('%s Knochen %d Flaechen %d Clip 0' % (k, nb, nf), a))
FB.montage(taf, 5, AUS + '/eich_ueberblick.png', 'RE2-Modelle als Handwerks-Eichmass, Clip 0 Bild 0, Gamma 0,7')
