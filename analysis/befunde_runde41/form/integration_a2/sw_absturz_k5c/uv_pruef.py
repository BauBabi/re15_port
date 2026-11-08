import sys, struct
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/tools')
import form_emd as FE
emd = open(sys.argv[1], 'rb').read(); tim = open(sys.argv[2], 'rb').read()
t = FE.tim_lesen(tim)
tw = t['pixel'].shape[1]; th = t['pixel'].shape[0]; nc = len(t['cluts'])
print('TIM %dx%d, %d CLUT-Zeilen ab VRAM-y %d -> Port-Textur %dx%d' % (tw, th, nc, t['clut_y'], tw, th*nc))
form = FE.lese_emd(emd)
print('Meshes %d, Clips %d' % (len(form['meshes']), len(form['clips'])))
n = 0; schlecht = 0; umax = vmax = 0; umin = vmin = 1 << 30; idx = set(); pages = set()
for mi, m in enumerate(form['meshes']):
    for art in ('dreiecke', 'vierecke'):
        for fi, f in enumerate(m[art]):
            n += 1
            po = (f['page'] & 0xF) * 128
            ci = ((f['clut'] >> 6) & 0x1FF) - t['clut_y']
            pages.add(f['page']); idx.add(ci)
            for u, v in f['uv']:
                x = u + po
                umax = max(umax, x); umin = min(umin, x); vmax = max(vmax, v); vmin = min(vmin, v)
                if not (0 <= x <= tw - 1 and 0 <= v <= th - 1 and 0 <= ci < nc):
                    schlecht += 1
                    if schlecht <= 10: print('  ausserhalb: Mesh %d %s %d uv(%d,%d) page 0x%x clut-idx %d' % (mi, art, fi, u, v, f['page'], ci))
print('Flaechen %d, Ecken ausserhalb %d; Texel-x %d..%d, v %d..%d, Seiten %s, CLUT-Indizes %s' % (n, schlecht, umin, umax, vmin, vmax, sorted(hex(p) for p in pages), sorted(idx)))
