import re, sys, os
G = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/abn1/gegenprobe'
W = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_waffen/re15_port'
out = []
for name in ('engine_cmds_neu.txt', 'support_cmds_neu.txt'):
    for l in open(os.path.join(G, name)):
        l = l.strip()
        src = re.search(r' -c (\S+)$', l).group(1)
        rel = src.replace(W + '/', '')
        obj = G + '/obj/' + rel.replace('/', '__') + '.obj'
        l = re.sub(r' -MD -MT \S+ -MF \S+', '', l)
        l = re.sub(r' -o \S+ -c \S+$', ' -o ' + obj + ' -c ' + G + '/re15_port/' + rel, l)
        # Include-Pfade auf den alten Baum (Assets bleiben: -D...ASSETS_PATH unveraendert)
        l = l.replace('-I' + W + '/include', '-I' + G + '/re15_port/include').replace('-I' + W + '/platform/pc/src', '-I' + G + '/re15_port/platform/pc/src')
        l = l.replace('C:\msys64\mingw64\bin\gcc.exe', 'gcc')
        out.append(l)
open(os.path.join(G, 'cmds_alt.txt'), 'w').write('\n'.join(out) + '\n')
print(len(out))
