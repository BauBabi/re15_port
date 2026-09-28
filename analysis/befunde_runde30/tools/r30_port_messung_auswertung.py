#!/usr/bin/env python3
"""Auswertung von build/r30_tuer-verschlossen/port_messung.txt (Sonde
probe_r30_tuer_verschlossen): alle Plaetze, an denen ein Schloss-Text aufging, nach Art
gruppiert, und ob dabei ein Ton fiel, der im Kontrolllauf OHNE Druck NICHT fiel."""
import re, collections, sys
KIND = [
 ('kartenleser', re.compile(r"card|\bID\b", re.I)),
 ('code',        re.compile(r"code|password|keypanel", re.I)),
 ('elektronisch', re.compile(r"electronic", re.I)),
 ('andere_seite', re.compile(r"other side|from inside|from the inside", re.I)),
 ('schluessel',  re.compile(r"\bkey\b", re.I)),
 ('verschlossen', re.compile(r"lock|latched|won't open", re.I)),
]
def kind(t):
    for k, p in KIND:
        if p.search(t): return k
    return None
rows = [l.rstrip('\n') for l in open('build/r30_tuer-verschlossen/port_messung.txt', encoding='utf-8', errors='replace') if l.startswith('ROOM')]
out = []; cnt = collections.Counter(); ton_mehr = []
for r in rows:
    m = re.search(r'\| (msg \d+ ".*?) \| TON: snd1=(\d+) snd0=(\d+) core=(\d+) re2panel=(\d+) re2elev=(\d+) scd_se_on=(\d+)(.*?) \| KONTROLLE ohne Druck: snd1=(\d+) uebrige=(\d+)', r)
    if not m: continue
    first = re.match(r'msg (\d+) "(.*?)"', m.group(1))
    k = kind(first.group(2))
    if k is None: continue
    se1, sn0, core, pan, elv, q = [int(m.group(i)) for i in range(2, 8)]
    k1, kr = int(m.group(9)), int(m.group(10))
    rest = sn0 + core + pan + elv + q
    cnt[k] += 1
    mehr = (rest > kr) or (se1 > k1)
    head = r.split(' flags=')[0]
    out.append((k, head, first.group(1), first.group(2), se1, rest, k1, kr, m.group(8).strip()))
    if mehr: ton_mehr.append(out[-1])
print('# Plaetze in der Messung: %d; davon ging an %d ein Schloss-Text auf' % (len(rows), len(out)))
print('# nach Art: %s' % dict(cnt))
print('# davon mit einem Ton, der im Kontrolllauf ohne Druck NICHT fiel: %d' % len(ton_mehr))
for t in ton_mehr: print('   TON>KONTROLLE:', t)
print()
for k, head, mi, txt, se1, rest, k1, kr, qq in sorted(out):
    print('%-12s %-28s msg %-2s snd1=%d(ktl %d) uebrige=%d(ktl %d) %s | %s' % (k, head, mi, se1, k1, rest, kr, qq, txt[:70]))
