# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Biegeprobe je Gelenkklasse zusammengefasst, aus biege.json und krater.json. Kerbe erst ab 30 Grad, darunter ist das Messdreieck zu schmal."""
import json, numpy as np
AUS = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/urteil_spiel'
b = json.load(open(AUS + '/biege.json'))
kr = json.load(open(AUS + '/krater.json'))
KL = {'Ellbogen': lambda n: n.startswith('ellbogen'), 'Knie': lambda n: n.startswith('knie'),
      'Wirbelsaeule': lambda n: n.startswith('wirbel') or n.startswith('brust'),
      'Schwanzglieder': lambda n: n.startswith('schwanz') and not n.startswith('schwanzwurzel'),
      'Schwanzwurzel': lambda n: n.startswith('schwanzwurzel')}
aus = {}
print('Klasse         | Modell | Kerbe max (Gelenk, Grad) | Kerbe Mittel | Dicke min | Krater max (Gelenk, Grad)')
for kl, f in KL.items():
    for k in 'ABC':
        ke = []
        di = []
        for nm, z in b[k].items():
            if not f(nm):
                continue
            for m in z:
                if m['soll'] >= 30:
                    ke.append((m['kerbe'], nm, m['soll']))
                    if m['soll'] <= 90:
                        di.append((m['dicke'], nm, m['soll']))
        kk = []
        for nm, z in kr[k]['krater'].items():
            if not f(nm):
                continue
            for g, v in z:
                kk.append((v, nm, g))
        w = max(ke)
        d = min(di)
        q = max(kk)
        print('%-14s | %s | %.3f %s %d | %.3f | %.2f %s %d | %.3f %s %d' % (kl, k, w[0], w[1], w[2], float(np.mean([x[0] for x in ke])), d[0], d[1], d[2], q[0], q[1], q[2]))
        aus.setdefault(kl, {})[k] = dict(kerbe_max=w, kerbe_mittel=round(float(np.mean([x[0] for x in ke])), 3), dicke_min=d, krater_max=q)
print()
print('Krater Ellbogen und Knie je Winkel, Mittel beider Seiten')
for k in 'ABC':
    for kl in ('ellbogen', 'knie'):
        a = np.array([[v for g, v in kr[k]['krater'][kl + s]] for s in ('_l', '_r')]).mean(0)
        print(' ', k, kl, dict(zip((30, 60, 90, 120, 150), np.round(a, 2).tolist())))
print('RE2 Krater in echten Bildern, Median je Klasse, p90-Bild')
for kl in ('ellbogen', 'knie', 'wirbel', 'schwanz'):
    v = [z[1][2] for n, z in kr['eich'].items() if kl in n and len(z) > 1]
    print(' ', kl, 'Median %.3f Max %.3f' % (float(np.median(v)), float(np.max(v))), 'n', len(v))
json.dump(aus, open(AUS + '/biege_zusammen.json', 'w'), indent=1)
