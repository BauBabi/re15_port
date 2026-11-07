"""V2 3.2 letzter Absatz (eingeeichte Riegel mit zweiter Saat): die Ketten-Riegel K1v2/K2v2/K3-K10/K4b an einer Variante mit den Saaten 15 (Riegel),
12345 (Zahlen-Pruefer r3) und 7 des Videofilters; Rundlauf und Original laufen mit derselben Saat. Ausgabe JSON und Tabelle.

    python kf_saaten.py <Variantenordner> <aus.json>"""
import json
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import riegel_farbe as RF  # noqa: E402
import p_zonen as PZ  # noqa: E402

orig = PZ.FV.video_filter
out = {}
for saat in (15, 12345, 7):
    PZ.FV.video_filter = lambda *a, _s=saat, **k: orig(*a, **dict(k, saat=_s))
    r = {}
    for k_, (v, ok) in RF.PR.kette_riegel(sys.argv[1]).items():
        if not k_.startswith(('K1 ', 'K2 ')):
            r[k_] = (v, bool(ok))
    for k_, (v, ok) in RF.kette_v2(sys.argv[1]).items():
        r[k_] = (v, bool(ok))
    out[saat] = {k_: dict(wert=str(v), ok=ok) for k_, (v, ok) in r.items()}
    print('Saat', saat, '%d/%d GRUEN' % (sum(ok for _v, ok in r.values()), len(r)), flush=True)
    for k_, (v, ok) in r.items():
        print('  %-50s %-60s %s' % (k_, str(v)[:60], 'GRUEN' if ok else 'ROT'))
PZ.FV.video_filter = orig
json.dump(out, open(sys.argv[2], 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
