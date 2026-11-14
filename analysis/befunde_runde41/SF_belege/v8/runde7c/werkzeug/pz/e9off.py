"""E9/E13-Aufteilung offline aus Bank-JSON (S16 ueber_soll, 177 Ansichten, 7 Varianten): vorn cos >= 0,4 (m5_cos.json) ohne Foto-Posen;
Foto PV/P1/P2; Seite Spiel (F..., cos < 0,4); Seite Nicht-Spiel (gedrehte Foto-Posen, cos < 0,4, E13 GELB).  python e9off.py JSON..."""
import sys, json
C = json.load(open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/erk/d/m5_cos.json'))
for p in sys.argv[1:]:
    J = json.load(open(p)); u = J['S16']['ueber_soll']
    kl = {'vorn': [], 'foto': [], 'seite': [], 'nicht_spiel': []}
    for nm, px, t, mt in u:
        if nm in ('PV', 'P1', 'P2'): kl['foto'].append((nm, px, t, mt))
        elif C[nm] >= 0.4: kl['vorn'].append((nm, px))
        elif nm.startswith('F'): kl['seite'].append((nm, px, t, mt))
        else: kl['nicht_spiel'].append((nm, px, t, mt))
    print(p.rsplit('/', 1)[-1], 'vorn %d / %d | foto %s | seite %s | nicht-Spiel %s' % (len(kl['vorn']), sum(x[1] for x in kl['vorn']), kl['foto'], kl['seite'], kl['nicht_spiel']))
