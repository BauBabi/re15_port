"""Runde 8d Messproben (kein Spiel-Asset): Stufe q Farbe b auf 07b6703f mit geaendertem Bauchfeld (nur Messung).
python x24_varianten.py <ausordner> <name> ..."""
import sys, os, copy, hashlib
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import schwanz_r8 as Q
ALT = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8/emd/ALT_07b6703f.EMD'
VAR = {'V1': {}, 'V1_5055': dict(v0=50, v1=55), 'V1_5155': dict(v0=51, v1=55), 'V1_4854': dict(v0=48, v1=54), 'V1mid': dict(v0=52, v1=52.0001), 'V1_4855': dict(v0=48, v1=55)}
blob = open(ALT, 'rb').read()
for nm in sys.argv[2:]:
    f = copy.deepcopy(Q.felder_laden())
    f['fleisch'].update(VAR[nm])
    neu = Q.stufe(blob, 'b', felder=f)
    open(os.path.join(sys.argv[1], '%s.EMD' % nm), 'wb').write(neu)
    print(nm, hashlib.sha256(neu).hexdigest()[:16], f['fleisch'])
