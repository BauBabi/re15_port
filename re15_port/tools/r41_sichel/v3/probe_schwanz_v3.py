# Probe: SCHWANZ_V3 mit geaenderten Parametern bauen (ohne Kopfmalen) und P_port messen
import sys, json, os, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools')
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v2')
import birkin_formen as BF
import P_port
from gelenke import winkel
import b2, birkin_buehne as BB
P = json.load(open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2/posen_seite.json'))
basis = BF.lade_em30()
aend = json.loads(sys.argv[1])
for k, v in aend.items():
    if isinstance(v, dict) and isinstance(BF.SCHWANZ_V3.get(k), dict): BF.SCHWANZ_V3[k] = dict(BF.SCHWANZ_V3[k], **v)
    else: BF.SCHWANZ_V3[k] = v
blob = BF.baue(basis, BF._variante_v3())
pf = os.path.join('C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/bau', 'probe.emd'); open(pf, 'wb').write(blob)
print('info', {k: round(v, 3) for k, v in BF.BAU_INFO['schwanz_v2'].items()})
mo = b2.modell(pf)
for clip, bild in [(1, 26), (11, 85)]:
    print(' kf', clip, bild, np.round(winkel(b2.kette_punkte(mo, BB.kf_von_clip(mo, clip, bild))), 1))
for h in ('85', '92'):
    d, vis = P_port.messen(pf, int(h), P[h])
    f = d['form']; c = d['farbe']; m = d['muster']
    print(h, 'D', [f['dicke_H'][x] for x in (0.2, 0.5, 0.9)], 'kn', f['knicke'], f['knick_bei'], f['knick_winkel'], 'max', f['max_dwinkel_005H'], f['max_bei'], 'dreh', f['gesamtdrehung'], 'aus', d['austritt_unter_OK_H'])
    print('   L %.3f C %.2f dh %.1f rot %.3f/%.3f de %.1f std_a %.2f per %.2f' % (c['L_verh'], c['C_verh'], c['dh'], c['rot_anteil_schwanz'], c['rot_anteil_haut'], c['de00_haut'], m['rest_std_a'], m['periode_a_ak']))
    import cv2; cv2.imwrite(pf[:-4] + '_%s.png' % h, cv2.cvtColor(vis, cv2.COLOR_RGB2BGR))
