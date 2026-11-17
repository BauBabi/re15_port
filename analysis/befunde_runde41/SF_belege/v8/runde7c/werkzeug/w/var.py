"""Variante aus R3B (= p5_stufe --r3 auf Stand 7b) mit UV-/Ecken-Aenderungen an Mesh 8.  python var.py NAME 'json-dict'
dict: {"uv": {"t60": [[k,u,v],...]}, "ecke": {"17": [x,y,z]}}  (k = Ecke im Flaechen-Umlauf)"""
import sys, json, hashlib
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools')
import kopf_orig_bau as KB
M = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/m/'
name, cfg = sys.argv[1], json.loads(sys.argv[2])
blob = open(M + cfg.get('basis', 'R3B') + '.EMD', 'rb').read()
d, _o, ms, tim = KB.teile(blob); m = ms[8]
for key, lst in cfg.get('uv', {}).items():
    U = m['tuv'] if key[0] == 't' else m['quv']; i = int(key[1:])
    for k, u, v in lst:
        U[i][3 * k] = u; U[i][3 * k + 1] = v
for e, p in cfg.get('ecke', {}).items():
    m['v'][int(e)][:3] = list(p)
neu = KB.packen(blob, d, ms, tim)
open(M + name + '.EMD', 'wb').write(neu)
print(name, hashlib.sha256(neu).hexdigest()[:12])
