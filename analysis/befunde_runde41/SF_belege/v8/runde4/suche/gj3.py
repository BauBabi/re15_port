import sys, json, itertools
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import ruecken_r4 as RR, v8_gemein as V
R = RR.Ruecken4(open(V.SFV8 + '/emd/EM30_V7.EMD', 'rb').read(), json.load(open('kr4b.json')))
d0 = {k: [float(c) for c in v] for k, v in json.load(open('rs4h.json'))['ecken'].items()}
for z41 in (10, 15, 20, 25):
    for s38 in (0, 5, 10, 15, 20, 30):
        for s58 in (0, 15):
            d = {a: list(b) for a, b in d0.items()}
            c = d.get('41', [0, 0, 0]); d['41'] = [c[0], c[1], c[2] + z41]
            c = d.get('38', [0, 0, 0]); d['38'] = [c[0] + 0.7 * s38, c[1] + 0.7 * s38, c[2]]
            c = d.get('58', [0, 0, 0]); d['58'] = [c[0] + 0.7 * s58, c[1] + 0.7 * s58, c[2]]
            cc, m = R.kosten(d)
            ok = m['S2']['b'][0] <= 2 and m['S2']['c']['huckel_px'] <= 3 and m['S2']['a']['mittel'] <= 4 and m['S2']['a']['max'] <= 6 and m['S3']['ok']
            print('%s %d %d %d | %.2f | S2 %s %s c %.1f %s | S3 %s %s | S4w %.1f | schl %d | vorn %s' % ('OK' if ok else '--', z41, s38, s58, cc, m['S2']['a'], m['S2']['b'], m['S2']['c']['huckel_px'], m['S2']['c']['bei_x'], m['S3']['b_huegel'], m['S3']['ok'], m['s4_wurzel'], sum(x for _n, x in m['schlitz']), [x['ueber_max'] for x in m['vorn']]), flush=True)
