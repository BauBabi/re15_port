"""Versuchsreihe Form (Messung, kein Bild): Parameter der Stufe q variieren, je S1, S2 (a/b/c mit s2c_r5 Variante 0), S4 messen."""
import os, sys, json, copy
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
import riegel_v8 as RV
import schwanz_r8 as Q
basis = open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8/emd/ALT_07b6703f.EMD', 'rb').read()
def mess(blob):
    mo, A = RV.render(blob)
    s1 = RV.s1(A['bone'][:480]); s2, s3, om = RV.s2_s3(mo, A['bone'][:480]); ov = {int(k): v for k, v in V.lade('m13b_oben.json')['oben_vorlage_maske'].items()}; c = RV.s2c_r5_eine(mo); s4 = RV.s4(mo)
    da = [om[x] - ov[x] for x in range(330, 386) if x in om and x in ov]
    return ('S2a argmax x %d ' % (330 + int(np.argmax(np.abs(da))))) + 'S1 %.1f/%.1f | S2a %.1f/%d b %.1f@%d c %d | S4 w %.1f m %s u %s mono %s folge %s' % (s1['mittel'], s1['max'], s2['a']['mittel'], s2['a']['max'], s2['b'][0], s2['b'][1], c,
        s4['wurzel'], s4['s90_130'], s4['unten']['s140_160'], s4['monoton'], s4['folge'])
std = dict(QUER=Q.QUER, KERBE_J=dict(Q.KERBE_J), MITTE=dict(Q.MITTE), WURZEL_FLACH=True, D16_QUER=None)
def setze(**kw):
    for k, v in std.items(): setattr(Q, k, copy.deepcopy(v))
    for k, v in kw.items(): setattr(Q, k, v)
faelle = [('E10 D16 0.1', dict(WURZEL_FLACH=False, D16_QUER=0.1)),
          ('E11 D16 0.15', dict(WURZEL_FLACH=False, D16_QUER=0.15)),
          ('E12 D16 0.2', dict(WURZEL_FLACH=False, D16_QUER=0.2))]
for nm, kw in faelle:
    setze(**kw)
    b = Q.stufe(basis, 'b')
    print(nm, '|', mess(b), flush=True)
