"""Raster Ecke 49 (Huegel) ab rs4a.json mit Kette kr4a.json - Herkunft der Verschiebung (0,-35,+40) in rs4f_start.json."""
import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import ruecken_r4 as RR, v8_gemein as V
R = RR.Ruecken4(open(V.SFV8 + '/emd/EM30_V7.EMD', 'rb').read(), json.load(open('kr4a.json')))
d0 = {k: [float(c) for c in v] for k, v in json.load(open('rs4a.json'))['ecken'].items()}
for dy in (-15, -25, -35, -50, -65):
    for dx in (-40, -20, 0, 20, 40, 80, 120):
        for dz in (-40, 0, 40, 80, 120):
            d = {a: list(b) for a, b in d0.items()}
            cur = list(d.get('49', [0, 0, 0])); cur[0] += dx; cur[1] += dy; cur[2] += dz; d['49'] = cur
            c, m = R.kosten(d)
            print((round(c, 2), dx, dy, dz, m['S3']['b_huegel'], m['S3']['a_kuppe'], m['S3']['d_sattel_kopf'], sum(v for _n, v in m['schlitz']),
                   [(v['ueber_max'], v['neue_ecken']) for v in m['vorn']], m['S2']['b']), flush=True)
