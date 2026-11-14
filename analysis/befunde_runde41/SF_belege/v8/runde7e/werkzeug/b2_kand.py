# Runde 7e2 Kandidaten (Scratch): K3 = 7e + Kastenfinger (10 Flaechen) auf Zeile 492 mit Capcom-UV (aus 5c902d49);
# K4 = K3 + Daumen-Flaechen v1 v4 v12 (Capcom-Texel im Original-Median a* >= 10) auf Zeile 492 mit Capcom-UV
import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V, kopf_orig_bau as KB, handfarbe_r7e as HF
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7e'
neu = open(V.ASSET, 'rb').read()
alt = open(SP + '/emd/ALT_5c902d49.EMD', 'rb').read()
assert V.sha(neu).startswith('07b6703f')
ma = KB.teile(alt)[2][12]
for name, daumen in (('K3', ()), ('K4', ('v1', 'v4', 'v12'))):
    d, _o, ms, tim = KB.teile(neu)
    m = ms[12]
    alt_uv = {k: uv for k, f, uv in HF._flaechen(ma)}
    n = 0
    for k, f, uv in HF._flaechen(m):
        if HF.klasse12(f) == 'kastenfinger' or k in daumen:
            uv[:] = list(alt_uv[k]); uv[2] = (492 << 6) | (uv[2] & 0x3F); n += 1
    b = KB.packen(neu, d, ms, tim)
    open(SP + '/emd/%s.EMD' % name, 'wb').write(b)
    print(name, n, 'Flaechen auf 492', V.sha(b))
