import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import ruecken_r4 as RR, v8_gemein as V, wurzel_r4 as W4
R = RR.Ruecken4(open(V.SFV8 + '/emd/EM30_V7.EMD', 'rb').read(), json.load(open('kr4b.json')))
d0 = {k: [float(c) for c in v] for k, v in json.load(open('rs4h.json'))['ecken'].items()}
for z41 in (0, 10, 20):
    for zn in (1.0, 0.97, 0.94, 0.9, 0.85):
        W4.ZUG_NACH = zn
        d = {a: list(b) for a, b in d0.items()}
        c = d.get('41', [0, 0, 0]); d['41'] = [c[0], c[1], c[2] + z41]
        cc, m = R.kosten(d)
        print(z41, zn, round(cc, 2), 'S2', m['S2']['a'], m['S2']['b'], 'c', m['S2']['c']['huckel_px'], m['S2']['c']['bei_x'], 'S4w', m['s4_wurzel'], 'bed', min(m['bedeckung']), flush=True)
