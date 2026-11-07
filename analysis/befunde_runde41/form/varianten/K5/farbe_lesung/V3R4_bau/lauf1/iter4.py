import sys, json, math, time
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/bau4')
import such4 as Q
ZIEL = {  # Gruppe -> Liste (Bild, Region) aus Messblatt-Nachtrag 5
    'Schwanz': [(1, 'Schwanz')], 'Hinterbeine': [(1, 'Hinterbeine')], 'Rumpf Ruecken': [(1, 'Ruecken')], 'Rumpf Flanke': [(1, 'Flanke')],
    'Vorderbein r B1': [(1, 'Vorderbeine')], 'Brust': [(2, 'Ruecken')],
    'Hals Bauch': [(2, 'Hals'), (2, 'Brust/Bauch')], 'Kopf': [(2, 'Kopf')], 'Vorderbeine': [(2, 'Vorderbeine')]}
par = json.load(open(sys.argv[1])) if len(sys.argv) > 1 and sys.argv[1].endswith('.json') else None
if par is None:
    par = {g: dict(h=-25.0, C=6.0, beta=0.5) for g in ['Schwanz', 'Hinterbeine', 'Rumpf', 'Vorderbein r B1']}
    par.update({g: dict(h=h, C=16.0, beta=0.5) for g, h in [('Brust Ruecken', -28.0), ('Brust Hals', -14.0), ('Kopf', -4.0), ('Vorderbeine', 34.0)]})
G = float(sys.argv[4]) if len(sys.argv) > 4 else 0.8
N = int(sys.argv[2]) if len(sys.argv) > 2 else 6
aus = sys.argv[3] if len(sys.argv) > 3 else 'par4.json'
import copy
verlauf = []
for it in range(N):
    r, g, n = Q.bewerten(par)
    verlauf.append(dict(par=copy.deepcopy(par), gruen=g, n=n, rot=[k_ for k_, v in r.items() if v['b'] and not v['ok']]))
    json.dump(verlauf, open(aus.replace('.json', '_verlauf.json'), 'w'), indent=1)
    zeile = []
    for gr, ziele in ZIEL.items():
        dh, da, dl, k = 0, 0, 0, 0
        iv = {'a': [], 'h': [], 'd': []}; mit = {'a': [], 'h': [], 'd': []}
        for b, rn in ziele:
            m = lambda x: r.get('B%d %s %s' % (b, rn, x))
            a, h, d, gg = m('rot_eng'), m('h_rot'), m('dL'), m('gross_eng_p')
            def fen(x, f=0.1):
                lo, hi = x['lo'], x['hi']; s = f * (hi - lo)
                return lo + s, hi - s
            if a:
                lo, hi = fen(a)
                if gg and gg['b']: hi = min(hi, a['w'] * 0.8 * gg['hi'] / max(gg['w'], 1e-6))
                lw = math.log(max(a['w'], 0.5))
                iv['a'].append((math.log(max(lo, 0.5)) - lw, math.log(max(hi, 0.5)) - lw))
            if h:
                lo, hi = fen(h); iv['h'].append((lo - h['w'], hi - h['w']))
            if d:
                lo, hi = fen(d); iv['d'].append((d['w'] - hi, d['w'] - lo))
            k += 1
        def wahl(lst):
            if not lst: return 0.0
            lo = max(x[0] for x in lst); hi = min(x[1] for x in lst)
            if lo <= hi:
                return 0.0 if lo <= 0 <= hi and len(lst) == 1 and False else 0.5 * (lo + hi) if not (lo <= 0 <= hi) else 0.0
            return sum(0.5 * (x[0] + x[1]) for x in lst) / len(lst)
        da, dh, dl = wahl(iv['a']), wahl(iv['h']), wahl(iv['d'])
        k = 1
        p = par[gr]
        p['h'] = round(p['h'] + G * dh / k, 2)
        p['C'] = round(p['C'] * math.exp(max(-0.5, min(0.4, G * da / k))), 3)
        p['beta'] = round(min(5.0, max(-1.0, p['beta'] + G * 0.2 * dl / k)), 3)
        zeile.append('%s h%.0f C%.1f b%.2f' % (gr[:8], p['h'], p['C'], p['beta']))
    rot = ['%s %.1f' % (k_, v['w']) for k_, v in r.items() if v['b'] and not v['ok']]
    print('it %d: %d/%d  ROT: %s' % (it, g, n, ', '.join(rot)))
    print('    ' + ' | '.join(zeile), flush=True)
    json.dump(par, open(aus, 'w'), indent=1)
