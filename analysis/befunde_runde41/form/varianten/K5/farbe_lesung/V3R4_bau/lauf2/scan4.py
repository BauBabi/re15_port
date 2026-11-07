import sys, json, copy
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/r4b/bau5')
import such4 as Q
Q.setze_f1(Q.K.K5 + '/farbe/K5c_farbe_v3')
par0 = json.load(open(sys.argv[1]))
if isinstance(par0, list):
    par0 = par0[int(sys.argv[2])]['par']
gr, feld, werte, bild, regs = sys.argv[3], sys.argv[4], [float(x) for x in sys.argv[5].split(',')], int(sys.argv[6]), sys.argv[7].split(',')
saaten = tuple(int(s) for s in sys.argv[8].split(',')) if len(sys.argv) > 8 else (15,)
for w in werte:
    p = copy.deepcopy(par0); p[gr][feld] = w
    lut = Q.I.lut_aus(Q.cluts(p))
    Q.SN._S.clear(); Q.SN._S.update(Q.SZ)
    r, g, n = Q.SN.bewerten(lut, saaten=saaten, bilder=(bild,))
    out = []
    for rn in regs:
        out.append('%s rot %.1f h %.1f dL %.1f gr %.1f' % (rn, *[r.get('B%d %s %s' % (bild, rn, m), {'w': float('nan')})['w'] for m in ('rot_eng', 'h_rot', 'dL', 'gross_eng_p')]))
    print('%s=%.2f  %d/%d  %s' % (feld, w, g, n, ' | '.join(out)), flush=True)
