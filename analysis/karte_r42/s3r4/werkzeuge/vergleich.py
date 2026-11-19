# Vergleich v0.8.30 (aaea5f8c2) / Runde 3 (b94479047) / Runde 4 an denselben Lagen, Rot-Mass der Pruefung (um >= 0.3).
import json, collections, sys
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte'
basis = {r['tag']: r for r in json.load(open(S + '/s3r2/auswertung_basis.json'))}
r3 = {r['tag']: r for r in json.load(open(S + '/s3r3/auswertung.json'))}
r4 = {r['tag']: r for r in json.load(open(S + '/s3r4b/auswertung.json'))}
def stage(t): return t[1]
gemein = sorted(t for t in basis if t in r4 and stage(t) != '1')
print('Gleiche %d Leon-Ankunftslagen STAGE2-6 (wie Runde 3)' % len(gemein))
for name, d in (('v0.8.30 aaea5f8c2', basis), ('Runde 3 b94479047', r3), ('jetzt', r4)):
    print('  %-20s %s' % (name, dict(collections.Counter(d[t]['urteil'] for t in gemein if t in d))))
schl = [t for t in gemein if basis[t]['urteil'] == 'ok' and r4[t]['urteil'] != 'ok']
bess = [t for t in gemein if basis[t]['urteil'] != 'ok' and r4[t]['urteil'] == 'ok']
print('besser als v0.8.30 (%d): %s' % (len(bess), ' '.join(bess)))
print('schlechter als v0.8.30 (%d): %s' % (len(schl), ' '.join('%s(%s um %s, vorher %s)' % (t, r4[t]['urteil'], r4[t].get('um'), basis[t].get('um')) for t in schl)))
print('Zahlwert um kleiner als v0.8.30 (nur Info):')
for t in gemein:
    a, b = basis[t].get('um'), r4[t].get('um')
    if a is not None and b is not None and b < a - 0.005 and r4[t]['urteil'] == 'ok':
        print('   %s %.2f -> %.2f' % (t, a, b))
print()
print('Runde 3 -> Runde 4, alle Lagen mit Aenderung (Punkt oder Urteil):')
for t in sorted(r4):
    if t in r3 and (r3[t].get('marker') != r4[t].get('marker') or r3[t]['urteil'] != r4[t]['urteil']):
        print('   %-10s %s um %s %s  ->  %s um %s %s' % (t, r3[t].get('marker'), r3[t].get('um'), r3[t]['urteil'], r4[t].get('marker'), r4[t].get('um'), r4[t]['urteil']))
z = collections.Counter((('S1' if stage(t) == '1' else 'S2-6'), r['urteil']) for t, r in r4.items())
print(); print('Summe jetzt:', dict(z))
