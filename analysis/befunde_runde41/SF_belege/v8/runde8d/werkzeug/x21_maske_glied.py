"""Messhilfe Runde 8d (kein Spiel-Asset): Masken-Probe je Glied und Klasse. Flaechen des Meshes N mit Klasse K (schwanz_r8.klasse_uv: grau /
bauch / band) bekommen EINEN hellen Original-Texel (Asset 494, Seite 1, u 40 v 34, fast weiss) -> einfarbig; Differenz zum Lauf des Eingangs
= Bildpunkte dieser Flaechen.   python x21_maske_glied.py <ein.EMD> <aus.EMD> <mesh> <klasse>"""
import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import kopf_orig_bau as KB
import schwanz_r8 as Q
ein, aus, mesh, kl = sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4]
blob = open(ein, 'rb').read()
d, _o, ms, tim = KB.teile(blob)
n = 0
for key in ('tuv', 'quv'):
    for uv in ms[mesh][key]:
        if Q.klasse_uv(uv) == kl:
            for k in range(0, len(uv), 3):
                uv[k], uv[k + 1] = 40, 34
            uv[2] = 494 << 6
            uv[5] = 128 + 1
            n += 1
open(aus, 'wb').write(KB.packen(blob, d, ms, tim))
print(mesh, kl, 'Flaechen', n)
