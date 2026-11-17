"""Runde 8c: Kandidaten-Assets (Messproben, kein Spiel-Asset) = Stufe q auf 07b6703f mit anderem Original-Bauchfeld und Ring-Feld (Probe b).
python x15_kandidaten.py <ausgabeordner> <name> [<name> ...]"""
import sys, os, json, copy, hashlib, time
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import schwanz_r8 as Q
ALT = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8/emd/ALT_07b6703f.EMD'
R2 = dict(u0=50, u1=61, v0=62, v1=75, richt='v-', zeile=493, seite=0)
R1 = dict(u0=38, u1=49, v0=66, v1=75, richt='v-', zeile=493, seite=0)
RECHT = lambda z, s, u0, u1, v0, v1: dict(v0=v0, v1=v1, u_v0=u0, u_v1=u0, breite=u1 - u0 + 1, zeile=z, seite=s)
K = {'K0': (None, R2), 'K1': (RECHT(494, 1, 34, 53, 4, 11), R2), 'K2': (RECHT(495, 2, 10, 29, 12, 19), R2),
     'K3': (RECHT(494, 1, 50, 69, 9, 40), R2), 'K4': (RECHT(493, 0, 26, 57, 46, 65), R1),
     'K5': (RECHT(494, 1, 68, 83, 66, 85), R1), 'K6': (RECHT(494, 1, 16, 39, 48, 67), R1), 'K7': (RECHT(493, 0, 26, 41, 38, 47), R1),
     'K8': (RECHT(494, 1, 80, 91, 40, 53), R1),
     'G1': (RECHT(494, 1, 50, 65, 18, 45), R1), 'G2': (RECHT(493, 0, 52, 67, 52, 65), R1), 'G3': (RECHT(493, 0, 12, 27, 10, 21), R1),
     'C1': (RECHT(493, 0, 54, 65, 48, 57), R1), 'D1': (RECHT(493, 0, 54, 67, 52, 63), R1), 'D2': (RECHT(494, 1, 52, 69, 24, 35), R1),
     'C2': (RECHT(494, 1, 54, 65, 24, 33), R1), 'C3': (RECHT(493, 0, 24, 35, 36, 43), R1),
     'C1s': (dict(RECHT(493, 0, 54, 65, 48, 57), abbildung='spiegel'), R1), 'E1': (RECHT(493, 0, 56, 67, 102, 109), R1),
     'RB': (RECHT(493, 0, 54, 65, 48, 57), dict(u0=93, u1=100, v0=10, v1=17, richt='u-', zeile=493, seite=0)),
     'RC': (RECHT(493, 0, 54, 65, 48, 57), dict(u0=45, u1=52, v0=97, v1=104, richt='v-', zeile=493, seite=0))}
blob = open(ALT, 'rb').read()
basis = Q.felder_laden()
for nm in sys.argv[2:]:
    f = copy.deepcopy(basis)
    fl, ri = K[nm]
    if fl:
        f['fleisch'] = fl
    f['ring'] = ri
    t0 = time.time()
    neu = Q.stufe(blob, 'b', felder=f)
    out = os.path.join(sys.argv[1], 'K_%s.EMD' % nm)
    open(out, 'wb').write(neu)
    print(nm, hashlib.sha256(neu).hexdigest()[:16], '%.0f s' % (time.time() - t0), json.dumps(f['fleisch']), json.dumps(ri))
