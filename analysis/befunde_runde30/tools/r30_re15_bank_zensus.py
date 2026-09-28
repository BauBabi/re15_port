#!/usr/bin/env python3
"""RE1.5-Raumbank-Zensus (snd0 = Se_on-Bank 2; RDT-Kopf +0x08/+0x0C/+0x10), alle 240 RDTs.
Frage: hat RE1.5 in seinen eigenen Raumbaenken einen 'verschlossen'-Satz (RE2-Konvention
Satz 0x16) - und welche Saetze sind ueberhaupt belegt?  Leer = 00 00 00 00 (re15_edt_decode
rec.empty) oder ff ff ff ff."""
import sys, os, glob, collections, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_re2_roombank import load_bank, resolve
files = sorted(glob.glob('re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT'))
belegt = collections.Counter(); nrooms = 0; ncount = collections.Counter()
w16 = collections.defaultdict(list)
for f in files:
    if os.path.getsize(f) < 0x100: continue
    b = load_bank(f)
    room = os.path.basename(f)[4:8]
    if b is None: continue
    nrooms += 1; ncount[b['n']] += 1
    for i in range(b['n']):
        r = b['d'][b['edt']+i*4:b['edt']+i*4+4]
        if r in (b'\0\0\0\0', b'\xff\xff\xff\xff'): continue
        belegt[i] += 1
        if i == 0x16:
            rr = resolve(b, i)
            L = rr['layers'][0]
            w16[(L.get('sha1','?')[:12], L.get('vag_size'))].append(room)
print('# %d Raeume mit snd0-Bank; EDT-Satzzahlen %s' % (nrooms, dict(ncount)))
print('# belegte Saetze (Id: Anzahl Raeume):')
print('  ' + '  '.join('0x%02x:%d' % (i, belegt[i]) for i in sorted(belegt)))
print('# Satz 0x16 belegt in %d Raeumen' % sum(len(v) for v in w16.values()))
for k, v in w16.items(): print('   ', k, ' '.join(v))
