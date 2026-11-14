"""Messhilfe Runde 8c (kein Spiel-Asset): Masken-Proben aus einem Schwanz-Asset. Gewaehlte Flaechen der Meshes 16-22 bekommen EINEN
hellen Original-Texel (Original-Zeile 481 = Asset 494, Seite 1, u 40 v 34; L* ~74, fast weiss), alle Ecken derselben UV -> einfarbig.
Differenz zum Lauf des Eingangs = Bildpunkte dieser Flaechen. Modus: bauch (Zeile 495), band (Zeile 493), alle (Meshes 16-22 ganz)."""
import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import kopf_orig_bau as KB
ein, aus, modus = sys.argv[1], sys.argv[2], sys.argv[3]
blob = open(ein, 'rb').read()
d, _o, ms, tim = KB.teile(blob)
n = 0
for b in range(16, 23):
    for key in ('tuv', 'quv'):
        for uv in ms[b][key]:
            z = (uv[2] >> 6) & 0x1FF
            if modus == 'alle' or (modus == 'bauch' and z == 495) or (modus == 'band' and z == 493):
                for k in range(0, len(uv), 3):
                    uv[k], uv[k + 1] = 40, 34
                uv[2] = 494 << 6
                uv[5] = 128 + 1
                n += 1
open(aus, 'wb').write(KB.packen(blob, d, ms, tim))
print(modus, 'Flaechen', n)
