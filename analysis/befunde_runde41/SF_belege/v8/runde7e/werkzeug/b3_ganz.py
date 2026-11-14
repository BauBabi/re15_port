# Runde 7e2: Rotanteil der GANZEN Hand (Mesh 12 sichtbar bzw. Vorlage-Arm ab dem Handgelenk W) bei a* >= 6 und >= 10, und
# Rotanteil je Klasse (Marker HF); Vorlage mit hand_mass.vorlage (Arm-Maske, W, T)
import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, riegel_handfarbe as RF, hand_mass as HM, v4_gemein as G
P = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8'
hf = P + '/r7e/mk/lauf_HF_NEU'
vor = {}
for f, b in RF.PAARE_F:
    v = HM.vorlage(b); m = v['_maske']; W, T = np.array(v['W']), np.array(v['T']); ax = (T - W) / np.linalg.norm(T - W)
    yy, xx = np.mgrid[0:m.shape[0], 0:m.shape[1]]
    hand = m & (((xx - W[0]) * ax[0] + (yy - W[1]) * ax[1]) >= 0)
    lab = G.lab(v['_bild'][..., 2::-1][hand].astype(float))
    vor[f] = ((lab[:, 1] >= 6).mean(), (lab[:, 1] >= 10).mean())
print('Vorlage ganze Hand (ab W) a*>=6 / >=10: ' + '  '.join('birkin%d %.3f/%.3f' % (b, *vor[f]) for f, b in RF.PAARE_F))
for name, d in [(n, P + '/r7e/laeufe/' + n) for n in sys.argv[1:]]:
    if name == 'ALT':
        d = P + '/r7d/szene'
    elif name == 'NEU':
        d = P + '/r7e/szene'
    zs = []
    for f, b in RF.PAARE_F:
        a = RF.ppm(d, f); k = RF.klassen(RF.ppm(hf, f), a)
        Hm = k['hand'] | k['finger'] | k['daumenkralle'] | k['kastenfinger']
        lab = G.lab(a[Hm].astype(float))
        je = {}
        for n in ('hand', 'kastenfinger', 'finger', 'daumenkralle'):
            l2 = G.lab(a[k[n]].astype(float)) if k[n].any() else np.zeros((1, 3))
            je[n] = (l2[:, 1] >= 10).mean()
        zs.append('F%d %.3f/%.3f (Hand %.2f Kasten %.2f Krallen %.2f Daumenkralle %.2f)' % (f, (lab[:, 1] >= 6).mean(), (lab[:, 1] >= 10).mean(),
                  je['hand'], je['kastenfinger'], je['finger'], je['daumenkralle']))
    print('%s ganze Hand a*>=6 / >=10: %s' % (name, '  '.join(zs)))
