import sys, struct, importlib.util
sys.argv = [sys.argv[0]] + sys.argv[1:]
spec = importlib.util.spec_from_file_location('z', r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_rolltor20a0/analysis/befunde_runde38/W_belege/zensus_abe.py')
src = open(spec.origin, encoding='utf-8').read().replace('\nmain()\n', '\n')
ns = {'__name__': 'z', '__file__': spec.origin}
exec(compile(src, spec.origin, 'exec'), ns)
rdt, oid = sys.argv[1], int(sys.argv[2])
d = open(rdt, 'rb').read()
tbl = struct.unpack_from('<I', d, 0x30)[0]
tim_off, md1_off = struct.unpack_from('<II', d, tbl + oid * 8)
tim = ns['tim_lesen'](d, tim_off)
print('TIM bpp', tim['bpp'], 'clut', tim['cx'], tim['cy'], tim['cw'], 'x', tim['ch'], 'img', tim['ix'], tim['iy'], tim['iw'], 'x', tim['ih'])
for r in range(tim['ch']):
    row = tim['clut'][r*tim['cw']:(r+1)*tim['cw']]
    print(' CLUT-Zeile %d: STP %d/%d, 0000h %d, erste %s' % (r, sum(1 for c in row if c & 0x8000), len(row), sum(1 for c in row if c == 0), ' '.join('%04x' % c for c in row[:8])))
polys = ns['md1_uvs'](d, md1_off)
cl = {}
for p, c, uv in polys: cl[(p, c)] = cl.get((p, c), 0) + 1
print('Seite/CLUT der Polygone:', {('0x%04x' % k[0], '0x%04x' % k[1]): v for k, v in cl.items()})
