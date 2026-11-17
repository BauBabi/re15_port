import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein, kopf_orig_bau as KB
a, b = open(sys.argv[1], 'rb').read(), open(sys.argv[2], 'rb').read()
da, _, ma, ta = KB.teile(a); db, _, mb, tb = KB.teile(b)
print('Sektionen 0-6 (EDD/EMR/Keyframes ...) bytegleich:', all(a[da[i]:da[i + 1]] == b[db[i]:db[i + 1]] for i in range(7)))
print('TIM (Textur+CLUT) bytegleich:', ta == tb)
print('Meshes gleich:', [i for i in range(len(ma)) if ma[i] == mb[i]])
print('Meshes anders:', [i for i in range(len(ma)) if ma[i] != mb[i]])
