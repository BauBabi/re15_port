"""Runde 3: Gegenprobe der von der Grundlage Runde 3 (Commit c8fbee2f6) neu belegten Zone 30 "geoeffnetes Maul" gegen die
Lage-Klassen dieser Messung (r3_kugel_*.json, drei Kieferstellungen). Dazu das einfachste Lagemass: aus wie vielen von 162 Richtungen
ist die Flaeche (nur Kopf + Kiefer, Rueckseiten weg) bei geschlossenem Maul (Clip 5/7, 0 Grad) und bei offenem (Griff 17,1 Grad,
Bild-2-Pose 18,7 Grad) zu sehen. Maul-Inneres = wird erst durch das Oeffnen sichtbar. Nur lesen."""
import json
import subprocess
from collections import Counter
from s_basis import WT, ZIEL
import r3_lage as RL

neu = json.loads(subprocess.run(['git', '-C', WT, 'show', 'c8fbee2f6:analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/zonen_v2.json'],
                                capture_output=True, check=True).stdout.decode('utf-8'))['zonen']
z30n = next(z for z in neu if z['nr'] == '30')['modell_zuordnung']['liste']
P = {n: json.load(open(ZIEL + '/messung/r3_kugel_%s.json' % n, encoding='utf-8'))['je_flaeche'] for n in ('zu', 'griff', 'bild2')}
erg = dict(zone30_neu=z30n, sechs_darin=[f for f in RL.SECHS if f in z30n], alte_zone30_darin=[f for f in RL.Z30 if f in z30n],
           klassen={n: dict(Counter(P[n][f]['klasse'] for f in z30n)) for n in P},
           je_flaeche={f: [P[n][f]['klasse'] for n in ('zu', 'griff', 'bild2')] + [P['griff'][f]['material']] for f in z30n})
def richt(L):
    return {f: [P[n][f]['richtungen'] for n in ('zu', 'griff', 'bild2')] for f in L}


erg['richtungen_zu_griff_bild2'] = dict(alt=richt(RL.Z30), neu=richt(z30n))
for nm, L in (('alt', RL.Z30), ('neu', z30n), ('sechs', RL.SECHS)):
    r = richt(L)
    erg.setdefault('zuwachs_beim_oeffnen', {})[nm] = dict(
        flaechen=len(L), mehr_sichtbar_offen=sum(1 for v in r.values() if min(v[1], v[2]) > v[0] + 3),
        gleich_sichtbar=sum(1 for v in r.values() if abs(v[1] - v[0]) <= 3 and abs(v[2] - v[0]) <= 3),
        weniger_sichtbar_offen=sum(1 for v in r.values() if max(v[1], v[2]) < v[0] - 3))
json.dump(erg, open(ZIEL + '/messung/r3_gegenprobe_zone30_neu.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
print(json.dumps({k: v for k, v in erg.items() if k not in ('je_flaeche', 'richtungen_zu_griff_bild2')}, ensure_ascii=False, indent=1))
for f, v in erg['je_flaeche'].items():
    print(f, v)
