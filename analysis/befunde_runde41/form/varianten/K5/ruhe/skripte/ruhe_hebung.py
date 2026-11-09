#!/usr/bin/env python3
"""Alternative zum Hautfaktor (Messblatt RUHE 3.4): Hebung L' = L + d statt L' = f x L, gleiche Kette, gleiche Masse. Nur Vorschau im Speicher."""
import json, os
import numpy as np
import rl, ruhe_mass as M, ruhe_vorschau as V, riegel_k5d as RK, ruhe_wunsch as W, ruhe_material as RM, ruhe_lauf as RL
V.MODUS = 'hebung'
vor = RK.Vorlage()
fig = {}
for n in (1, 2):
    _s, Kc = rl.v4f.szene_id(rl.BASIS, n)
    fig[n] = RK.nutzer_union(n) & Kc['maske'] & ~vor.d[n]['clip']
ziel2 = float(vor.d[2]['LV'][fig[2]][:, 0].mean())
lo, hi, lauf = 0.0, 40.0, []
for _ in range(9):
    d = 0.5 * (lo + hi)
    mo, *_r = V.bauen(d)
    h2 = V.hell_bild(mo, 2, fig[2])[0]
    lauf.append(dict(d=round(d, 3), bild2=h2)); print(lauf[-1], flush=True)
    if h2['video'] >= ziel2: hi = d
    else: lo = d
d = round(hi, 3)
mo, worte, info, zj = V.bauen(d)
erg = dict(modus='hebung', d=d, bisektion=lauf, rampen=info, helligkeit={n: V.hell_bild(mo, n, fig[n])[0] for n in (1, 2)})
ws, ks = [], []
for wn, az, el in rl.WINKEL4:
    img, K = rl.weiss_id(mo, 2, az, el)
    m = M.masse(img, K['maske'], 2); ws.append(m); erg.setdefault('weiss', {})[wn] = m
    ks.append(RL.kachel(img, 'K5e-Plan HEBUNG d %.1f, %s' % (d, wn), RL.zeilen_text(m)))
erg['weiss']['MITTEL'] = M.mittel(ws)
acc = np.zeros(24)
for d_ in ws:
    for s_, p_ in d_['sektoren'].items():
        acc[int(s_) // 15] += p_ * d_['px'] * d_['bunt_pct'] / 1e4
acc /= acc.sum()
erg['weiss']['S5_gepoolt'] = int((acc >= 0.05).sum())
for n in (1, 2):
    s, K = V.szene_vorschau(mo, n)
    vids = [rl.v4f.video(s, n, sa) for sa in rl.v4f.SAATEN]
    erg.setdefault('szene', {})['bild%d' % n] = dict(ohne_filter=M.masse(s, K['maske'], 2), video=M.mittel([M.masse(v, K['maske'], 3) for v in vids]))
F, _f, _t = RM.flaechen(rl.BASIS)
erg['wuensche'] = W.wuensche(W.albedo(F, zj, worte), mo, 2, W.mat())
V.MODUS = 'faktor'
f = json.load(open(os.path.join(rl.OUT, 'vorschau.json'), encoding='utf-8'))['f']
mo2, *_r = V.bauen(f)
k2 = [RL.kachel(rl.weiss_id(mo2, 2, az, el)[0], 'K5e-Plan FAKTOR f %.3f, %s' % (f, wn)) for wn, az, el in rl.WINKEL4]
RL.speichern('v4_plan_faktor_gegen_hebung.png', RL.stapel([RL.reihe(k2), RL.reihe(ks)], 'V4 Helligkeit der Haut: oben EIN Faktor f x L* (Auftrag), unten Hebung L* + d '
                                                          '(G4-Kontrast 1:1); beide Bild 2 Video = Vorlage. Vorschau im Speicher, weisses Licht, roh.'))
json.dump(erg, open(os.path.join(rl.OUT, 'vorschau_hebung.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
print(json.dumps({k: erg[k] for k in ('d', 'helligkeit')}), json.dumps({k: erg['weiss']['MITTEL'].get(k) for k in ('S5', 'F', 'F_fremd', 'C_std', 'L_mittel', 'spanne90', 'block_ton_streu')}))
print(json.dumps(erg['wuensche'])[:1500])
