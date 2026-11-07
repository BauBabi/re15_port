# -*- coding: utf-8 -*-
"""Biegeprobe: alle Gelenke kraeftig gebeugt, Rundumblatt. Zeigt, ob die Ueberlappung haelt. Schreibt nur nach lauf/biege."""
from bau import *
import form_pruefung as PR

T = pickle.load(open('teile.pkl', 'rb'))
kn = knochen_tafel(T)
emd = open('FORM.EMD', 'rb').read()
form = FE.lese_emd(emd)
d = {1: dreh('z', 22), 2: dreh('z', -32), 3: dreh('z', 24), 5: dreh('z', -22), 6: dreh('z', 32), 7: dreh('z', -24), 9: dreh('z', 10), 10: dreh('z', -12) @ dreh('y', 12),
     11: dreh('z', -28) @ dreh('x', 12), 12: dreh('z', 34), 13: dreh('z', -24), 14: dreh('z', 18), 15: dreh('z', -26), 20: dreh('z', 40), 16: dreh('z', 24) @ dreh('x', -14), 17: dreh('z', -30), 18: dreh('z', 26), 19: dreh('z', -24)}
P = Pose(kn, d)
neu = FE.neu()
neu['knochen'] = form['knochen']
neu['meshes'] = form['meshes']
neu['tim'] = form['tim']
J0 = P.T[0]
for nm, q in (('a', Pose(kn)), ('b', Pose(kn)), ('c', P)):
    FE.clip_dazu(neu, nm, [FE.bild(neu, wurzel=tuple(int(round(c)) for c in q.T[0]), winkel=q.winkel())])
FE.boden_nachfuehren(neu, 0)
aus = os.path.join(AUS, 'lauf', 'biege')
os.makedirs(aus, exist_ok=True)
open(os.path.join(aus, 'B.EMD'), 'wb').write(FE.schreibe_emd(neu))
open(os.path.join(aus, 'B.TIM'), 'wb').write(open('FORM.TIM', 'rb').read())
j = json.load(open('lage.json'))
j.update(name='Biege', emd='B.EMD', tim='B.TIM', ruhe=dict(clip=2, bild=0))
json.dump(j, open(os.path.join(aus, 'B.json'), 'w'))
k = PR.Kandidat(os.path.join(aus, 'B.json'))
PR.rundum(k, os.path.join(aus, 'rundum.png'), os.path.join(aus, 'rundum_hell.png'), 2, 0)
print('fertig')
