import sys, math, struct, numpy as np
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\re15_port\tools')
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\re15_port\tools\maske')
import emd_ansichtsblatt as EA, geom
import liege
def gier_zu(dx, dz):   # rot_y (0=+X) der Richtung (dx,dz): atan2(dz,dx)-1024 nach re15_actor.h
    a = math.atan2(dx, dz) * 4096 / (2*math.pi)    # re15_atan2_q12(dz,dx): 0 = +Z, +x = 1024
    return int(round(a - 1024)) & 0xfff
# RE2: W (e07) und liegend @0x01162 / @0x0116A
W = (-6600, -14800); A = (-12300, -15300); D = 3300
peil = gier_zu(A[0]-W[0], A[1]-W[1]); rel = (D - peil) & 0xfff
print('RE2 Peilung W->A', peil, 'rel', rel)
# Pruefung Konvention: RE2 Annette stehend R4087 bei (-11097,-15001) Blick auf W
print('Kontrolle stehend->W', gier_zu(W[0]+11097, W[1]+15001), '(RAM 4087)')
kf, ks, ents, hdr = liege.rbj_skel_clip()
m = liege.M(kind=0x49, kf=kf, ksize=ks)
k0 = ents[0] & 0xfff
def welt(lage, gier, y=900):
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c,0,s],[0,4096,0],[-s,0,c]], dtype=np.int64)
    R, T = m.pose(k0)
    V, N, F = m.geometrie(k0)
    Vw = (Y.astype(float) @ V.T).T / 4096 + np.array([lage[0], y, lage[1]])
    Tw = [(((Y @ t) >> 12) + np.array([lage[0], y, lage[1]])) for t in T]
    return Vw, Tw
# Kontrolle gegen RE2-RAM e23 (Teile-Weltlagen)
Vw, Tw = welt(A, D)
print('RE2 nachgerechnet Teil 0/4/8:', [tuple(int(v) for v in Tw[i]) for i in (0,4,8)], ' RAM e23: (-12300,-171,-15300) (-11680,-73,-13980) (-12611,-188,-15840)')
L = (-1378, -23395); S = (3700, -22500)
def leiche_lage(sx, sz):
    ux, uz = sx - L[0], sz - L[1]; l = math.hypot(ux, uz); ex, ez = ux/l, uz/l
    return (sx + ex*1215 - ez*245, sz + ez*1215 + ex*245)
P = leiche_lage(*S); g = (gier_zu(P[0]-L[0], P[1]-L[1]) + rel) & 0xfff
print('Port Leiche', [int(v) for v in P], 'Gier', g)
Vw, Tw = welt(P, g)
print('  Mesh x %d..%d z %d..%d y %d..%d' % (Vw[:,0].min(), Vw[:,0].max(), Vw[:,2].min(), Vw[:,2].max(), Vw[:,1].min(), Vw[:,1].max()))
print('  Kopf', [int(v) for v in Tw[8]], 'Fuesse', [int(v) for v in Tw[4]], [int(v) for v in Tw[7]])
rdt = open(r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\re15_port\shared_assets\PSX\STAGE5\ROOM5090.RDT','rb').read()
for (x, z, w, d) in geom.sca_wandzellen(rdt):
    if x - 3000 < P[0] < x + w + 3000 and z - 3000 < P[1] < z + d + 3000:
        inn = ((Vw[:,0] > x) & (Vw[:,0] < x+w) & (Vw[:,2] > z) & (Vw[:,2] < z+d)).sum()
        print('  Wand x %d..%d z %d..%d  Punkte darin %d' % (x, x+w, z, z+d, inn))
