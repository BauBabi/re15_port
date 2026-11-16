"""Vergleich zweier EMD: Abschnitte 0-6 roh, Meshes Feld fuer Feld, TIM.   python vgl.py A B [C]
Mit C: Meshes 8/9 von A gegen C (R3), alle anderen A gegen B (Eingang)."""
import sys
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools')
import kopf_orig_bau as KB
a, b = open(sys.argv[1], 'rb').read(), open(sys.argv[2], 'rb').read()
c = open(sys.argv[3], 'rb').read() if len(sys.argv) > 3 else None
da, _, ma, ta = KB.teile(a); db, _, mb, tb = KB.teile(b)
for i in range(7):
    print('Abschnitt', i, 'gleich' if a[da[i]:da[i + 1]] == b[db[i]:db[i + 1]] else 'UNGLEICH')
print('TIM/CLUT', 'gleich' if ta == tb else 'UNGLEICH')
mc = KB.teile(c)[2] if c else None
ung = []
for i in range(len(ma)):
    ref = mc if (c and i in (8, 9)) else mb
    if ma[i] != ref[i]:
        ung.append(i)
print('Meshes', len(ma), len(mb), 'ungleich (8/9 gegen C, Rest gegen B):' if c else 'ungleich:', ung)
