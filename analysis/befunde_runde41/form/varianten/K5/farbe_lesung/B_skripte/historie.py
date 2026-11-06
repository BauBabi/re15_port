"""Farbstand je Variante (Texel aller Meshes ausser 21-23 = Koerper; plus gesamt): mittleres Lab, Chroma, Rot-Anteil, Glanz, Farben je Teil, HF."""
import json
import numpy as np
import fl, texel, stil

V = fl.FORM + '/varianten/'
aus = {}
for nm in ('A_rueckrechnung', 'B_screenshot', 'C_mitte', 'D_fleisch', 'F_sauber', 'G_treu', 'H_stoff/h2', 'J_dicker', 'K_alligator/K4_kroko', 'K5/K5c'):
    mo = fl.lade_var(V + nm)
    per, tri, feld = texel.sammeln(mo)
    nk = len(mo.meshes)
    koerper = [b for b in per if b not in (21, 22, 23)]
    r = stil.messen(mo, per, tri, feld, koerper, name=nm)
    L = []
    for b in koerper:
        L.append(fl.lab(per[b]['rgb']))
    L = np.concatenate(L)
    r['lab_mittel'] = [round(float(x), 1) for x in L.mean(0)]
    r.pop('T1_palette')
    aus[nm] = r
    print(nm, 'meshes', nk, 'Lab', r['lab_mittel'], 'C', r['T6_chroma_mittel'], 'rot%', r['T7_rot_anteil_proz'], 'glanz%', r['T4_glanz_L85_proz'],
          'farben/teil', r['T2_farben_je_teil'], 'hf', r['T5_hf'], 'klassen', r['klassen'])
json.dump(aus, open(fl.S + '/historie.json', 'w'), indent=1)
