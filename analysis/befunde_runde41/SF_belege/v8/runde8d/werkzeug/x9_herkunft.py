"""Herkunft (Runde 8b2, Runde 8c je Klasse): Texel, die die Flaechen der Meshes 16-22 im Spiel-Asset benutzen, gegen das Disc-Original EM30
(Texturbild) und die Nutzung der Original-Zeile durch Original-Flaechen. Ausgabe je Klasse/Zeile: Texel, davon gleich Original, in Original-Nutzung."""
import sys, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import kopf_orig_bau as KB, birkin_formen as BF, adern_r4 as A4, schwanz_r8 as Q
pfad = sys.argv[1]
blob = open(pfad, 'rb').read()
d, _o, ms, tim = KB.teile(blob)
o = BF.lade_em30(); _d, _oo, mso, timo = KB.teile(o)
t, to = KB.tim_teile(tim), KB.tim_teile(timo)
print(pfad)
print('Texturbild Asset == Disc-Original: %s ; Pixel verschieden (Erbe der Stufen bis 7e): %d' % (bool(np.array_equal(t['pix'], to['pix'])),
                                                                                                  int((t['pix'] != to['pix']).sum())))
gruppen = {}
for b in range(16, 23):
    for uv in ms[b]['tuv'] + ms[b]['quv']:
        z = (uv[2] >> 6) & 0x1FF
        kl = Q.klasse_uv(uv) or 'ohne Klasse'
        if z == 495:
            kl = 'Wurzel (Stufe w/u)' if b == 16 else kl
        g = gruppen.setdefault((z, kl), dict(mk=np.zeros((256, 384), bool), n=0))
        g['mk'] |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
        g['n'] += 1
for (z, kl), g in sorted(gruppen.items()):
    mk = g['mk']
    n = int(mk.sum()); gl = int((mk & (t['pix'] == to['pix'])).sum())
    s = 'Zeile %d %-18s %3d Flaechen, %5d Texel, davon gleich Disc-Original %5d' % (z, kl, g['n'], n, gl)
    if z in (493, 494, 495):
        nu = A4.nutzung(None, z - 13)
        s += ', in Nutzung der Original-Zeile %d durch Original-Flaechen %d; CLUT-Zeile %d == Original %d: %s' % (
            z - 13, int((mk & nu).sum()), z, z - 13, bool(np.array_equal(t['clut'][z - 480] & 0x7FFF, to['clut'][z - 493] & 0x7FFF)))
    print(s)
