"""Rollreihe der Analyse (teil_b.py / roll_render.py, scratchpad/schwanz_v8/analyse) am GEBAUTEN Asset: die echten Meshes 16-22 des Kandidaten
entlang derselben Ketten (Szene h22/h24/h64 = lauf_v8_debug.log Zeile h+1, PV-Kette), Hochachse = Bauchachse e_h (innen_phi) -> lokale x,
Querachse e_q -> lokale z; Rolle phi = Winkel der Hochachse zum Sehstrahl (0 = Bauch zur Kamera). Kein zusaetzliches Abflachen (L_quer None).
Vergleich: B_rolle_L280.json (Vorhersage der Analyse, flach phi 0 / phi 90) und Video (Nutzer-Umriss). Ausgabe JSON + Text."""
import os, sys, json
import numpy as np
AN = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/analyse'
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.dont_write_bytecode = True
os.chdir(AN)
sys.path.insert(0, AN)
import kam
import roll_render as RR
import pv as PVM
sys.path.insert(0, W)
import v8_gemein as V

emd = sys.argv[1]
aus = sys.argv[2]
mo = V.modell(open(emd, 'rb').read())
phi = {int(k): float(v) for k, v in json.load(open(os.path.join(W, 'unterseite_v8.json')))['innen_phi'].items()}
MESH = {}
for b in range(16, 23):
    m = mo.meshes[b]
    Vv = np.array([v[:3] for v in m.tv] + [v[:3] for v in m.qv], float)
    p = np.radians(phi[b])
    x = Vv[:, 0] * np.cos(p) + Vv[:, 2] * np.sin(p)          # Anteil entlang e_h
    z = -Vv[:, 0] * np.sin(p) + Vv[:, 2] * np.cos(p)         # Anteil entlang e_q
    Vr = np.stack([x, Vv[:, 1], z], 1)
    nt = len(m.tv)
    F = [(f[1], f[3], f[5]) for f in m.tf]
    for f in m.qf:
        a, bb, c, d = nt + f[1], nt + f[3], nt + f[5], nt + f[7]
        F += [(a, bb, c), (bb, d, c)]
    MESH[b] = (Vr, np.array(F))
RR.MESH = MESH
G, Z, B = kam.lese()
N = json.load(open('nutzer_mass.json'))
Mt = json.load(open('messtabelle.json'))
BR = json.load(open('B_rolle_L280.json'))
mo_pv = PVM.mo_laden()
PVW, _ = PVM.pv_kette(mo_pv)
faelle = {22: (G[23], 0, 'flach phi 0'), 24: (G[25], 0, 'flach phi 0'), 64: (G[65], 0, 'flach phi 0'), 90: (PVW, 90, 'flach phi 90')}
erg = {}
for h, (P, ph, schl) in faelle.items():
    if h == 90:
        u0, u1 = 0.0, 7.0
    else:
        u0, u1 = Mt[str(h)]['p_sicht_von_bis_u']; u1 = min(u1, 7.0)
    _im, mask, _info = RR.kette_render(P, None, ph, 'phi')
    br = RR.stationen_breite(mask, P, u_von=u0, u_bis=u1)
    vor = BR[str(h)][schl]
    dif = [round(a - b, 1) for a, b in zip(br, vor)]
    erg[h] = dict(phi=ph, neu=br, analyse=vor, abweichung=dif, video=N['h%d' % h]['breiten'], u=[u0, u1],
                  max_abw=max(abs(x) for x in dif))
    print('h%d phi %d: neu %s | Analyse (%s) %s | Abweichung %s (max %.1f) | Video %s' % (h, ph, br, schl, vor, dif, erg[h]['max_abw'], N['h%d' % h]['breiten']))
json.dump(erg, open(aus, 'w'), indent=1)
