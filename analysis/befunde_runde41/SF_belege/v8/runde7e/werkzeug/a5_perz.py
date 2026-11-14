import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, riegel_handfarbe as RF, hand_mass as HM, v4_gemein as G
P = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8'
plain = sys.argv[1] if len(sys.argv) > 1 else P + '/r7d/szene'
hf = sys.argv[2] if len(sys.argv) > 2 else P + '/r7e/mk/lauf_HF_ALT'
def q(rgb, M, nm):
    lab = G.lab(rgb[M].astype(float)); C = np.hypot(lab[:, 1], lab[:, 2])
    print('   %-16s n %5d L p10/50/90 %4.1f %4.1f %4.1f | a p50/90 %4.1f %4.1f | b p10/50 %5.1f %5.1f | C>8 %.3f  b<-5 %.3f  a>=10 %.3f  L<10 %.3f' % (
        nm, len(lab), *np.percentile(lab[:, 0], [10, 50, 90]), *np.percentile(lab[:, 1], [50, 90]), *np.percentile(lab[:, 2], [10, 50]),
        (C > 8).mean(), (lab[:, 2] < -5).mean(), (lab[:, 1] >= 10).mean(), (lab[:, 0] < 10).mean()))
vorl = '--vorlage' in sys.argv
for f, b in RF.PAARE_F[:3]:
    a = RF.ppm(plain, f); h = RF.ppm(hf, f)
    bb = RF.bereiche_modell(RF.klassen(h, a))
    print('F%d' % f)
    for n in RF.BEREICHE: q(a, bb[n], n)
    if vorl:
        v = HM.vorlage(b); bv = RF.bereiche_vorlage(v); rgb = v['_bild'][..., 2::-1].astype(float)
        print('birkin%d' % b)
        for n in RF.BEREICHE: q(rgb, bv[n], n)
