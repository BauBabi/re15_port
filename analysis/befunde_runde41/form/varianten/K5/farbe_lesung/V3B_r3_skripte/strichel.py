"""Herkunft der rosa Strichel (Sicht-Pruefer V3 r2, Schulter/Brust): je Mesh Anteil rosa-magenta Texel (Rotklasse a* >= 12, C* >= 15, L* >= 20,
Farbton < 10 Grad) am Inselrand (Texel mit einem Nachbarn ausserhalb der G4-Insel) gegen das Inselinnere. python strichel.py <Variante>"""
import sys, numpy as np, cv2, collections
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import riegel_farbe_v3 as RV, kf_g4 as KG
v = RV.Var(sys.argv[1])
g4, g4tim, q = KG.g4_modell(); G = KG.G4Quelle(g4)
rand = G.global_maske & ~cv2.erode(G.global_maske.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
st = collections.defaultdict(lambda: np.zeros(4))
for key, mi, z, ecken, a3 in v.flaechen:
    if mi not in (10, 16, 21, 11, 17): continue
    xy = v.texel(ecken)
    xy = xy[(xy[:, 0] < 384)]
    if not len(xy): continue
    rgb = v.rgb_zeilen[z][v.px[xy[:, 1], xy[:, 0]]]
    L = RV.lab(rgb); C = np.hypot(L[:, 1], L[:, 2]); h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    rosa = (L[:, 1] >= 12) & (C >= 15) & (L[:, 0] >= 20) & (h < 10)
    r = rand[xy[:, 1], xy[:, 0]]
    st[(mi, 'b1' if z in (3, 4, 5) else 'g4')] += [ (rosa & r).sum(), r.sum(), (rosa & ~r).sum(), (~r).sum()]
for k, a in sorted(st.items()):
    print('Mesh %2d Zeilen %s: rosa am Inselrand %5.1f %% (%d/%d), im Inneren %5.1f %% (%d/%d)' % (k[0], k[1], 100*a[0]/max(a[1],1), a[0], a[1], 100*a[2]/max(a[3],1), a[2], a[3]))
