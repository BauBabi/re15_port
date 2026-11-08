"""Zahlen-Pruefer V4 r4: T5/T6 Breitenprofil mit eigener Mittellinie (pz4_mess.schwanz), Achse Knick -> Beginn wahlweise ab der Saat
(Oberkante Ansatz, wie pz2/pz4_mess) oder ab dem Mittellinienpunkt naechst dem T1-Beginn (Id 16 an Id 7/8, Messblatt-Wortlaut);
Breite quer zur Mittellinie in allen Schwanz-Ids; dazu das volle Profil je 0,05 f. V4 r4 / V4 r2 / V3 r2."""
import json, numpy as np, sys
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz4/'
M = json.load(open(SP + 'mess_V4_V4R3_V3.json'))
T1 = {'V4': (358, 238), 'V4R3': (358, 238), 'V3': (352, 233)}
for nm in ('V4', 'V4R3', 'V3'):
    T = M[nm]['seite']['T']; H = M[nm]['seite']['H']
    P, W, s, jk = np.array(T['_P']), np.array(T['_W']), np.array(T['_s']), T['_jk']
    j0 = int(np.argmin(np.linalg.norm(P[:jk] - np.array(T1[nm]), axis=1)))
    for art, a in (('ab Saat', 0), ('ab T1', j0)):
        L = s[jk] - s[a]
        wf = lambda f: W[int(np.argmin(np.abs(s - (s[jk] - f * L))))] / H
        pf = [round(wf(f), 3) for f in np.arange(0.0, 1.0001, 0.05)]
        seq = [wf(f) for f in np.linspace(0.05, 0.75, 15)]
        ab = max(max(seq[:i + 1]) - seq[i] for i in range(len(seq)))
        soll = {0.1: (0.03, 0.015), 0.3: (0.065, 0.02), 0.5: (0.125, 0.025), 0.7: (0.14, 0.025)}
        ok = all(abs(wf(f) - z) <= t for f, (z, t) in soll.items()) and ab <= 0.01
        print('%-5s %-8s f0,1/0,3/0,5/0,7 = %s  Abfall(0,05-0,75) %.3f -> %s' % (nm, art, [round(wf(f), 3) for f in (0.1, 0.3, 0.5, 0.7)], ab, 'OK' if ok else 'ROT'))
        print('      Profil f 0..1 je 0,05:', pf)
