import sys, numpy as np, cv2, colorsys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools')
import emd_ansichtsblatt as EA
def modell(blob):
    mo = EA.Modell.__new__(EA.Modell); d = EA.emd_dir(blob)
    mo.meshes = EA.md1_parse(blob[d[7]:d[8]]); mo.tex, mo.pal, mo.clut_y = EA.tim_decode(blob[d[8]:]); mo.texh, mo.texw = mo.tex.shape
    return mo
def texel_farben(mo):
    out = []
    for b, m in enumerate(mo.meshes):
        for uvl, n in ((m.tuv, 3), (m.quv, 4)):
            for uv in uvl:
                row = mo.clut_row(uv[2]); ub = mo.page_ubase(uv[5])
                pts = np.array([[uv[3*k] + ub, uv[3*k+1]] for k in range(n)], np.int32)
                if n == 4: pts = pts[[0, 1, 3, 2]]
                mk = np.zeros((mo.texh, mo.texw), np.uint8); cv2.fillPoly(mk, [pts], 1)
                out.append(mo.pal[row][mo.tex[mk > 0]])
    return np.concatenate(out).astype(float)
def haut(c):
    r, g, b = c[:, 0], c[:, 1], c[:, 2]; L = c @ np.array([0.299, 0.587, 0.114])
    jeans = b > r; wunde = (r > 2.2 * np.maximum(g, 1)) & (r > 80); dunkel = L < 20
    s = ~jeans & ~wunde & ~dunkel
    m = c[s].mean(0); h, sat, v = colorsys.rgb_to_hsv(*(m / 255))
    return m, h * 360, sat, s.mean(), wunde.mean()
ems = open(EA.EMS_PFAD, 'rb').read(); o, l = EA.ems_entry(ems, EA.EMS_ORDER.index(0x30))
quellen = [('EM30 Auslieferung (3070/5080/5090 1. Form)', ems[o:o+l]), ('EM30_HANGAR NB2 6a1e9430d', open('em30h_nb2.emd', 'rb').read()),
           ('EM30_HANGAR 3f91e5c57', open('em30h_3f91.emd', 'rb').read()),
           ('Bahnsteig-Form RE2/BAHNSTEIG_G4.EMD', open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/RE2/BAHNSTEIG_G4.EMD', 'rb').read())]
for name, blob in quellen:
    try:
        c = texel_farben(modell(blob)); m, h, s, ant, wa = haut(c)
        print('%-45s Haut RGB (%3d,%3d,%3d) Farbton %5.1f Grad Saettigung %.2f  Hautanteil %.2f Wundanteil %.2f' % (name, m[0], m[1], m[2], h, s, ant, wa))
    except Exception as e:
        print(name, 'Fehler', e)
import re, os
import birkin_bahnsteig as BB
def re2_modell(emd, tim):
    d = BB.verzeichnis(emd); mo = EA.Modell.__new__(EA.Modell)
    mo.meshes = EA.md1_parse(emd[d[7]:]); mo.tex, mo.pal, mo.clut_y = EA.tim_decode(tim); mo.texh, mo.texw = mo.tex.shape
    return mo
inc = open(BB.TOC_PFAD, encoding='utf-8', errors='replace').read()
toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S).group(1))]
ems2 = open(BB.EMS_PFAD, 'rb').read()
def rec(kind, r):
    i = ((kind - 0x10) * 4 + r) * 2
    return ems2[toc[i] * 0x800:toc[i] * 0x800 + toc[i + 1]]
for name, (emd, tim) in (('Bahnsteig-Form RE2/BAHNSTEIG_G4 (5090 nach der Mutation)', (open(os.path.join(BB.RE2_DIR, 'BAHNSTEIG_G4.EMD'), 'rb').read(), open(os.path.join(BB.RE2_DIR, 'BAHNSTEIG_G4.TIM'), 'rb').read())),
                         ('G4 RE2 Auslieferung (Vorlage der Bahnsteig-Form)', (rec(0x34, 3), rec(0x34, 2))),
                         ('G5 RE2 EM36 (5090 Endkampf)', (rec(0x36, 3), rec(0x36, 2)))):
    try:
        c = texel_farben(re2_modell(emd, tim)); m, h, s, ant, wa = haut(c)
        print('%-45s Haut RGB (%3d,%3d,%3d) Farbton %5.1f Grad Saettigung %.2f  Hautanteil %.2f Wundanteil %.2f' % (name, m[0], m[1], m[2], h, s, ant, wa))
    except Exception as e:
        import traceback; traceback.print_exc()
