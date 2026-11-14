"""E9-Aufteilung echte Dumps (szene.py luecke -> N_luecke.json, F235-F378): vorn = cos >= 0,4 (Kopf-Vorwaertsachse zur Kamera,
P5/erk/d/m5_cos.json), ohne Vordergrund F316-F333 (Bank VORDERGRUND). Klasse vorn: Anzahl ueber Soll / Summe px dieser Bilder;
Seite: Liste.  python e9echt.py N1 N2 ..."""
import sys, json
C = json.load(open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/erk/d/m5_cos.json'))
L_ = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/laeufe/'
for n in sys.argv[1:]:
    J = json.load(open(L_ + n + '_luecke.json'))
    vorn, seite = [], []
    for m in J:
        if 316 <= m['f'] <= 333 or m['ok']: continue
        (vorn if C['F%d' % m['f']] >= 0.4 else seite).append((m['f'], m['summe'], m['tief'], m['maxtiefe']))
    print('%-4s vorn ueber Soll %d / Summe %d | Seite %s | vorn %s' % (n, len(vorn), sum(v[1] for v in vorn), seite, [v[:2] for v in vorn]))
