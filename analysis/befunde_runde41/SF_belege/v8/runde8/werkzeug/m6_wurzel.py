"""Messung: Tiefe der Wurzelring-Ecken (Mesh 16 Ecken 11-21) unter der Haut von Mesh 8 in PV (wie v8_bau.einbetten / wurzel_r4), alt und neu."""
import os, sys
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
import kopf_orig_bau as KB
import r3_gemein as R3
for p in sys.argv[1:]:
    blob = open(p, 'rb').read()
    mo = V.modell(blob)
    R, T = V.pose_r_t(mo)
    R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
    R16, T16 = R[16].astype(float) / 4096.0, T[16].astype(float)
    A = R8.T @ R16; t = R8.T @ (T16 - T8)
    d, _o, ms, tim = KB.teile(blob)
    tri = R3.dreiecke(ms[8])
    P = np.array([v[:3] for v in ms[16]['v'][11:22]], float)
    dd = [round(float(a * b), 1) for a, b in R3.abstand_innen(P @ A.T + t, tri)]
    print(os.path.basename(p), 'Tiefe unter der Haut (E., + = innen):', dd, 'min', min(dd))
