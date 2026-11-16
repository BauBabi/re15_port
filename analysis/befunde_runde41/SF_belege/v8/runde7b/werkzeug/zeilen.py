import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, kopf_orig_bau as KB
blob = open(V.ASSET, 'rb').read()
d, _o, ms, tim = KB.teile(blob); t = KB.tim_teile(tim)
print('clut rows', t['clut'].shape, 'tex', t['pix'].shape, 'ih', t['ih'])
for z in range(t['clut'].shape[0]):
    print(480 + z, 'Eintraege 0:', int((t['clut'][z] == 0).sum()))
use = {}
for b, m in enumerate(ms):
    for kind, U in (('d', m['tuv']), ('v', m['quv'])):
        for uv in U:
            z = (uv[2] >> 6) & 0x1FF
            use.setdefault(b, {}).setdefault(z, 0); use[b][z] += 1
for b in sorted(use): print('mesh', b, use[b])
# transparente Texel unter Flaechen
tr = 0
for b, m in enumerate(ms):
    for U, n in ((m['tuv'], 3), (m['quv'], 4)):
        for uv in U:
            z = (uv[2] >> 6) & 0x1FF
            mk = KB.raster(uv, n, (uv[5] & 0xF) * 128)
            tr += int((t['clut'][z - 480][t['pix'][mk]] == 0).sum())
print('transparente Texel unter Flaechen', tr)
