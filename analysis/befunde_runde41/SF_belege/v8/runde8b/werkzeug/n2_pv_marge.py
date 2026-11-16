"""Messung: Bodenabstand in PV und in den Szenen-Keyframes (Clip 11 / 17), welche Clips/Keyframes kritisch sind."""
import os, sys, json, collections
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
import birkin_buehne as BB
blob = open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8/emd/ALT_07b6703f.EMD', 'rb').read()
mo = V.modell(blob)
s = mo.skel
kpv = BB.kf_von_clip(mo, 17, 47)
Pb = {b: np.array([v[:3] for v in mo.meshes[b].tv], float) for b in range(16) if mo.meshes[b].tv}
def marge(k):
    R, T = mo.pose(k)
    sole = max(float(((Pb[b] @ (R[b].astype(float) / 4096.0).T) + T[b].astype(float))[:, 1].max()) for b in Pb)
    R22 = R[22].astype(float) / 4096.0
    return sole - float((T[22].astype(float) + R22 @ np.array([0, 541.0, 0]))[1]), sole
print('PV Keyframe', kpv, 'Gelenk 7 ueber der Sohle %.0f E. (Sohle y %.0f)' % marge(kpv))
# Clip -> Keyframes
import emd_ansichtsblatt as EA
d = EA.emd_dir(blob)
clips, frames = EA.edd_parse(blob[d[1]:d[2]])
for c in (11, 17):
    f0, n = clips[c]
    ks = [frames[f0 + i] & 0xFFF for i in range(n)]
    ms = [round(marge(k)[0]) for k in ks]
    print('Clip', c, 'Bilder', n, 'Keyframes', ks[:5], '... Marge min %d, Bilder mit Marge < 600: %d' % (min(ms), sum(1 for x in ms if x < 600)))
    print('   Marge je Bild:', ms)
