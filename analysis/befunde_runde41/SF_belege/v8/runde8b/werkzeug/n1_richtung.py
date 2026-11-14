"""Messung: Stachel-Richtung. Je Richtung u im Knochenraum 22 (Winkel a in der Ringebene x-z gegen e_h = Bauch, Neigung b gegen die Achse y):
- PV: Bildverschiebung je 100 E. an der Glied-22-Spitze (dx, dy px);
- Keyframes: groesste Absenkung (Welt-y je E. Nadellaenge, + = nach unten) in den Keyframes mit wenig Bodenabstand und Abstand der Spitze
  (Gelenk 7) zur Sohle. Ziel: in PV bildabwaerts, in den kritischen Keyframes nicht weltabwaerts."""
import os, sys, json
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
blob = open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8/emd/ALT_07b6703f.EMD', 'rb').read()
mo = V.modell(blob)
s = mo.skel
Pb = {b: np.array([v[:3] for v in mo.meshes[b].tv], float) for b in range(16) if mo.meshes[b].tv}
G, M = [], []
for k in range(s.kf_count):
    R, T = mo.pose(k)
    sole = max(float(((Pb[b] @ (R[b].astype(float) / 4096.0).T) + T[b].astype(float))[:, 1].max()) for b in Pb)
    R22 = R[22].astype(float) / 4096.0
    base = T[22].astype(float) + R22 @ np.array([0, 541.0, 0])
    G.append(R22.T @ np.array([0, 1.0, 0]))          # Welt-unten im Knochenraum 22
    M.append(sole - base[1])                           # Abstand Gelenk 7 ueber der Sohle (+ = darueber)
G, M = np.array(G), np.array(M)
R, T = V.pose_r_t(mo)
R22p, T22p = R[22].astype(float) / 4096.0, T[22].astype(float)
b0 = np.array([0, 541.0, 0])
p0 = V.proj(V.PV, (T22p + R22p @ b0)[None])[0]
phi = np.radians(26.43)
eh = np.array([np.cos(phi), 0, np.sin(phi)]); eq = np.array([-np.sin(phi), 0, np.cos(phi)])
print('PV Gelenk 7 im Bild', p0.round(1), '; Keyframes', len(M), 'Abstand Gelenk 7 ueber der Sohle: min %.0f, < 300: %d, < 600: %d' % (M.min(), (M < 300).sum(), (M < 600).sum()))
rows = []
for a in range(0, 360, 15):
    for bdeg in (90, 60, 45, 30):
        b = np.radians(bdeg)
        u = np.cos(b) * np.array([0, 1.0, 0]) + np.sin(b) * (np.cos(np.radians(a)) * eh + np.sin(np.radians(a)) * eq)
        p1 = V.proj(V.PV, (T22p + R22p @ (b0 + 100 * u))[None])[0]
        d = p1 - p0
        ab = G @ u                                     # Absenkung je E. Nadel
        # groesste zulaessige Nadellaenge: Abstand / Absenkung in jedem Keyframe mit Absenkung > 0 (20 E. Rand)
        lim = np.where(ab > 1e-6, (M - 20) / np.maximum(ab, 1e-6), 1e9)
        lmax = float(lim.min())
        rows.append((a, bdeg, round(d[0], 1), round(d[1], 1), round(lmax), int((ab > 0.5).sum())))
rows.sort(key=lambda r: -min(r[4], 2000) * max(r[3], 0))
print('a (Grad ab Bauch Richtung e_q), Neigung gegen Achse, PV dx dy je 100 E., groesste Nadellaenge ohne Sohle (20 E. Rand), Keyframes mit Absenkung > 0,5')
for r in rows[:25]:
    print(r)
