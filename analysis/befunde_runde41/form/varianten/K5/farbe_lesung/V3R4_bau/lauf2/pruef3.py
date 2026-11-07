import sys, json
B = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/r4b/bau5'
sys.path.insert(0, B)
import such4 as Q
src = sys.argv[1]
d = json.load(open(src))
p = d[int(sys.argv[2])]['par'] if isinstance(d, list) else d
r, g, n = Q.bewerten(p, saaten=(15, 12345, 7))
rand = sorted(((v['hi'] - v['w']) / max(v['hi'], 1e-9) if 'gross' in k else min(v['w'] - v['lo'], v['hi'] - v['w']) / (v['hi'] - v['lo']), k, v['w'], v['lo'], v['hi']) for k, v in r.items() if v['b'])
print('RV', g, n, 'min %.3f' % rand[0][0], ['%s %.2f [%.2f..%.2f]' % (k, w, lo, hi) for _, k, w, lo, hi in rand[:7]], flush=True)
json.dump(p, open(sys.argv[3], 'w'), indent=1)
print(json.dumps(p))
