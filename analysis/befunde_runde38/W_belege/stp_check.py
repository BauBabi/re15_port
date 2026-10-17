"""stp_check.py - Runde 38 Spur W N1: CLUT-Zeilen (STP-Anteil) einer Objekt-TIM und die Seiten/CLUT-Woerter ihrer Polygone.
Aufruf: python stp_check.py <RDT> <obj>"""
import os, sys, struct, importlib.util
sys.argv = [sys.argv[0]] + sys.argv[1:]
spec = importlib.util.spec_from_file_location('z', os.path.join(os.path.dirname(os.path.abspath(__file__)), 'zensus_abe.py'))
src = open(spec.origin, encoding='utf-8').read()
src = src[:src.rfind('main()')]
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
