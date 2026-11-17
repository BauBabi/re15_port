# L*-Verteilung (Perzentile 10/25/50/75/90, geteilt durch den L*-Median des oberen Unterarm-Drittels im selben Bild) je Bereich: Kandidat gegen Vorlage
import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, riegel_handfarbe as RF, hand_mass as HM, v4_gemein as G
P = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8'
PR = [10, 25, 50, 75, 90]
def vert(rgb, bb):
    ref = np.median(G.lab(rgb[bb['unterarm_oben']].astype(float))[:, 0])
    return {n: np.percentile(G.lab(rgb[bb[n]].astype(float))[:, 0], PR) / ref for n in ('handruecken', 'unterarm_unten')}
vor = {}
for f, b in RF.PAARE_F:
    v = HM.vorlage(b); vor[f] = vert(v['_bild'][..., 2::-1].astype(float), RF.bereiche_vorlage(v))
for name, d in (('ALT', P + '/r7d/szene'), ('K1b', P + '/r7e/laeufe/K1b'), ('K2', P + '/r7e/laeufe/K2')):
    ab = {'handruecken': [], 'unterarm_unten': []}
    for f, b in RF.PAARE_F:
        a = RF.ppm(d, f); h = RF.ppm(P + '/r7e/mk/lauf_HF_ALT', f)
        m = vert(a, RF.bereiche_modell(RF.klassen(h, a)))
        for n in ab:
            ab[n].append(np.abs(m[n] - vor[f][n]).mean())
        if f == 371:
            print(name, 'F371', {n: np.round(m[n], 2).tolist() for n in m}, 'Vorlage', {n: np.round(vor[f][n], 2).tolist() for n in m})
    print(name, 'mittlere |Abweichung| der normierten Perzentile: Handruecken %.3f  Unterarm unten %.3f  zusammen %.3f' % (
        np.mean(ab['handruecken']), np.mean(ab['unterarm_unten']), np.mean(ab['handruecken'] + ab['unterarm_unten'])))
