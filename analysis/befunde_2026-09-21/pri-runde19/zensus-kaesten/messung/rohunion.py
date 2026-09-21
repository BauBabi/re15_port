# -*- coding: utf-8 -*-
"""Was wuerde bau_p2.py aus den Kaesten machen? (nur_kunst wird dort NICHT ausgewertet:
bau_p2.objekt_region gibt fuer "quader" None zurueck, bau_p2.bau_cut setzt
r = kaesten_anwenden(tr.copy(), o, e) - die ROHE Quader-Silhouette.)

Zusaetzlich: welche Cuts sieht der Silhouetten-Riegel (test_pri_silhouette.c laeuft ueber
die .PBM-Dateien) ueberhaupt?"""
import glob, io, json, os, struct, sys
import numpy as np
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild

CD = 're15_port/shared_assets/PSX'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))

for room, cut, ist in (('ROOM10F0', 4, 12068), ('ROOM10F0', 5, 9285),
                       ('ROOM1100', 1, 28160), ('ROOM1100', 2, 25211)):
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    u = np.zeros((240, 320), bool)
    n = 0
    for o in AUS[room][str(cut)]['objekte']:
        if 'quader' not in o:
            continue
        q = [int(x) for x in o['quader']]
        vz, tr = geometrie.quader_auf_band(R, t, H, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4], 0)
        u |= tr
        n += 1
    print('%s C%d  %d Kaesten: Vereinigung der ROHEN Quader %6d Punkte | ausgeliefert %6d | Faktor %.1f'
          % (room, cut, n, int(u.sum()), ist, u.sum() / float(ist)))

print()
pbm = sorted(os.path.basename(p) for p in glob.glob(os.path.join(CD, 'MASKS', '*_PRI*.PBM')))
msk = sorted(os.path.basename(p) for p in glob.glob(os.path.join(CD, 'MASKS', '*.MSK')))
cuts = 0
for p in msk:
    b = open(os.path.join(CD, 'MASKS', p), 'rb').read()
    _, nc = struct.unpack_from('<II', b, 4)
    for c in range(nc):
        if maskenbild.masken(b, c):
            cuts += 1
print('Silhouetten-Riegel (test_pri_silhouette.c, laeuft ueber die .PBM): %d PBM-Cuts' % len(pbm))
print('Cuts mit einer Sektion im Container: %d' % cuts)
ohne=[]
for q in sorted(glob.glob(os.path.join(CD,'MASKS','*_PRI*.TIM'))):
    b=os.path.basename(q)
    if 'ROOM209' in b:
        continue
    if not os.path.exists(os.path.join(CD,'MASKS',b[:-4]+'.PBM')):
        ohne.append('%s C%d'%(b[:8],int(b[12:14])))
print('-> ABDECKUNG des Riegels: %.1f %% der Cuts mit Maske' % (100.0*len(pbm)/cuts))
print('-> NICHT gesehen (ohne ROOM2090/2091): %s' % ', '.join(ohne))
