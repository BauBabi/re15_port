import numpy as np, lage, bildmass
FEUER = [(3000,-20999),(5450,-20950),(8450,-22700),(11350,-20650),(12600,-21250),(3600,-25600),(1200,-21100),(2400,-22600),
         (-900,-24600),(-600,-23900),(-900,-23250),(-600,-22750),(9900,-21200),(11250,-21550),(9900,-24500),(4000,-24600),(-1000,-22250)]
import struct
rdt = open(r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\re15_port\shared_assets\PSX\STAGE5\ROOM5090.RDT','rb').read()
WAND = lage.geom.sca_wandzellen(rdt)
def pruef(P, g):
    Vw, Tw = lage.welt(P, g)
    xz = Vw[:, [0, 2]]
    dw = 1e9
    for (x, z, w, d) in WAND:
        dx = np.maximum(np.maximum(x - xz[:,0], xz[:,0] - (x + w)), 0)
        dz = np.maximum(np.maximum(z - xz[:,1], xz[:,1] - (z + d)), 0)
        inn = (xz[:,0] > x) & (xz[:,0] < x + w) & (xz[:,1] > z) & (xz[:,1] < z + d)
        dd = np.where(inn, -1, np.hypot(dx, dz)).min()
        dw = min(dw, dd)
    df = min(np.hypot(xz[:,0]-fx, xz[:,1]-fz).min() for fx, fz in FEUER)
    return dw, df
P = (4854, -22047); g0 = 1170
print('RE2-Lage', pruef(P, g0))
res = []
for d in range(-2048, 2048, 32):
    g = (g0 + d) & 0xfff
    dw, df = pruef(P, g)
    res.append((abs(d), d, g, int(dw), int(df)))
ok = sorted(r for r in res if r[3] >= 100 and r[4] >= 500)
print('kleinste Drehung frei (Wand>=100, Feuer>=500):', ok[:6])
print('--- Verschiebung bei RE2-Drehung g0')
res = []
for dx in range(-1600, 1601, 100):
    for dz in range(-1600, 1601, 100):
        Q = (P[0] + dx, P[1] + dz)
        dw, df = pruef(Q, g0)
        if dw >= 100 and df >= 500:
            res.append((int(np.hypot(dx, dz)), dx, dz, int(dw), int(df)))
res.sort(); print(res[:8])
