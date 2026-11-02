import numpy as np, sys
from fk import *
g = lade()
k2 = FK.lade_kamera(2); k1 = FK.lade_kamera(1)
print('Bild 2 Auge', k2['auge'], 'Blick', k2['R'][2], 'Nick Grad', np.degrees(np.arcsin(k2['R'][2][1])))
print('Bild 1 Auge', k1['auge'], 'Blick', k1['R'][2], 'Nick Grad', np.degrees(np.arcsin(k1['R'][2][1])))
# Figurraum Bild 2: Lage + Gier, vorn/links
R, T = fk(g, 'bild2')
p = g['posen']['bild2']
G = p['gier'] * E
vorn = np.array([math.cos(G), 0, -math.sin(G)]); links = np.array([math.sin(G), 0, math.cos(G)])
w = T[0]*[1,0,1]
for name, (x, y) in dict(Handgelenk=(262,395), Y_Mitte=(239,418), Hand_u=(240,445), Klaue_LO=(147,350), Spitze=(337,316), Strip_m=(300,357), Links_Spitze1=(468,417), Links_Spitze2=(437,418), Links_Spitze3=(460,392), Links_Keil=(431,384), Hand_l=(411,367)).items():
    aus = []
    for h in (0, 300, 650):
        q = FK.strahl_auf_boden(k2, np.array([x, y], float), y=-h)
        aus.append('h%3d Welt (%6.0f,%6.0f) Tiefe %5.0f vor %6.0f links %6.0f' % (h, q[0], q[2], FK.tiefe(k2, q), (q-w)@vorn, (q-w)@links))
    print('%-14s (%3d,%3d) ' % (name, x, y), ' | '.join(aus))
