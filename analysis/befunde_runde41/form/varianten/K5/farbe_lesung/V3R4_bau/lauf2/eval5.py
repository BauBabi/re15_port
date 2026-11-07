"""Bauer V3 R4 Lauf 2: Parametersatz bewerten (RV1-RV4 im Flaechen-ID-Raster, Saaten waehlbar) + Ader/Ring (Texel, Kette).
    python eval5.py par.json [3]   (3 = drei Saaten)"""
import sys, json
B = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/r4b/bau5'
sys.path.insert(0, B)
import such4 as Q
Q.setze_f1(B + '/f1')     # Zeile 6 des Neubaus (Baufehler A behoben)
d = json.load(open(sys.argv[1], encoding='utf-8'))
p = d.get('par', d)
saaten = (15, 12345, 7) if len(sys.argv) > 2 and sys.argv[2] == '3' else (15,)
r, g, n = Q.bewerten(p, saaten=saaten)
print('RV %d/%d' % (g, n))
for k, v in r.items():
    if v['b']:
        rand = (v['hi'] - v['w']) if 'gross' in k else min(v['w'] - v['lo'], v['hi'] - v['w'])
        print('  %-34s %7.2f [%7.2f..%7.2f] %s  Rand %.2f' % (k, v['w'], v['lo'], v['hi'], 'GRUEN' if v['ok'] else 'ROT', rand))
if len(sys.argv) > 3:
    Q.ader_ring(p)
