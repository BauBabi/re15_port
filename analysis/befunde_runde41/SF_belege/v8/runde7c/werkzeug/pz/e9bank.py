"""E9 echt mit dem Bank-Mass (voll_p5.s16s, voll.json je_bild): vorn cos >= 0,4 (m5_cos.json) ohne Vordergrund F316-F333.  python e9bank.py NAME=voll.json ..."""
import sys, json
C = json.load(open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/erk/d/m5_cos.json'))
for arg in sys.argv[1:]:
    n, p = arg.split('=')
    J = json.load(open(p))['S16s']['je_bild']
    vorn, seite = [], []
    for m in J:
        if m.get('fehlt') or 316 <= m['f'] <= 333 or m['ok']: continue
        (vorn if C['F%d' % m['f']] >= 0.4 else seite).append((m['f'], m['px'], m['tief'], m['max_tiefe']))
    ohne = [v for v in vorn if v[0] != 340]
    print('%-4s vorn %d / %d (ohne F340 %d / %d) | Seite %s' % (n, len(vorn), sum(v[1] for v in vorn), len(ohne), sum(v[1] for v in ohne), seite))
