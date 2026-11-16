"""Runde 3: Zusammenfassung der r3_rot_alle-Messungen ueber die Asset-Staende + Vergleich K5e_f1 / Krallen-Kandidat -> messung/r3_staende.json."""
import json
import os
from s_basis import ZIEL

STAENDE = [('asset_0e10882f9', '0e10882f9', '2026-11-02', 'erste Riesenform'), ('asset_6200b3d35', '6200b3d35', '2026-11-02', 'Nachschliff'),
           ('asset_a452893f1', 'a452893f1', '2026-11-02', 'K3'), ('asset_67408cfcb', '67408cfcb', '2026-11-03', 'K4'),
           ('asset_a005b357a', 'a005b357a', '2026-11-07', 'K5c'), ('K5e_f1', '62bd0001b', '2026-11-09', 'K5e_f1 (heute)'),
           ('Krallen-Kandidat', '-', '-', 'Krallen-Kandidat')]
M = ZIEL + '/messung/'
out = {'staende': []}
for dat, commit, datum, name in STAENDE:
    p = M + 'r3_rot_alle_%s.json' % dat
    if not os.path.exists(p):
        continue
    e = json.load(open(p, encoding='utf-8'))
    tx = e['textur_streifen_flaechen']
    out['staende'].append(dict(name=name, commit=commit, datum=datum, clips=e['clips'], schritt=e['schritt'],
                               flaechen_an_der_stelle=len(tx), davon_rot=sum(1 for v in tx.values() if v['rotanteil'] >= 0.5),
                               zeilen=sorted(set(v['zeile'] for v in tx.values())),
                               von_unten=[e['zusammenfassung']['von unten'][k] for k in ('bilder_mit_rot_aus_streifen', 'bilder', 'median_rot_streifen', 'max_rot_streifen')],
                               spiel_5090=[e['zusammenfassung']['Spiel 5090'][k] for k in ('bilder_mit_rot_aus_streifen', 'bilder', 'median_rot_streifen', 'max_rot_streifen')],
                               rot_hals_kopf_kiefer_von_unten=[e['zusammenfassung']['von unten']['bilder_mit_rot_hals_kopf_kiefer'], e['zusammenfassung']['von unten']['median_rot_alle']]))
a = json.load(open(M + 'r3_rot_alle_K5e_f1.json', encoding='utf-8'))['je_bild']
b = json.load(open(M + 'r3_rot_alle_Krallen-Kandidat.json', encoding='utf-8'))['je_bild']
d = [abs(x[n]['rot_px_streifen'] - y[n]['rot_px_streifen']) for x, y in zip(a, b) for n in ('Spiel 5090', 'von unten')]
out['k5e_gegen_krallen'] = dict(ansichten=len(d), abweichend=sum(1 for v in d if v), max_abweichung_px=max(d),
                                bilder_mit_rot_gleich=all((x[n]['rot_px_streifen'] > 0) == (y[n]['rot_px_streifen'] > 0)
                                                          for x, y in zip(a, b) for n in ('Spiel 5090', 'von unten')))
json.dump(out, open(M + 'r3_staende.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
for s in out['staende']:
    print(s)
print(out['k5e_gegen_krallen'])
