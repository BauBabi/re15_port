"""Versuch Wurzel (Messung): Wurzelring teilweise abflachen (w) und Ring D16 (d) - S2 (a, b, c mit s2c_r5 Variante 0), S4 Wurzel, Vorderansicht
s .1 der Rollreihe (h22/h24/h64, phi 0)."""
import os, sys, json, subprocess
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
R8 = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
import riegel_v8 as RV
import schwanz_r8 as Q
basis = open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8/emd/ALT_07b6703f.EMD', 'rb').read()
for w, dq in ((0.5, 0.15), (0.5, 0.1), (0.55, 0.1), (0.5, 0.05)):
    Q.WURZEL_FLACH = w; Q.D16_QUER = dq
    blob = Q.stufe(basis, 'b')
    p = os.path.join(R8, 'emd', 'x3.EMD'); open(p, 'wb').write(blob)
    mo, A = RV.render(blob)
    s2, s3, om = RV.s2_s3(mo, A['bone'][:480]); c = RV.s2c_r5_eine(mo); s4 = RV.s4(mo); s1 = RV.s1(A['bone'][:480])
    out = subprocess.run([sys.executable, os.path.join(R8, 'py', 'r2_rolle.py'), p, os.path.join(R8, 'x3_rolle.json')], capture_output=True, text=True)
    r = json.load(open(os.path.join(R8, 'x3_rolle.json')))
    print('w %.2f d16 %.2f | S1 %.1f/%.1f S2a %.1f/%d b %.1f c %d | S4 wurzel %.1f | vorn s.1 h22/24/64 %s %s %s' % (w, dq, s1['mittel'], s1['max'],
          s2['a']['mittel'], s2['a']['max'], s2['b'][0], c, s4['wurzel'], r['22']['neu'][0], r['24']['neu'][0], r['64']['neu'][0]), flush=True)
