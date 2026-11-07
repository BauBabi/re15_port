"""Ketten-Vorhersage probe.json gegen die bindenden Ziele RV1-RV4 (ziele.json)."""
import json
P = json.load(open('probe.json')); Z = json.load(open('ziele.json'))
R = {}
for key, r in P.items():
    n = key[4]
    g = t = 0; rot = []
    for rn, v in r.items():
        z = Z.get('bild%s|%s|T' % (n, rn))
        if not z:
            continue
        for m in ('rot_eng', 'h_rot', 'dL', 'gross_eng_p'):
            x = z.get(m)
            if not x or not x.get('ziel') or not x.get('bindend') or v.get(m) is None:
                continue
            lo, hi = x['ziel']
            ok = v[m] <= hi if m == 'gross_eng_p' else lo <= v[m] <= hi
            t += 1; g += ok
            if not ok:
                rot.append('%s %s %.1f' % (rn, m, v[m]))
    R[key] = (g, t, rot)
    print(key, '%d/%d' % (g, t), '; '.join(rot))
json.dump(R, open('probe_auswert.json', 'w'), indent=1, ensure_ascii=False)
