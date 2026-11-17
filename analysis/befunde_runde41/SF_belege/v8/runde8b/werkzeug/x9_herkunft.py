"""Herkunft Runde 8b2: Texel, die die Flaechen der Meshes 16-22 im Spiel-Asset benutzen, gegen das Disc-Original EM30 (Texturbild)
und die Nutzung der Original-Zeile durch Original-Flaechen. Ausgabe je Zeile: Texel, davon gleich Original, in Original-Nutzung."""
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
print('Texturbild Asset == Disc-Original:', bool(np.array_equal(t['pix'], to['pix'])), '; Pixel verschieden:', int((t['pix'] != to['pix']).sum()))
for z, zo in ((495, 482), (493, 480), (487, None)):
    mk = np.zeros((256, 384), bool)
    for b in range(16, 23):
        for uv in ms[b]['tuv'] + ms[b]['quv']:
            if ((uv[2] >> 6) & 0x1FF) == z:
                mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
    n = int(mk.sum()); gl = int((mk & (t['pix'] == to['pix'])).sum())
    s = 'Zeile %d: %d Texel, davon gleich Disc-Original %d' % (z, n, gl)
    if zo is not None:
        nu = A4.nutzung(None, zo)
        s += ', in Nutzung der Original-Zeile %d durch Original-Flaechen %d; CLUT-Zeile Asset %d == Original %d: %s' % (
            zo, int((mk & nu).sum()), z, zo, bool(np.array_equal(t['clut'][z - 480] & 0x7FFF, to['clut'][zo - 480] & 0x7FFF)))
    print(s)
