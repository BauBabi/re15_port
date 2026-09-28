#!/usr/bin/env python3
"""Runde 30 / tuer-verschlossen: sind die verschiedenen RE2-'verschlossen'-Wellen (Raumbank
Satz 0x16) verschiedene GERAEUSCHE oder dasselbe Geraeusch in anderer Abtastrate?

Verfahren: jede Welle dekodieren, mit IHRER Abspielrate (Port-Weg note2pitch2) auf eine
gemeinsame Zeitachse legen (Huellkurve = mittlerer Betrag je 10 ms), auf Spitze 1 normieren
und paarweise die Pearson-Korrelation der Huellkurven ueber die gemeinsame Laenge bilden.
>= 0.90 bei gleicher Dauer (+-5 %) = dasselbe Geraeusch. Das ist eine MESSUNG der
Huellkurve, kein Hoerurteil.
"""
import sys, os, glob, struct, collections, math
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from r30_re2_roombank import load_bank, resolve
from r30_re2_wellen_wav import vag_decode, note2pitch2

def huelle(pcm, rate, dt=0.010):
    n = max(1, int(rate * dt))
    out = []
    for i in range(0, len(pcm) - n + 1, n):
        out.append(sum(abs(v) for v in pcm[i:i + n]) / n)
    m = max(out) if out else 1
    return [v / (m or 1) for v in out]

def pearson(a, b):
    n = min(len(a), len(b))
    if n < 5: return 0.0
    a = a[:n]; b = b[:n]
    ma = sum(a) / n; mb = sum(b) / n
    sa = math.sqrt(sum((x - ma) ** 2 for x in a)); sb = math.sqrt(sum((x - mb) ** 2 for x in b))
    if sa == 0 or sb == 0: return 0.0
    return sum((x - ma) * (y - mb) for x, y in zip(a, b)) / (sa * sb)

def main():
    sid = int(sys.argv[1], 16) if len(sys.argv) > 1 else 0x16
    files = sorted(glob.glob('info/re2leon/PL0/RDT/ROOM[1-7]*.RDT'))
    waves = collections.OrderedDict()
    for f in files:
        b = load_bank(f)
        if b is None: continue
        r = resolve(b, sid)
        if r is None or r['empty']: continue
        L = r['layers'][0]
        if 'sha1' not in L: continue
        room = os.path.basename(f)[4:8]
        pitch = note2pitch2(L['mn'], L['shift'], L['center'], L['shift'])
        rate = 44100 * pitch // 4096
        key = L['sha1'][:12]
        if key not in waves:
            raw = b['d'][L['vag_off']:L['vag_off'] + L['vag_size']]
            pcm = vag_decode(raw)
            waves[key] = dict(size=L['vag_size'], pcm=pcm, rooms=[], rates=collections.Counter())
        waves[key]['rooms'].append(room)
        waves[key]['rates'][rate] += 1
    keys = list(waves)
    for k in keys:
        w = waves[k]
        rate = w['rates'].most_common(1)[0][0]
        w['rate'] = rate
        w['env'] = huelle(w['pcm'], rate)
        w['dauer'] = len(w['pcm']) / rate
    print('# Satz 0x%02x: %d verschiedene Wellen' % (sid, len(keys)))
    for i, k in enumerate(keys):
        w = waves[k]
        print('W%-2d %s %6d B  Raten %s  Dauer %.3f s  Raeume %s' % (
            i, k, w['size'], dict(w['rates']), w['dauer'], ' '.join(w['rooms'])))
    print()
    print('# Huellkurven-Korrelation (Zeile/Spalte = W-Nummer), * = gleiche Dauer +-5 % UND r >= 0.90')
    print('     ' + ' '.join('W%-5d' % i for i in range(len(keys))))
    fam = list(range(len(keys)))
    def find(x):
        while fam[x] != x: x = fam[x]
        return x
    for i, a in enumerate(keys):
        row = []
        for j, b in enumerate(keys):
            r = pearson(waves[a]['env'], waves[b]['env'])
            gl = abs(waves[a]['dauer'] - waves[b]['dauer']) <= 0.05 * max(waves[a]['dauer'], waves[b]['dauer'])
            mark = '*' if (i != j and gl and r >= 0.90) else ' '
            if mark == '*': fam[find(i)] = find(j)
            row.append('%5.2f%s' % (r, mark))
        print('W%-3d ' % i + ' '.join(row))
    groups = collections.defaultdict(list)
    for i in range(len(keys)): groups[find(i)].append(i)
    print()
    print('# Familien (gleiches Geraeusch nach Huellkurve):')
    for g, mem in sorted(groups.items(), key=lambda kv: -sum(len(waves[keys[i]]['rooms']) for i in kv[1])):
        rooms = sum((waves[keys[i]]['rooms'] for i in mem), [])
        print('  Familie {%s}: %d Raeume: %s' % (', '.join('W%d %s' % (i, keys[i]) for i in mem), len(rooms), ' '.join(sorted(rooms))))

if __name__ == '__main__':
    main()
