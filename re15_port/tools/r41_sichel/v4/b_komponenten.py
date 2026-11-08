# Leser B: Komponenten-Zugehoerigkeit von Start und Ende (8-Nachbarschaft) + letzte Teilstrecke des Auge-Hakens (Bild 1)
import sys, json, numpy as np
from scipy import ndimage as ndi
from b_linien_lib import belegung
OUT = sys.argv[1]; R = json.load(open(f'{OUT}/B_L1.json'))
for bi in ('1', '2'):
    M = np.load(f'{OUT}/B_linienmaske_{bi}.npy'); lab, n = ndi.label(M, structure=np.ones((3, 3)))
    for z in R[bi]['zuordnung']:
        s, e = z['linienstart'], z['ende']
        print(bi, z['text'][:30], 'Komp(Start)', lab[s[1], s[0]], 'Komp(Ende)', lab[e[1], e[0]])
    print(bi, 'Komponente je Ende', [(e, int(lab[e[1], e[0]])) for e in R[bi]['tabelle']['spalten']])
M = np.load(f'{OUT}/B_linienmaske_1.npy')
for K in ([386, 150], [396, 143]):
    print('Haken: Ecke', K, [(e, round(belegung(M, K, e), 3)) for e in R['1']['tabelle']['spalten']])
