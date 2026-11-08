from r4lib import *
for f in (dict(sx=0.85), dict(sx=0.85, unten=True, unten_waag=True, unten_f=1.3, unten_z=0.9), dict(sx=0.85, unten=True, unten_waag=True, unten_f=1.3, unten_z=0.9, unten_x0=60)):
    g = dict(f, auge_dy=60, auge_e=(39,))
    blob, prot = bauen(g, versatz=(80, 70, 0))
    mo = b2.modell(blob); M = KM.messen(mo, P, bilder=True); RG.hell_unten(mo, M)
    for z in RG.kopf_r2(mo, M, P, prot['zonen']) + RG.tr2_tr3(mo, M):
        if any(k in z['mass'] for k in ('K6', 'K5', 'K10', 'K1k')):
            print('OK ' if z['ok'] else 'ROT', z['mass'][:30], z['ist'][:230])
    print('kb', M['135']['kopfbox'])
