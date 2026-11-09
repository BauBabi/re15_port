"""Bild 2, Spiel-Asset: Maler-Ordnung (Tafel/Spiel) gegen Tiefenpuffer. Gegenprobe gegen die Tafel-Kennung render_asset_2.npz."""
import time
from collections import Counter
from s_basis import *  # noqa: F401,F403

t0 = time.time()
k, tris, Mg, kam = szene(ASSET, 2)
H, W = grund_rgba(2).shape[:2]
R = rastern_beide(tris, k.tex, H, W, Mg)
print('Dreiecke', len(tris), 'Zeit %.1f s' % (time.time() - t0))
d = dict(np.load(os.environ['GRUNDLAGE_ARBEIT'] + '/render_asset_2.npz'))
tm = tri_mesh(tris)
mm = np.where(R['maler_i'] >= 0, tm[np.maximum(R['maler_i'], 0)], -1)
print('Maler-Kennung == Tafel-Kennung: Anteil %.5f, abweichend %d px' % ((mm == d['mesh']).mean(), int((mm != d['mesh']).sum())))
pm = np.where(R['puffer_i'] >= 0, tm[np.maximum(R['puffer_i'], 0)], -1)
diff = (R['maler_i'] != R['puffer_i']) & (R['maler_i'] >= 0)
print('Bildpunkte Maler != Tiefenpuffer: %d von %d' % (int(diff.sum()), int((R['maler_i'] >= 0).sum())))
c = Counter(zip(mm[diff].tolist(), pm[diff].tolist()))
for (a, b), nn in c.most_common(30):
    dz = (R['maler_z'] - R['puffer_z'])[diff & (mm == a) & (pm == b)]
    print('Maler zeigt %2d %-26s | vorn liegt %2d %-26s | %6d px | Tiefen-Abstand median %.0f max %.0f' % (a, TEILNAME[a], b, TEILNAME[b], nn, np.median(dz), dz.max()))
np.savez_compressed(AUS + '/r_K5e_f1_2.npz', **{a: R[a] for a in ('maler_i', 'puffer_i', 'maler_z', 'puffer_z', 'rgb_maler', 'rgb_puffer')}, tm=tm)
